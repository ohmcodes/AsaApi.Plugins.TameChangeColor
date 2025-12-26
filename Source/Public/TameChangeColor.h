#pragma once

#include "json.hpp"

#include "Database/DatabaseFactory.h"

#include "Requests.h"

namespace TameChangeColor
{
	inline nlohmann::json config;
	inline bool isDebug{ false };

	inline int counter = 0;

	inline std::unique_ptr<IDatabaseConnector> permissionsDB;

	inline std::unique_ptr<IDatabaseConnector> pointsDB;

	static API::Requests& req = API::Requests::Get();

	inline std::string lastMessageID;

	inline APrimalDinoCharacter* CopiedColorDino;
	inline TMap<int, int> CopiedColor;

}