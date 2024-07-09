#include <string>
#include <fstream>
#include <iostream>

#include "hardcorestatus.h"
#include "mathlib.h"
#include "eiface.h"
#include "extdll.h"
#include "util.h"
#include "pm_materials.h"
#include <map>
#include <vector>
#include <algorithm>

/*
 * This is just a helper function to display a text message to the HUD with the given parameters.
 * It's private because it will only be called from a player object. Shouldn't be visible to the outside.
 */
void HardCoreStatus::DisplayMsg(const float textXCoord, const float textYCoord, const float displayTime, const int channel, const std::string& textToDisplay)
{
	hudtextparms_s tTextParam;
	tTextParam.x = textXCoord;
	tTextParam.y = textYCoord;
	tTextParam.effect = 0;
	tTextParam.r1 = 255;
	tTextParam.g1 = 255;
	tTextParam.b1 = 255;
	tTextParam.a1 = 255;
	tTextParam.r2 = 255;
	tTextParam.g2 = 255;
	tTextParam.b2 = 255;
	tTextParam.a2 = 255;
	tTextParam.fadeinTime = 0;
	tTextParam.fadeoutTime = 0;
	tTextParam.holdTime = displayTime;
	tTextParam.fxTime = 0;
	tTextParam.channel = channel;

	UTIL_HudMessageAll(tTextParam, textToDisplay.c_str());
}

/*
 * Display the death count to the top left of the screen.
 */
void HardCoreStatus::DisplayDeathCountIfNotDisplayed()
{
	// Don't display if it's already displayed
	if (hcData.isDeathCountDisplayed)
	{
		return;
	}

	// This will expand to "Death Count: X"
	std::string deathCountMsg = std::string(DEATH_COUNT_TEXT) + std::string(DELIMITER) + " " + std::to_string(hcData.deathCount);

	// - Top left corner of the screen
	// - Display for 99999 seconds (until death)
	// - Display Death Count: text
	DisplayMsg(0.02, 0.1, 99999.0f, 1, deathCountMsg);

	// Tell the PreThink() function that we have this displayed so no need to re-display
	hcData.isDeathCountDisplayed = true;
}

/*
 * Display the welcome message to the middle of the screen.
 */
void HardCoreStatus::DisplayWelcomeMessageIfNotDisplayed()
{
	// Don't display if it's already displayed
	if (hcData.isWelcomeMessageDisplayed)
	{
		return;
	}

	// - Middle of the screen
	// - Display for 2 seconds
	// - Display Good Luck!
	DisplayMsg(-1, -1, 2.0f, 2, WELCOME_MSG);

	// Tell the PreThink() function that we have this displayed so no need to re-display
	hcData.isWelcomeMessageDisplayed = true;
}

/** Helper function to load a map.*/
void HardCoreStatus::LoadCheckPoint()
{
	std::string checkpointDebugMsg = "Loading checkpoint: " + hcData.lastCheckpoint + "\n";
	ALERT(at_console, checkpointDebugMsg.c_str());
	SERVER_COMMAND(UTIL_VarArgs("map \"%s\"\n", hcData.lastCheckpoint.c_str()));

	// If we load a new level we need to re-give the items
	hcData.hasCheckpointItems = false;
}

/*
 * This function will check if we need to update the last checkpoint
 * i.e. if the player passed through a certain level that a new checkpoint needs to be stored
 */
void HardCoreStatus::UpdateCheckPointIfNeeded(std::string& mapName)
{
	// If the map we transitioned to is the same as our checkpoint we do nothing
	if (mapName == hcData.lastCheckpoint) {
		return;
	}

	// Check if the map we transitioned to is a checkpoint
	// Check if our checkpoint difficulty is ON or BELOW the checkpoint's difficulty, if yes we save the checkpoint!
	if (checkpointMap.find(mapName) != checkpointMap.end() && hcConfig.checkpointDifficulty <= checkpointMap[mapName].first)
	{
		const std::string ckpDebugMsg = "Player transitioned to " + mapName + " this is a checkpoint.\n";
		ALERT(at_console, ckpDebugMsg.c_str());

		// The Map we transitioned to is in the checkpoint list so we need to update the checkpoint
		hcData.lastCheckpoint = mapName;

		// Write it to file
		UpdateHardCoreStatusFile();

		// Display the message
		DisplayMsg(-1, -0.5, 5.0f, 1, "Checkpoint Reached!");
	}

	// TODO We should somehow order the checkpoints to make sure we don't update the checkpoint if the player goes back?
	// IDK maybe this is not even a problem... not my problem if they go back lol
}

