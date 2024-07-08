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

#define CKP_DIFF "checkpoint_difficulty"
#define AMMO_DIFF "ammo_difficulty"
#define ENEMY_DIFF "enemy_difficulty"

enum Difficulty
{
	WALK_IN_THE_PARK,
	MILD_CHALLENGE,
	HARD,
	TRUE_HARDCORE
};

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
	bool hardcoreConfigLoaded = false;
	bool hasCheckpointItems = false;
};

struct HardCoreConfig
{
	Difficulty checkpointDifficulty;
	Difficulty ammoDifficulty;
	Difficulty enemyDifficulty;
};

class HardCoreStatus
{
	public:
		static void LoadHardCoreStatus();
		static void LoadHardCoreConfig();

		static void UpdateHardCoreStatusFile();

		static void LoadCheckPoint();
		static void UpdateCheckPointIfNeeded(const std::string& mapName);
		static void GiveCheckpointItemsIfNeeded(CBasePlayer* plr);
		
		static void Death();

		static void DisplayDeathCountIfNotDisplayed();
		static void DisplayWelcomeMessageIfNotDisplayed();
		
		/* This will just return the hardcoreStatusLoaded to avoid exposing the whole data */
		static const bool IsHardCoreStatusLoaded()
		{
			return hcData.hardcoreStatusLoaded;
		}

		static const Difficulty GetAmmoDifficulty()
		{
			return hcConfig.ammoDifficulty;
		}

		static void DisplayMsg(const float textXCoord, const float textYCoord, const float displayTime, const int channel, const std::string& textToDisplay);

	private:
		/*
		 * Please note that these are STATIC because we only want a single instance of this status throughout the whole game.
		 */
		static HardCoreStatusData hcData;
		static HardCoreConfig hcConfig;
		static std::map<std::string, std::pair<Difficulty, std::vector<std::string>>> checkpointMap;

		static std::map<std::string, Difficulty> difficultyRepresentation;
};
