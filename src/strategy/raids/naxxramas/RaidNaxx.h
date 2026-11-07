#ifndef _SHADOW_RAIDNAXX_H
#define _SHADOW_RAIDNAXX_H

#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_anubrekhan.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_faerlina.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_four_horsemen.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_gluth.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_gothik.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_grobbulus.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_heigan.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_kelthuzad.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_loatheb.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_maexxna.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_noth.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_patchwerk.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_razuvious.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_sapphiron.h"
#include "../../../../src/server/scripts/Northrend/Naxxramas/boss_thaddius.h"
#include <string>
#include "AiObject.h"
#include "AiObjectContext.h"
#include "EventMap.h"
#include "Log.h"
#include "NamedObjectContext.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "ScriptedCreature.h"
#include "SharedDefines.h"
#include "GenericTriggers.h"
#include "ShadowAIConfig.h"
#include "Trigger.h"
#include "Action.h"
#include "AttackAction.h"
#include "GenericActions.h"
#include "MovementActions.h"
#include "Multiplier.h"
#include "Strategy.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const uint32 NAXX_MAP_ID = 533;

template <class BossAiType>
class GenericBossHelper : public AiObject
{
public:
    GenericBossHelper(ShadowAI* botAI, std::string name) : AiObject(botAI), _name(name) {}
    virtual bool UpdateBossAI()
    {
        if (!bot->IsInCombat())
        {
            _unit = nullptr;
        }
        if (_unit && (!_unit->IsInWorld() || !_unit->IsAlive()))
        {
            _unit = nullptr;
        }
        if (!_unit)
        {
            _unit = AI_VALUE2(Unit*, "find target", _name);
            if (!_unit)
            {
                return false;
            }
            _target = _unit->ToCreature();
            if (!_target)
            {
                return false;
            }
            _ai = dynamic_cast<BossAiType*>(_target->GetAI());
            if (!_ai)
            {
                return false;
            }
            _event_map = &_ai->events;
            if (!_event_map)
            {
                return false;
            }
        }
        if (!_event_map)
        {
            return false;
        }
        _timer = _event_map->GetTimer();
        return true;
    }
    virtual void Reset()
    {
        _unit = nullptr;
        _target = nullptr;
        _ai = nullptr;
        _event_map = nullptr;
        _timer = 0;
    }

protected:
    std::string _name;
    Unit* _unit = nullptr;
    Creature* _target = nullptr;
    BossAiType* _ai = nullptr;
    EventMap* _event_map = nullptr;
    uint32 _timer = 0;
};

class KelthuzadBossHelper : public GenericBossHelper<Kelthuzad::boss_kelthuzad::boss_kelthuzadAI>
{
public:
    KelthuzadBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "kel'thuzad") {}
    const std::pair<float, float> center = {3716.19f, -5106.58f};
    const std::pair<float, float> tank_pos = {3709.19f, -5104.86f};
    const std::pair<float, float> assist_tank_pos = {3746.05f, -5112.74f};
    bool IsPhaseOne() { return _event_map->GetNextEventTime(Kelthuzad::EVENT_PHASE_2) != 0; }
    bool IsPhaseTwo() { return !IsPhaseOne(); }
    Unit* GetAnyShadowFissure()
    {
        Unit* shadow_fissure = nullptr;
        GuidVector units = *context->GetValue<GuidVector>("nearest triggers");
        for (auto i = units.begin(); i != units.end(); i++)
        {
            Unit* unit = botAI->GetUnit(*i);
            if (!unit)
                continue;
            if (botAI->EqualLowercaseName(unit->GetName(), "shadow fissure"))
            {
                shadow_fissure = unit;
            }
        }
        return shadow_fissure;
    }
};

class RazuviousBossHelper : public GenericBossHelper<Razuvious::boss_razuvious::boss_razuviousAI>
{
public:
    RazuviousBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "instructor razuvious") {}
};

class SapphironBossHelper : public GenericBossHelper<Sapphiron::boss_sapphiron::boss_sapphironAI>
{
public:
    const std::pair<float, float> mainTankPos = {3512.07f, -5274.06f};
    const std::pair<float, float> center = {3517.31f, -5253.74f};
    const float GENERIC_HEIGHT = 137.29f;
    SapphironBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "sapphiron") {}
    bool UpdateBossAI() override
    {
        if (!GenericBossHelper::UpdateBossAI())
        {
            return false;
        }
        uint32 nextEventGround = _event_map->GetNextEventTime(Sapphiron::EVENT_GROUND);
        if (nextEventGround && nextEventGround != lastEventGround)
            lastEventGround = nextEventGround;
        return true;
    }
    bool IsPhaseGround() { return _target->GetReactState() == REACT_AGGRESSIVE; }
    bool IsPhaseFlight() { return !IsPhaseGround(); }
    bool JustLanded()
    {
        return (_event_map->GetNextEventTime(Sapphiron::EVENT_FLIGHT_START) - _timer) >=
               EVENT_FLIGHT_INTERVAL - POSITION_TIME_AFTER_LANDED;
    }
    bool WaitForExplosion() { return _event_map->GetNextEventTime(Sapphiron::EVENT_FLIGHT_SPELL_EXPLOSION); }
    bool FindPosToAvoidChill(std::vector<float>& dest)
    {
        Aura* aura = botAI->GetAura("chill", bot);
        if (!aura)
        {
            return false;
        }
        DynamicObject* dyn_obj = aura->GetDynobjOwner();
        if (!dyn_obj)
        {
            return false;
        }
        Unit* currentTarget = AI_VALUE(Unit*, "current target");
        float angle = 0;
        uint32 index = botAI->GetGroupSlotIndex(bot);
        if (currentTarget)
        {
            if (botAI->IsRanged(bot))
            {
                if (bot->GetExactDist2d(currentTarget) <= 45.0f)
                {
                    angle = bot->GetAngle(dyn_obj) - M_PI + (rand_norm() - 0.5) * M_PI / 2;
                }
                else
                {
                    if (index % 2 == 0)
                    {
                        angle = bot->GetAngle(currentTarget) + M_PI / 2;
                    }
                    else
                    {
                        angle = bot->GetAngle(currentTarget) - M_PI / 2;
                    }
                }
            }
            else
            {
                if (index % 3 == 0)
                {
                    angle = bot->GetAngle(currentTarget);
                }
                else if (index % 3 == 1)
                {
                    angle = bot->GetAngle(currentTarget) + M_PI / 2;
                }
                else
                {
                    angle = bot->GetAngle(currentTarget) - M_PI / 2;
                }
            }
        }
        else
        {
            angle = bot->GetAngle(dyn_obj) - M_PI + (rand_norm() - 0.5) * M_PI / 2;
        }
        dest = {bot->GetPositionX() + cos(angle) * 5.0f, bot->GetPositionY() + sin(angle) * 5.0f, bot->GetPositionZ()};
        return true;
    }

