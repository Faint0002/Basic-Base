#pragma once
#include "../Includes.hpp"

namespace rage {
	class netSyncTree {
	public:
		char pad_0000[48]; //0x0000
		class netSyncTreeNode* m_sync_tree_node; //0x0030
	}; //Size: 0x0038
	class netSyncTreeNode {
	public:
		char pad_0000[192]; //0x0000
		uint32_t m_player_model; //0x00C0
		uint32_t m_ped_model; //0x00C4
		uint32_t m_vehicle_model; //0x00C8
		char pad_00CC[84]; //0x00CC
		uint32_t m_pickup_model; //0x0120
		char pad_0124[44]; //0x0124
		uint32_t m_object_model; //0x0150
		char pad_0154[692]; //0x0154
	}; //Size: 0x0408
}