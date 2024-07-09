#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "schedule.h"
#include "nodes.h"
#include "effects.h"
#include "decals.h"
#include "soundent.h"
#include "game.h"
#include <string>
#include <vector>

class CBaseJackInTheBox : public CBaseMonster
{
	protected:
		void SpawnRandomEnemy(std::vector<std::string>& monsters)
		{
			int randNum = RANDOM_LONG(0, monsters.size() - 1);

			edict_t* pent;
			entvars_t* pevCreate;

			string_t monsterName = ALLOC_STRING(monsters[randNum].c_str());

			pent = CREATE_NAMED_ENTITY(monsterName);

			if (FNullEnt(pent))
			{
				ALERT(at_console, "NULL Ent in MonsterMaker!\n");
				return;
			}

			if (!FStringNull(pev->target))
			{
				FireTargets(STRING(pev->target), this, this, USE_TOGGLE, 0);
			}

			pevCreate = VARS(pent);
			pevCreate->origin = pev->origin;
			pevCreate->angles = pev->angles;
			SetBits(pevCreate->spawnflags, SF_MONSTER_FALL_TO_GROUND);

			DispatchSpawn(ENT(pevCreate));
			pevCreate->owner = this->edict();

			if (!FStringNull(pev->netname))
			{
				pevCreate->targetname = pev->netname;
			}
		}
};

class CJackInTheBox : public CBaseJackInTheBox
{
	public:
		void Spawn() override
		{
			CBaseMonster::Precache();

			for (const std::string& monster : monsters)
			{
				UTIL_PrecacheOther(monster.c_str());
			}

			float randFloat = RANDOM_FLOAT(0, 1);

			if (randFloat < 0.50)
			{
				ALERT(at_console, "Spawning random enemy!\n");
				SpawnRandomEnemy(monsters);
			}
		}

	private:
		std::vector<std::string> monsters = {
			"monster_alien_slave",
			"monster_bullchicken",
			"monster_headcrab",
			"monster_houndeye",
			"monster_zombie",
		};
};

class CJackInTheBoxAdvanced : public CBaseJackInTheBox
{
	public:
		void Spawn() override
		{
			CBaseMonster::Precache();

			for (const std::string& monster : monsters)
			{
				UTIL_PrecacheOther(monster.c_str());
			}

			float randFloat = RANDOM_FLOAT(0, 1);

			if (randFloat < 0.50)
			{
				ALERT(at_console, "Spawning random enemy!\n");
				SpawnRandomEnemy(monsters);
			}
		}

	private:
		std::vector<std::string> monsters = {
			"monster_alien_slave",
			"monster_bullchicken",
			"monster_headcrab",
			"monster_houndeye",
			"monster_zombie",
			"monster_human_grunt"
		};
};

class CJackInTheBoxAdvancedWithAGrunt : public CBaseJackInTheBox
{
	public:
		void Spawn() override
		{
			CBaseMonster::Precache();

			for (const std::string& monster : monsters)
			{
				UTIL_PrecacheOther(monster.c_str());
			}

			float randFloat = RANDOM_FLOAT(0, 1);

			if (randFloat < 0.50)
			{
				ALERT(at_console, "Spawning random enemy!\n");
				SpawnRandomEnemy(monsters);
			}
		}

	private:
		std::vector<std::string> monsters = {
			"monster_alien_slave",
			"monster_bullchicken",
			"monster_headcrab",
			"monster_houndeye",
			"monster_zombie",
			"monster_human_grunt"
			"monster_alien_grunt"
		};
};

class CJackInTheBoxAdvancedWithAGruntAndAssassins : public CBaseJackInTheBox
{
	public:
		void Spawn() override
		{
			CBaseMonster::Precache();

			for (const std::string& monster : monsters)
			{
				UTIL_PrecacheOther(monster.c_str());
			}

			float randFloat = RANDOM_FLOAT(0, 1);

			if (randFloat < 0.50)
			{
				ALERT(at_console, "Spawning random enemy!\n");
				SpawnRandomEnemy(monsters);
			}
		}

	private:
		std::vector<std::string> monsters = {
			"monster_alien_slave",
			"monster_bullchicken",
			"monster_headcrab",
			"monster_houndeye",
			"monster_zombie",
			"monster_human_grunt",
			"monster_alien_grunt",
			"monster_human_assassin"
		};
};


LINK_ENTITY_TO_CLASS(monster_jackinthebox, CJackInTheBox);
LINK_ENTITY_TO_CLASS(monster_jackinthebox_advanced, CJackInTheBoxAdvanced);
LINK_ENTITY_TO_CLASS(monster_jackinthebox_advanced_agrunt, CJackInTheBoxAdvancedWithAGrunt);
LINK_ENTITY_TO_CLASS(monster_jackinthebox_advanced_agrunt_assassins, CJackInTheBoxAdvancedWithAGruntAndAssassins);