private:
    const uint32 POSITION_TIME_AFTER_LANDED = 5000;
    const uint32 EVENT_FLIGHT_INTERVAL = 45000;
    uint32 lastEventGround = 0;
};

class GluthBossHelper : public GenericBossHelper<Gluth::boss_gluth::boss_gluthAI>
{
public:
    const std::pair<float, float> mainTankPos25 = {3331.48f, -3109.06f};
    const std::pair<float, float> mainTankPos10 = {3278.29f, -3162.06f};
    const std::pair<float, float> beforeDecimatePos = {3267.34f, -3175.68f};
    const std::pair<float, float> leftSlowDownPos = {3290.68f, -3141.65f};
    const std::pair<float, float> rightSlowDownPos = {3300.78f, -3151.98f};
    const std::pair<float, float> rangedPos = {3301.45f, -3139.29f};
    const std::pair<float, float> healPos = {3303.09f, -3135.24f};

    const float decimatedZombiePct = 10.0f;
    GluthBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "gluth") {}
    bool BeforeDecimate()
    {
        uint32 decimate = _event_map->GetNextEventTime(Gluth::EVENT_DECIMATE);
        return decimate && decimate - _timer <= 3000;
    }
    bool JustStartCombat() { return _timer < 10000; }
};

class LoathebBossHelper : public GenericBossHelper<Loatheb::boss_loatheb::boss_loathebAI>
{
public:
    const std::pair<float, float> mainTankPos = {2877.57f, -3967.00f};
    const std::pair<float, float> rangePos = {2896.96f, -3980.61f};
    LoathebBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "loatheb") {}
};

class FourhorsemanBossHelper : public GenericBossHelper<FourHorsemen::boss_four_horsemen::boss_four_horsemenAI>
{
public:
    const float posZ = 241.27f;
    const std::pair<float, float> attractPos[2] = {{2502.03f, -2910.90f},
                                                   {2484.61f, -2947.07f}};  // left (sir zeliek), right (lady blaumeux)
    FourhorsemanBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "sir zeliek") {}
    bool UpdateBossAI() override
    {
        if (!GenericBossHelper::UpdateBossAI())
        {
            return false;
        }
        if (!bot->IsInCombat())
        {
            Reset();
        }
        sir = _unit;
        lady = AI_VALUE2(Unit*, "find target", "lady blaumeux");
        if (!lady)
        {
            return true;
        }
        ladyAI = dynamic_cast<FourHorsemen::boss_four_horsemen::boss_four_horsemenAI*>(lady->GetAI());
        if (!ladyAI)
        {
            return true;
        }
        ladyEvent = &ladyAI->events;
        const uint32 voidZone = ladyEvent->GetNextEventTime(FourHorsemen::EVENT_SECONDARY_SPELL);
        if (voidZone && lastEventVoidZone != voidZone)
        {
            voidZoneCounter++;
            voidZoneCounter %= 8;
            lastEventVoidZone = voidZone;
        }
        return true;
    }
    void Reset() override
    {
        GenericBossHelper::Reset();
        sir = nullptr;
        lady = nullptr;
        ladyAI = nullptr;
        ladyEvent = nullptr;
        lastEventVoidZone = 0;
        voidZoneCounter = 0;
        posToGo = 0;
    }
    bool IsAttracter(Player* bot)
    {
        Difficulty diff = bot->GetRaidDifficulty();
        if (diff == RAID_DIFFICULTY_25MAN_NORMAL)
        {
            return botAI->IsRangedDpsAssistantOfIndex(bot, 0) || botAI->IsHealAssistantOfIndex(bot, 0) ||
                   botAI->IsHealAssistantOfIndex(bot, 1) || botAI->IsHealAssistantOfIndex(bot, 2);
        }
        return botAI->IsRangedDpsAssistantOfIndex(bot, 0) || botAI->IsHealAssistantOfIndex(bot, 0);
    }
    void CalculatePosToGo(Player* bot)
    {
        bool raid25 = bot->GetRaidDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL;
        if (!lady)
        {
            posToGo = 0;
        }
        else
        {
            // Interval: 24s - 15s - 15s - ...
            posToGo = !(_timer <= 9000 || ((_timer - 9000) / 67500) % 2 == 0);
            if (botAI->IsRangedDpsAssistantOfIndex(bot, 0) || (raid25 && botAI->IsHealAssistantOfIndex(bot, 1)))
            {
                posToGo = 1 - posToGo;
            }
        }
    }
    std::pair<float, float> CurrentAttractPos()
    {
        float posX = attractPos[posToGo].first, posY = attractPos[posToGo].second;
        if (posToGo == 1)
        {
            float offset_x;
            float offset_y;
            if (voidZoneCounter < 4)
            {
                offset_x = voidZoneCounter * (-4.5f);
                offset_y = voidZoneCounter * (4.5f);
            }
            if (voidZoneCounter >= 4)
            {
                offset_x = (7 - voidZoneCounter) * (-4.5f);
                offset_y = (7 - voidZoneCounter) * (4.5f);
                offset_x += 4.5f;
                offset_y += 4.5f;
            }
            posX += offset_x;
            posY += offset_y;
        }
        return {posX, posY};
    }
    Unit* CurrentAttackTarget()
    {
        if (posToGo == 0)
        {
            return sir;
        }
        return lady;
    }

