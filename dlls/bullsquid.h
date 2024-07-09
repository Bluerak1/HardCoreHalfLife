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
#include <time.h>
#include <string>

class CBullsquid : public CBaseMonster
{
public:
	void Spawn() override;
	void Precache() override;
	void SetYawSpeed() override;
	int ISoundMask() override;
	int Classify() override;
	void HandleAnimEvent(MonsterEvent_t* pEvent) override;
	void IdleSound() override;
	void PainSound() override;
	void DeathSound() override;
	void AlertSound() override;
	void AttackSound();
	void StartTask(Task_t* pTask) override;
	void RunTask(Task_t* pTask) override;
	bool CheckMeleeAttack1(float flDot, float flDist) override;
	bool CheckMeleeAttack2(float flDot, float flDist) override;
	bool CheckRangeAttack1(float flDot, float flDist) override;
	void RunAI() override;
	bool FValidateHintType(short sHint) override;
	Schedule_t* GetSchedule() override;
	Schedule_t* GetScheduleOfType(int Type) override;
	bool TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) override;
	int IRelationship(CBaseEntity* pTarget) override;
	int IgnoreConditions() override;
	MONSTERSTATE GetIdealState() override;

	bool Save(CSave& save) override;
	bool Restore(CRestore& restore) override;

	CUSTOM_SCHEDULES;
	static TYPEDESCRIPTION m_SaveData[];

	bool m_fCanThreatDisplay; // this is so the squid only does the "I see a headcrab!" dance one time.

	float m_flLastHurtTime; // we keep track of this, because if something hurts a squid, it will forget about its love of headcrabs for a while.
	float m_flNextSpitTime; // last time the bullsquid used the spit attack.
};