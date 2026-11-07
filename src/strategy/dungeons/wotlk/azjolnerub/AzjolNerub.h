#ifndef _SHADOW_AZJOLNERUB_H
#define _SHADOW_AZJOLNERUB_H

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

enum AzjolNerubIDs
{
    // Krik'thir the Gatewatcher
    NPC_KRIKTHIR                    = 28684,
    NPC_WATCHER_SILTHIK             = 28731,
    NPC_WATCHER_GASHRA              = 28730,
    NPC_WATCHER_NARJIL              = 28729,
    NPC_WATCHER_SKIRMISHER          = 28734,
    NPC_WATCHER_SHADOWCASTER        = 28733,
    NPC_WATCHER_WARRIOR             = 28732,
    DEBUFF_WEB_WRAP                 = 52086,
    NPC_WEB_WRAP                    = 28619,

    // Anub'arak
    // Not sure how to track this - first one is cast as a buff on himself,
    // which triggers periodic casts of the spikes spell.
    SPELL_IMPALE_PERIODIC           = 53456,
    SPELL_IMPALE_SPIKES             = 53457,
    SPELL_POUND_N                   = 53472,
    SPELL_POUND_H                   = 59433,
};

#define SPELL_POUND                 DUNGEON_MODE(bot, SPELL_POUND_N, SPELL_POUND_H)

class KrikthirWebWrapTrigger : public Trigger
{
public:
    KrikthirWebWrapTrigger(ShadowAI* ai) : Trigger(ai, "krik'thir web wrap") {}
    bool IsActive() override;
};

class KrikthirWatchersTrigger : public Trigger
{
public:
    KrikthirWatchersTrigger(ShadowAI* ai) : Trigger(ai, "krik'thir watchers") {}
    bool IsActive() override;
};

// class AnubarakImpaleTrigger : public Trigger
// {
// public:
//     AnubarakImpaleTrigger(ShadowAI* ai) : Trigger(ai, "anub'arak impale") {}
//     bool IsActive() override;
// };

class AnubarakPoundTrigger : public Trigger
{
public:
    AnubarakPoundTrigger(ShadowAI* ai) : Trigger(ai, "anub'arak pound") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class AttackWebWrapAction : public AttackAction
{
public:
    AttackWebWrapAction(ShadowAI* ai) : AttackAction(ai, "attack web wrap") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class WatchersTargetAction : public AttackAction
{
public:
    WatchersTargetAction(ShadowAI* ai) : AttackAction(ai, "krik'thir priority") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class AnubarakDodgePoundAction : public AttackAction
{
public:
    AnubarakDodgePoundAction(ShadowAI* ai) : AttackAction(ai, "anub'arak dodge pound") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class KrikthirMultiplier : public Multiplier
{
    public:
        KrikthirMultiplier(ShadowAI* ai) : Multiplier(ai, "krik'thir the gatewatcher") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonANStrategy : public Strategy
{
public:
    WotlkDungeonANStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "azjol'nerub"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonANActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonANActionContext() {
            creators["attack web wrap"] = &WotlkDungeonANActionContext::attack_web_wrap;
            creators["krik'thir priority"] = &WotlkDungeonANActionContext::krikthir_priority;
            creators["dodge pound"] = &WotlkDungeonANActionContext::dodge_pound;
        }
    private:
        static Action* attack_web_wrap(ShadowAI* ai) { return new AttackWebWrapAction(ai); }
        static Action* krikthir_priority(ShadowAI* ai) { return new WatchersTargetAction(ai); }
        static Action* dodge_pound(ShadowAI* ai) { return new AnubarakDodgePoundAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonANTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonANTriggerContext()
        {
            creators["krik'thir web wrap"] = &WotlkDungeonANTriggerContext::krikthir_web_wrap;
            creators["krik'thir watchers"] = &WotlkDungeonANTriggerContext::krikthir_watchers;
            // creators["anub'arak impale"] = &WotlkDungeonANTriggerContext::anubarak_impale;
            creators["anub'arak pound"] = &WotlkDungeonANTriggerContext::anubarak_pound;
        }
    private:
        static Trigger* krikthir_web_wrap(ShadowAI* ai) { return new KrikthirWebWrapTrigger(ai); }
        static Trigger* krikthir_watchers(ShadowAI* ai) { return new KrikthirWatchersTrigger(ai); }
        // static Trigger* anubarak_impale(ShadowAI* ai) { return new AnubarakImpaleTrigger(ai); }
        static Trigger* anubarak_pound(ShadowAI* ai) { return new AnubarakPoundTrigger(ai); }
};

#endif
