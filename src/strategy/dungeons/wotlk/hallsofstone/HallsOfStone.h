#ifndef _SHADOW_HALLSOFSTONE_H
#define _SHADOW_HALLSOFSTONE_H

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

enum HallsOfStoneIDs
{
    // Krystallus
    SPELL_GROUND_SLAM               = 50827,
    DEBUFF_GROUND_SLAM              = 50833,

    // Sjonnir The Ironshaper
    SPELL_LIGHTNING_RING_N          = 50840,
    SPELL_LIGHTNING_RING_H          = 59848,
};

#define SPELL_LIGHTNING_RING        DUNGEON_MODE(bot, SPELL_LIGHTNING_RING_N, SPELL_LIGHTNING_RING_H)

class KrystallusGroundSlamTrigger : public Trigger
{
public:
    KrystallusGroundSlamTrigger(ShadowAI* ai) : Trigger(ai, "krystallus ground slam") {}
    bool IsActive() override;
};

class SjonnirLightningRingTrigger : public Trigger
{
public:
    SjonnirLightningRingTrigger(ShadowAI* ai) : Trigger(ai, "sjonnir lightning ring") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class ShatterSpreadAction : public MovementAction
{
public:
    ShatterSpreadAction(ShadowAI* ai) : MovementAction(ai, "shatter spread") {}
    bool Execute(Event event) override;
};

class AvoidLightningRingAction : public MovementAction
{
public:
    AvoidLightningRingAction(ShadowAI* ai) : MovementAction(ai, "avoid lightning ring") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class KrystallusMultiplier : public Multiplier
{
    public:
        KrystallusMultiplier(ShadowAI* ai) : Multiplier(ai, "krystallus") {}

    public:
        virtual float GetValue(Action* action);
};

class SjonnirMultiplier : public Multiplier
{
    public:
        SjonnirMultiplier(ShadowAI* ai) : Multiplier(ai, "sjonnir the ironshaper") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonHoSStrategy : public Strategy
{
public:
    WotlkDungeonHoSStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "halls of stone"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonHoSActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonHoSActionContext() {
            creators["shatter spread"] = &WotlkDungeonHoSActionContext::shatter_spread;
            creators["avoid lightning ring"] = &WotlkDungeonHoSActionContext::avoid_lightning_ring;
        }
    private:
        static Action* shatter_spread(ShadowAI* ai) { return new ShatterSpreadAction(ai); }
        static Action* avoid_lightning_ring(ShadowAI* ai) { return new AvoidLightningRingAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonHoSTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonHoSTriggerContext()
        {
            creators["ground slam"] = &WotlkDungeonHoSTriggerContext::ground_slam;
            creators["lightning ring"] = &WotlkDungeonHoSTriggerContext::lightning_ring;
        }
    private:
        static Trigger* ground_slam(ShadowAI* ai) { return new KrystallusGroundSlamTrigger(ai); }
        static Trigger* lightning_ring(ShadowAI* ai) { return new SjonnirLightningRingTrigger(ai); }
};

#endif
