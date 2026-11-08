#ifndef _SHADOW_RAIDKARAZHAN_H
#define _SHADOW_RAIDKARAZHAN_H

#include "AiObject.h"
#include "Shadows.h"
#include "Position.h"
#include "Trigger.h"
#include "Action.h"
#include "MovementActions.h"
#include "Multiplier.h"
#include "Strategy.h"
#include "NamedObjectContext.h"
#include "AiObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum KarazhanSpells
{
    // Maiden of Virtue
    SPELL_REPENTANCE                 = 29511,

    // Opera Event
    SPELL_LITTLE_RED_RIDING_HOOD     = 30756,

    // Shade of Aran
    SPELL_FLAME_WREATH               = 30004,
    SPELL_AURA_FLAME_WREATH          = 29946,
    SPELL_ARCANE_EXPLOSION           = 29973,
    SPELL_WARLOCK_BANISH             = 18647, // Rank 2

    // Netherspite
    SPELL_GREEN_BEAM_DEBUFF          = 30422,
    SPELL_BLUE_BEAM_DEBUFF           = 30423,
    SPELL_NETHER_EXHAUSTION_RED      = 38637,
    SPELL_NETHER_EXHAUSTION_GREEN    = 38638,
    SPELL_NETHER_EXHAUSTION_BLUE     = 38639,
    SPELL_NETHERSPITE_BANISHED       = 39833,

    // Prince Malchezaar
    SPELL_ENFEEBLE                   = 30843,
};

enum KarazhanNpcs
{
    // Attumen the Huntsman
    NPC_ATTUMEN_THE_HUNTSMAN_MOUNTED = 16152,

    // Terestian Illhoof
    NPC_KILREK                       = 17229,
    NPC_DEMON_CHAINS                 = 17248,

    // Shade of Aran
    NPC_CONJURED_ELEMENTAL           = 17167,

    // Netherspite
    NPC_VOID_ZONE                    = 16697,
    NPC_RED_PORTAL                   = 17369,
    NPC_BLUE_PORTAL                  = 17368,
    NPC_GREEN_PORTAL                 = 17367,

    // Prince Malchezaar
    NPC_NETHERSPITE_INFERNAL         = 17646,
};

extern const Position KARAZHAN_MAIDEN_OF_VIRTUE_BOSS_POSITION;
extern const Position KARAZHAN_MAIDEN_OF_VIRTUE_RANGED_POSITION[8];
extern const Position KARAZHAN_BIG_BAD_WOLF_BOSS_POSITION;
extern const Position KARAZHAN_BIG_BAD_WOLF_RUN_POSITION[4];
extern const Position KARAZHAN_THE_CURATOR_BOSS_POSITION;

class RaidKarazhanHelpers : public AiObject
{
public:
    explicit RaidKarazhanHelpers(ShadowAI* botAI) : AiObject(botAI) {}

