#ifndef _SHADOW_FORGEOFSOULS_H
#define _SHADOW_FORGEOFSOULS_H

#include "Trigger.h"
#include "ShadowAIConfig.h"
#include "GenericTriggers.h"
#include "DungeonStrategyUtils.h"
#include "Action.h"
#include "AttackAction.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "Multiplier.h"
#include "Strategy.h"
#include "NamedObjectContext.h"
#include "AiObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum ForgeOfSoulsBronjahmIDs
{
    // Bronjahm
    NPC_CORRUPTED_SOUL_FRAGMENT = 36535,

    SPELL_CORRUPT_SOUL          = 68839,
    SPELL_SOULSTORM_VISUAL      = 68870,
    SPELL_SOULSTORM_VISUAL2     = 68904,
    SPELL_SOULSTORM             = 68872,

    // Devourer of Souls
    SPELL_WAILING_SOULS         = 68899,
};

class MoveFromBronjahmTrigger : public Trigger
{
public:
    MoveFromBronjahmTrigger(ShadowAI* ai) : Trigger(ai, "move from bronjahm") {}

    bool IsActive() override;
};

class SwitchToSoulFragment : public Trigger
{
public:
    SwitchToSoulFragment(ShadowAI* ai) : Trigger(ai, "switch to soul fragment") {}

    bool IsActive() override;
};

class BronjahmPositionTrigger : public Trigger
{
public:
    BronjahmPositionTrigger(ShadowAI* ai) : Trigger(ai, "bronjahm position") {}
    bool IsActive() override;
};

class DevourerOfSoulsTrigger : public Trigger
{
public:
    DevourerOfSoulsTrigger(ShadowAI* ai) : Trigger(ai, "devourer of souls") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position BRONJAHM_TANK_POSITION = Position(5297.920f, 2506.698f, 686.068f);

class MoveFromBronjahmAction : public MovementAction
{
public:
    MoveFromBronjahmAction(ShadowAI* ai) : MovementAction(ai, "move from bronjahm") {}
    bool Execute(Event event) override;
};

class AttackCorruptedSoulFragmentAction : public AttackAction
{
public:
    AttackCorruptedSoulFragmentAction(ShadowAI* ai) : AttackAction(ai, "attack corrupted soul fragment") {}
    bool Execute(Event event) override;
};

class BronjahmGroupPositionAction : public AttackAction
{
public:
    BronjahmGroupPositionAction(ShadowAI* ai) : AttackAction(ai, "bronjahm group position") {}

    bool Execute(Event event) override;

    bool isUseful() override;
};

class DevourerOfSoulsAction : public AttackAction
{
public:
    DevourerOfSoulsAction(ShadowAI * ai, float distance = 10.0f, float delta_angle = M_PI / 8)
        : AttackAction(ai, "devourer of souls")
    {
        this->distance = distance;
        this->delta_angle = delta_angle;
    }
    virtual bool Execute(Event event);

protected:
    float distance, delta_angle;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class BronjahmMultiplier : public Multiplier
{
    public:
    BronjahmMultiplier(ShadowAI* ai) : Multiplier(ai, "bronjahm") {}

    public:
        virtual float GetValue(Action* action);
};

class AttackFragmentMultiplier : public Multiplier
{
public:
    AttackFragmentMultiplier(ShadowAI* ai) : Multiplier(ai, "attack fragment") { }

    float GetValue(Action* action) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonFoSStrategy : public Strategy
{
public:
    WotlkDungeonFoSStrategy(ShadowAI* ai) : Strategy(ai) {}
    std::string const getName() override { return "forge of souls"; }
    void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    void InitMultipliers(std::vector<Multiplier*> &multipliers) override;

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonFoSActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonFoSActionContext()
        {
            creators["move from bronjahm"] = &WotlkDungeonFoSActionContext::move_from_bronjahm;
            creators["attack corrupted soul fragment"] = &WotlkDungeonFoSActionContext::attack_corrupted_soul_fragment;
            creators["bronjahm group position"] = &WotlkDungeonFoSActionContext::bronjahm_group_position;
            creators["devourer of souls"] = &WotlkDungeonFoSActionContext::devourer_of_souls;
        }
    private:
        static Action* move_from_bronjahm(ShadowAI* ai) { return new MoveFromBronjahmAction(ai); }
        static Action* attack_corrupted_soul_fragment(ShadowAI* ai) { return new AttackCorruptedSoulFragmentAction(ai); }
        static Action* bronjahm_group_position(ShadowAI* ai) { return new BronjahmGroupPositionAction(ai); }
        static Action* devourer_of_souls(ShadowAI* ai) { return new DevourerOfSoulsAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonFoSTriggerContext : public NamedObjectContext<Trigger>
{
public:
    WotlkDungeonFoSTriggerContext()
    {
        creators["bronjahm position"] = &WotlkDungeonFoSTriggerContext::bronjahm_position;
        creators["move from bronjahm"] = &WotlkDungeonFoSTriggerContext::move_from_bronjahm;
        creators["switch to soul fragment"] = &WotlkDungeonFoSTriggerContext::switch_to_soul_fragment;
        creators["devourer of souls"] = &WotlkDungeonFoSTriggerContext::devourer_of_souls;
    }

private:
    static Trigger* move_from_bronjahm(ShadowAI* ai) { return new MoveFromBronjahmTrigger(ai); }
    static Trigger* switch_to_soul_fragment(ShadowAI* ai) { return new SwitchToSoulFragment(ai); }
    static Trigger* bronjahm_position(ShadowAI* ai) { return new BronjahmPositionTrigger(ai); }
    static Trigger* devourer_of_souls(ShadowAI* ai) { return new DevourerOfSoulsTrigger(ai); }
};

#endif
