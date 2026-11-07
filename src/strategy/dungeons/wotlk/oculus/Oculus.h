#ifndef _SHADOW_OCULUS_H
#define _SHADOW_OCULUS_H

#include "Trigger.h"
#include "ShadowAIConfig.h"
#include "GenericTriggers.h"
#include "DungeonStrategyUtils.h"
#include "Action.h"
#include "AttackAction.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "UseItemAction.h"
#include "GenericSpellActions.h"
#include "Multiplier.h"
#include "Unit.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum OculusIDs
{
    // Drakos the Interrogator
    NPC_UNSTABLE_SPHERE             = 28166,
    SPELL_UNSTABLE_SPHERE_PASSIVE   = 50756,
    SPELL_UNSTABLE_SPHERE_PULSE     = 50757,
    SPELL_UNSTABLE_SPHERE_TIMER     = 50758,

    // Drakes
    NPC_AMBER_DRAKE                 = 27755,
    NPC_EMERALD_DRAKE               = 27692,
    NPC_RUBY_DRAKE                  = 27756,
    ITEM_AMBER_ESSENCE              = 37859,
    ITEM_EMERALD_ESSENCE            = 37815,
    ITEM_RUBY_ESSENCE               = 37860,
    SPELL_AMBER_ESSENCE             = 49461,
    SPELL_EMERALD_ESSENCE           = 49345,
    SPELL_RUBY_ESSENCE              = 49462,
    // Abilities:
    // Amber
    SPELL_SHOCK_LANCE               = 49840,
    SPELL_SHOCK_CHARGE              = 49836,
    SPELL_STOP_TIME                 = 49838,
    SPELL_TEMPORAL_RIFT             = 49592,
    // Emerald
    SPELL_LEECHING_POISON           = 50328,
    SPELL_TOUCH_THE_NIGHTMARE       = 50341,
    SPELL_DREAM_FUNNEL              = 50344,
    // Ruby
    SPELL_SEARING_WRATH             = 50232,
    SPELL_EVASIVE_MANEUVERS         = 50240,
    SPELL_EVASIVE_CHARGES           = 50241,
    SPELL_MARTYR                    = 50253,

    // Varos Cloudstrider
    NPC_CENTRIFUGE_CORE             = 28183,

    // Mage-Lord Urom
    NPC_MAGE_LORD_UROM              = 27655,
    SPELL_TIME_BOMB_N               = 51121,
    SPELL_TIME_BOMB_H               = 59376,
    SPELL_EMPOWERED_ARCANE_EXPLOSION_N = 51110,
    SPELL_EMPOWERED_ARCANE_EXPLOSION_H = 59377,

    // Ley-Guardian Eregos
    SPELL_ENRAGED_ASSAULT           = 51170,
    SPELL_PLANAR_SHIFT              = 51162,
};

#define SPELL_EMPOWERED_ARCANE_EXPLOSION        DUNGEON_MODE(bot, SPELL_EMPOWERED_ARCANE_EXPLOSION_N, SPELL_EMPOWERED_ARCANE_EXPLOSION_H)
#define SPELL_TIME_BOMB                         DUNGEON_MODE(bot, SPELL_TIME_BOMB_N, SPELL_TIME_BOMB_H)

const std::vector<uint32> DRAKE_ITEMS = {ITEM_AMBER_ESSENCE, ITEM_EMERALD_ESSENCE, ITEM_RUBY_ESSENCE};
const std::vector<uint32> DRAKE_SPELLS = {SPELL_AMBER_ESSENCE, SPELL_EMERALD_ESSENCE, SPELL_RUBY_ESSENCE};
const uint32 OCULUS_MAP_ID = 578;

// const float uromCoords[4][4] =
// {
//     {1177.47f, 937.722f, 527.405f, 2.21657f},
//     {968.66f, 1042.53f, 527.32f, 0.077f},
//     {1164.02f, 1170.85f, 527.321f, 3.66f},
//     {1118.31f, 1080.377f, 508.361f, 4.25f}      // Inner ring, actual boss fight
// };