    void MarkTargetWithSkull(Unit* /*target*/);
    Unit* GetFirstAliveUnit(const std::vector<Unit*>& /*units*/);
    Unit* GetFirstAliveUnitByEntry(uint32 /*entry*/);
    Unit* GetNearestPlayerInRadius(float /*radius*/ = 5.0f);
    bool IsFlameWreathActive();
    Position GetPositionOnBeam(Unit* boss, Unit* portal, float distanceFromBoss);
    std::vector<Player*> GetRedBlockers();
    std::vector<Player*> GetBlueBlockers();
    std::vector<Player*> GetGreenBlockers();
    std::tuple<Player*, Player*, Player*> GetCurrentBeamBlockers();
    std::vector<Unit*> GetAllVoidZones();
    bool IsSafePosition (float x, float y, float z,
         const std::vector<Unit*>& hazards, float hazardRadius);
    std::vector<Unit*> GetSpawnedInfernals() const;
    bool IsStraightPathSafe(const Position& start, const Position& target, 
         const std::vector<Unit*>& hazards, float hazardRadius, float stepSize);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class KarazhanAttumenTheHuntsmanTrigger : public Trigger
{
public:
    KarazhanAttumenTheHuntsmanTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan attumen the huntsman") {}
    bool IsActive() override;
};

class KarazhanMoroesTrigger : public Trigger
{
public:
    KarazhanMoroesTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan moroes") {}
    bool IsActive() override;
};

class KarazhanMaidenOfVirtueTrigger : public Trigger
{
public:
    KarazhanMaidenOfVirtueTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan maiden of virtue") {}
    bool IsActive() override;
};

class KarazhanBigBadWolfTrigger : public Trigger
{
public:
    KarazhanBigBadWolfTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan big bad wolf") {}
    bool IsActive() override;
};

class KarazhanRomuloAndJulianneTrigger : public Trigger
{
public:
    KarazhanRomuloAndJulianneTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan romulo and julianne") {}
    bool IsActive() override;
};

class KarazhanWizardOfOzTrigger : public Trigger
{
public:
    KarazhanWizardOfOzTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan wizard of oz") {}
    bool IsActive() override;
};

class KarazhanTheCuratorTrigger : public Trigger
{
public:
    KarazhanTheCuratorTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan the curator") {}
    bool IsActive() override;
};

class KarazhanTerestianIllhoofTrigger : public Trigger
{
public:
    KarazhanTerestianIllhoofTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan terestian illhoof") {}
    bool IsActive() override;
};

class KarazhanShadeOfAranTrigger : public Trigger
{
public:
    KarazhanShadeOfAranTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan shade of aran") {}
    bool IsActive() override;
};

class KarazhanNetherspiteTrigger : public Trigger
{
public:
    KarazhanNetherspiteTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan netherspite") {}
    bool IsActive() override;
};

class KarazhanPrinceMalchezaarTrigger : public Trigger
{
public:
    KarazhanPrinceMalchezaarTrigger(ShadowAI* botAI) : Trigger(botAI, "karazhan prince malchezaar") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class KarazhanAttumenTheHuntsmanStackBehindAction : public MovementAction
{
public:
    KarazhanAttumenTheHuntsmanStackBehindAction(ShadowAI* botAI, std::string const name = "karazhan attumen the huntsman stack behind") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanMoroesMarkTargetAction : public Action
{
public:
    KarazhanMoroesMarkTargetAction(ShadowAI* botAI, std::string const name = "karazhan moroes mark target") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanMaidenOfVirtuePositionBossAction : public MovementAction
{
public:
    KarazhanMaidenOfVirtuePositionBossAction(ShadowAI* botAI, std::string const name = "karazhan maiden of virtue position boss") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanMaidenOfVirtuePositionRangedAction : public MovementAction
{
public:
    KarazhanMaidenOfVirtuePositionRangedAction(ShadowAI* botAI, std::string const name = "karazhan maiden of virtue position ranged") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanBigBadWolfPositionBossAction : public MovementAction
{
public:
    KarazhanBigBadWolfPositionBossAction(ShadowAI* botAI, std::string const name = "karazhan big bad wolf position boss") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanBigBadWolfRunAwayAction : public MovementAction
{
public:
    KarazhanBigBadWolfRunAwayAction(ShadowAI* botAI, std::string const name = "karazhan big bad wolf run away") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;

private:
    size_t currentIndex = 0;
};

class KarazhanRomuloAndJulianneMarkTargetAction : public Action
{
public:
    KarazhanRomuloAndJulianneMarkTargetAction(ShadowAI* botAI, std::string const name = "karazhan romulo and julianne mark target") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanWizardOfOzMarkTargetAction : public Action
{
public:
    KarazhanWizardOfOzMarkTargetAction(ShadowAI* botAI, std::string const name = "karazhan wizard of oz mark target") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanWizardOfOzScorchStrawmanAction : public Action
{
public:
    KarazhanWizardOfOzScorchStrawmanAction(ShadowAI* botAI, std::string const name = "karazhan wizard of oz scorch strawman") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanTheCuratorMarkTargetAction : public Action
{
public:
    KarazhanTheCuratorMarkTargetAction(ShadowAI* botAI, std::string const name = "karazhan the curator mark target") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanTheCuratorPositionBossAction : public MovementAction
{
public:
    KarazhanTheCuratorPositionBossAction(ShadowAI* botAI, std::string const name = "karazhan the curator position boss") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanTheCuratorSpreadRangedAction : public MovementAction
{
public:
    KarazhanTheCuratorSpreadRangedAction(ShadowAI* botAI, std::string const name = "karazhan the curator spread ranged") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanTerestianIllhoofMarkTargetAction : public Action
{
public:
    KarazhanTerestianIllhoofMarkTargetAction(ShadowAI* botAI, std::string const name = "karazhan terestian illhoof mark target") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanShadeOfAranArcaneExplosionRunAwayAction : public MovementAction
{
public:
    KarazhanShadeOfAranArcaneExplosionRunAwayAction(ShadowAI* botAI, std::string const name = "karazhan shade of aran arcane explosion run away") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanShadeOfAranFlameWreathStopMovementAction : public MovementAction
{
public:
    KarazhanShadeOfAranFlameWreathStopMovementAction(ShadowAI* botAI, std::string const name = "karazhan shade of aran flame wreath stop bot") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanShadeOfAranMarkConjuredElementalAction : public Action
{
public:
    KarazhanShadeOfAranMarkConjuredElementalAction(ShadowAI* botAI, std::string const name = "karazhan shade of aran mark conjured elemental") : Action(botAI, name) {}

    bool Execute(Event event) override;
};

class KarazhanShadeOfAranSpreadRangedAction : public MovementAction
{
public:
    KarazhanShadeOfAranSpreadRangedAction(ShadowAI* botAI, std::string const name = "karazhan shade of aran spread ranged") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanNetherspiteBlockRedBeamAction : public MovementAction
{
public:
    KarazhanNetherspiteBlockRedBeamAction(ShadowAI* botAI, std::string const name = "karazhan netherspite block red beam") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanNetherspiteBlockBlueBeamAction : public MovementAction
{
public:
    KarazhanNetherspiteBlockBlueBeamAction(ShadowAI* botAI, std::string const name = "karazhan netherspite block blue beam") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanNetherspiteBlockGreenBeamAction : public MovementAction
{
public:
    KarazhanNetherspiteBlockGreenBeamAction(ShadowAI* botAI, std::string const name = "karazhan netherspite block green beam") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanNetherspiteAvoidBeamAndVoidZoneAction : public MovementAction
{
public:
    KarazhanNetherspiteAvoidBeamAndVoidZoneAction(ShadowAI* botAI, std::string const name = "karazhan netherspite avoid beam and void zone") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanNetherspiteBanishPhaseAvoidVoidZoneAction : public MovementAction
{
public:
    KarazhanNetherspiteBanishPhaseAvoidVoidZoneAction(ShadowAI* botAI, std::string const name = "karazhan netherspite banish phase avoid void zone") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanPrinceMalchezaarNonTankAvoidHazardAction : public MovementAction
{
public:
    KarazhanPrinceMalchezaarNonTankAvoidHazardAction(ShadowAI* botAI, std::string const name = "karazhan prince malchezaar non-tank avoid hazard") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class KarazhanPrinceMalchezaarTankAvoidHazardAction : public MovementAction
{
public:
    KarazhanPrinceMalchezaarTankAvoidHazardAction(ShadowAI* botAI, std::string const name = "karazhan prince malchezaar tank avoid hazard") : MovementAction(botAI, name) {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class KarazhanAttumenTheHuntsmanMultiplier : public Multiplier
{
public:
    KarazhanAttumenTheHuntsmanMultiplier(ShadowAI* botAI) : Multiplier(botAI, "karazhan attumen the huntsman multiplier") {}
    virtual float GetValue(Action* action);
};

class KarazhanBigBadWolfMultiplier : public Multiplier
{
public:
    KarazhanBigBadWolfMultiplier(ShadowAI* botAI) : Multiplier(botAI, "karazhan big bad wolf multiplier") {}
    virtual float GetValue(Action* action);
};

class KarazhanShadeOfAranMultiplier : public Multiplier
{
public:
    KarazhanShadeOfAranMultiplier(ShadowAI* botAI) : Multiplier(botAI, "karazhan shade of aran multiplier") {}
    virtual float GetValue(Action* action);
};

class KarazhanNetherspiteBlueAndGreenBeamMultiplier : public Multiplier
{
public:
    KarazhanNetherspiteBlueAndGreenBeamMultiplier(ShadowAI* botAI) : Multiplier(botAI, "karazhan netherspite blue and green beam multiplier") {}
    virtual float GetValue(Action* action);
};

class KarazhanNetherspiteRedBeamMultiplier : public Multiplier
{
public:
    KarazhanNetherspiteRedBeamMultiplier(ShadowAI* botAI) : Multiplier(botAI, "karazhan netherspite red beam multiplier") {}
    virtual float GetValue(Action* action);
};

class KarazhanPrinceMalchezaarMultiplier : public Multiplier
{
public:
    KarazhanPrinceMalchezaarMultiplier(ShadowAI* botAI) : Multiplier(botAI, "karazhan prince malchezaar multiplier") {}
    virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidKarazhanStrategy : public Strategy
{
public:
    RaidKarazhanStrategy(ShadowAI* ai) : Strategy(ai) {}

    std::string const getName() override { return "karazhan"; }

    void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidKarazhanActionContext : public NamedObjectContext<Action>
{
public:
    RaidKarazhanActionContext()
    {
        creators["karazhan attumen the huntsman stack behind"] = &RaidKarazhanActionContext::karazhan_attumen_the_huntsman_stack_behind;

        creators["karazhan moroes mark target"] = &RaidKarazhanActionContext::karazhan_moroes_mark_target;

        creators["karazhan maiden of virtue position boss"] = &RaidKarazhanActionContext::karazhan_maiden_of_virtue_position_boss;
        creators["karazhan maiden of virtue position ranged"] = &RaidKarazhanActionContext::karazhan_maiden_of_virtue_position_ranged;

        creators["karazhan big bad wolf position boss"] = &RaidKarazhanActionContext::karazhan_big_bad_wolf_position_boss;
        creators["karazhan big bad wolf run away"] = &RaidKarazhanActionContext::karazhan_big_bad_wolf_run_away;

        creators["karazhan romulo and julianne mark target"] = &RaidKarazhanActionContext::karazhan_romulo_and_julianne_mark_target;

        creators["karazhan wizard of oz mark target"] = &RaidKarazhanActionContext::karazhan_wizard_of_oz_mark_target;
        creators["karazhan wizard of oz scorch strawman"] = &RaidKarazhanActionContext::karazhan_wizard_of_oz_scorch_strawman;

        creators["karazhan the curator mark target"] = &RaidKarazhanActionContext::karazhan_the_curator_mark_target;
        creators["karazhan the curator position boss"] = &RaidKarazhanActionContext::karazhan_the_curator_position_boss;
        creators["karazhan the curator spread ranged"] = &RaidKarazhanActionContext::karazhan_the_curator_spread_ranged;

        creators["karazhan terestian illhoof mark target"] = &RaidKarazhanActionContext::karazhan_terestian_illhoof_mark_target;

        creators["karazhan shade of aran arcane explosion run away"] = &RaidKarazhanActionContext::karazhan_shade_of_aran_arcane_explosion_run_away;
        creators["karazhan shade of aran flame wreath stop movement"] = &RaidKarazhanActionContext::karazhan_shade_of_aran_flame_wreath_stop_movement;
        creators["karazhan shade of aran mark conjured elemental"] = &RaidKarazhanActionContext::karazhan_shade_of_aran_mark_conjured_elemental;
        creators["karazhan shade of aran spread ranged"] = &RaidKarazhanActionContext::karazhan_shade_of_aran_spread_ranged;

        creators["karazhan netherspite block red beam"] = &RaidKarazhanActionContext::karazhan_netherspite_block_red_beam;
        creators["karazhan netherspite block blue beam"] = &RaidKarazhanActionContext::karazhan_netherspite_block_blue_beam;
        creators["karazhan netherspite block green beam"] = &RaidKarazhanActionContext::karazhan_netherspite_block_green_beam;
        creators["karazhan netherspite avoid beam and void zone"] = &RaidKarazhanActionContext::karazhan_netherspite_avoid_beam_and_void_zone;
        creators["karazhan netherspite banish phase avoid void zone"] = &RaidKarazhanActionContext::karazhan_netherspite_banish_phase_avoid_void_zone;

        creators["karazhan prince malchezaar non tank avoid hazard"] = &RaidKarazhanActionContext::karazhan_prince_malchezaar_non_tank_avoid_hazard;
        creators["karazhan prince malchezaar tank avoid hazard"] = &RaidKarazhanActionContext::karazhan_prince_malchezaar_tank_avoid_hazard;
    }

private:
    static Action* karazhan_attumen_the_huntsman_stack_behind(ShadowAI* botAI) { return new KarazhanAttumenTheHuntsmanStackBehindAction(botAI); }

    static Action* karazhan_moroes_mark_target(ShadowAI* botAI) { return new KarazhanMoroesMarkTargetAction(botAI); }

    static Action* karazhan_maiden_of_virtue_position_boss(ShadowAI* botAI) { return new KarazhanMaidenOfVirtuePositionBossAction(botAI); }
    static Action* karazhan_maiden_of_virtue_position_ranged(ShadowAI* botAI) { return new KarazhanMaidenOfVirtuePositionRangedAction(botAI); }

    static Action* karazhan_big_bad_wolf_position_boss(ShadowAI* botAI) { return new KarazhanBigBadWolfPositionBossAction(botAI); }
    static Action* karazhan_big_bad_wolf_run_away(ShadowAI* botAI) { return new KarazhanBigBadWolfRunAwayAction(botAI); }

    static Action* karazhan_romulo_and_julianne_mark_target(ShadowAI* botAI) { return new KarazhanRomuloAndJulianneMarkTargetAction(botAI); }

    static Action* karazhan_wizard_of_oz_mark_target(ShadowAI* botAI) { return new KarazhanWizardOfOzMarkTargetAction(botAI); }
    static Action* karazhan_wizard_of_oz_scorch_strawman(ShadowAI* botAI) { return new KarazhanWizardOfOzScorchStrawmanAction(botAI); }

    static Action* karazhan_the_curator_mark_target(ShadowAI* botAI) { return new KarazhanTheCuratorMarkTargetAction(botAI); }
    static Action* karazhan_the_curator_position_boss(ShadowAI* botAI) { return new KarazhanTheCuratorPositionBossAction(botAI); }
    static Action* karazhan_the_curator_spread_ranged(ShadowAI* botAI) { return new KarazhanTheCuratorSpreadRangedAction(botAI); }

    static Action* karazhan_terestian_illhoof_mark_target(ShadowAI* botAI) { return new KarazhanTerestianIllhoofMarkTargetAction(botAI); }

    static Action* karazhan_shade_of_aran_arcane_explosion_run_away(ShadowAI* botAI) { return new KarazhanShadeOfAranArcaneExplosionRunAwayAction(botAI); }
    static Action* karazhan_shade_of_aran_flame_wreath_stop_movement(ShadowAI* botAI) { return new KarazhanShadeOfAranFlameWreathStopMovementAction(botAI); }
    static Action* karazhan_shade_of_aran_mark_conjured_elemental(ShadowAI* botAI) { return new KarazhanShadeOfAranMarkConjuredElementalAction(botAI); }
    static Action* karazhan_shade_of_aran_spread_ranged(ShadowAI* botAI) { return new KarazhanShadeOfAranSpreadRangedAction(botAI); }

    static Action* karazhan_netherspite_block_red_beam(ShadowAI* botAI) { return new KarazhanNetherspiteBlockRedBeamAction(botAI); }
    static Action* karazhan_netherspite_block_blue_beam(ShadowAI* botAI) { return new KarazhanNetherspiteBlockBlueBeamAction(botAI); }
    static Action* karazhan_netherspite_block_green_beam(ShadowAI* botAI) { return new KarazhanNetherspiteBlockGreenBeamAction(botAI); }
    static Action* karazhan_netherspite_avoid_beam_and_void_zone(ShadowAI* botAI) { return new KarazhanNetherspiteAvoidBeamAndVoidZoneAction(botAI); }
    static Action* karazhan_netherspite_banish_phase_avoid_void_zone(ShadowAI* botAI) { return new KarazhanNetherspiteBanishPhaseAvoidVoidZoneAction(botAI); }

    static Action* karazhan_prince_malchezaar_non_tank_avoid_hazard(ShadowAI* botAI) { return new KarazhanPrinceMalchezaarNonTankAvoidHazardAction(botAI); }
    static Action* karazhan_prince_malchezaar_tank_avoid_hazard(ShadowAI* botAI) { return new KarazhanPrinceMalchezaarTankAvoidHazardAction(botAI); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidKarazhanTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidKarazhanTriggerContext()
    {
        creators["karazhan attumen the huntsman"] = &RaidKarazhanTriggerContext::karazhan_attumen_the_huntsman;
        creators["karazhan moroes"] = &RaidKarazhanTriggerContext::karazhan_moroes;
        creators["karazhan maiden of virtue"] = &RaidKarazhanTriggerContext::karazhan_maiden_of_virtue;
        creators["karazhan big bad wolf"] = &RaidKarazhanTriggerContext::karazhan_big_bad_wolf;
        creators["karazhan romulo and julianne"] = &RaidKarazhanTriggerContext::karazhan_romulo_and_julianne;
        creators["karazhan wizard of oz"] = &RaidKarazhanTriggerContext::karazhan_wizard_of_oz;
        creators["karazhan the curator"] = &RaidKarazhanTriggerContext::karazhan_the_curator;
        creators["karazhan terestian illhoof"] = &RaidKarazhanTriggerContext::karazhan_terestian_illhoof;
        creators["karazhan shade of aran"] = &RaidKarazhanTriggerContext::karazhan_shade_of_aran;
        creators["karazhan netherspite"] = &RaidKarazhanTriggerContext::karazhan_netherspite;
        creators["karazhan prince malchezaar"] = &RaidKarazhanTriggerContext::karazhan_prince_malchezaar;
    }

private:
    static Trigger* karazhan_attumen_the_huntsman(ShadowAI* botAI) { return new KarazhanAttumenTheHuntsmanTrigger(botAI); }
    static Trigger* karazhan_moroes(ShadowAI* botAI) { return new KarazhanMoroesTrigger(botAI); }
    static Trigger* karazhan_maiden_of_virtue(ShadowAI* botAI) { return new KarazhanMaidenOfVirtueTrigger(botAI); }
    static Trigger* karazhan_big_bad_wolf(ShadowAI* botAI) { return new KarazhanBigBadWolfTrigger(botAI); }
    static Trigger* karazhan_romulo_and_julianne(ShadowAI* botAI) { return new KarazhanRomuloAndJulianneTrigger(botAI); }
    static Trigger* karazhan_wizard_of_oz(ShadowAI* botAI) { return new KarazhanWizardOfOzTrigger(botAI); }
    static Trigger* karazhan_the_curator(ShadowAI* botAI) { return new KarazhanTheCuratorTrigger(botAI); }
    static Trigger* karazhan_terestian_illhoof(ShadowAI* botAI) { return new KarazhanTerestianIllhoofTrigger(botAI); }
    static Trigger* karazhan_shade_of_aran(ShadowAI* botAI) { return new KarazhanShadeOfAranTrigger(botAI); }
    static Trigger* karazhan_netherspite(ShadowAI* botAI) { return new KarazhanNetherspiteTrigger(botAI); }
    static Trigger* karazhan_prince_malchezaar(ShadowAI* botAI) { return new KarazhanPrinceMalchezaarTrigger(botAI); }
};

#endif
