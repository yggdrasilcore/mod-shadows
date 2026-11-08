#ifndef _SHADOW_UTGARDEKEEP_H
#define _SHADOW_UTGARDEKEEP_H

#include "Trigger.h"
#include "ShadowAIConfig.h"
#include "GenericTriggers.h"
#include "DungeonStrategyUtils.h"
#include "Action.h"
#include "AttackAction.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "Multiplier.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum UtgardeKeepIDs
{
    // Prince Keleseth
    SPELL_FROST_TOMB               = 48400,
    NPC_FROST_TOMB                 = 23965,
};

class KelesethFrostTombTrigger : public Trigger
{
public:
    KelesethFrostTombTrigger(ShadowAI* ai) : Trigger(ai, "keleseth frost tomb") {}
    bool IsActive() override;
};

class DalronnDpsTrigger : public Trigger
{
public:
    DalronnDpsTrigger(ShadowAI* ai) : Trigger(ai, "dalronn dps") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class AttackFrostTombAction : public AttackAction
{
public:
    AttackFrostTombAction(ShadowAI* ai) : AttackAction(ai, "attack frost tomb") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class AttackDalronnAction : public AttackAction
{
public:
    AttackDalronnAction(ShadowAI* ai) : AttackAction(ai, "attack dalronn") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class PrinceKelesethMultiplier : public Multiplier
{
    public:
        PrinceKelesethMultiplier(ShadowAI* ai) : Multiplier(ai, "prince keleseth") {}

    public:
        virtual float GetValue(Action* action);
};

class SkarvaldAndDalronnMultiplier : public Multiplier
{
    public:
        SkarvaldAndDalronnMultiplier(ShadowAI* ai) : Multiplier(ai, "skarvald and dalronn") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonUKStrategy : public Strategy
{
public:
    WotlkDungeonUKStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "utgarde keep"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonUKActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonUKActionContext() {
            creators["attack frost tomb"] = &WotlkDungeonUKActionContext::attack_frost_tomb;
            creators["attack dalronn"] = &WotlkDungeonUKActionContext::attack_dalronn;
        }
    private:
        static Action* attack_frost_tomb(ShadowAI* ai) { return new AttackFrostTombAction(ai); }
        static Action* attack_dalronn(ShadowAI* ai) { return new AttackDalronnAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonUKTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonUKTriggerContext()
        {
            creators["keleseth frost tomb"] = &WotlkDungeonUKTriggerContext::keleseth_frost_tomb;
            creators["dalronn priority"] = &WotlkDungeonUKTriggerContext::dalronn_priority_target;
        }
    private:
        static Trigger* keleseth_frost_tomb(ShadowAI* ai) { return new KelesethFrostTombTrigger(ai); }
        static Trigger* dalronn_priority_target(ShadowAI* ai) { return new DalronnDpsTrigger(ai); }
};

#endif
