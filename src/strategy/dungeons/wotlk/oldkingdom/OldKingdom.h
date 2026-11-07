#ifndef _SHADOW_OLDKINGDOM_H
#define _SHADOW_OLDKINGDOM_H

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

enum OldKingdomIDs
{
    // Elder Nadox
    BUFF_GUARDIAN_AURA                 = 56153,

    // Jedoga Shadowseeker
    NPC_TWILIGHT_VOLUNTEER             = 30385,

    // Forgotten One(s)
    SPELL_SHADOW_CRASH_N               = 60833,
    SPELL_SHADOW_CRASH_H               = 60848,
};

#define SPELL_SHADOW_CRASH             DUNGEON_MODE(bot, SPELL_SHADOW_CRASH_N, SPELL_SHADOW_CRASH_H)

class NadoxGuardianTrigger : public Trigger
{
public:
    NadoxGuardianTrigger(ShadowAI* ai) : Trigger(ai, "elder nadox guardian") {}
    bool IsActive() override;
};

class JedogaVolunteerTrigger : public Trigger
{
public:
    JedogaVolunteerTrigger(ShadowAI* ai) : Trigger(ai, "jedoga volunteer") {}
    bool IsActive() override;
};

class ShadowCrashTrigger : public Trigger
{
public:
    ShadowCrashTrigger(ShadowAI* ai) : Trigger(ai, "shadow crash") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class AttackNadoxGuardianAction : public AttackAction
{
public:
    AttackNadoxGuardianAction(ShadowAI* ai) : AttackAction(ai, "attack nadox guardian") {}
    bool Execute(Event event) override;
};

class AttackJedogaVolunteerAction : public AttackAction
{
public:
    AttackJedogaVolunteerAction(ShadowAI* ai) : AttackAction(ai, "attack jedoga volunteer") {}
    bool Execute(Event event) override;
};

class AvoidShadowCrashAction : public MovementAction
{
public:
    AvoidShadowCrashAction(ShadowAI* ai) : MovementAction(ai, "avoid shadow crash") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class ElderNadoxMultiplier : public Multiplier
{
    public:
        ElderNadoxMultiplier(ShadowAI* ai) : Multiplier(ai, "elder nadox") {}

    public:
        virtual float GetValue(Action* action);
};

class JedogaShadowseekerMultiplier : public Multiplier
{
    public:
        JedogaShadowseekerMultiplier(ShadowAI* ai) : Multiplier(ai, "jedoga shadowseeker") {}

    public:
        virtual float GetValue(Action* action);
};

class ForgottenOneMultiplier : public Multiplier
{
    public:
        ForgottenOneMultiplier(ShadowAI* ai) : Multiplier(ai, "forgotten one") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonOKStrategy : public Strategy
{
public:
    WotlkDungeonOKStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "old kingdom"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonOKActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonOKActionContext() {
            creators["attack nadox guardian"] = &WotlkDungeonOKActionContext::attack_nadox_guardian;
            creators["attack jedoga volunteer"] = &WotlkDungeonOKActionContext::attack_jedoga_volunteer;
            creators["avoid shadow crash"] = &WotlkDungeonOKActionContext::avoid_shadow_crash;
        }
    private:
        static Action* attack_nadox_guardian(ShadowAI* ai) { return new AttackNadoxGuardianAction(ai); }
        static Action* attack_jedoga_volunteer(ShadowAI* ai) { return new AttackJedogaVolunteerAction(ai); }
        static Action* avoid_shadow_crash(ShadowAI* ai) { return new AvoidShadowCrashAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonOKTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonOKTriggerContext()
        {
            creators["nadox guardian"] = &WotlkDungeonOKTriggerContext::nadox_guardian;
            creators["jedoga volunteer"] = &WotlkDungeonOKTriggerContext::jedoga_volunteer;
            creators["shadow crash"] = &WotlkDungeonOKTriggerContext::shadow_crash;
        }
    private:
        static Trigger* nadox_guardian(ShadowAI* ai) { return new NadoxGuardianTrigger(ai); }
        static Trigger* jedoga_volunteer(ShadowAI* ai) { return new JedogaVolunteerTrigger(ai); }
        static Trigger* shadow_crash(ShadowAI* ai) { return new ShadowCrashTrigger(ai); }
};

#endif
