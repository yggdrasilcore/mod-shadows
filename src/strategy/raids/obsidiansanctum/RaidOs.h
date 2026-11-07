#ifndef _SHADOW_RAIDOS_H
#define _SHADOW_RAIDOS_H

#include "ShadowAI.h"
#include "Shadows.h"
#include "Trigger.h"
#include "MovementActions.h"
#include "AttackAction.h"
#include "Multiplier.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "Action.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum ObsidianSanctumIDs
{
    // Bosses
    NPC_SARTHARION                              = 28860,
    NPC_SHADRON                                 = 30451,
    NPC_TENEBRON                                = 30452,
    NPC_VESPERON                                = 30449,

    // Mini-boss shared
    SPELL_SHADOW_BREATH                         = 57570,
    SPELL_SHADOW_FISSURE                        = 57579,
    SPELL_SUMMON_TWILIGHT_WHELP                 = 58035,
    SPELL_GIFT_OF_TWILIGHT_SHADOW               = 57835,
    SPELL_TWILIGHT_TORMENT_VESPERON             = 57935,

    // Sartharion
    SPELL_SARTHARION_CLEAVE                     = 56909,
    SPELL_SARTHARION_FLAME_BREATH               = 56908,
    SPELL_SARTHARION_TAIL_LASH                  = 56910,
    SPELL_CYCLONE_AURA_PERIODIC                 = 57598,
    SPELL_LAVA_STRIKE_DUMMY                     = 57578,
    SPELL_LAVA_STRIKE_DUMMY_TRIGGER             = 57697,
    SPELL_LAVA_STRIKE_SUMMON                    = 57572,
    SPELL_SARTHARION_PYROBUFFET                 = 56916,
    SPELL_SARTHARION_BERSERK                    = 61632,
    SPELL_SARTHARION_TWILIGHT_REVENGE           = 60639,

    // Sartharion with drakes
    SPELL_WILL_OF_SARTHARION                    = 61254,
    SPELL_POWER_OF_TENEBRON                     = 61248,
    SPELL_POWER_OF_VESPERON                     = 61251,
    SPELL_POWER_OF_SHADRON                      = 58105,
    SPELL_GIFT_OF_TWILIGHT_FIRE                 = 58766,

    // Visuals
    SPELL_EGG_MARKER_VISUAL                     = 58547,
    SPELL_FLAME_TSUNAMI_VISUAL                  = 57494,

    // Misc
    SPELL_FADE_ARMOR                            = 60708,
    SPELL_FLAME_TSUNAMI_DAMAGE_AURA             = 57492,
    SPELL_FLAME_TSUNAMI_LEAP                    = 60241,
    SPELL_SARTHARION_PYROBUFFET_TRIGGER         = 57557,

    NPC_TWILIGHT_EGG                            = 30882,
    NPC_TWILIGHT_WHELP                          = 30890,
    NPC_DISCIPLE_OF_SHADRON                     = 30688,
    NPC_DISCIPLE_OF_VESPERON                    = 30858,
    NPC_ACOLYTE_OF_SHADRON                      = 31218,
    NPC_ACOLYTE_OF_VESPERON                     = 31219,

    // Sartharion fight
    NPC_LAVA_BLAZE                              = 30643,
    NPC_FLAME_TSUNAMI                           = 30616,
    NPC_SAFE_AREA_TRIGGER                       = 30494,
    NPC_TWILIGHT_FISSURE                        = 30641,
    GO_TWILIGHT_PORTAL                          = 193988,
    GO_NORMAL_PORTAL                            = 193989,
    SPELL_TWILIGHT_SHIFT                        = 57874,
};

const uint32 OS_MAP_ID = 615;

class SartharionTankTrigger : public Trigger
{
public:
    SartharionTankTrigger(ShadowAI* botAI) : Trigger(botAI, "sartharion tank") {}
    bool IsActive() override;
};

class FlameTsunamiTrigger : public Trigger
{
public:
    FlameTsunamiTrigger(ShadowAI* botAI) : Trigger(botAI, "flame tsunami") {}
    bool IsActive() override;
};

class TwilightFissureTrigger : public Trigger
{
public:
    TwilightFissureTrigger(ShadowAI* botAI) : Trigger(botAI, "twilight fissure") {}
    bool IsActive() override;
};

class SartharionDpsTrigger : public Trigger
{
public:
    SartharionDpsTrigger(ShadowAI* botAI) : Trigger(botAI, "sartharion dps") {}
    bool IsActive() override;
};

class SartharionMeleePositioningTrigger : public Trigger
{
public:
    SartharionMeleePositioningTrigger(ShadowAI* botAI) : Trigger(botAI, "sartharion melee positioning") {}
    bool IsActive() override;
};

class TwilightPortalEnterTrigger : public Trigger
{
public:
    TwilightPortalEnterTrigger(ShadowAI* botAI) : Trigger(botAI, "twilight portal enter") {}
    bool IsActive() override;
};