protected:
    Unit* sir = nullptr;
    Unit* lady = nullptr;
    FourHorsemen::boss_four_horsemen::boss_four_horsemenAI* ladyAI = nullptr;
    EventMap* ladyEvent = nullptr;
    uint32 lastEventVoidZone = 0;
    uint32 voidZoneCounter = 0;
    int posToGo = 0;
};
class ThaddiusBossHelper : public GenericBossHelper<Thaddius::boss_thaddius::boss_thaddiusAI>
{
public:
    const std::pair<float, float> tankPosFeugen = {3522.94f, -3002.60f};
    const std::pair<float, float> tankPosStalagg = {3436.14f, -2919.98f};
    const std::pair<float, float> rangedPosFeugen = {3500.45f, -2997.92f};
    const std::pair<float, float> rangedPosStalagg = {3441.01f, -2942.04f};
    const float tankPosZ = 312.61f;
    ThaddiusBossHelper(ShadowAI* botAI) : GenericBossHelper(botAI, "thaddius") {}
    bool UpdateBossAI() override
    {
        if (!GenericBossHelper::UpdateBossAI())
        {
            return false;
        }
        feugen = AI_VALUE2(Unit*, "find target", "feugen");
        stalagg = AI_VALUE2(Unit*, "find target", "stalagg");
        return true;
    }
    bool IsPhasePet() { return (feugen && feugen->IsAlive()) || (stalagg && stalagg->IsAlive()); }
    bool IsPhaseTransition()
    {
        if (IsPhasePet())
        {
            return false;
        }
        return _unit->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
    }
    bool IsPhaseThaddius() { return !IsPhasePet() && !IsPhaseTransition(); }
    Unit* GetNearestPet()
    {
        Unit* unit = nullptr;
        if (feugen && feugen->IsAlive())
        {
            unit = feugen;
        }
        if (stalagg && stalagg->IsAlive() && (!feugen || bot->GetDistance(stalagg) < bot->GetDistance(feugen)))
        {
            unit = stalagg;
        }
        return unit;
    }
    std::pair<float, float> PetPhaseGetPosForTank()
    {
        if (GetNearestPet() == feugen)
        {
            return tankPosFeugen;
        }
        return tankPosStalagg;
    }
    std::pair<float, float> PetPhaseGetPosForRanged()
    {
        if (GetNearestPet() == feugen)
        {
            return rangedPosFeugen;
        }
        return rangedPosStalagg;
    }

protected:
    Unit* feugen = nullptr;
    Unit* stalagg = nullptr;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class MutatingInjectionTrigger : public HasAuraTrigger
{
public:
    MutatingInjectionTrigger(ShadowAI* ai) : HasAuraTrigger(ai, "mutating injection", 1) {}
};

class AuraRemovedTrigger : public Trigger
{
public:
    AuraRemovedTrigger(ShadowAI* botAI, std::string name) : Trigger(botAI, name, 1) { this->prev_check = false; }
    virtual bool IsActive() override;

protected:
    bool prev_check;
};

class MutatingInjectionRemovedTrigger : public HasNoAuraTrigger
{
public:
    MutatingInjectionRemovedTrigger(ShadowAI* ai) : HasNoAuraTrigger(ai, "mutating injection") {}
    virtual bool IsActive();
};

template <class T>
class BossEventTrigger : public Trigger
{
public:
    BossEventTrigger(ShadowAI* ai, uint32 boss_entry, uint32 event_id, std::string name = "boss event")
        : Trigger(ai, name, 1)
    {
        this->boss_entry = boss_entry;
        this->event_id = event_id;
        this->last_event_time = -1;
    }
    virtual bool IsActive();

protected:
    uint32 boss_entry, event_id, last_event_time;
};

class GrobbulusCloudTrigger : public BossEventTrigger<Grobbulus::boss_grobbulus::boss_grobbulusAI>
{
public:
    GrobbulusCloudTrigger(ShadowAI* ai) : BossEventTrigger(ai, 15931, 2, "grobbulus cloud event") {}
    virtual bool IsActive();
};

class HeiganMeleeTrigger : public Trigger
{
public:
    HeiganMeleeTrigger(ShadowAI* ai) : Trigger(ai, "heigan melee") {}
    virtual bool IsActive();
};

class HeiganRangedTrigger : public Trigger
{
public:
    HeiganRangedTrigger(ShadowAI* ai) : Trigger(ai, "heigan ranged") {}
    bool IsActive() override;
};

class RazuviousTankTrigger : public Trigger
{
public:
    RazuviousTankTrigger(ShadowAI* ai) : Trigger(ai, "instructor razuvious tank"), helper(ai) {}
    bool IsActive() override;

private:
    RazuviousBossHelper helper;
};

class RazuviousNontankTrigger : public Trigger
{
public:
    RazuviousNontankTrigger(ShadowAI* ai) : Trigger(ai, "instructor razuvious non-tank"), helper(ai) {}
    bool IsActive() override;

private:
    RazuviousBossHelper helper;
};

class KelthuzadTrigger : public Trigger
{
public:
    KelthuzadTrigger(ShadowAI* ai) : Trigger(ai, "kel'thuzad trigger"), helper(ai) {}
    bool IsActive() override;

private:
    KelthuzadBossHelper helper;
};

class AnubrekhanTrigger : public Trigger
{
public:
    AnubrekhanTrigger(ShadowAI* ai) : Trigger(ai, "anub'rekhan") {}
    bool IsActive() override;
};

class ThaddiusPhasePetTrigger : public Trigger
{
public:
    ThaddiusPhasePetTrigger(ShadowAI* ai) : Trigger(ai, "thaddius phase pet"), helper(ai) {}
    bool IsActive() override;

private:
    ThaddiusBossHelper helper;
};

class ThaddiusPhasePetLoseAggroTrigger : public ThaddiusPhasePetTrigger
{
public:
    ThaddiusPhasePetLoseAggroTrigger(ShadowAI* ai) : ThaddiusPhasePetTrigger(ai) {}
    virtual bool IsActive()
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        return ThaddiusPhasePetTrigger::IsActive() && botAI->IsTank(bot) && target && target->GetVictim() != bot;
    }
};