/*
 * This is just a helper function to update the hardcore status file.
 * This means truncating the file and rewriting the two lines:
 *	- "dc":100 -- Death Count line
 *  - "cp": "c1a1" -- Checkpoint line
 *
 * It's private because it will only be called from a player object. Shouldn't be visible to the outside.
 */
void HardCoreStatus::UpdateHardCoreStatusFile()
{
	ALERT(at_console, "Updating HardCore Status file...\n");

	// Open the file as an outputstream, this will create it if it doesn't exist yet
	std::ofstream outFile("status.hlhc");

	// Write death count line
	outFile << DEATH_COUNT_STATUS_TEXT << DELIMITER << hcData.deathCount << std::endl;

	// Write checkpoint line
	outFile << CHECKPOINT_STATUS_TEXT << DELIMITER << hcData.lastCheckpoint << std::endl;

	outFile.close();

	ALERT(at_console, "HardCore Status file updated.\n");
}

/*
 * This function loads the current hardcore status from the "status.hlhc file".
 * If this file doesn't exist it will create it and populate with default values:
 *	- Checkpoint: c1a1 -- Basically the starting point of the mod
 *	- Death Count: 0 -- The player hasn't died yet or at least that we know of
 */
void HardCoreStatus::LoadHardCoreStatus()
{

	// If we already loaded the status we don't do it again
	if (hcData.hardcoreStatusLoaded)
	{
		ALERT(at_console, "HardCore Status is already loaded!\n");
		return;
	}

	// read the file as inputStream#
	// FIXME This is hardcoded!!! Should be configurable.
	std::ifstream inFile("status.hlhc");

	// File doesn't exist
	if (inFile.fail())
	{
		ALERT(at_console, "HardCore Status file not found, creating!\n");

		// File doesn't exist so we create it
		// Write 0 death count since we have no historic death data
		// Write MOD_STARTING_LEVEL as the checkpoint since this is the starting point of the mod
		// These are the default values:
		//		- int deathCount = 0;
		//		- std::string lastCheckpoint = MOD_STARTING_LEVEL;
		UpdateHardCoreStatusFile();
	}
	else
	{
		ALERT(at_console, "HardCore Status file found, loading...\n");

		// File exists so we read and process it line-by-line
		std::string fileLine;
		while (inFile >> fileLine)
		{
			// File structure is "<key>:<value>" so we split the file line to <key> and <value>
			std::string key = fileLine.substr(0, fileLine.find(DELIMITER));
			std::string value = fileLine.substr(fileLine.find(DELIMITER) + 1, fileLine.length());

			// We log a few debug messages just in case
			std::string keyDebugMsg = "Found key: " + key + "\n";
			std::string valueDebugMsg = "Value key: " + value + "\n";

			ALERT(at_console, keyDebugMsg.c_str());
			ALERT(at_console, valueDebugMsg.c_str());

			if (key == DEATH_COUNT_STATUS_TEXT)
			{
				// If the key is "dc" then we know it is the death count number
				std::string deathCountDebugMsg = "Setting death count to: " + value + "\n";
				ALERT(at_console, deathCountDebugMsg.c_str());

				// Set the death count to the one from the file
				hcData.deathCount = atoi(value.c_str());
			}
			else if (key == CHECKPOINT_STATUS_TEXT)
			{
				// If the key is "cp" then we know it is the checkpoint
				std::string checkpointDebugMsg = "Setting last checkpoint to: " + value + "\n";
				ALERT(at_console, checkpointDebugMsg.c_str());

				// Set the checkpoint to the one from the file
				hcData.lastCheckpoint = value;
			}
			else
			{
				// Currently we only have "cp" and "dc" keys in the file so if we find anything else we just log it as an error
				std::string errorMsg = "Unknown key in status file: " + key + "\n";
				ALERT(at_console, errorMsg.c_str());
			}
		}

		ALERT(at_console, "HardCore Status file loaded.\n");

		// Close the input stream
		inFile.close();
	}

	// We indicate that the hardcore status file was loaded, no need to load it again
	hcData.hardcoreStatusLoaded = true;
}

