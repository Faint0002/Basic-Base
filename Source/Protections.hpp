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
	//Report Events
	inline bool m_blockReports = false;
	inline std::vector<modelHandler> m_reportHashes = {
		{ rageJoaat("MPPLY_BAD_CREW_STATUS"), "Bad Crew Status", &m_blockReports },
		{ rageJoaat("MPPLY_BAD_CREW_MOTTO"), "Bad Crew Motto", &m_blockReports },
		{ rageJoaat("MPPLY_BAD_CREW_NAME"), "Bad Crew Name", &m_blockReports },
		{ rageJoaat("MPPLY_BAD_CREW_EMBLEM"), "Bad Crew Emblem", &m_blockReports },
		{ rageJoaat("MPPLY_VC_ANNOYINGME"), "Annoying Me In VC", &m_blockReports },
		{ rageJoaat("MPPLY_VC_HATE"), "VC Hate", &m_blockReports },
		{ rageJoaat("MPPLY_EXPLOITS"), "Exploits", &m_blockReports },
		{ rageJoaat("MPPLY_PLAYERMADE_TITLE"), "Offsensive Player Title", &m_blockReports },
		{ rageJoaat("MPPLY_PLAYERMADE_DESC"), "Offsensive Player Description", &m_blockReports }
	};
	//Objects
	inline bool m_blockSlodCrash;
	inline std::vector<modelHandler> m_crashModels = {
		{ rageJoaat("SLOD_Human"), "Human (Slod)", &m_blockSlodCrash },
		{ rageJoaat("SLOD_Large_Quadped"), "Large Quadped (Slod)", &m_blockSlodCrash },
		{ rageJoaat("SLOD_Small_Quadped"), "Large Quadped (Slod)", &m_blockSlodCrash }
	};
}