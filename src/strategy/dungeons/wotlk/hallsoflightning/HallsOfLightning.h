#ifndef _SHADOW_HALLSOFLIGHTNING_H
#define _SHADOW_HALLSOFLIGHTNING_H

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

enum HallsOfLightningIDs
{
    // General Bjarngrim
    NPC_STORMFORGED_LIEUTENANT      = 29240,
    SPELL_WHIRLWIND_BJARNGRIM       = 52027,

    // Ionar
    SPELL_STATIC_OVERLOAD_N         = 52658,
    SPELL_STATIC_OVERLOAD_H         = 59795,
    SPELL_BALL_LIGHTNING_N          = 52780,
    SPELL_BALL_LIGHTNING_H          = 59800,
    SPELL_DISPERSE                  = 52770,
    NPC_SPARK_OF_IONAR              = 28926,

    // Loken
    SPELL_LIGHTNING_NOVA_N          = 52960,
    SPELL_LIGHTNING_NOVA_H          = 59835,
};

#define SPELL_STATIC_OVERLOAD       DUNGEON_MODE(bot, SPELL_STATIC_OVERLOAD_N, SPELL_STATIC_OVERLOAD_H)
#define SPELL_BALL_LIGHTNING        DUNGEON_MODE(bot, SPELL_BALL_LIGHTNING_N, SPELL_BALL_LIGHTNING_H)
#define SPELL_LIGHTNING_NOVA        DUNGEON_MODE(bot, SPELL_LIGHTNING_NOVA_N, SPELL_LIGHTNING_NOVA_H)

class StormforgedLieutenantTrigger : public Trigger
{
public:
    StormforgedLieutenantTrigger(ShadowAI* ai) : Trigger(ai, "stormforged lieutenant") {}
    bool IsActive() override;
};

class BjarngrimWhirlwindTrigger : public Trigger
{
public:
    BjarngrimWhirlwindTrigger(ShadowAI* ai) : Trigger(ai, "bjarngrim whirlwind") {}
    bool IsActive() override;
};

class VolkhanTrigger : public Trigger
{
public:
    VolkhanTrigger(ShadowAI* ai) : Trigger(ai, "volkhan") {}
    bool IsActive() override;
};

class IonarStaticOverloadTrigger : public Trigger
{
public:
    IonarStaticOverloadTrigger(ShadowAI* ai) : Trigger(ai, "ionar static overload") {}
    bool IsActive() override;
};

class IonarBallLightningTrigger : public Trigger
{
public:
    IonarBallLightningTrigger(ShadowAI* ai) : Trigger(ai, "ionar ball lightning spread") {}
    bool IsActive() override;
};

class IonarTankAggroTrigger : public Trigger
{
public:
    IonarTankAggroTrigger(ShadowAI* ai) : Trigger(ai, "ionar tank aggro") {}
    bool IsActive() override;
};

class IonarDisperseTrigger : public Trigger
{
public:
    IonarDisperseTrigger(ShadowAI* ai) : Trigger(ai, "ionar disperse") {}
    bool IsActive() override;
};

class LokenRangedTrigger : public Trigger
{
public:
    LokenRangedTrigger(ShadowAI* ai) : Trigger(ai, "loken ranged") {}
    bool IsActive() override;
};

