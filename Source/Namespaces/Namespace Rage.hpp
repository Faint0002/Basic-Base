#pragma once
#include "../Includes.hpp"
#include "SCRThread.hpp"

extern class CNetGamePlayer;
extern class CNonPhysicalPlayerData;

namespace rage {
	//Fw Defines
	class fwRefAwareBase;
	class fwExtensibleBase;
	class fwEntity;
	class fwArchetype;
	class datBitBuffer;
	class sysMemAllocator;
	//Script Defines
	class scriptIdBase;
	class scriptId;
	class scriptHandler;
	class scriptHandlerNetComponent;
	class scriptHandlerObject;
	class tlsContext;
	//Joaat Defines
	using joaat_t = std::uint32_t;
	inline joaat_t joaat(std::string_view str) {
		joaat_t hash = 0;
		for (char c : str) {
			hash += tolower(c);
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}
	//Script Thread Defines
	//AtArray
	template <typename T>
	class atArray {
	public:
		T* begin() { return m_data; }
		T* end() { return m_data + m_size; }
		const T* begin() const { return m_data; }
		const T* end() const { return m_data + m_size; }
		T* data() { return m_data; }
		const T* data() const { return m_data; }
		std::uint16_t size() const { return m_size; }
		std::uint16_t capacity() const { return m_capacity; }
		T& operator[](std::uint16_t index) { return m_data[index]; }
		const T& operator[](std::uint16_t index) const { return m_data[index]; }
	private:
		T* m_data;
		std::uint16_t m_size;
		std::uint16_t m_capacity;
	};
	//datBase
	class datBase { public: virtual ~datBase() = default; };
	//pgBase
	class pgBase { public: virtual ~pgBase() = default; private: void* m_pgunk; };
	//Program
	class scrProgram : public pgBase {
	public:
		uint8_t** m_code_blocks;  // 0x10
		uint32_t m_hash;          // 0x18
		uint32_t m_code_size;     // 0x1C
		uint32_t m_arg_count;     // 0x20
		uint32_t m_local_count;   // 0x24
		uint32_t m_global_count;  // 0x28
		uint32_t m_native_count;  // 0x2C
		void* m_local_data;            // 0x30
		int64_t** m_global_data;  // 0x38
		void** m_native_entrypoints;   // 0x40
		char m_padding6[0x10];         // 0x48
		uint32_t m_name_hash;     // 0x58
		char m_padding7[0x04];         // 0x5C
		const char* m_name;            // 0x60
		const char** m_strings_data;   // 0x68
		uint32_t m_strings_count; // 0x70
		char m_padding8[0x0C];         // 0x74
		bool is_valid() const { return m_code_size != 0; }
		uint32_t get_num_code_pages() const { return (m_code_size + 0x3FFF) >> 14; }
		uint32_t get_code_page_size(uint32_t page) const {
			auto num = get_num_code_pages();
			if (page < num) {
				if (page == num - 1)
					return m_code_size & 0x3FFF;
				return 0x4000;
			}
			return 0;
		}
		uint32_t get_full_code_size() const {
			auto numPages = get_num_code_pages();
			if (!numPages)
				return 0;
			if (numPages == 1)
				--numPages;
			return (numPages * 0x4000) + (m_code_size & 0x3FFF);
		}
		uint8_t* get_code_page(uint32_t page) const {
			return m_code_blocks[page];
		}
		uint8_t* get_code_address(uint32_t index) const {
			if (index < m_code_size)
				return &m_code_blocks[index >> 14][index & 0x3FFF];
			return nullptr;
		}
		const char* get_string(uint32_t index) const {
			if (index < m_strings_count)
				return &m_strings_data[index >> 14][index & 0x3FFF];
			return nullptr;
		}
		void** get_address_of_native_entrypoint(void* entrypoint) {
			for (uint32_t i = 0; i < m_native_count; ++i) {
				if (m_native_entrypoints[i] == entrypoint) {
					return m_native_entrypoints + i;
				}
			}
			return nullptr;
		}
	};
	//Program Table Entry
	class scrProgramTableEntry {
	public:
		scrProgram* m_program;     // 0x00
		char m_Pad1[0x04];         // 0x08
		joaat_t m_hash;            // 0x0C
	};
	//Program Table
	class scrProgramTable {
	public:
		scrProgramTableEntry* m_data;    // 0x00
		char m_padding[0x10];            // 0x08
		std::uint32_t m_size;            // 0x18
		scrProgram* find_script(joaat_t hash) {
			for (std::uint32_t i = 0; i < m_size; ++i) {
				if (m_data[i].m_hash == hash) {
					return m_data[i].m_program;
				}
			}
			return nullptr;
		}
		scrProgramTableEntry* begin() {
			return m_data;
		}
		scrProgramTableEntry* end() {
			return m_data + m_size;
		}
	};
	//Resource
	class scriptResource { public: virtual ~scriptResource() = default; };
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
	//Array Defines
	//DNode
	template <typename T, typename Base = datBase>
	class atDNode : public Base {
	public:
		T m_data;
		void* m_unk;
		atDNode<T, Base>* m_next;
	};
	//dList
	template <typename Node>
	class atDList {
	public:
		Node* m_head;
		Node* m_tail;
	};
	//Handler
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
	//Invoker Defines
	struct scrNativeCallContext {
	protected:
		void* m_return;
		std::uint32_t m_argCount;
		char padding[4];
		void* m_args;
		std::uint32_t m_dataCount;
		char padding1[4];
		alignas(std::uintptr_t) std::uint8_t m_vectorSpace[192];
	};
	using scrNativeHash = std::uint64_t;
	using scrNativeHandler = void(*)(scrNativeCallContext*);
	//Vector Defines
	struct vector2 { float x, y; };
	struct vector3 { float x, y, z; };
	struct vector4 { float x, y, z, w; };
	//Network Defines
	class netAddress {
	public:
		netAddress() = default;
		netAddress(std::uint8_t field1, std::uint8_t field2, std::uint8_t field3, std::uint8_t field4) {
			m_field1 = field1;
			m_field2 = field2;
			m_field3 = field3;
			m_field4 = field4;
		}
	public:
		std::uint8_t m_field1;
		std::uint8_t m_field2;
		std::uint8_t m_field3;
		std::uint8_t m_field4;
	};
	class netPlayerData {
	public:
		char pad_0000[8]; //0x0000
		uint64_t m_rockstar_id; //0x0008
		char pad_0010[52]; //0x0010
		class netAddress m_relay_ip; //0x0044
		uint16_t m_relay_port; //0x0048
		char pad_004A[2]; //0x004A
		class netAddress m_external_ip; //0x004C
		uint16_t m_external_port; //0x0050
		char pad_0052[2]; //0x0052
		class netAddress m_internal_ip; //0x0054
		uint16_t m_internal_port; //0x0058
		char pad_005A[6]; //0x005A
		uint64_t m_host_token; //0x0060
		uint64_t m_peer_id; //0x0068
		uint64_t m_rockstar_id2; //0x0070
		char pad_0078[12]; //0x0078
		char m_name[20]; //0x0084
	}; //Size: 0x0098
	class netPlayer {
	public:
		virtual ~netPlayer();
		virtual void reset();
		virtual bool is_valid();
		virtual const char* get_name();
		virtual void _0x20();
		virtual bool is_host();
		virtual netPlayerData* get_net_data();
		virtual void _0x38();
	}; //Size: 0x0008
	class netPlayerMgrBase {
	public:
		virtual ~netPlayerMgrBase() = default;
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void unk_0x18() = 0;
		virtual CNetGamePlayer* AddPlayer_raw(void* a1, void* a2, void* a3, rage::netPlayerData* net_player_data, CNonPhysicalPlayerData* non_physical_player_data) = 0;
		virtual void RemovePlayer(CNetGamePlayer* net_game_player) = 0;
		virtual void UpdatePlayerListsForPlayer(CNetGamePlayer* net_game_player) = 0;
		virtual CNetGamePlayer* AddPlayer(void* a1, void* a2, void* a3, rage::netPlayerData* net_player_data, CNonPhysicalPlayerData* non_physical_player_data) = 0;
	}; //Size: 0x0008
}
using Void = void;
using Any = std::uint32_t;
using Hash = std::uint32_t;
using Entity = std::int32_t;
using Player = std::int32_t;
using FireId = std::int32_t;
using Interior = std::int32_t;
using Ped = Entity;
using Vehicle = Entity;
using Cam = std::int32_t;
using Object = Entity;
using Pickup = Object;
using Blip = std::int32_t;
using Camera = Entity;
using ScrHandle = Entity;
class Vector2 {
public:
	Vector2() = default;
	Vector2(float x, float y) : x(x), y(y) {}
public: float x;
private: char m_paddingx[0x04];
public: float y;
private: char m_paddingy[0x04];
};
class Vector3 {
public:
	Vector3() = default;
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
public: float x;
private: char m_paddingx[0x04];
public: float y;
private: char m_paddingy[0x04];
public: float z;
private: char m_paddingz[0x04];
};
class Vector4 {
public:
	Vector4() = default;
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
public: float x;
private: char m_paddingx[0x04];
public: float y;
private: char m_paddingy[0x04];
public: float z;
private: char m_paddingz[0x04];
public: float w;
private: char m_paddingw[0x04];
};

//Reconstructed Classes (Offsets)
class CNonPhysicalPlayerData {
public:
	int32_t m_bubble_id; //0x0008
	int32_t m_player_id; //0x000C
	rage::vector3 m_position; //0x0010
}; //Size: 0x001C
class CPlayerInfo {
public:
	char pad_0000[32]; //0x0000
	class rage::netPlayerData m_net_player_data; //0x0020
	char pad_00B8[184]; //0x00B8
	float m_swim_speed; //0x0170
	char pad_0174[20]; //0x0174
	uint32_t m_water_proof; //0x0188
	char pad_018C[92]; //0x018C
	class CPed* m_ped; //0x01E8
	char pad_01F0[40]; //0x01F0
	uint32_t m_frame_flags; //0x0218
	char pad_021C[52]; //0x021C
	uint32_t m_player_controls; //0x0250
	char pad_0254[1256]; //0x0254
	float m_wanted_can_change; //0x073C
	char pad_0740[304]; //0x0740
	uint32_t m_npc_ignore; //0x0870
	char pad_0874[12]; //0x0874
	bool m_is_wanted; //0x0880
	char pad_0881[7]; //0x0881
	uint32_t m_wanted_level; //0x0888
	uint32_t m_wanted_level_display; //0x088C
	char pad_0890[1120]; //0x0890
	float m_run_speed; //0x0CF0
	float m_stamina; //0x0CF4
	float m_stamina_regen; //0x0CF8
	char pad_0CFC[16]; //0x0CFC
	float m_weapon_damage_mult; //0x0D0C
	float m_weapon_defence_mult; //0x0D10
	char pad_0D14[4]; //0x0D14
	float m_melee_weapon_damage_mult; //0x0D18
	float m_melee_damage_mult; //0x0D1C
	float m_melee_defence_mult; //0x0D20
	char pad_0D24[8]; //0x0D24
	float m_melee_weapon_defence_mult; //0x0D2C
}; //Size: 0x0D30
class CNetGamePlayer : public rage::netPlayer {
public:
	char pad_0008[8]; //0x0008
	class CNonPhysicalPlayerData* m_non_physical_player; //0x0010
	uint32_t m_msg_id; //0x0018
	char pad_001C[4]; //0x001C
	uint8_t m_active_id; //0x0020
	uint8_t m_player_id; //0x0021
	char pad_0022[126]; //0x0022
	class CPlayerInfo* m_player_info; //0x00A0
}; //Size: 0x00A8
class GtaThread : public rage::scrThread {
public:
	rage::joaat_t m_script_hash;                // 0x120
	char m_padding3[0x14];                      // 0x124
	std::int32_t m_instance_id;                 // 0x138
	char m_padding4[0x04];                      // 0x13C
	std::uint8_t m_flag1;                       // 0x140
	bool m_safe_for_network_game;               // 0x141
	char m_padding5[0x02];                      // 0x142
	bool m_is_minigame_script;                  // 0x144
	char m_padding6[0x02];                      // 0x145
	bool m_can_be_paused;                       // 0x147
	bool m_can_remove_blips_from_other_scripts; // 0x148
	char m_padding7[0x0F];                      // 0x149
};
class CNetworkPlayerMgr : public rage::netPlayerMgrBase {
public:
	char pad_0008[224]; //0x0008
	class CNetGamePlayer* m_local_net_player; //0x00E8
	char pad_00F0[144]; //0x00F0
	class CNetGamePlayer* m_player_list[32]; //0x0180
	uint16_t m_player_limit; //0x0280
	char pad_0282[10]; //0x0282
	uint16_t m_player_count; //0x028C
}; //Size: 0x028E