#pragma once
#include "Includes.hpp"
#include "Namespaces/Joaat.hpp"
#include "Enums.hpp"

class modelHandler {
public:
	modelHandler(rage::joaat_t hash, std::string name, bool* toggle) {
		modelHash = hash;
		modelName = name;
		modelBlockToggle = toggle;
	}
public:
	std::string modelName;
	rage::joaat_t modelHash;
	bool* modelBlockToggle;
};
class eventHandler {
public:
	eventHandler(int64_t hash, std::string name, bool* toggle) {
		eventHash = hash;
		eventName = name;
		eventBlockToggle = toggle;
	}
public:
	std::string eventName;
	int64_t eventHash;
	bool* eventBlockToggle;
};

namespace Protections {
	inline bool m_logScriptEvents = false;
	//Script Events
	inline bool m_blockCEOBan = false;
	inline bool m_blockCEOKick = false;
	inline std::vector<eventHandler> m_scriptEvents = {
		{ ScriptEventIds::CEO_BAN, "CEO Ban", &m_blockCEOBan },
		{ ScriptEventIds::CEO_KICK, "CEO Kick", &m_blockCEOKick }
	};
	//Objects
	inline bool m_blockSlodCrash;
	inline std::vector<modelHandler> m_crashModels = {
		{ rageJoaat("SLOD_Human"), "Human (Slod)", &m_blockSlodCrash },
		{ rageJoaat("SLOD_Large_Quadped"), "Large Quadped (Slod)", &m_blockSlodCrash },
		{ rageJoaat("SLOD_Small_Quadped"), "Large Quadped (Slod)", &m_blockSlodCrash }
	};
}