#ifndef _SHADOW_CULLINGOFSTRATHOLME_H
#define _SHADOW_CULLINGOFSTRATHOLME_H

#include "Trigger.h"
#include "ShadowAIConfig.h"
#include "GenericTriggers.h"
#include "DungeonStrategyUtils.h"
#include "Action.h"
#include "AttackAction.h"
#include "GenericSpellActions.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "Multiplier.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum CullingOfStratholmeIDs
{
    // Salramm the Fleshcrafter
    NPC_GHOUL_MINION                   = 27733,
};

class ExplodeGhoulTrigger : public Trigger
{
public:
    ExplodeGhoulTrigger(ShadowAI* ai) : Trigger(ai, "explode ghoul") {}
    bool IsActive() override;
};

class EpochRangedTrigger : public Trigger
{
public:
    EpochRangedTrigger(ShadowAI* ai) : Trigger(ai, "chrono-lord epoch ranged") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class ExplodeGhoulSpreadAction : public MovementAction
{
public:
    ExplodeGhoulSpreadAction(ShadowAI* ai) : MovementAction(ai, "explode ghoul spread") {}
    bool Execute(Event event) override;
};

class EpochStackAction : public MovementAction
{
public:
    EpochStackAction(ShadowAI* ai) : MovementAction(ai, "epoch stack") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class EpochMultiplier : public Multiplier
{
    public:
        EpochMultiplier(ShadowAI* ai) : Multiplier(ai, "chrono-lord epoch") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonCoSStrategy : public Strategy
{
public:
    WotlkDungeonCoSStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "culling of stratholme"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonCoSActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonCoSActionContext() {
            creators["explode ghoul spread"] = &WotlkDungeonCoSActionContext::explode_ghoul_spread;
            creators["epoch stack"] = &WotlkDungeonCoSActionContext::epoch_stack;
        }
    private:
        static Action* explode_ghoul_spread(ShadowAI* ai) { return new ExplodeGhoulSpreadAction(ai); }
        static Action* epoch_stack(ShadowAI* ai) { return new EpochStackAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonCoSTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonCoSTriggerContext()
        {
            creators["explode ghoul"] = &WotlkDungeonCoSTriggerContext::explode_ghoul;
            creators["epoch ranged"] = &WotlkDungeonCoSTriggerContext::epoch_ranged;

        }
    private:
        static Trigger* explode_ghoul(ShadowAI* ai) { return new ExplodeGhoulTrigger(ai); }
        static Trigger* epoch_ranged(ShadowAI* ai) { return new EpochRangedTrigger(ai); }
};

#endif
