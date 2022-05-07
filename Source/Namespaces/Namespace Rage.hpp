#pragma once
#include "../Includes.hpp"
#include "SCRThread.hpp"
#include "AtArray.hpp"
#include "NetGameEvent.hpp"
#include "NetPlayer.hpp"
#include "NetObject.hpp"
#include "SyncTree.hpp"
#include "FwEntity.hpp"

namespace rage {
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