/*
 * This function will do everything we need to do when the player dies:
 * - Increase death count
 * - Reset isDeathCountDisplayed
 * - Reset isWelcomeMessageDisplayed
 */
void HardCoreStatus::Death()
{
	ALERT(at_console, "Player died, increasing Death Count and updating file.\n");

	hcData.deathCount++;
	hcData.isDeathCountDisplayed = false;
	hcData.isWelcomeMessageDisplayed = false;
	UpdateHardCoreStatusFile();
	LoadCheckPoint();

	// If we die we need to re-give the items
	hcData.hasCheckpointItems = false;
}

void HardCoreStatus::GiveCheckpointItemsIfNeeded(CBasePlayer* plr)
{
	if (hcData.hasCheckpointItems)
	{
		// No need to give items if player already has them
		return;
	}

	// Get the items associated with the checkpoint
	std::vector<std::string> itemsToGive = checkpointMap[hcData.lastCheckpoint].second;

	// Check if we have no items
	if (itemsToGive.empty())
	{
		// Throw error if there are no items to give
		std::string errorMsg = "No items to give at checkpoint: " + hcData.lastCheckpoint + "\n";
		ALERT(at_error, errorMsg.c_str());
		return;
	}

	// Iterate through the items and give them to the player
	for (const std::string& item : itemsToGive)
	{
		const std::string itemMsg = "Giving item: " + item + "\n";
		ALERT(at_console, itemMsg.c_str());
		SERVER_COMMAND(UTIL_VarArgs("give \"%s\"\n", item.c_str()));
	}

	// Set the has items flag to true
	hcData.hasCheckpointItems = true;
}

// TODO: This function is very similar to the status load, we should have a common function for file opening/reading
void HardCoreStatus::LoadHardCoreConfig()
{
	// If we already loaded the config we don't do it again
	if (hcData.hardcoreConfigLoaded)
	{
		ALERT(at_console, "HardCore Config is already loaded!\n");
		return;
	}

	// read the file as inputStream
	// FIXME This is hardcoded!!! Should be configurable.
	std::ifstream inFile("config.hlhc");

	// File doesn't exist
	if (inFile.fail())
	{
		ALERT(at_console, "HardCore Config file not found, creating!\n");

		// Open the file as an outputstream, this will create it if it doesn't exist yet
		std::ofstream outFile("config.hlhc");

		// Set everything to TRUE_HARDCORE if no config is present
		outFile << CKP_DIFF << DELIMITER << "TRUE_HARDCORE" << std::endl;
		outFile << AMMO_DIFF << DELIMITER << "TRUE_HARDCORE" << std::endl;
		outFile << ENEMY_DIFF << DELIMITER << "TRUE_HARDCORE" << std::endl;

		outFile.close();

		ALERT(at_console, "HardCore Config file updated.\n");
	}
	else
	{
		ALERT(at_console, "HardCore Config file found, loading...\n");

		// File exists so we read and process it line-by-line
		std::string fileLine;
		while (inFile >> fileLine)
		{
			// File structure is "<key>:<value>" so we split the file line to <key> and <value>
			std::string key = fileLine.substr(0, fileLine.find(DELIMITER));
			std::string value = fileLine.substr(fileLine.find(DELIMITER) + 1, fileLine.length());

			// We log a few debug messages just in case
			std::string keyDebugMsg = "Found key: " + key + "\n";
			std::string valueDebugMsg = "Value key: " + value + "\n";

			ALERT(at_console, keyDebugMsg.c_str());
			ALERT(at_console, valueDebugMsg.c_str());

			if (key == CKP_DIFF)
			{
				std::string deathCountDebugMsg = "Setting Checkpoint Difficulty to: " + value + "\n";
				ALERT(at_console, deathCountDebugMsg.c_str());

				// Set the Checkpoint Difficulty to the one from the file
				hcConfig.checkpointDifficulty = difficultyRepresentation[value];
			}
			else if (key == AMMO_DIFF)
			{
				std::string checkpointDebugMsg = "Setting Ammo Difficulty to: " + value + "\n";
				ALERT(at_console, checkpointDebugMsg.c_str());

				// Set the Ammo Difficulty to the one from the file
				hcConfig.ammoDifficulty = difficultyRepresentation[value];
			}
			else if (key == ENEMY_DIFF)
			{
				std::string checkpointDebugMsg = "Setting Enemy Difficulty to: " + value + "\n";
				ALERT(at_console, checkpointDebugMsg.c_str());

				// Set the Enemy Difficulty to the one from the file
				hcConfig.enemyDifficulty = difficultyRepresentation[value];
			}
			else
			{
				std::string errorMsg = "Unknown key in config file: " + key + "\n";
				ALERT(at_console, errorMsg.c_str());
			}
		}

		ALERT(at_console, "HardCore Config file loaded.\n");

		// Close the input stream
		inFile.close();
	}

	// We indicate that the hardcore status file was loaded, no need to load it again
	hcData.hardcoreConfigLoaded = true;
}