class ThaddiusPhaseTransitionTrigger : public Trigger
{
public:
    ThaddiusPhaseTransitionTrigger(ShadowAI* ai) : Trigger(ai, "thaddius phase transition"), helper(ai) {}
    bool IsActive() override;

private:
    ThaddiusBossHelper helper;
};

class ThaddiusPhaseThaddiusTrigger : public Trigger
{
public:
    ThaddiusPhaseThaddiusTrigger(ShadowAI* ai) : Trigger(ai, "thaddius phase thaddius"), helper(ai) {}
    bool IsActive() override;

private:
    ThaddiusBossHelper helper;
};

class HorsemanAttractorsTrigger : public Trigger
{
public:
    HorsemanAttractorsTrigger(ShadowAI* ai) : Trigger(ai, "fourhorsemen attractors"), helper(ai) {}
    bool IsActive() override;

private:
    FourhorsemanBossHelper helper;
};

class HorsemanExceptAttractorsTrigger : public Trigger
{
public:
    HorsemanExceptAttractorsTrigger(ShadowAI* ai) : Trigger(ai, "fourhorsemen except attractors"), helper(ai) {}
    bool IsActive() override;

private:
    FourhorsemanBossHelper helper;
};

class SapphironGroundTrigger : public Trigger
{
public:
    SapphironGroundTrigger(ShadowAI* ai) : Trigger(ai, "sapphiron ground"), helper(ai) {}
    bool IsActive() override;

private:
    SapphironBossHelper helper;
};


class SapphironFlightTrigger : public Trigger
{
public:
    SapphironFlightTrigger(ShadowAI* ai) : Trigger(ai, "sapphiron flight"), helper(ai) {}
    bool IsActive() override;

private:
    SapphironBossHelper helper;
};

class GluthTrigger : public Trigger
{
public:
    GluthTrigger(ShadowAI* ai) : Trigger(ai, "gluth trigger"), helper(ai) {}
    bool IsActive() override;

private:
    GluthBossHelper helper;
};

class GluthMainTankMortalWoundTrigger : public Trigger
{
public:
    GluthMainTankMortalWoundTrigger(ShadowAI* ai) : Trigger(ai, "gluth main tank mortal wound trigger"), helper(ai)
    {
    }
    bool IsActive() override;

private:
    GluthBossHelper helper;
};