class DrakosUnstableSphereTrigger : public Trigger
{
public:
    DrakosUnstableSphereTrigger(ShadowAI* ai) : Trigger(ai, "drakos unstable sphere") {}
    bool IsActive() override;
};

class DrakeMountTrigger : public Trigger
{
public:
    DrakeMountTrigger(ShadowAI* ai) : Trigger(ai, "drake mount") {}
    bool IsActive() override;
};

class DrakeDismountTrigger : public Trigger
{
public:
    DrakeDismountTrigger(ShadowAI* ai) : Trigger(ai, "drake dismount") {}
    bool IsActive() override;
};

class GroupFlyingTrigger : public Trigger
{
public:
    GroupFlyingTrigger(ShadowAI* ai) : Trigger(ai, "drake fly") {}
    bool IsActive() override;
};

class DrakeCombatTrigger : public Trigger
{
public:
    DrakeCombatTrigger(ShadowAI* ai) : Trigger(ai, "drake combat") {}
    bool IsActive() override;
};

class VarosCloudstriderTrigger : public Trigger
{
public:
    VarosCloudstriderTrigger(ShadowAI* ai) : Trigger(ai, "varos cloudstrider") {}
    bool IsActive() override;
};

class UromArcaneExplosionTrigger : public Trigger
{
public:
    UromArcaneExplosionTrigger(ShadowAI* ai) : Trigger(ai, "urom arcane explosion") {}
    bool IsActive() override;
};

class UromTimeBombTrigger : public Trigger
{
public:
    UromTimeBombTrigger(ShadowAI* ai) : Trigger(ai, "urom time bomb") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position uromSafePositions[3] =
{
    Position(1138.88f, 1052.22f, 508.36f),
    Position(1084.62f, 1079.71f, 508.36f),
    Position(1087.42f, 1020.132f, 508.36f)
};

class AvoidUnstableSphereAction : public MovementAction
{
public:
    AvoidUnstableSphereAction(ShadowAI* ai) : MovementAction(ai, "avoid unstable sphere") {}
    bool Execute(Event event) override;
};

class MountDrakeAction : public UseItemAction
{
public:
    MountDrakeAction(ShadowAI* ai) : UseItemAction(ai, "mount drake") {}
    bool Execute(Event event) override;
    bool isPossible() override;
};

class DismountDrakeAction : public Action
{
public:
    DismountDrakeAction(ShadowAI* ai) : Action(ai, "dismount drake") {}
    bool Execute(Event event) override;
};

class OccFlyDrakeAction : public MovementAction
{
public:
    OccFlyDrakeAction(ShadowAI* ai) : MovementAction(ai, "occ fly drake") {}
    bool Execute(Event event) override;
};

class OccDrakeAttackAction : public Action
{
public:
    OccDrakeAttackAction(ShadowAI* botAI) : Action(botAI, "occ drake attack") {}
    bool Execute(Event event) override;

protected:
    Unit* vehicleBase;
    bool CastDrakeSpellAction(Unit* target, uint32 spellId, uint32 cooldown);
    bool AmberDrakeAction(Unit* target);
    bool EmeraldDrakeAction(Unit* target);
    bool RubyDrakeAction(Unit* target);
};

class AvoidArcaneExplosionAction : public MovementAction
{
public:
    AvoidArcaneExplosionAction(ShadowAI* ai) : MovementAction(ai, "avoid arcane explosion") {}
    bool Execute(Event event) override;
};

class TimeBombSpreadAction : public MovementAction
{
public:
    TimeBombSpreadAction(ShadowAI* ai) : MovementAction(ai, "time bomb spread") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const float uromCoords[4][4] =
{   // Platform coordinates
    {1177.47f, 937.722f, 527.405f, 2.21657f},
    {968.66f, 1042.53f, 527.32f, 0.077f},
    {1164.02f, 1170.85f, 527.321f, 3.66f},
    {1118.31f, 1080.377f, 508.361f, 4.25f}      // Inner ring, actual boss fight
};

class MountingDrakeMultiplier : public Multiplier
{
    public:
        MountingDrakeMultiplier(ShadowAI* ai) : Multiplier(ai, "mounting drake") {}

