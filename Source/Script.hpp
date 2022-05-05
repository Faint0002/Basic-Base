#pragma once
#include "Includes.hpp"
#include "Namespaces/Namespace Rage.hpp"
#include "Pointers.hpp"

namespace Base::Core {
	template <typename F, typename ...Args>
	void executeUnderScr(rage::joaat_t script_hash, F&& callback, Args &&...args) {
		auto tlsCtx = rage::tlsContext::get();
		for (auto scrThr : *g_Pointers->m_ScriptThreads) {
			//If our scr thread is null, continue. Same with thread id and if scr hash is not equal to the current scr hash, contuine and stop exec
			if (!scrThr || !scrThr->m_context.m_thread_id || scrThr->m_context.m_script_hash != script_hash)
				continue;
			//Get our current thread (og) and set it here as a rage::scrThread function
			auto OrginialThread = *tlsCtx->getScriptThread();
			//Get our script thread and make it our scr thread (ptr)
			*tlsCtx->getScriptThread() = scrThr;
			//Set our thread active to true
			*tlsCtx->isScriptThreadActive() = true;
			//Invoke our callback and any args.
			std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);
			//After invoke, set our thread back to our orginial thread
			*tlsCtx->getScriptThread() = OrginialThread;
			//Set our active thread to true/false according to if our og thread is null or not.
			*tlsCtx->isScriptThreadActive() = OrginialThread != nullptr;
			return;
		}
	}
	//Script Class
	class fiber {
	public:
		fiber(std::function<void()> func, std::optional<std::size_t> statckSize = std::nullopt) : m_func(func), m_secondaryFbr(nullptr), m_primaryFbr(nullptr) {
			m_secondaryFbr = CreateFiber(statckSize.has_value() ? statckSize.value() : 0, [](void* param) { static_cast<fiber*>(param)->fbrFunc(); }, this);
		}
		~fiber() {
			if (m_secondaryFbr)
				DeleteFiber(m_secondaryFbr);
		}
	public:
		void tick() {
			m_primaryFbr = GetCurrentFiber();
			if (!m_time.has_value() || m_time.value() <= std::chrono::high_resolution_clock::now())
				SwitchToFiber(m_secondaryFbr);
		}
		void wait(std::optional<std::chrono::high_resolution_clock::duration> time = std::nullopt) {
			if (time.has_value())
				m_time = std::chrono::high_resolution_clock::now() + time.value();
			else
				m_time = std::nullopt;
			SwitchToFiber(m_primaryFbr);
		}
		static fiber* cur() {
			return static_cast<fiber*>(GetFiberData());
		}
	public:
		void fbrFunc() {
			try {
				m_func();
			}
			catch (...) {
			
			}
			while (true) {
				wait();
			}
		}
	public:
		void* m_primaryFbr;
		void* m_secondaryFbr;
		std::function<void()> m_func;
	public:
		std::optional<std::chrono::high_resolution_clock::time_point> m_time;
	};
	//Script Manager Class
	class fbrMgr {
	public:
		fbrMgr() = default;
		~fbrMgr() = default;
	public:
		void add(std::shared_ptr<fiber> fbr, std::string fbrId) {
			std::lock_guard lck(m_mutex);
			m_scripts.insert({ fbrId, std::move(fbr) });
			logEntry(LogColor::White, "FiberMgr", "Registered fbr {}", fbrId);
		}
		void remove(std::string fbrId) {
			if (auto var = m_scripts.find(fbrId); var != m_scripts.end()) {
				logEntry(LogColor::White, "FiberMgr", "Removed fiber {}", fbrId);
				m_scripts.erase(var);
			}
			else {
				logEntry(LogColor::Red, "FiberMgr", "Fiber {} does not exist in the fiber stack", fbrId);
			}
		}
		void removeAll() {
			std::lock_guard lck(m_mutex);
			m_scripts.clear();
		}
		void scrTick(std::string scr = "main_persistent") {
			executeUnderScr(rage::joaat(scr), std::mem_fn(&fbrMgr::tick), this);
		}
		void tick() {
			static bool EnsureMainFiber = (ConvertThreadToFiber(nullptr), true);
			std::lock_guard lck(m_mutex);
			for (auto const& scr : m_scripts) {
				scr.second->tick();
			}
		}
	public:
		std::recursive_mutex m_mutex;
		std::map<std::string, std::shared_ptr<fiber>> m_scripts;
	};
	inline fbrMgr g_fbrMgr;
	//Fiber Pool
	static void fiberFunc();
	class fbrPool {
	public:
		fbrPool() = default;
		~fbrPool() = default;
	public:
		void registerFbrPool() {
			for (auto i = 0; i < m_numOfFbrs; ++i) {
				g_fbrMgr.add(std::make_shared<fiber>(&fiberFunc), "fbrPool_" + std::to_string(i));
			}
			logEntry(LogColor::Grey, "FiberPool", "Registered all {} fibers inside the fiberpool.", m_numOfFbrs);
		}
	public:
		void queue(std::function<void()> Func) {
			if (Func) {
				std::lock_guard lck(m_mutex);
				m_jobs.push(std::move(Func));
			}
		}
		void tick() {
			std::unique_lock lck(m_mutex);
			if (!m_jobs.empty()) {
				auto job = std::move(m_jobs.top());
				m_jobs.pop();
				lck.unlock();
				std::invoke(std::move(job));
			}
		}
	public:
		size_t m_numOfFbrs = 10;
		std::recursive_mutex m_mutex;
		std::stack<std::function<void()>> m_jobs;
	};
	inline fbrPool g_fbrPool;
	static void fiberFunc() {
		while (true) {
			g_fbrPool.tick();
			fiber::cur()->wait();
		}
	}
}

#define JobStart(...) Base::Core::g_fbrPool.queue([__VA_ARGS__] {
#define JobEnd });