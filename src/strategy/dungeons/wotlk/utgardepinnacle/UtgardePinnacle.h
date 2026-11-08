#ifndef _SHADOW_UTGARDEPINNACLE_H
#define _SHADOW_UTGARDEPINNACLE_H

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

enum UtgardePinnacleIDs
{
    // Skadi the Ruthless
    SPELL_FREEZING_CLOUD_N          = 47579,
    SPELL_FREEZING_CLOUD_H          = 60020,
    SPELL_FREEZING_CLOUD_BREATH     = 47592,
    NPC_BREATH_TRIGGER              = 28351,
    SPELL_SKADI_WHIRLWIND_N         = 50228,
    SPELL_SKADI_WHIRLWIND_H         = 59322,

    // King Ymiron
    SPELL_BANE_N                    = 48294,
    SPELL_BANE_H                    = 59301,
};

#define SPELL_FREEZING_CLOUD        DUNGEON_MODE(bot, SPELL_FREEZING_CLOUD_N, SPELL_FREEZING_CLOUD_H)
#define SPELL_SKADI_WHIRLWIND       DUNGEON_MODE(bot, SPELL_SKADI_WHIRLWIND_N, SPELL_SKADI_WHIRLWIND_H)
#define SPELL_BANE                  DUNGEON_MODE(bot, SPELL_BANE_N, SPELL_BANE_H)

// const float SKADI_BREATH_CENTRELINE = -512.46875f;

class SkadiFreezingCloudTrigger : public Trigger
{
public:
    SkadiFreezingCloudTrigger(ShadowAI* ai) : Trigger(ai, "skadi freezing cloud") {}
    bool IsActive() override;
};

class SkadiWhirlwindTrigger : public Trigger
{
public:
    SkadiWhirlwindTrigger(ShadowAI* ai) : Trigger(ai, "skadi whirlwind") {}
    bool IsActive() override;
};

class YmironBaneTrigger : public Trigger
{
public:
    YmironBaneTrigger(ShadowAI* ai) : Trigger(ai, "ymiron bane") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class AvoidFreezingCloudAction : public MovementAction
{
public:
    AvoidFreezingCloudAction(ShadowAI* ai) : MovementAction(ai, "avoid freezing cloud") {}
    bool Execute(Event event) override;
};

class AvoidSkadiWhirlwindAction : public MovementAction
{
public:
    AvoidSkadiWhirlwindAction(ShadowAI* ai) : MovementAction(ai, "avoid skadi whirlwind") {}
    bool Execute(Event event) override;
};

class AvoidYmironBaneAction : public MovementAction
{
public:
    AvoidYmironBaneAction(ShadowAI* ai) : MovementAction(ai, "avoid ymiron bane") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class SkadiMultiplier : public Multiplier
{
    public:
        SkadiMultiplier(ShadowAI* ai) : Multiplier(ai, "skadi the ruthless") {}

    public:
        virtual float GetValue(Action* action);
};

class YmironMultiplier : public Multiplier
{
    public:
        YmironMultiplier(ShadowAI* ai) : Multiplier(ai, "king ymiron") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonUPStrategy : public Strategy
{
public:
    WotlkDungeonUPStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "utgarde pinnacle"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonUPActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonUPActionContext() {
            creators["avoid freezing cloud"] = &WotlkDungeonUPActionContext::avoid_freezing_cloud;
            creators["avoid skadi whirlwind"] = &WotlkDungeonUPActionContext::avoid_whirlwind;
            creators["avoid ymiron bane"] = &WotlkDungeonUPActionContext::avoid_ymiron_bane;
        }
    private:
        static Action* avoid_freezing_cloud(ShadowAI* ai) { return new AvoidFreezingCloudAction(ai); }
        static Action* avoid_whirlwind(ShadowAI* ai) { return new AvoidSkadiWhirlwindAction(ai); }
        static Action* avoid_ymiron_bane(ShadowAI* ai) { return new AvoidYmironBaneAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonUPTriggerContext : public NamedObjectContext<Trigger> 
{
    public:
        WotlkDungeonUPTriggerContext()
        {
            creators["freezing cloud"] = &WotlkDungeonUPTriggerContext::freezing_cloud;
            creators["skadi whirlwind"] = &WotlkDungeonUPTriggerContext::whirlwind;
            creators["ymiron bane"] = &WotlkDungeonUPTriggerContext::bane;
        }
    private:
        static Trigger* freezing_cloud(ShadowAI* ai) { return new SkadiFreezingCloudTrigger(ai); }
        static Trigger* whirlwind(ShadowAI* ai) { return new SkadiWhirlwindTrigger(ai); }
        static Trigger* bane(ShadowAI* ai) { return new YmironBaneTrigger(ai); }
};

#endif