class TwilightPortalExitTrigger : public Trigger
{
public:
    TwilightPortalExitTrigger(ShadowAI* botAI) : Trigger(botAI, "twilight portal exit") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const float TSUNAMI_LEFT_SAFE_MELEE = 552.0f;
const float TSUNAMI_LEFT_SAFE_RANGED = 504.0f;
const float TSUNAMI_RIGHT_SAFE_ALL = 529.0f;
const std::pair<float, float> SARTHARION_MAINTANK_POSITION = {3258.5f, 532.5f};
const std::pair<float, float> SARTHARION_OFFTANK_POSITION = {3230.0f, 526.0f};
const std::pair<float, float> SARTHARION_RANGED_POSITION = {3248.0f, 507.0f};

class SartharionTankPositionAction : public AttackAction
{
public:
    SartharionTankPositionAction(ShadowAI* botAI, std::string const name = "sartharion tank position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;
};

class AvoidTwilightFissureAction : public MovementAction
{
public:
    AvoidTwilightFissureAction(ShadowAI* botAI, std::string const name = "avoid twilight fissure")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

class AvoidFlameTsunamiAction : public MovementAction
{
public:
    AvoidFlameTsunamiAction(ShadowAI* botAI, std::string const name = "avoid flame tsunami")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

class SartharionAttackPriorityAction : public AttackAction
{
public:
    SartharionAttackPriorityAction(ShadowAI* botAI, std::string const name = "sartharion attack priority")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;
};

class EnterTwilightPortalAction : public MovementAction
{
public:
    EnterTwilightPortalAction(ShadowAI* botAI, std::string const name = "enter twilight portal")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

class ExitTwilightPortalAction : public MovementAction
{
public:
    ExitTwilightPortalAction(ShadowAI* botAI, std::string const name = "exit twilight portal")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class SartharionMultiplier : public Multiplier
{
public:
    SartharionMultiplier(ShadowAI* ai) : Multiplier(ai, "sartharion") {}

public:
    virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidOsStrategy : public Strategy
{
public:
    RaidOsStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "wotlk-os"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidOsActionContext : public NamedObjectContext<Action>
{
public:
    RaidOsActionContext()
    {
        creators["sartharion tank position"] = &RaidOsActionContext::tank_position;
        creators["avoid twilight fissure"] = &RaidOsActionContext::avoid_twilight_fissure;
        creators["avoid flame tsunami"] = &RaidOsActionContext::avoid_flame_tsunami;
        creators["sartharion attack priority"] = &RaidOsActionContext::attack_priority;
        creators["enter twilight portal"] = &RaidOsActionContext::enter_twilight_portal;
        creators["exit twilight portal"] = &RaidOsActionContext::exit_twilight_portal;
    }

private:
    static Action* tank_position(ShadowAI* ai) { return new SartharionTankPositionAction(ai); }
    static Action* avoid_twilight_fissure(ShadowAI* ai) { return new AvoidTwilightFissureAction(ai); }
    static Action* avoid_flame_tsunami(ShadowAI* ai) { return new AvoidFlameTsunamiAction(ai); }
    static Action* attack_priority(ShadowAI* ai) { return new SartharionAttackPriorityAction(ai); }
    static Action* enter_twilight_portal(ShadowAI* ai) { return new EnterTwilightPortalAction(ai); }
    static Action* exit_twilight_portal(ShadowAI* ai) { return new ExitTwilightPortalAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidOsTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidOsTriggerContext()
    {
        creators["sartharion tank"] = &RaidOsTriggerContext::sartharion_tank;
        creators["flame tsunami"] = &RaidOsTriggerContext::flame_tsunami;
        creators["twilight fissure"] = &RaidOsTriggerContext::twilight_fissure;
        creators["sartharion dps"] = &RaidOsTriggerContext::sartharion_dps;
        creators["sartharion melee positioning"] = &RaidOsTriggerContext::sartharion_melee;
        creators["twilight portal enter"] = &RaidOsTriggerContext::twilight_portal_enter;
        creators["twilight portal exit"] = &RaidOsTriggerContext::twilight_portal_exit;
    }

private:
    static Trigger* sartharion_tank(ShadowAI* ai) { return new SartharionTankTrigger(ai); }
    static Trigger* flame_tsunami(ShadowAI* ai) { return new FlameTsunamiTrigger(ai); }
    static Trigger* twilight_fissure(ShadowAI* ai) { return new TwilightFissureTrigger(ai); }
    static Trigger* sartharion_dps(ShadowAI* ai) { return new SartharionDpsTrigger(ai); }
    static Trigger* sartharion_melee(ShadowAI* ai) { return new SartharionMeleePositioningTrigger(ai); }
    static Trigger* twilight_portal_enter(ShadowAI* ai) { return new TwilightPortalEnterTrigger(ai); }
    static Trigger* twilight_portal_exit(ShadowAI* ai) { return new TwilightPortalExitTrigger(ai); }
};

#endif
