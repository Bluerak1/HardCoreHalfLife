#pragma once


#include "extdll.h"
#include "progdefs.h"
#include "util.h"
#include "cbase.h"
#include "basemonster.h"
#include "vector.h"
#include "pm_materials.h"
#include "player.h" 
#include <map>
#include <vector>
#include <string>

#define DEATH_COUNT_STATUS_TEXT "dc"
#define CHECKPOINT_STATUS_TEXT "cp"

#define MOD_STARTING_LEVEL "c1a1"
#define DELIMITER ":"
#define DEATH_COUNT_TEXT "Death Count"
#define WELCOME_MSG "Good Luck!"

/*
 * This is a data structure representing the current HardCore Status:
 * - deathCount
 * - lastCheckpoint
 *
 * - isDeathCountDisplayed
 * - isWelcomeMessageDisplayed
 * - hardcoreStatusLoaded
 */
struct HardCoreStatusData
{
	int deathCount = 0;
	std::string lastCheckpoint = MOD_STARTING_LEVEL;

	bool isDeathCountDisplayed = false;
	bool isWelcomeMessageDisplayed = false;
	bool hardcoreStatusLoaded = false;
	bool hasCheckpointItems = false;
};

class HardCoreStatus
{
	public:
		static void LoadCheckPoint();
		static void UpdateCheckPointIfNeeded(const std::string& mapName);
		static void UpdateHardCoreStatusFile();
		static void LoadHardCoreStatus();
		static void Death();
		static void DisplayDeathCountIfNotDisplayed();
		static void DisplayWelcomeMessageIfNotDisplayed();
		static void GiveCheckpointItemsIfNeeded(CBasePlayer* plr);

		/* This will just return the hardcoreStatusLoaded to avoid exposing the whole data */
		static bool IsHardCoreStatusLoaded()
		{
			return hcData.hardcoreStatusLoaded;
		}

	private:
		/*
		 * Please note that this is STATIC because we only want a single instance of this status throughout the whole game.
		 */
		static HardCoreStatusData hcData;

		static std::map<std::string, std::vector<std::string>> checkpointItemMap;
		static std::vector<std::string> possibleCheckpoints;

		static void DisplayMsg(const float textXCoord, const float textYCoord, const float displayTime, const int channel, const std::string& textToDisplay);
};
