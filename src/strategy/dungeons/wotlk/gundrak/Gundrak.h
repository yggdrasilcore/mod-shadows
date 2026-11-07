#ifndef _SHADOW_GUNDRAK_H
#define _SHADOW_GUNDRAK_H

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

enum GundrakIDs
{
    // Slad'ran
    SPELL_POISON_NOVA_N             = 55081,
    SPELL_POISON_NOVA_H             = 59842,
    NPC_SNAKE_WRAP                  = 29742,

    // Gal'darah
    SPELL_WHIRLING_SLASH_N          = 55250,
    SPELL_WHIRLING_SLASH_H          = 59824,
};

#define SPELL_POISON_NOVA           DUNGEON_MODE(bot, SPELL_POISON_NOVA_N, SPELL_POISON_NOVA_H)
#define SPELL_WHIRLING_SLASH        DUNGEON_MODE(bot, SPELL_WHIRLING_SLASH_N, SPELL_WHIRLING_SLASH_H)

class SladranPoisonNovaTrigger : public Trigger
{
public:
    SladranPoisonNovaTrigger(ShadowAI* ai) : Trigger(ai, "slad'ran poison nova") {}
    bool IsActive() override;
};

class SladranSnakeWrapTrigger : public Trigger
{
public:
    SladranSnakeWrapTrigger(ShadowAI* ai) : Trigger(ai, "slad'ran snake wrap") {}
    bool IsActive() override;
};

class GaldarahWhirlingSlashTrigger : public Trigger
{
public:
    GaldarahWhirlingSlashTrigger(ShadowAI* ai) : Trigger(ai, "gal'darah whirling slash") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class AvoidPoisonNovaAction : public MovementAction
{
public:
    AvoidPoisonNovaAction(ShadowAI* ai) : MovementAction(ai, "avoid poison nova") {}
    bool Execute(Event event) override;
};

class AttackSnakeWrapAction : public AttackAction
{
public:
    AttackSnakeWrapAction(ShadowAI* ai) : AttackAction(ai, "attack snake wrap") {}
    bool Execute(Event event) override;
};

class AvoidWhirlingSlashAction : public MovementAction
{
public:
    AvoidWhirlingSlashAction(ShadowAI* ai) : MovementAction(ai, "avoid whirling slash") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class SladranMultiplier : public Multiplier
{
    public:
        SladranMultiplier(ShadowAI* ai) : Multiplier(ai, "slad'ran") {}

    public:
        virtual float GetValue(Action* action);
};

class GaldarahMultiplier : public Multiplier
{
    public:
        GaldarahMultiplier(ShadowAI* ai) : Multiplier(ai, "gal'darah") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonGDStrategy : public Strategy
{
public:
    WotlkDungeonGDStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "gundrak"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonGDActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonGDActionContext() {
            creators["avoid poison nova"] = &WotlkDungeonGDActionContext::avoid_poison_nova;
            creators["attack snake wrap"] = &WotlkDungeonGDActionContext::attack_snake_wrap;
            creators["avoid whirling slash"] = &WotlkDungeonGDActionContext::avoid_whirling_slash;
        }
    private:
        static Action* avoid_poison_nova(ShadowAI* ai) { return new AvoidPoisonNovaAction(ai); }
        static Action* attack_snake_wrap(ShadowAI* ai) { return new AttackSnakeWrapAction(ai); }
        static Action* avoid_whirling_slash(ShadowAI* ai) { return new AvoidWhirlingSlashAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonGDTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonGDTriggerContext()
        {
            creators["poison nova"] = &WotlkDungeonGDTriggerContext::poison_nova;
            creators["snake wrap"] = &WotlkDungeonGDTriggerContext::snake_wrap;
            creators["whirling slash"] = &WotlkDungeonGDTriggerContext::whirling_slash;
        }
    private:
        static Trigger* poison_nova(ShadowAI* ai) { return new SladranPoisonNovaTrigger(ai); }
        static Trigger* snake_wrap(ShadowAI* ai) { return new SladranSnakeWrapTrigger(ai); }
        static Trigger* whirling_slash(ShadowAI* ai) { return new GaldarahWhirlingSlashTrigger(ai); }
};

#endif
