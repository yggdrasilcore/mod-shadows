#ifndef _SHADOW_DRAKTHARONKEEP_H
#define _SHADOW_DRAKTHARONKEEP_H

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

enum DrakTharonIDs
{
    // Trollgore
    NPC_DRAKKARI_INVADER               = 27709,

    // Novos the Summoner
    NPC_NOVOS                          = 26631,
    SPELL_ARCANE_FIELD                 = 47346,
    NPC_CRYSTAL_HANDLER                = 26627,
    NPC_HULKING_CORPSE                 = 27597,
    NPC_RISEN_SHADOWCASTER             = 27600,
    NPC_FETID_TROLL_CORPSE             = 27598,

    // The Prophet Tharon'ja
    SPELL_GIFT_OF_THARONJA             = 52509,
};

class CorpseExplodeTrigger : public Trigger
{
public:
    CorpseExplodeTrigger(ShadowAI* ai) : Trigger(ai, "corpse explode") {}
    bool IsActive() override;
};

class ArcaneFieldTrigger : public Trigger
{
public:
    ArcaneFieldTrigger(ShadowAI* ai) : Trigger(ai, "arcane field") {}
    bool IsActive() override;
};

// class CrystalHandlerTrigger : public Trigger
// {
// public:
//     CrystalHandlerTrigger(ShadowAI* ai) : Trigger(ai, "crystal handler") {}
//     bool IsActive() override;
// };

class GiftOfTharonjaTrigger : public Trigger
{
public:
    GiftOfTharonjaTrigger(ShadowAI* ai) : Trigger(ai, "gift of tharon'ja") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position NOVOS_PARTY_POSITION = Position(-378.852f, -760.349f, 28.587f);

class CorpseExplodeSpreadAction : public MovementAction
{
public:
    CorpseExplodeSpreadAction(ShadowAI* ai) : MovementAction(ai, "corpse explode spread") {}
    bool Execute(Event event) override;
};

class AvoidArcaneFieldAction : public MovementAction
{
public:
    AvoidArcaneFieldAction(ShadowAI* ai) : MovementAction(ai, "avoid arcane field") {}
    bool Execute(Event event) override;
};

class NovosDefaultPositionAction : public MovementAction
{
public:
    NovosDefaultPositionAction(ShadowAI* ai) : MovementAction(ai, "novos default position") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class NovosTargetPriorityAction : public AttackAction
{
public:
    NovosTargetPriorityAction(ShadowAI* ai) : AttackAction(ai, "novos target priority") {}
    bool Execute(Event event) override;
    // bool isUseful() override;
};

class CastSlayingStrikeAction : public CastMeleeSpellAction
{
public:
    CastSlayingStrikeAction(ShadowAI* botAI) : CastMeleeSpellAction(botAI, "slaying strike") {}
};

class CastTauntAction : public CastSpellAction
{
public:
    CastTauntAction(ShadowAI* botAI) : CastSpellAction(botAI, "taunt") {}
};

class CastBoneArmorAction : public CastSpellAction
{
public:
    CastBoneArmorAction(ShadowAI* botAI) : CastSpellAction(botAI, "bone armor") {}
};

class CastTouchOfLifeAction : public CastSpellAction
{
public:
    CastTouchOfLifeAction(ShadowAI* botAI) : CastSpellAction(botAI, "touch of life") {}
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class NovosMultiplier : public Multiplier
{
    public:
        NovosMultiplier(ShadowAI* ai) : Multiplier(ai, "novos the summoner") {}

    public:
        virtual float GetValue(Action* action);
};

class TharonjaMultiplier : public Multiplier
{
    public:
        TharonjaMultiplier(ShadowAI* ai) : Multiplier(ai, "the prophet tharon'ja") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonDTKStrategy : public Strategy
{
public:
    WotlkDungeonDTKStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "drak'tharon keep"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonDTKActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonDTKActionContext() {
            creators["corpse explode spread"] = &WotlkDungeonDTKActionContext::corpse_explode_spread;
            creators["avoid arcane field"] = &WotlkDungeonDTKActionContext::avoid_arcane_field;
            creators["novos positioning"] = &WotlkDungeonDTKActionContext::novos_positioning;
            creators["novos target priority"] = &WotlkDungeonDTKActionContext::novos_target_priority;
            creators["slaying strike"] = &WotlkDungeonDTKActionContext::slaying_strike;
            creators["tharonja taunt"] = &WotlkDungeonDTKActionContext::taunt;
            creators["bone armor"] = &WotlkDungeonDTKActionContext::bone_armor;
            creators["touch of life"] = &WotlkDungeonDTKActionContext::touch_of_life;
        }
    private:
        static Action* corpse_explode_spread(ShadowAI* ai) { return new CorpseExplodeSpreadAction(ai); }
        static Action* avoid_arcane_field(ShadowAI* ai) { return new AvoidArcaneFieldAction(ai); }
        static Action* novos_positioning(ShadowAI* ai) { return new NovosDefaultPositionAction(ai); }
        static Action* novos_target_priority(ShadowAI* ai) { return new NovosTargetPriorityAction(ai); }
        static Action* slaying_strike(ShadowAI* ai) { return new CastSlayingStrikeAction(ai); }
        static Action* taunt(ShadowAI* ai) { return new CastTauntAction(ai); }
        static Action* bone_armor(ShadowAI* ai) { return new CastBoneArmorAction(ai); }
        static Action* touch_of_life(ShadowAI* ai) { return new CastTouchOfLifeAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonDTKTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonDTKTriggerContext()
        {
            creators["corpse explode"] = &WotlkDungeonDTKTriggerContext::corpse_explode;
            creators["arcane field"] = &WotlkDungeonDTKTriggerContext::arcane_field;
            // creators["crystal handler"] = &WotlkDungeonDTKTriggerContext::crystal_handler;
            creators["gift of tharon'ja"] = &WotlkDungeonDTKTriggerContext::gift_of_tharonja;
            creators["tharon'ja out of melee"] = &WotlkDungeonDTKTriggerContext::tharonja_out_of_melee;

        }
    private:
        static Trigger* corpse_explode(ShadowAI* ai) { return new CorpseExplodeTrigger(ai); }
        static Trigger* arcane_field(ShadowAI* ai) { return new ArcaneFieldTrigger(ai); }
        // static Trigger* crystal_handler(ShadowAI* ai) { return new CrystalHandlerTrigger(ai); }
        static Trigger* gift_of_tharonja(ShadowAI* ai) { return new GiftOfTharonjaTrigger(ai); }
        static Trigger* tharonja_out_of_melee(ShadowAI* ai) { return new TwoTriggers(ai, "gift of tharon'ja", "enemy out of melee"); }
};

#endif