class LoathebTrigger : public Trigger
{
public:
    LoathebTrigger(ShadowAI* ai) : Trigger(ai, "loatheb"), helper(ai) {}
    bool IsActive() override;

private:
    LoathebBossHelper helper;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// just for test
// class TryToGetBossAIAction : public Action
// {
// public:
//     TryToGetBossAIAction(ShadowAI* ai) : Action(ai, "try to get boss ai") {}

// public:
//     virtual bool Execute(Event event);
// };

class GrobbulusGoBehindAction : public MovementAction
{
public:
    GrobbulusGoBehindAction(ShadowAI* ai, float distance = 24.0f, float delta_angle = M_PI / 8)
        : MovementAction(ai, "grobbulus go behind")
    {
        this->distance = distance;
        this->delta_angle = delta_angle;
    }
    virtual bool Execute(Event event);

protected:
    float distance, delta_angle;
};

class GrobbulusRotateAction : public RotateAroundTheCenterPointAction
{
public:
    GrobbulusRotateAction(ShadowAI* botAI)
        : RotateAroundTheCenterPointAction(botAI, "rotate grobbulus", 3281.23f, -3310.38f, 35.0f, 8, true, M_PI)
    {
    }
    virtual bool isUseful() override
    {
        return RotateAroundTheCenterPointAction::isUseful() && botAI->IsMainTank(bot) &&
               AI_VALUE2(bool, "has aggro", "boss target");
    }
    uint32 GetCurrWaypoint() override;
};

class GrobblulusMoveCenterAction : public MoveInsideAction
{
public:
    GrobblulusMoveCenterAction(ShadowAI* ai) : MoveInsideAction(ai, 3281.23f, -3310.38f, 5.0f) {}
};

class HeiganDanceAction : public MovementAction
{
public:
    HeiganDanceAction(ShadowAI* ai) : MovementAction(ai, "heigan dance")
    {
        this->prev_phase = 0;
        this->prev_erupt = 0;
        this->prev_timer = 0;
        ResetSafe();
        waypoints.push_back(std::make_pair(2794.88f, -3668.12f));
        waypoints.push_back(std::make_pair(2775.49f, -3674.43f));
        waypoints.push_back(std::make_pair(2762.30f, -3684.59f));
        waypoints.push_back(std::make_pair(2755.99f, -3703.96f));
    }

protected:
    bool CalculateSafe();
    void ResetSafe()
    {
        curr_safe = 0;
        curr_dir = 1;
    }
    void NextSafe()
    {
        curr_safe += curr_dir;
        if (curr_safe == 3 || curr_safe == 0)
        {
            curr_dir = -curr_dir;
        }
    }
    uint32 prev_phase, prev_erupt, prev_timer;
    uint32 curr_safe, curr_dir;
    std::vector<std::pair<float, float>> waypoints;
};

class HeiganDanceMeleeAction : public HeiganDanceAction
{
public:
    HeiganDanceMeleeAction(ShadowAI* ai) : HeiganDanceAction(ai) {}
    virtual bool Execute(Event event);
};

class HeiganDanceRangedAction : public HeiganDanceAction
{
public:
    HeiganDanceRangedAction(ShadowAI* ai) : HeiganDanceAction(ai) { platform = std::make_pair(2794.26f, -3706.67f); }
    virtual bool Execute(Event event);

protected:
    std::pair<float, float> platform;
};

class ThaddiusAttackNearestPetAction : public AttackAction
{
public:
    ThaddiusAttackNearestPetAction(ShadowAI* ai) : AttackAction(ai, "thaddius attack nearest pet"), helper(ai) {}
    virtual bool Execute(Event event);
    virtual bool isUseful();

private:
    ThaddiusBossHelper helper;
};

// class ThaddiusMeleeToPlaceAction : public MovementAction
// {
// public:
//     ThaddiusMeleeToPlaceAction(ShadowAI* ai) : MovementAction(ai, "thaddius melee to place") {}
//     virtual bool Execute(Event event);
//     virtual bool isUseful();
// };

// class ThaddiusRangedToPlaceAction : public MovementAction
// {
// public:
//     ThaddiusRangedToPlaceAction(ShadowAI* ai) : MovementAction(ai, "thaddius ranged to place") {}
//     virtual bool Execute(Event event);
//     virtual bool isUseful();
// };

class ThaddiusMoveToPlatformAction : public MovementAction
{
public:
    ThaddiusMoveToPlatformAction(ShadowAI* ai) : MovementAction(ai, "thaddius move to platform") {}
    virtual bool Execute(Event event);
    virtual bool isUseful();
};

class ThaddiusMovePolarityAction : public MovementAction
{
public:
    ThaddiusMovePolarityAction(ShadowAI* ai) : MovementAction(ai, "thaddius move polarity") {}
    virtual bool Execute(Event event);
    virtual bool isUseful();
};

class RazuviousUseObedienceCrystalAction : public MovementAction
{
public:
    RazuviousUseObedienceCrystalAction(ShadowAI* ai)
        : MovementAction(ai, "razuvious use obedience crystal"), helper(ai)
    {
    }
    bool Execute(Event event) override;

private:
    RazuviousBossHelper helper;
};

class RazuviousTargetAction : public AttackAction
{
public:
    RazuviousTargetAction(ShadowAI* ai) : AttackAction(ai, "razuvious target"), helper(ai) {}
    bool Execute(Event event) override;

private:
    RazuviousBossHelper helper;
};

class HorsemanAttractAlternativelyAction : public AttackAction
{
public:
    HorsemanAttractAlternativelyAction(ShadowAI* ai) : AttackAction(ai, "horseman attract alternatively"), helper(ai)
    {
    }
    bool Execute(Event event) override;

protected:
    FourhorsemanBossHelper helper;
};

class HorsemanAttactInOrderAction : public AttackAction
{
public:
    HorsemanAttactInOrderAction(ShadowAI* ai) : AttackAction(ai, "horseman attact in order"), helper(ai) {}
    bool Execute(Event event) override;

protected:
    FourhorsemanBossHelper helper;
};

// class SapphironGroundMainTankPositionAction : public MovementAction
// {
// public:
//     SapphironGroundMainTankPositionAction(ShadowAI* ai) : MovementAction(ai, "sapphiron ground main tank
//     position") {} virtual bool Execute(Event event);
// };

class SapphironGroundPositionAction : public MovementAction
{
public:
    SapphironGroundPositionAction(ShadowAI* ai) : MovementAction(ai, "sapphiron ground position"), helper(ai) {}
    bool Execute(Event event) override;

protected:
    SapphironBossHelper helper;
};

class SapphironFlightPositionAction : public MovementAction
{
public:
    SapphironFlightPositionAction(ShadowAI* ai) : MovementAction(ai, "sapphiron flight position"), helper(ai) {}
    bool Execute(Event event) override;

protected:
    SapphironBossHelper helper;
    bool MoveToNearestIcebolt();
};

// class SapphironAvoidChillAction : public MovementAction
// {
// public:
//     SapphironAvoidChillAction(ShadowAI* ai) : MovementAction(ai, "sapphiron avoid chill") {}
//     virtual bool Execute(Event event);
// };

class KelthuzadChooseTargetAction : public AttackAction
{
public:
    KelthuzadChooseTargetAction(ShadowAI* ai) : AttackAction(ai, "kel'thuzad choose target"), helper(ai) {}
    virtual bool Execute(Event event);

private:
    KelthuzadBossHelper helper;
};

class KelthuzadPositionAction : public MovementAction
{
public:
    KelthuzadPositionAction(ShadowAI* ai) : MovementAction(ai, "kel'thuzad position"), helper(ai) {}
    virtual bool Execute(Event event);

private:
    KelthuzadBossHelper helper;
};

class AnubrekhanChooseTargetAction : public AttackAction
{
public:
    AnubrekhanChooseTargetAction(ShadowAI* ai) : AttackAction(ai, "anub'rekhan choose target") {}
    bool Execute(Event event) override;
};

class AnubrekhanPositionAction : public RotateAroundTheCenterPointAction
{
public:
    AnubrekhanPositionAction(ShadowAI* ai)
        : RotateAroundTheCenterPointAction(ai, "anub'rekhan position", 3272.49f, -3476.27f, 45.0f, 16)
    {
    }
    bool Execute(Event event) override;
};

class GluthChooseTargetAction : public AttackAction
{
public:
    GluthChooseTargetAction(ShadowAI* ai) : AttackAction(ai, "gluth choose target"), helper(ai) {}
    bool Execute(Event event) override;

private:
    GluthBossHelper helper;
};

class GluthPositionAction : public RotateAroundTheCenterPointAction
{
public:
    GluthPositionAction(ShadowAI* ai)
        : RotateAroundTheCenterPointAction(ai, "gluth position", 3293.61f, -3149.01f, 12.0f, 12), helper(ai)
    {
    }
    bool Execute(Event event) override;

private:
    GluthBossHelper helper;
};

class GluthSlowdownAction : public Action
{
public:
    GluthSlowdownAction(ShadowAI* ai) : Action(ai, "gluth slowdown"), helper(ai) {}
    bool Execute(Event event) override;

private:
    GluthBossHelper helper;
};

class LoathebPositionAction : public MovementAction
{
public:
    LoathebPositionAction(ShadowAI* ai) : MovementAction(ai, "loatheb position"), helper(ai) {}
    virtual bool Execute(Event event);

private:
    LoathebBossHelper helper;
};

class LoathebChooseTargetAction : public AttackAction
{
public:
    LoathebChooseTargetAction(ShadowAI* ai) : AttackAction(ai, "loatheb choose target"), helper(ai) {}
    virtual bool Execute(Event event);

private:
    LoathebBossHelper helper;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class GrobbulusMultiplier : public Multiplier
{
public:
    GrobbulusMultiplier(ShadowAI* ai) : Multiplier(ai, "grobbulus") {}

public:
    virtual float GetValue(Action* action);
};
class HeiganDanceMultiplier : public Multiplier
{
public:
    HeiganDanceMultiplier(ShadowAI* ai) : Multiplier(ai, "helgan dance") {}

public:
    virtual float GetValue(Action* action);
};

class LoathebGenericMultiplier : public Multiplier
{
public:
    LoathebGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "loatheb generic") {}

public:
    virtual float GetValue(Action* action);
};

class ThaddiusGenericMultiplier : public Multiplier
{
public:
    ThaddiusGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "thaddius generic"), helper(ai) {}

public:
    virtual float GetValue(Action* action);

private:
    ThaddiusBossHelper helper;
};

