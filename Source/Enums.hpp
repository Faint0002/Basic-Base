#pragma once
#include "Includes.hpp"

enum ScriptEventIds {
	CEO_BAN = -764524031,
	CEO_KICK = 248967238,
};
enum eObjType : int32_t {
	carObjType = 0,
	bikeObjType = 1,
	boatObjType = 2,
	doorObjType = 3,
	heliObjType = 4,
	objType = 5,
	pedObjType = 6,
	pickupObjType = 7,
	pickupPlacementObjType = 8,
	planeObjType = 9,
	submarineObjType = 10,
	playerObjType = 11,
	trailerObjType = 12,
	trainObjType = 13,
	unkObjType = -1
};