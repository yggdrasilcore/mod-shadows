#ifndef _SHADOW_RAIDEOE_H
#define _SHADOW_RAIDEOE_H

#include "ShadowAI.h"
#include "Shadows.h"
#include "Trigger.h"
#include "MovementActions.h"
#include "AttackAction.h"
#include "GenericSpellActions.h"
#include "Multiplier.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "Action.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum EyeOfEternityIDs
{
    NPC_MALYGOS                         = 28859,
    NPC_POWER_SPARK                     = 30084,
    NPC_NEXUS_LORD                      = 30245,
    NPC_SCION_OF_ETERNITY               = 30249,
    NPC_WYRMREST_SKYTALON               = 30161,

    SPELL_POWER_SPARK_VISUAL            = 55845,
    SPELL_POWER_SPARK_GROUND_BUFF       = 55852,
    SPELL_POWER_SPARK_MALYGOS_BUFF      = 56152,

    SPELL_TELEPORT_VISUAL               = 52096,

    SPELL_SCION_ARCANE_BARRAGE          = 56397,
    SPELL_ARCANE_SHOCK_N                = 57058,
    SPELL_ARCANE_SHOCK_H                = 60073,
    SPELL_HASTE                         = 57060,

    SPELL_ALEXSTRASZA_GIFT              = 61028,

    // Drake Abilities:
    // DPS
    SPELL_FLAME_SPIKE                   = 56091,
    SPELL_ENGULF_IN_FLAMES              = 56092,
    // Healing
    SPELL_REVIVIFY                      = 57090,
    SPELL_LIFE_BURST                    = 57143,
    // Utility
    SPELL_FLAME_SHIELD                  = 57108,
    SPELL_BLAZING_SPEED                 = 57092,
};

const uint32 EOE_MAP_ID = 616;

class MalygosTrigger : public Trigger
{
public:
    MalygosTrigger(ShadowAI* botAI) : Trigger(botAI, "malygos") {}
    bool IsActive() override;
    uint8 static getPhase(Player* bot, Unit* boss);
};

class PowerSparkTrigger : public Trigger
{
public:
    PowerSparkTrigger(ShadowAI* botAI) : Trigger(botAI, "power spark") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const std::pair<float, float> MALYGOS_MAINTANK_POSITION = {757.0f, 1337.0f};
const std::pair<float, float> MALYGOS_STACK_POSITION = {755.0f, 1301.0f};

class MalygosPositionAction : public MovementAction
{
public:
    MalygosPositionAction(ShadowAI* botAI, std::string const name = "malygos position")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

class MalygosTargetAction : public AttackAction
{
public:
    MalygosTargetAction(ShadowAI* botAI, std::string const name = "malygos target")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;
};

class PullPowerSparkAction : public CastSpellAction
{
public:
    PullPowerSparkAction(ShadowAI* botAI, std::string const name = "pull power spark")
        : CastSpellAction(botAI, name) {}
    bool Execute(Event event) override;
    bool isPossible() override;
    bool isUseful() override;
};

class KillPowerSparkAction : public AttackAction
{
public:
    KillPowerSparkAction(ShadowAI* botAI, std::string const name = "kill power spark")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;
};

class EoEFlyDrakeAction : public MovementAction
{
public:
    EoEFlyDrakeAction(ShadowAI* ai) : MovementAction(ai, "eoe fly drake") {}
    bool Execute(Event event) override;
    bool isPossible() override;
};

class EoEDrakeAttackAction : public Action
{
public:
    EoEDrakeAttackAction(ShadowAI* botAI) : Action(botAI, "eoe drake attack") {}
    bool Execute(Event event) override;
    bool isPossible() override;

protected:
    Unit* vehicleBase;
    bool CastDrakeSpellAction(Unit* target, uint32 spellId, uint32 cooldown);
    bool DrakeDpsAction(Unit* target);
    bool DrakeHealAction();
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class MalygosMultiplier : public Multiplier
{
public:
    MalygosMultiplier(ShadowAI* ai) : Multiplier(ai, "malygos") {}

public:
    virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidEoEStrategy : public Strategy
{
public:
    RaidEoEStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "wotlk-eoe"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidEoEActionContext : public NamedObjectContext<Action>
{
public:
    RaidEoEActionContext()
    {
        creators["malygos position"] = &RaidEoEActionContext::position;
        creators["malygos target"] = &RaidEoEActionContext::target;
        // creators["pull power spark"] = &RaidEoEActionContext::pull_power_spark;
        // creators["kill power spark"] = &RaidEoEActionContext::kill_power_spark;
        creators["eoe fly drake"] = &RaidEoEActionContext::eoe_fly_drake;
        creators["eoe drake attack"] = &RaidEoEActionContext::eoe_drake_attack;
    }

private:
    static Action* position(ShadowAI* ai) { return new MalygosPositionAction(ai); }
    static Action* target(ShadowAI* ai) { return new MalygosTargetAction(ai); }
    // static Action* pull_power_spark(ShadowAI* ai) { return new PullPowerSparkAction(ai); }
    // static Action* kill_power_spark(ShadowAI* ai) { return new KillPowerSparkAction(ai); }
    static Action* eoe_fly_drake(ShadowAI* ai) { return new EoEFlyDrakeAction(ai); }
    static Action* eoe_drake_attack(ShadowAI* ai) { return new EoEDrakeAttackAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidEoETriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidEoETriggerContext()
    {
        creators["malygos"] = &RaidEoETriggerContext::malygos;
        creators["power spark"] = &RaidEoETriggerContext::power_spark;
    }

private:
    static Trigger* power_spark(ShadowAI* ai) { return new PowerSparkTrigger(ai); }
    static Trigger* malygos(ShadowAI* ai) { return new MalygosTrigger(ai); }
};

#endif
