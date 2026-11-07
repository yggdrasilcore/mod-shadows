#ifndef _SHADOW_NEXUS_H
#define _SHADOW_NEXUS_H

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

enum NexusIDs
{
    // Faction Commander
    NPC_ALLIANCE_COMMANDER          = 27949,
    NPC_HORDE_COMMANDER             = 27947,
    NPC_COMMANDER_STOUTBEARD        = 26796,
    NPC_COMMANDER_KOLURG            = 26798,
    // SPELL_FRIGHTENING_SHOUT         = 19134,
    SPELL_WHIRLWIND                 = 38618,

    // Grand Magus Telestra
    NPC_TELESTRA                    = 26731,
    NPC_FIRE_MAGUS                  = 26928,
    NPC_FROST_MAGUS                 = 26930,
    NPC_ARCANE_MAGUS                = 26929,

    // Anomalus
    BUFF_RIFT_SHIELD                = 47748,

    // Ormorok the Tree Shaper
    // NPC_CRYSTAL_SPIKE               = 27099,
    GO_CRYSTAL_SPIKE                = 188537,
};

class FactionCommanderWhirlwindTrigger : public Trigger
{
public:
    FactionCommanderWhirlwindTrigger(ShadowAI* ai) : Trigger(ai, "faction commander whirlwind") {}
    bool IsActive() override;
};

class TelestraFirebombTrigger : public Trigger
{
public:
    TelestraFirebombTrigger(ShadowAI* ai) : Trigger(ai, "telestra firebomb spread") {}
    bool IsActive() override;
};

class TelestraSplitPhaseTrigger : public Trigger
{
public:
    TelestraSplitPhaseTrigger(ShadowAI* ai) : Trigger(ai, "telestra split phase") {}
    bool IsActive() override;
};

class ChaoticRiftTrigger : public Trigger
{
public:
    ChaoticRiftTrigger(ShadowAI* ai) : Trigger(ai, "chaotic rift") {}
    bool IsActive() override;
};

class OrmorokSpikesTrigger : public Trigger
{
public:
    OrmorokSpikesTrigger(ShadowAI* ai) : Trigger(ai, "ormorok spikes") {}
    bool IsActive() override;
};

class OrmorokStackTrigger : public Trigger
{
public:
    OrmorokStackTrigger(ShadowAI* ai) : Trigger(ai, "ormorok stack") {}
    bool IsActive() override;
};

class IntenseColdTrigger : public Trigger
{
public:
    IntenseColdTrigger(ShadowAI* ai) : Trigger(ai, "intense cold") {}
    bool IsActive() override;
};