    public:
        virtual float GetValue(Action* action);
};

class OccFlyingMultiplier : public Multiplier
{
    public:
        OccFlyingMultiplier(ShadowAI* ai) : Multiplier(ai, "occ flying drake") {}

    public:
        virtual float GetValue(Action* action);
};

class UromMultiplier : public Multiplier
{
    public:
        UromMultiplier(ShadowAI* ai) : Multiplier(ai, "mage-lord urom") {}

    public:
        virtual float GetValue(Action* action);
    protected:
        uint8 GetPhaseByCurrentPosition(Unit* boss);
};

class EregosMultiplier : public Multiplier
{
    public:
        EregosMultiplier(ShadowAI* ai) : Multiplier(ai, "ley-guardian eregos") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonOccStrategy : public Strategy
{
public:
    WotlkDungeonOccStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "oculus"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonOccActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonOccActionContext() {
            creators["avoid unstable sphere"] = &WotlkDungeonOccActionContext::avoid_unstable_sphere;
            creators["mount drake"] = &WotlkDungeonOccActionContext::mount_drake;
            creators["dismount drake"] = &WotlkDungeonOccActionContext::dismount_drake;
            creators["occ fly drake"] = &WotlkDungeonOccActionContext::occ_fly_drake;
            creators["occ drake attack"] = &WotlkDungeonOccActionContext::occ_drake_attack;
            creators["avoid arcane explosion"] = &WotlkDungeonOccActionContext::avoid_arcane_explosion;
            creators["time bomb spread"] = &WotlkDungeonOccActionContext::time_bomb_spread;
        }
    private:
        static Action* avoid_unstable_sphere(ShadowAI* ai) { return new AvoidUnstableSphereAction(ai); }
        static Action* mount_drake(ShadowAI* ai) { return new MountDrakeAction(ai); }
        static Action* dismount_drake(ShadowAI* ai) { return new DismountDrakeAction(ai); }
        static Action* occ_fly_drake(ShadowAI* ai) { return new OccFlyDrakeAction(ai); }
        static Action* occ_drake_attack(ShadowAI* ai) { return new OccDrakeAttackAction(ai); }
        static Action* avoid_arcane_explosion(ShadowAI* ai) { return new AvoidArcaneExplosionAction(ai); }
        static Action* time_bomb_spread(ShadowAI* ai) { return new TimeBombSpreadAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonOccTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonOccTriggerContext()
        {
            creators["unstable sphere"] = &WotlkDungeonOccTriggerContext::unstable_sphere;
            creators["drake mount"] = &WotlkDungeonOccTriggerContext::drake_mount;
            creators["drake dismount"] = &WotlkDungeonOccTriggerContext::drake_dismount;
            creators["group flying"] = &WotlkDungeonOccTriggerContext::group_flying;
            creators["drake combat"] = &WotlkDungeonOccTriggerContext::drake_combat;
            creators["varos cloudstrider"] = &WotlkDungeonOccTriggerContext::varos_cloudstrider;
            creators["arcane explosion"] = &WotlkDungeonOccTriggerContext::arcane_explosion;
            creators["time bomb"] = &WotlkDungeonOccTriggerContext::time_bomb;
        }
    private:
        static Trigger* unstable_sphere(ShadowAI* ai) { return new DrakosUnstableSphereTrigger(ai); }
        static Trigger* drake_mount(ShadowAI* ai) { return new DrakeMountTrigger(ai); }
        static Trigger* drake_dismount(ShadowAI* ai) { return new DrakeDismountTrigger(ai); }
        static Trigger* group_flying(ShadowAI* ai) { return new GroupFlyingTrigger(ai); }
        static Trigger* drake_combat(ShadowAI* ai) { return new DrakeCombatTrigger(ai); }
        static Trigger* varos_cloudstrider(ShadowAI* ai) { return new VarosCloudstriderTrigger(ai); }
        static Trigger* arcane_explosion(ShadowAI* ai) { return new UromArcaneExplosionTrigger(ai); }
        static Trigger* time_bomb(ShadowAI* ai) { return new UromTimeBombTrigger(ai); }
};

#endif
