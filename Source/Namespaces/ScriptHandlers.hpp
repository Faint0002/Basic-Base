#pragma once
#include "../Includes.hpp"
#include "AtArray.hpp"
#include "Joaat.hpp"
#include "SCRThread.hpp"
#include "Base.hpp"
#include "BitBuffer.hpp"

namespace rage {
	class scriptHandlerObject;
	class scriptHandler {
	public:
		class atDScriptObjectNode : public atDNode<scriptHandlerObject*> {};
	public:
		virtual ~scriptHandler() = default;                                                                   //  0 (0x00)
		virtual bool _0x08() = 0;                                                                             //  1 (0x08)
		virtual void _0x10() = 0;                                                                             //  2 (0x10)
		virtual void cleanup_objects() = 0;                                                                   //  3 (0x18)
		virtual scriptId* _0x20() = 0;                                                                        //  4 (0x20)
		virtual scriptId* get_id() = 0;                                                                       //  5 (0x28)
		//Returns whether the script handler belongs to a networked script.
		virtual bool is_networked() = 0;                                                                      //  6 (0x30)
		//Initializes the network component for the script handler.
		virtual void init_net_component() = 0;                                                                //  7 (0x38)
		//Deletes the script handler's network component, if it exists.
		virtual void reset_net_component() = 0;                                                               //  8 (0x40)
		//Destroys the script handler.
		virtual bool destroy() = 0;                                                                           //  9 (0x48)
		//Adds the object to the script handler's list of objects.
		virtual void add_object(scriptHandlerObject*, bool is_network, bool is_network_and_scriptcheck) = 0; // 10 (0x50)
		//Something related to reservations.
		virtual void _0x58(void*) = 0;                                                                        // 11 (0x58)
		virtual void register_resource(scriptResource*, void*) = 0;                                           // 12 (0x60)
		virtual void _0x68() = 0;                                                                             // 13 (0x68)
		virtual void _0x70() = 0;                                                                             // 14 (0x70)
		virtual void _0x78() = 0;                                                                             // 15 (0x78)
		virtual void _0x80() = 0;                                                                             // 16 (0x80)
		virtual void _0x88() = 0;                                                                             // 17 (0x88)
		virtual void _0x90() = 0;                                                                             // 18 (0x90)
		virtual void _0x98() = 0;                                                                             // 19 (0x98)
	public:
		void* m_0x08;                                // 0x08
		void* m_0x10;                                // 0x10
		scrThread* m_script_thread;                  // 0x18
		atDList<atDScriptObjectNode> m_objects;      // 0x20
		scriptResource* m_resource_list_head;        // 0x30
		scriptResource* m_resource_list_tail;        // 0x38
		void* m_0x40;                                // 0x40
		scriptHandlerNetComponent* m_net_component;  // 0x48
		std::uint32_t m_0x50;                        // 0x50
		std::uint32_t m_0x54;                        // 0x54
		std::uint32_t m_0x58;                        // 0x58
		std::uint32_t m_0x60;                        // 0x5C
	};
	//Resource
	class scriptResource {
	public:
		virtual ~scriptResource() = default;
	};
	//Resource Entry
	class scriptResourceEntry {
	public:
		scriptResource* m_data;      // 0x00
		uint32_t m_unk;				 // 0x04
		char m_padding[0x0C];        // 0x0C
		scriptResourceEntry* m_next; // 0x18
	};
	//Handler Net Component
	class scriptHandlerNetComponent {
	public:
		virtual ~scriptHandlerNetComponent() = default;
	public:
		scriptHandler* m_script_handler; // 0x08
	};
	//Id Base
	class scriptIdBase {
	public:
		virtual ~scriptIdBase() = default;                            // 0 (0x00)
		//Assumes the script thread's identity.
		virtual void assume_thread_identity(scrThread*) {};           // 1 (0x08)
		//Returns whether the hash of the script id is valid.
		virtual bool is_valid() {};                                   // 2 (0x10)
		//Gets the hash of the script id.
		virtual joaat_t* get_hash(joaat_t* out) {};                   // 3 (0x18)
		//Gets an unknown value from the script id.
		virtual std::uint32_t* get_hash2(std::uint32_t* out) {};      // 4 (0x20)
		//Gets the name of the script id.
		virtual const char* get_name() {};                            // 5 (0x28)
		//Serializes the script id from the buffer.
		virtual void deserialize(datBitBuffer* buffer) {};            // 6 (0x30)
		//Serializes the script id to the buffer.
		virtual void serialize(datBitBuffer* buffer) {};              // 7 (0x38)
		//Calculates some information with the position hash & instance id.
		virtual std::uint32_t _0x40() {};                             // 8 (0x40)
		//Calls _0x40 and returns it's value added to another value.
		virtual std::uint32_t _0x48() {};                             // 9 (0x48)
		//Logs some information about the script id.
		virtual void log_information(void* logger) {}; // 10 (0x50)
		//Copies the information of other to this object.
		virtual void copy_data(scriptIdBase* other) {}                // 11 (0x58)
		//Returns whether the other script id is equal.
		virtual bool operator==(scriptIdBase*) {};                    // 12 (0x60)
		virtual bool _0x68(void*) {};                                 // 13 (0x68)
	};
	//Id
	class scriptId : public scriptIdBase {
	public:
		joaat_t m_hash;           // 0x08
		char m_name[0x20];        // 0x0C
	};
}