class KeristraszaPositioningTrigger : public Trigger
{
public:
    KeristraszaPositioningTrigger(ShadowAI* ai) : Trigger(ai, "keristrasza positioning") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class MoveFromWhirlwindAction : public MovementAction
{
public:
    MoveFromWhirlwindAction(ShadowAI* ai) : MovementAction(ai, "move from whirlwind") {}
    bool Execute(Event event) override;
};

class FirebombSpreadAction : public MovementAction
{
public:
    FirebombSpreadAction(ShadowAI* ai) : MovementAction(ai, "firebomb spread") {}
    bool Execute(Event event) override;
};

class TelestraSplitTargetAction : public AttackAction
{
public:
    TelestraSplitTargetAction(ShadowAI* ai) : AttackAction(ai, "telestra split target") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class ChaoticRiftTargetAction : public AttackAction
{
public:
    ChaoticRiftTargetAction(ShadowAI* ai) : AttackAction(ai, "chaotic rift target") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class DodgeSpikesAction : public MovementAction
{
public:
    DodgeSpikesAction(ShadowAI* ai) : MovementAction(ai, "dodge spikes") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class IntenseColdJumpAction : public MovementAction
{
public:
    IntenseColdJumpAction(ShadowAI* ai) : MovementAction(ai, "intense cold jump") {}
    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class FactionCommanderMultiplier : public Multiplier
{
    public:
        FactionCommanderMultiplier(ShadowAI* ai) : Multiplier(ai, "faction commander") {}

    public:
        virtual float GetValue(Action* action);
};

class TelestraMultiplier : public Multiplier
{
    public:
        TelestraMultiplier(ShadowAI* ai) : Multiplier(ai, "grand magus telestra") {}

    public:
        virtual float GetValue(Action* action);
};

class AnomalusMultiplier : public Multiplier
{
    public:
        AnomalusMultiplier(ShadowAI* ai) : Multiplier(ai, "anomalus") {}

    public:
        virtual float GetValue(Action* action);
};

class OrmorokMultiplier : public Multiplier
{
    public:
        OrmorokMultiplier(ShadowAI* ai) : Multiplier(ai, "ormorok the tree-shaper") {}

    public:
        virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonNexStrategy : public Strategy
{
public:
    WotlkDungeonNexStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "nexus"; }
    virtual void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonNexActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonNexActionContext() {
            creators["move from whirlwind"] = &WotlkDungeonNexActionContext::move_from_whirlwind;
            creators["firebomb spread"] = &WotlkDungeonNexActionContext::firebomb_spread;
            creators["telestra split target"] = &WotlkDungeonNexActionContext::telestra_split_target;
            creators["chaotic rift target"] = &WotlkDungeonNexActionContext::chaotic_rift_target;
            creators["dodge spikes"] = &WotlkDungeonNexActionContext::dodge_spikes;
            creators["intense cold jump"] = &WotlkDungeonNexActionContext::intense_cold_jump;
        }
    private:
        static Action* move_from_whirlwind(ShadowAI* ai) { return new MoveFromWhirlwindAction(ai); }
        static Action* firebomb_spread(ShadowAI* ai) { return new FirebombSpreadAction(ai); }
        static Action* telestra_split_target(ShadowAI* ai) { return new TelestraSplitTargetAction(ai); }
        static Action* chaotic_rift_target(ShadowAI* ai) { return new ChaoticRiftTargetAction(ai); }
        static Action* dodge_spikes(ShadowAI* ai) { return new DodgeSpikesAction(ai); }
        static Action* intense_cold_jump(ShadowAI* ai) { return new IntenseColdJumpAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonNexTriggerContext : public NamedObjectContext<Trigger>
{
    public:
        WotlkDungeonNexTriggerContext()
        {
            creators["faction commander whirlwind"] = &WotlkDungeonNexTriggerContext::faction_commander_whirlwind;
            creators["telestra firebomb"] = &WotlkDungeonNexTriggerContext::telestra_firebomb;
            creators["telestra split phase"] = &WotlkDungeonNexTriggerContext::telestra_split_phase;
            creators["chaotic rift"] = &WotlkDungeonNexTriggerContext::chaotic_rift;
            creators["ormorok spikes"] = &WotlkDungeonNexTriggerContext::ormorok_spikes;
            creators["ormorok stack"] = &WotlkDungeonNexTriggerContext::ormorok_stack;
            creators["intense cold"] = &WotlkDungeonNexTriggerContext::intense_cold;
            creators["keristrasza positioning"] = &WotlkDungeonNexTriggerContext::keristrasza_positioning;
        }
    private:
        static Trigger* faction_commander_whirlwind(ShadowAI* ai) { return new FactionCommanderWhirlwindTrigger(ai); }
        static Trigger* telestra_firebomb(ShadowAI* ai) { return new TelestraFirebombTrigger(ai); }
        static Trigger* telestra_split_phase(ShadowAI* ai) { return new TelestraSplitPhaseTrigger(ai); }
        static Trigger* chaotic_rift(ShadowAI* ai) { return new ChaoticRiftTrigger(ai); }
        static Trigger* ormorok_spikes(ShadowAI* ai) { return new OrmorokSpikesTrigger(ai); }
        static Trigger* ormorok_stack(ShadowAI* ai) { return new OrmorokStackTrigger(ai); }
        static Trigger* intense_cold(ShadowAI* ai) { return new IntenseColdTrigger(ai); }
        static Trigger* keristrasza_positioning(ShadowAI* ai) { return new KeristraszaPositioningTrigger(ai); }
};

#endif