class LokenLightningNovaTrigger : public Trigger
{
public:
    LokenLightningNovaTrigger(ShadowAI* ai) : Trigger(ai, "lightning nova") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position IONAR_TANK_POSITION = Position(1078.860f, -261.928f, 61.226f);
const Position DISPERSE_POSITION = Position(1161.152f, -261.584f, 53.223f);

class BjarngrimTargetAction : public AttackAction
{
public:
    BjarngrimTargetAction(ShadowAI* ai) : AttackAction(ai, "bjarngrim target") {}
    bool Execute(Event event) override;
};

class AvoidWhirlwindAction : public MovementAction
{
public:
    AvoidWhirlwindAction(ShadowAI* ai) : MovementAction(ai, "avoid whirlwind") {}
    bool Execute(Event event) override;
};

class VolkhanTargetAction : public AttackAction
{
public:
    VolkhanTargetAction(ShadowAI* ai) : AttackAction(ai, "volkhan target") {}
    bool Execute(Event event) override;
};

class StaticOverloadSpreadAction : public MovementAction
{
public:
    StaticOverloadSpreadAction(ShadowAI* ai) : MovementAction(ai, "static overload spread") {}
    bool Execute(Event event) override;
};

class BallLightningSpreadAction : public MovementAction
{
public:
    BallLightningSpreadAction(ShadowAI* ai) : MovementAction(ai, "ball lightning spread") {}
    bool Execute(Event event) override;
};

class IonarTankPositionAction : public MovementAction
{
public:
    IonarTankPositionAction(ShadowAI* ai) : MovementAction(ai, "ionar tank position") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class DispersePositionAction : public MovementAction
{
public:
    DispersePositionAction(ShadowAI* ai) : MovementAction(ai, "disperse position") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class LokenStackAction : public MovementAction
{
public:
    LokenStackAction(ShadowAI* ai) : MovementAction(ai, "loken stack") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class AvoidLightningNovaAction : public MovementAction
{
public:
    AvoidLightningNovaAction(ShadowAI* ai) : MovementAction(ai, "avoid lightning nova") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class BjarngrimMultiplier : public Multiplier
{
    public:
        BjarngrimMultiplier(ShadowAI* ai) : Multiplier(ai, "general bjarngrim") {}

    public:
        virtual float GetValue(Action* action);
};

class VolkhanMultiplier : public Multiplier
{
    public:
        VolkhanMultiplier(ShadowAI* ai) : Multiplier(ai, "volkhan") {}

    public:
        virtual float GetValue(Action* action);
};

class IonarMultiplier : public Multiplier
{
    public:
        IonarMultiplier(ShadowAI* ai) : Multiplier(ai, "ionar") {}

    public:
        virtual float GetValue(Action* action);
};

class LokenMultiplier : public Multiplier
{
    public:
        LokenMultiplier(ShadowAI* ai) : Multiplier(ai, "loken") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonHoLStrategy : public Strategy
{
public:
    WotlkDungeonHoLStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "halls of lightning"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonHoLActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonHoLActionContext() {
            creators["bjarngrim target"] = &WotlkDungeonHoLActionContext::bjarngrim_target;
            creators["avoid whirlwind"] = &WotlkDungeonHoLActionContext::avoid_whirlwind;
            creators["volkhan target"] = &WotlkDungeonHoLActionContext::volkhan_target;
            creators["static overload spread"] = &WotlkDungeonHoLActionContext::static_overload_spread;
            creators["ball lightning spread"] = &WotlkDungeonHoLActionContext::ball_lightning_spread;
            creators["ionar tank position"] = &WotlkDungeonHoLActionContext::ionar_tank_position;
            creators["disperse position"] = &WotlkDungeonHoLActionContext::disperse_position;
            creators["loken stack"] = &WotlkDungeonHoLActionContext::loken_stack;
            creators["avoid lightning nova"] = &WotlkDungeonHoLActionContext::avoid_lightning_nova;
        }
    private:
        static Action* bjarngrim_target(ShadowAI* ai) { return new BjarngrimTargetAction(ai); }
        static Action* avoid_whirlwind(ShadowAI* ai) { return new AvoidWhirlwindAction(ai); }
        static Action* volkhan_target(ShadowAI* ai) { return new VolkhanTargetAction(ai); }
        static Action* static_overload_spread(ShadowAI* ai) { return new StaticOverloadSpreadAction(ai); }
        static Action* ball_lightning_spread(ShadowAI* ai) { return new BallLightningSpreadAction(ai); }
        static Action* ionar_tank_position(ShadowAI* ai) { return new IonarTankPositionAction(ai); }
        static Action* disperse_position(ShadowAI* ai) { return new DispersePositionAction(ai); }
        static Action* loken_stack(ShadowAI* ai) { return new LokenStackAction(ai); }
        static Action* avoid_lightning_nova(ShadowAI* ai) { return new AvoidLightningNovaAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonHoLTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonHoLTriggerContext()
        {
            creators["stormforged lieutenant"] = &WotlkDungeonHoLTriggerContext::stormforged_lieutenant;
            creators["whirlwind"] = &WotlkDungeonHoLTriggerContext::bjarngrim_whirlwind;
            creators["volkhan"] = &WotlkDungeonHoLTriggerContext::volkhan;
            creators["static overload"] = &WotlkDungeonHoLTriggerContext::static_overload;
            creators["ball lightning"] = &WotlkDungeonHoLTriggerContext::ball_lightning;
            creators["ionar tank aggro"] = &WotlkDungeonHoLTriggerContext::ionar_tank_aggro;
            creators["ionar disperse"] = &WotlkDungeonHoLTriggerContext::ionar_disperse;
            creators["loken ranged"] = &WotlkDungeonHoLTriggerContext::loken_ranged;
            creators["lightning nova"] = &WotlkDungeonHoLTriggerContext::lightning_nova;
        }
    private:
        static Trigger* stormforged_lieutenant(ShadowAI* ai) { return new StormforgedLieutenantTrigger(ai); }
        static Trigger* bjarngrim_whirlwind(ShadowAI* ai) { return new BjarngrimWhirlwindTrigger(ai); }
        static Trigger* volkhan(ShadowAI* ai) { return new VolkhanTrigger(ai); }
        static Trigger* static_overload(ShadowAI* ai) { return new IonarStaticOverloadTrigger(ai); }
        static Trigger* ball_lightning(ShadowAI* ai) { return new IonarBallLightningTrigger(ai); }
        static Trigger* ionar_tank_aggro(ShadowAI* ai) { return new IonarTankAggroTrigger(ai); }
        static Trigger* ionar_disperse(ShadowAI* ai) { return new IonarDisperseTrigger(ai); }
        static Trigger* loken_ranged(ShadowAI* ai) { return new LokenRangedTrigger(ai); }
        static Trigger* lightning_nova(ShadowAI* ai) { return new LokenLightningNovaTrigger(ai); }
};

#endif
