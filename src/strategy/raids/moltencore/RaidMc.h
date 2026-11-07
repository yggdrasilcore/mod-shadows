#ifndef _SHADOW_RAIDMC_H
#define _SHADOW_RAIDMC_H

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

class McLivingBombDebuffTrigger : public Trigger
{
public:
    McLivingBombDebuffTrigger(ShadowAI* botAI) : Trigger(botAI, "mc living bomb debuff") {}
    bool IsActive() override;
};

class McBaronGeddonInfernoTrigger : public Trigger
{
public:
    McBaronGeddonInfernoTrigger(ShadowAI* botAI) : Trigger(botAI, "mc baron geddon inferno") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class McCheckShouldMoveFromGroupAction : public Action
{
public:
    McCheckShouldMoveFromGroupAction(ShadowAI* botAI, std::string const name = "mc check should move from group")
        : Action(botAI, name) {}
    bool Execute(Event event) override;
};

class McMoveFromBaronGeddonAction : public MovementAction
{
public:
    McMoveFromBaronGeddonAction(ShadowAI* botAI, std::string const name = "mc move from baron geddon")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidMcStrategy : public Strategy
{
public:
    RaidMcStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "mc"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    // virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidMcActionContext : public NamedObjectContext<Action>
{
public:
    RaidMcActionContext()
    {
        creators["mc check should move from group"] = &RaidMcActionContext::check_should_move_from_group;
        creators["mc move from baron geddon"] = &RaidMcActionContext::move_from_baron_geddon;
    }

private:
    static Action* check_should_move_from_group(ShadowAI* ai) { return new McCheckShouldMoveFromGroupAction(ai); }
    static Action* move_from_baron_geddon(ShadowAI* ai) { return new McMoveFromBaronGeddonAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidMcTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidMcTriggerContext()
    {
        creators["mc living bomb debuff"] = &RaidMcTriggerContext::living_bomb_debuff;
        creators["mc baron geddon inferno"] = &RaidMcTriggerContext::baron_geddon_inferno;
    }

private:
    static Trigger* living_bomb_debuff(ShadowAI* ai) { return new McLivingBombDebuffTrigger(ai); }
    static Trigger* baron_geddon_inferno(ShadowAI* ai) { return new McBaronGeddonInfernoTrigger(ai); }
};

#endif
