#ifndef _SHADOW_VIOLETHOLD_H
#define _SHADOW_VIOLETHOLD_H

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

enum VioletHoldIDs
{
    // Ichoron
    SPELL_DRAINED                      = 59820,
    NPC_ICHOR_GLOBULE                  = 29321,

    // Zuramat the Obliterator
    SPELL_VOID_SHIFTED                 = 54343,
    SPELL_SHROUD_OF_DARKNESS_N         = 54524,
    SPELL_SHROUD_OF_DARKNESS_H         = 59745,
    NPC_VOID_SENTRY                    = 29364,
};

#define SPELL_SHROUD_OF_DARKNESS    DUNGEON_MODE(bot, SPELL_SHROUD_OF_DARKNESS_N, SPELL_SHROUD_OF_DARKNESS_H)

class ErekemTargetTrigger : public Trigger
{
public:
    ErekemTargetTrigger(ShadowAI* ai) : Trigger(ai, "erekem target") {}
    bool IsActive() override;
};

class IchoronTargetTrigger : public Trigger
{
public:
    IchoronTargetTrigger(ShadowAI* ai) : Trigger(ai, "ichoron target") {}
    bool IsActive() override;
};

class VoidShiftTrigger : public Trigger
{
public:
    VoidShiftTrigger(ShadowAI* ai) : Trigger(ai, "void shift") {}
    bool IsActive() override;
};

class ShroudOfDarknessTrigger : public Trigger
{
public:
    ShroudOfDarknessTrigger(ShadowAI* ai) : Trigger(ai, "shroud of darkness") {}
    bool IsActive() override;
};

class CyanigosaPositioningTrigger : public Trigger
{
public:
    CyanigosaPositioningTrigger(ShadowAI* ai) : Trigger(ai, "cyanigosa positioning") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class AttackErekemAction : public AttackAction
{
public:
    AttackErekemAction(ShadowAI* ai) : AttackAction(ai, "attack erekem") {}
    bool Execute(Event event) override;
};

class AttackIchoronElementalsAction : public AttackAction
{
public:
    AttackIchoronElementalsAction(ShadowAI* ai) : AttackAction(ai, "attack ichoron elementals") {}
    bool Execute(Event event) override;
};

class AttackIchorGlobuleAction : public AttackAction
{
public:
    AttackIchorGlobuleAction(ShadowAI* ai) : AttackAction(ai, "attack ichor globule") {}
    bool Execute(Event event) override;
};

class AttackVoidSentryAction : public AttackAction
{
public:
    AttackVoidSentryAction(ShadowAI* ai) : AttackAction(ai, "attack void sentry") {}
    bool Execute(Event event) override;
};

class StopAttackAction : public Action
{
public:
    StopAttackAction(ShadowAI* ai) : Action(ai, "stop attack") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class ErekemMultiplier : public Multiplier
{
    public:
        ErekemMultiplier(ShadowAI* ai) : Multiplier(ai, "erekem") {}

    public:
        virtual float GetValue(Action* action);
};

class IchoronMultiplier : public Multiplier
{
    public:
        IchoronMultiplier(ShadowAI* ai) : Multiplier(ai, "ichoron") {}

    public:
        virtual float GetValue(Action* action);
};

class ZuramatMultiplier : public Multiplier
{
    public:
        ZuramatMultiplier(ShadowAI* ai) : Multiplier(ai, "zuramat the obliterator") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonVHStrategy : public Strategy
{
public:
    WotlkDungeonVHStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "violet hold"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonVHActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonVHActionContext() {
            creators["attack erekem"] = &WotlkDungeonVHActionContext::attack_erekem;
            creators["attack ichor globule"] = &WotlkDungeonVHActionContext::attack_ichor_globule;
            creators["attack void sentry"] = &WotlkDungeonVHActionContext::attack_void_sentry;
            creators["stop attack"] = &WotlkDungeonVHActionContext::stop_attack;
        }
    private:
        static Action* attack_erekem(ShadowAI* ai) { return new AttackErekemAction(ai); }
        static Action* attack_ichor_globule(ShadowAI* ai) { return new AttackIchorGlobuleAction(ai); }
        static Action* attack_void_sentry(ShadowAI* ai) { return new AttackVoidSentryAction(ai); }
        static Action* stop_attack(ShadowAI* ai) { return new StopAttackAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonVHTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonVHTriggerContext()
        {
            creators["erekem target"] = &WotlkDungeonVHTriggerContext::erekem_target;
            creators["ichoron target"] = &WotlkDungeonVHTriggerContext::ichoron_target;
            creators["void shift"] = &WotlkDungeonVHTriggerContext::void_shift;
            creators["shroud of darkness"] = &WotlkDungeonVHTriggerContext::shroud_of_darkness;
            creators["cyanigosa positioning"] = &WotlkDungeonVHTriggerContext::cyanigosa_positioning;
        }
    private:
        static Trigger* erekem_target(ShadowAI* ai) { return new ErekemTargetTrigger(ai); }
        static Trigger* ichoron_target(ShadowAI* ai) { return new IchoronTargetTrigger(ai); }
        static Trigger* void_shift(ShadowAI* ai) { return new VoidShiftTrigger(ai); }
        static Trigger* shroud_of_darkness(ShadowAI* ai) { return new ShroudOfDarknessTrigger(ai); }
        static Trigger* cyanigosa_positioning(ShadowAI* ai) { return new CyanigosaPositioningTrigger(ai); }
};

#endif
