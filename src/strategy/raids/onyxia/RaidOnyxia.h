#ifndef _SHADOW_RAIDONYXIA_H
#define _SHADOW_RAIDONYXIA_H

#include "ShadowAI.h"
#include "Trigger.h"
#include "Action.h"
#include "AttackAction.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "Strategy.h"
#include "NamedObjectContext.h"
#include "AiObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------



// Mechanics
class OnyxiaDeepBreathTrigger : public Trigger
{
public:
    OnyxiaDeepBreathTrigger(ShadowAI* botAI);
    bool IsActive() override;
};

class OnyxiaNearTailTrigger : public Trigger
{
public:
    OnyxiaNearTailTrigger(ShadowAI* botAI);
    bool IsActive() override;
};

class RaidOnyxiaFireballSplashTrigger : public Trigger
{
public:
    RaidOnyxiaFireballSplashTrigger(ShadowAI* botAI);
    bool IsActive() override;
};

class RaidOnyxiaWhelpsSpawnTrigger : public Trigger
{
public:
    RaidOnyxiaWhelpsSpawnTrigger(ShadowAI* botAI);
    bool IsActive() override;
};

class OnyxiaAvoidEggsTrigger : public Trigger
{
public:
    OnyxiaAvoidEggsTrigger(ShadowAI* botAI);
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------



class ShadowAI;

class RaidOnyxiaMoveToSideAction : public MovementAction
{
public:
    RaidOnyxiaMoveToSideAction(ShadowAI* botAI, std::string const name = "ony move to side")
        : MovementAction(botAI, name)
    {
    }
    bool Execute(Event event) override;
};

class RaidOnyxiaSpreadOutAction : public MovementAction
{
public:
    RaidOnyxiaSpreadOutAction(ShadowAI* botAI, std::string const name = "ony spread out")
        : MovementAction(botAI, name)
    {
    }
    bool Execute(Event event) override;
};

struct SafeZone
{
    Position pos;
    float radius;
};

class RaidOnyxiaMoveToSafeZoneAction : public MovementAction
{
public:
    RaidOnyxiaMoveToSafeZoneAction(ShadowAI* botAI, std::string const name = "ony move to safe zone")
        : MovementAction(botAI, name)
    {
    }
    bool Execute(Event event) override;

private:
    std::vector<SafeZone> GetSafeZonesForBreath(uint32 spellId)
    {
        // Define your safe zone coordinates based on the map
        // Example assumes Onyxia's lair map coordinates
        float z = bot->GetPositionZ();  // Stay at current height

        switch (spellId)
        {
            case 17086:  // N to S
            case 18351:  // S to N
                return {SafeZone{Position(-10.0f, -180.0f, z), 5.0f},
                        SafeZone{Position(-20.0f, -250.0f, z), 5.0f}};  // Bottom Safe Zone

            case 18576:  // E to W
            case 18609:  // W to E
                return {
                    SafeZone{Position(20.0f, -210.0f, z), 5.0f},
                    SafeZone{Position(-75.0f, -210.0f, z), 5.0f},
                };  // Left Safe Zone

            case 18564:  // SE to NW
            case 18584:  // NW to SE
                return {
                    SafeZone{Position(-60.0f, -195.0f, z), 5.0f},
                    SafeZone{Position(10.0f, -240.0f, z), 5.0f},
                };  // NW Safe Zone

            case 18596:  // SW to NE
            case 18617:  // NE to SW
                return {
                    SafeZone{Position(7.0f, -185.0f, z), 5.0f},
                    SafeZone{Position(-60.0f, -240.0f, z), 5.0f},
                };  // NE Safe Zone

            default:
                return {SafeZone{Position(0.0f, 0.0f, z), 5.0f}};  // Fallback center - shouldn't ever happen
        }
    }
};

class RaidOnyxiaKillWhelpsAction : public AttackAction
{
public:
    RaidOnyxiaKillWhelpsAction(ShadowAI* botAI, std::string const name = "ony kill whelps")
        : AttackAction(botAI, name)
    {
    }

    bool Execute(Event event) override;
};

class OnyxiaAvoidEggsAction : public MovementAction
{
public:
    OnyxiaAvoidEggsAction(ShadowAI* botAI) : MovementAction(botAI, "ony avoid eggs move") {}

    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------



class RaidOnyxiaStrategy : public Strategy
{
public:
    RaidOnyxiaStrategy(ShadowAI* ai) : Strategy(ai) {}

    std::string const getName() override { return "onyxia"; }

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidOnyxiaActionContext : public NamedObjectContext<Action>
{
public:
    RaidOnyxiaActionContext()
    {
        creators["ony move to side"] = &RaidOnyxiaActionContext::move_to_side;
        creators["ony spread out"] = &RaidOnyxiaActionContext::spread_out;
        creators["ony move to safe zone"] = &RaidOnyxiaActionContext::move_to_safe_zone;
        creators["ony kill whelps"] = &RaidOnyxiaActionContext::kill_whelps;
        creators["ony avoid eggs move"] = &RaidOnyxiaActionContext::avoid_eggs;
    }

private:
    static Action* move_to_side(ShadowAI* ai) { return new RaidOnyxiaMoveToSideAction(ai); }
    static Action* spread_out(ShadowAI* ai) { return new RaidOnyxiaSpreadOutAction(ai); }
    static Action* move_to_safe_zone(ShadowAI* ai) { return new RaidOnyxiaMoveToSafeZoneAction(ai); }
    static Action* kill_whelps(ShadowAI* ai) { return new RaidOnyxiaKillWhelpsAction(ai); }
    static Action* avoid_eggs(ShadowAI* ai) { return new OnyxiaAvoidEggsAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidOnyxiaTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidOnyxiaTriggerContext()
    {
        creators["ony near tail"] = &RaidOnyxiaTriggerContext::near_tail;
        creators["ony deep breath warning"] = &RaidOnyxiaTriggerContext::deep_breath;
        creators["ony fireball splash incoming"] = &RaidOnyxiaTriggerContext::fireball_splash;
        creators["ony whelps spawn"] = &RaidOnyxiaTriggerContext::whelps_spawn;
        creators["ony avoid eggs"] = &RaidOnyxiaTriggerContext::avoid_eggs;
    }

private:
    static Trigger* near_tail(ShadowAI* ai) { return new OnyxiaNearTailTrigger(ai); }
    static Trigger* deep_breath(ShadowAI* ai) { return new OnyxiaDeepBreathTrigger(ai); }
    static Trigger* fireball_splash(ShadowAI* ai) { return new RaidOnyxiaFireballSplashTrigger(ai); }
    static Trigger* whelps_spawn(ShadowAI* ai) { return new RaidOnyxiaWhelpsSpawnTrigger(ai); }
    static Trigger* avoid_eggs(ShadowAI* ai) { return new OnyxiaAvoidEggsTrigger(ai); }
};

#endif
