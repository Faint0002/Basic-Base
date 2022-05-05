#pragma once
#include "Includes.hpp"
#include "TLSContext.hpp"

namespace rage {
	//Thread State
	enum class eThreadState : uint32_t {
		idle,
		running,
		killed,
		unk_3,
		unk_4,
	};
	//Thread
	class scrThreadContext {
	public:
		uint32_t m_thread_id;				 // 0x00
		uint32_t m_script_hash;				 // 0x04
		eThreadState m_state;                // 0x08
		uint32_t m_instruction_pointer;		 // 0x0C
		uint32_t m_frame_pointer;			 // 0x10
		uint32_t m_stack_pointer;			 // 0x14
		float m_timer_a;                     // 0x18
		float m_timer_b;                     // 0x1C
		float m_timer_c;                     // 0x20
		char m_padding1[0x2C];               // 0x24
		uint32_t m_stack_size;				 // 0x50
		char m_padding2[0x54];               // 0x54
	};
	class scriptHandler;
	class scriptHandlerNetComponent;
	class scrThread {
	public:
		virtual ~scrThread() = default;                                                                 // 0 (0x00)
		virtual void reset(uint32_t script_hash, void* args, std::uint32_t arg_count) = 0;				// 1 (0x08)
		virtual eThreadState run() = 0;                                                                 // 2 (0x10)
		virtual eThreadState tick(uint32_t ops_to_execute) = 0;											// 3 (0x18)
		virtual void kill() = 0;                                                                        // 4 (0x20)
		static scrThread* get() {
			return *tlsContext::get()->getScriptThread();
		}
	public:
		scrThreadContext m_context;                 // 0x08
		void* m_stack;                              // 0xB0
		char m_padding[0x10];                       // 0xB8
		const char* m_exit_message;                 // 0xC8
		char m_name[0x40];                          // 0xD0
		scriptHandler* m_handler;                   // 0x110
		scriptHandlerNetComponent* m_net_component; // 0x118
	};
}