class SapphironGenericMultiplier : public Multiplier
{
public:
    SapphironGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "sapphiron generic"), helper(ai) {}

    virtual float GetValue(Action* action);

private:
    SapphironBossHelper helper;
};

class InstructorRazuviousGenericMultiplier : public Multiplier
{
public:
    InstructorRazuviousGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "instructor razuvious generic"), helper(ai)
    {
    }
    virtual float GetValue(Action* action);

private:
    RazuviousBossHelper helper;
};

class KelthuzadGenericMultiplier : public Multiplier
{
public:
    KelthuzadGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "kelthuzad generic"), helper(ai) {}
    virtual float GetValue(Action* action);

private:
    KelthuzadBossHelper helper;
};

class AnubrekhanGenericMultiplier : public Multiplier
{
public:
    AnubrekhanGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "anubrekhan generic") {}

public:
    virtual float GetValue(Action* action);
};

class FourhorsemanGenericMultiplier : public Multiplier
{
public:
    FourhorsemanGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "fourhorseman generic") {}

public:
    virtual float GetValue(Action* action);
};

// class GothikGenericMultiplier : public Multiplier
// {
// public:
//     GothikGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "gothik generic") {}

// public:
//     virtual float GetValue(Action* action);
// };

class GluthGenericMultiplier : public Multiplier
{
public:
    GluthGenericMultiplier(ShadowAI* ai) : Multiplier(ai, "gluth generic"), helper(ai) {}
    float GetValue(Action* action) override;

private:
    GluthBossHelper helper;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidNaxxStrategy : public Strategy
{
public:
    RaidNaxxStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "naxx"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// /*
//  * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
//  and/or modify it under version 2 of the License, or (at your option), any later version.
//  */



class RaidNaxxActionContext : public NamedObjectContext<Action>
{
public:
    RaidNaxxActionContext()
    {
        creators["grobbulus go behind the boss"] = &RaidNaxxActionContext::go_behind_the_boss;
        creators["rotate grobbulus"] = &RaidNaxxActionContext::rotate_grobbulus;
        creators["grobbulus move center"] = &RaidNaxxActionContext::grobbulus_move_center;

        creators["heigan dance melee"] = &RaidNaxxActionContext::heigan_dance_melee;
        creators["heigan dance ranged"] = &RaidNaxxActionContext::heigan_dance_ranged;
        creators["thaddius attack nearest pet"] = &RaidNaxxActionContext::thaddius_attack_nearest_pet;
        // creators["thaddius melee to place"] = &RaidNaxxActionContext::thaddius_tank_to_place;
        // creators["thaddius ranged to place"] = &RaidNaxxActionContext::thaddius_ranged_to_place;
        creators["thaddius move to platform"] = &RaidNaxxActionContext::thaddius_move_to_platform;
        creators["thaddius move polarity"] = &RaidNaxxActionContext::thaddius_move_polarity;

        creators["razuvious use obedience crystal"] = &RaidNaxxActionContext::razuvious_use_obedience_crystal;
        creators["razuvious target"] = &RaidNaxxActionContext::razuvious_target;

        creators["horseman attract alternatively"] = &RaidNaxxActionContext::horseman_attract_alternatively;
        creators["horseman attack in order"] = &RaidNaxxActionContext::horseman_attack_in_order;

        creators["sapphiron ground position"] = &RaidNaxxActionContext::sapphiron_ground_position;
        creators["sapphiron flight position"] = &RaidNaxxActionContext::sapphiron_flight_position;

        creators["kel'thuzad choose target"] = &RaidNaxxActionContext::kelthuzad_choose_target;
        creators["kel'thuzad position"] = &RaidNaxxActionContext::kelthuzad_position;

        creators["anub'rekhan choose target"] = &RaidNaxxActionContext::anubrekhan_choose_target;
        creators["anub'rekhan position"] = &RaidNaxxActionContext::anubrekhan_position;

        creators["gluth choose target"] = &RaidNaxxActionContext::gluth_choose_target;
        creators["gluth position"] = &RaidNaxxActionContext::gluth_position;
        creators["gluth slowdown"] = &RaidNaxxActionContext::gluth_slowdown;

        creators["loatheb position"] = &RaidNaxxActionContext::loatheb_position;
        creators["loatheb choose target"] = &RaidNaxxActionContext::loatheb_choose_target;
    }

private:
    static Action* go_behind_the_boss(ShadowAI* ai) { return new GrobbulusGoBehindAction(ai); }
    static Action* rotate_grobbulus(ShadowAI* ai) { return new GrobbulusRotateAction(ai); }
    static Action* grobbulus_move_center(ShadowAI* ai) { return new GrobblulusMoveCenterAction(ai); }
    static Action* heigan_dance_melee(ShadowAI* ai) { return new HeiganDanceMeleeAction(ai); }
    static Action* heigan_dance_ranged(ShadowAI* ai) { return new HeiganDanceRangedAction(ai); }
    static Action* thaddius_attack_nearest_pet(ShadowAI* ai) { return new ThaddiusAttackNearestPetAction(ai); }
    // static Action* thaddius_tank_to_place(ShadowAI* ai) { return new ThaddiusMeleeToPlaceAction(ai); }
    // static Action* thaddius_ranged_to_place(ShadowAI* ai) { return new ThaddiusRangedToPlaceAction(ai); }
    static Action* thaddius_move_to_platform(ShadowAI* ai) { return new ThaddiusMoveToPlatformAction(ai); }
    static Action* thaddius_move_polarity(ShadowAI* ai) { return new ThaddiusMovePolarityAction(ai); }
    static Action* razuvious_target(ShadowAI* ai) { return new RazuviousTargetAction(ai); }
    static Action* razuvious_use_obedience_crystal(ShadowAI* ai)
    {
        return new RazuviousUseObedienceCrystalAction(ai);
    }
    static Action* horseman_attract_alternatively(ShadowAI* ai)
    {
        return new HorsemanAttractAlternativelyAction(ai);
    }
    static Action* horseman_attack_in_order(ShadowAI* ai) { return new HorsemanAttactInOrderAction(ai); }
    // static Action* sapphiron_ground_main_tank_position(ShadowAI* ai) { return new
    // SapphironGroundMainTankPositionAction(ai); }
    static Action* sapphiron_ground_position(ShadowAI* ai) { return new SapphironGroundPositionAction(ai); }
    static Action* sapphiron_flight_position(ShadowAI* ai) { return new SapphironFlightPositionAction(ai); }
    // static Action* sapphiron_avoid_chill(ShadowAI* ai) { return new SapphironAvoidChillAction(ai); }
    static Action* kelthuzad_choose_target(ShadowAI* ai) { return new KelthuzadChooseTargetAction(ai); }
    static Action* kelthuzad_position(ShadowAI* ai) { return new KelthuzadPositionAction(ai); }
    static Action* anubrekhan_choose_target(ShadowAI* ai) { return new AnubrekhanChooseTargetAction(ai); }
    static Action* anubrekhan_position(ShadowAI* ai) { return new AnubrekhanPositionAction(ai); }
    static Action* gluth_choose_target(ShadowAI* ai) { return new GluthChooseTargetAction(ai); }
    static Action* gluth_position(ShadowAI* ai) { return new GluthPositionAction(ai); }
    static Action* gluth_slowdown(ShadowAI* ai) { return new GluthSlowdownAction(ai); }
    static Action* loatheb_position(ShadowAI* ai) { return new LoathebPositionAction(ai); }
    static Action* loatheb_choose_target(ShadowAI* ai) { return new LoathebChooseTargetAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// /*
//  * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
//  and/or modify it under version 2 of the License, or (at your option), any later version.
//  */



class RaidNaxxTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidNaxxTriggerContext()
    {
        creators["mutating injection"] = &RaidNaxxTriggerContext::mutating_injection;
        creators["mutating injection removed"] = &RaidNaxxTriggerContext::mutating_injection_removed;
        creators["grobbulus cloud"] = &RaidNaxxTriggerContext::grobbulus_cloud;
        creators["heigan melee"] = &RaidNaxxTriggerContext::heigan_melee;
        creators["heigan ranged"] = &RaidNaxxTriggerContext::heigan_ranged;

        creators["thaddius phase pet"] = &RaidNaxxTriggerContext::thaddius_phase_pet;
        creators["thaddius phase pet lose aggro"] = &RaidNaxxTriggerContext::thaddius_phase_pet_lose_aggro;
        creators["thaddius phase transition"] = &RaidNaxxTriggerContext::thaddius_phase_transition;
        creators["thaddius phase thaddius"] = &RaidNaxxTriggerContext::thaddius_phase_thaddius;

        creators["razuvious tank"] = &RaidNaxxTriggerContext::razuvious_tank;
        creators["razuvious nontank"] = &RaidNaxxTriggerContext::razuvious_nontank;

        creators["horseman attractors"] = &RaidNaxxTriggerContext::horseman_attractors;
        creators["horseman except attractors"] = &RaidNaxxTriggerContext::horseman_except_attractors;

        creators["sapphiron ground"] = &RaidNaxxTriggerContext::sapphiron_ground;
        creators["sapphiron flight"] = &RaidNaxxTriggerContext::sapphiron_flight;

        creators["kel'thuzad"] = &RaidNaxxTriggerContext::kelthuzad;

        creators["anub'rekhan"] = &RaidNaxxTriggerContext::anubrekhan;

        creators["gluth"] = &RaidNaxxTriggerContext::gluth;
        creators["gluth main tank mortal wound"] = &RaidNaxxTriggerContext::gluth_main_tank_mortal_wound;

        creators["loatheb"] = &RaidNaxxTriggerContext::loatheb;
    }

private:
    static Trigger* mutating_injection(ShadowAI* ai) { return new MutatingInjectionTrigger(ai); }
    static Trigger* mutating_injection_removed(ShadowAI* ai) { return new MutatingInjectionRemovedTrigger(ai); }
    static Trigger* grobbulus_cloud(ShadowAI* ai) { return new GrobbulusCloudTrigger(ai); }
    static Trigger* heigan_melee(ShadowAI* ai) { return new HeiganMeleeTrigger(ai); }
    static Trigger* heigan_ranged(ShadowAI* ai) { return new HeiganRangedTrigger(ai); }

    static Trigger* thaddius_phase_pet(ShadowAI* ai) { return new ThaddiusPhasePetTrigger(ai); }
    static Trigger* thaddius_phase_pet_lose_aggro(ShadowAI* ai) { return new ThaddiusPhasePetLoseAggroTrigger(ai); }
    static Trigger* thaddius_phase_transition(ShadowAI* ai) { return new ThaddiusPhaseTransitionTrigger(ai); }
    static Trigger* thaddius_phase_thaddius(ShadowAI* ai) { return new ThaddiusPhaseThaddiusTrigger(ai); }
    static Trigger* razuvious_tank(ShadowAI* ai) { return new RazuviousTankTrigger(ai); }
    static Trigger* razuvious_nontank(ShadowAI* ai) { return new RazuviousNontankTrigger(ai); }

    static Trigger* horseman_attractors(ShadowAI* ai) { return new HorsemanAttractorsTrigger(ai); }
    static Trigger* horseman_except_attractors(ShadowAI* ai) { return new HorsemanExceptAttractorsTrigger(ai); }

    static Trigger* sapphiron_ground(ShadowAI* ai) { return new SapphironGroundTrigger(ai); }
    static Trigger* sapphiron_flight(ShadowAI* ai) { return new SapphironFlightTrigger(ai); }
    static Trigger* kelthuzad(ShadowAI* ai) { return new KelthuzadTrigger(ai); }
    static Trigger* anubrekhan(ShadowAI* ai) { return new AnubrekhanTrigger(ai); }
    static Trigger* gluth(ShadowAI* ai) { return new GluthTrigger(ai); }
    static Trigger* gluth_main_tank_mortal_wound(ShadowAI* ai) { return new GluthMainTankMortalWoundTrigger(ai); }
    static Trigger* loatheb(ShadowAI* ai) { return new LoathebTrigger(ai); }
};

#endif
