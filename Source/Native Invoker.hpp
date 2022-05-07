#pragma once
#include "Includes.hpp"
#include "RageHashMap.hpp"
#include "Logger.hpp"
#include "Namespaces/Namespace Rage.hpp"
#include "Namespaces/Vector.hpp"
#include "Pointers.hpp"

namespace Base::Core::Invoke {
	struct nativeCtx : public rage::scrNativeCallContext {
	public:
		nativeCtx() {
			reset();
		}
	public:
		void reset() {
			m_return = &m_returnData[0];
			m_argCount = 0;
			m_args = &m_argumentData[0];
			m_dataCount = 0;
			std::fill(std::begin(m_vectorSpace), std::end(m_vectorSpace), '\0');
			std::fill(std::begin(m_returnData), std::end(m_returnData), 0);
			std::fill(std::begin(m_argumentData), std::end(m_argumentData), 0);
		}
	public:
		template <typename T>
		void pushArg(T value) {
			static_assert(sizeof(T) <= 8);
			std::uintptr_t data{};
			*(T*)(&data) = value;
			m_argumentData[m_argCount++] = data;
		}
		template <>
		void pushArg<Vector3>(Vector3 value) {
			pushArg(value.x);
			pushArg(value.y);
			pushArg(value.z);
		}
	public:
		template <typename T> T getRetVal() { return *static_cast<T*>(m_return); }
		template <> void getRetVal<void>() {}
	public:
		std::uintptr_t m_returnData[10];
		std::uintptr_t m_argumentData[100];
	};
	class nativeInvoker {
	public:
		nativeInvoker() = default;
		~nativeInvoker() = default;
	public:
		void cacheHandlers();
	public:
		void beginCall();
		void endCall(rage::scrNativeHash hash);
	public:
		template <typename type>
		void pushArg(type&& value) {
			m_context.pushArg(std::forward<type>(value));
		}
		template <typename type>
		type getRet() {
			return m_context.getRetVal<type>();
		}
	public:
		std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> m_cache;
		nativeCtx m_context;
	};
}
inline Base::Core::Invoke::nativeInvoker g_nativeInvoker;