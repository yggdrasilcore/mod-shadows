#ifndef _SHADOW_RAIDBWL_H
#define _SHADOW_RAIDBWL_H

#include "ShadowAI.h"
#include "Shadows.h"
#include "Trigger.h"
#include "Action.h"
#include "AttackAction.h"
#include "GenericActions.h"
#include "MovementActions.h"
#include "AiObjectContext.h"
#include "Multiplier.h"
#include "Strategy.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class BwlSuppressionDeviceTrigger : public Trigger
{
public:
    BwlSuppressionDeviceTrigger(ShadowAI* botAI) : Trigger(botAI, "bwl suppression device") {}
    bool IsActive() override;
};

class BwlAfflictionBronzeTrigger : public Trigger
{
public:
    BwlAfflictionBronzeTrigger(ShadowAI* botAI) : Trigger(botAI, "bwl affliction bronze") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class BwlOnyxiaScaleCloakAuraCheckAction : public Action
{
public:
    BwlOnyxiaScaleCloakAuraCheckAction(ShadowAI* botAI) : Action(botAI, "bwl onyxia scale cloak aura check") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class BwlTurnOffSuppressionDeviceAction : public Action
{
public:
    BwlTurnOffSuppressionDeviceAction(ShadowAI* botAI) : Action(botAI, "bwl turn off suppression device") {}
    bool Execute(Event event) override;
};

class BwlUseHourglassSandAction : public Action
{
public:
    BwlUseHourglassSandAction(ShadowAI* botAI) : Action(botAI, "bwl use hourglass sand") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidBwlStrategy : public Strategy
{
public:
    RaidBwlStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "bwl"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    // virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidBwlActionContext : public NamedObjectContext<Action>
{
public:
    RaidBwlActionContext()
    {
        creators["bwl check onyxia scale cloak"] = &RaidBwlActionContext::bwl_check_onyxia_scale_cloak;
        creators["bwl turn off suppression device"] = &RaidBwlActionContext::bwl_turn_off_suppression_device;
        creators["bwl use hourglass sand"] = &RaidBwlActionContext::bwl_use_hourglass_sand;
    }

private:
    static Action* bwl_check_onyxia_scale_cloak(ShadowAI* botAI) { return new BwlOnyxiaScaleCloakAuraCheckAction(botAI); }
    static Action* bwl_turn_off_suppression_device(ShadowAI* botAI) { return new BwlTurnOffSuppressionDeviceAction(botAI); }
    static Action* bwl_use_hourglass_sand(ShadowAI* botAI) { return new BwlUseHourglassSandAction(botAI); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidBwlTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidBwlTriggerContext()
    {
        creators["bwl suppression device"] = &RaidBwlTriggerContext::bwl_suppression_device;
        creators["bwl affliction bronze"] = &RaidBwlTriggerContext::bwl_affliction_bronze;
    }

private:
    static Trigger* bwl_suppression_device(ShadowAI* ai) { return new BwlSuppressionDeviceTrigger(ai); }
    static Trigger* bwl_affliction_bronze(ShadowAI* ai) { return new BwlAfflictionBronzeTrigger(ai); }
};

#endif
