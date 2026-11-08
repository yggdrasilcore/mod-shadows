#ifndef _SHADOW_RAIDAQ20_H
#define _SHADOW_RAIDAQ20_H

#include "GameObject.h"
#include "Unit.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "Trigger.h"
#include "MovementActions.h"
#include "AiObjectContext.h"
#include "Multiplier.h"
#include "Strategy.h"
#include "Action.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidAq20Utils
{
public:
    static bool IsOssirianBuffActive(Unit* ossirian);
    static int32 GetOssirianDebuffTimeRemaining(Unit* ossirian);
    static GameObject* GetNearestCrystal(Unit* ossirian);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class Aq20MoveToCrystalTrigger : public Trigger
{
public:
    Aq20MoveToCrystalTrigger(ShadowAI* botAI) : Trigger(botAI, "aq20 move to crystal") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class Aq20UseCrystalAction : public MovementAction
{
public:
    Aq20UseCrystalAction(ShadowAI* botAI, std::string const name = "aq20 use crystal") : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidAq20Strategy : public Strategy
{
public:
    RaidAq20Strategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "aq20"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    // virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidAq20ActionContext : public NamedObjectContext<Action>
{
public:
    RaidAq20ActionContext()
    {
        creators["aq20 use crystal"] = &RaidAq20ActionContext::use_crystal;
    }

private:
    static Action* use_crystal(ShadowAI* ai) { return new Aq20UseCrystalAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidAq20TriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidAq20TriggerContext()
    {
        creators["aq20 move to crystal"] = &RaidAq20TriggerContext::move_to_crystal;
    }

private:
    static Trigger* move_to_crystal(ShadowAI* ai) { return new Aq20MoveToCrystalTrigger(ai); }
};

#endif