// Set the defaults for the hardcore data
HardCoreStatusData HardCoreStatus::hcData = {
	0,
	MOD_STARTING_LEVEL,
	false,
	false,
	false,
	false
};

// Stores the checkpoint name (map) and the difficulty and items associated with it.
// For Example:
// Blast Pit -> Pair(MILD_CHALLENGE, {crowbar, suit, shotgun, glock})
std::map<std::string, std::pair<Difficulty, std::vector<std::string>>> HardCoreStatus::checkpointMap = {

	{
		// Resonance Cascade available on all difficulties
		// For the first checkpoint we only need the suit
		"c1a1",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::TRUE_HARDCORE, 
			{"item_suit"}
		)
	},

	{
		// Blast Pit available on MILD_CHALLENGE
		"c1a4",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::MILD_CHALLENGE, 
			{"item_suit"}
		)
	},
	{
		// ON A RAIL available on MILD_CHALLENGE
		"c2a2",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::MILD_CHALLENGE, 
			{"item_suit"}
		)
	},
	{
		// Power Up available on WALK_IN_THE_PARK
		"c2a1",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::WALK_IN_THE_PARK, 
			{"item_suit"}
		)
	},
	{
		// Apprehension available on HARD
		"c2a3",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::HARD, 
			{"item_suit"}
		)
	},
	{
		// Surface Tension available on MILD_CHALLENGE
		"c2a5",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::MILD_CHALLENGE, 
			{"item_suit"}
		)
	},
	{
		// Lambda Core available on WALK_IN_THE_PARK
		"c3a2e", 
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::HARD,
			{"item_suit"}
		)
	},
	{
		// Xen available on HARD
		"c4a1",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::HARD, 
			{"item_suit"}
		)
	},
	{
		// Interloper available on MILD_CHALLENGE
		"c4a1a",
		std::pair<Difficulty, std::vector<std::string>>(Difficulty::MILD_CHALLENGE, 
			{"item_suit"}
		)
	}
};

// By default it's true hardcore
HardCoreConfig HardCoreStatus::hcConfig = {
	TRUE_HARDCORE,
	TRUE_HARDCORE,
	TRUE_HARDCORE
};

// This is only used for mapping
std::map<std::string, Difficulty> HardCoreStatus::difficultyRepresentation = {
	{
		"WALK_IN_THE_PARK",
		Difficulty::WALK_IN_THE_PARK
	},
	{
		"MILD_CHALLENGE",
		Difficulty::MILD_CHALLENGE
	},
	{
		"HARD",
		Difficulty::HARD
	},
	{
		"TRUE_HARDCORE",
		Difficulty::TRUE_HARDCORE
	}
};