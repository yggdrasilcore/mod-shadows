#ifndef _SHADOW_PITOFSARON_H
#define _SHADOW_PITOFSARON_H

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

enum PitOfSaronIDs
{
    //NPCs
    NPC_GARFROST                        = 36494,
    NPC_KRICK                           = 36477,
    NPC_ICK                             = 36476,
    NPC_TYRANNUS                        = 36658,
    NPC_RIMEFANG                        = 36661,

    //Spells
    SPELL_PURSUIT                       = 68987,
    SPELL_POISON_NOVA_POS               = 68989,
    SPELL_POISON_NOVA_POS_HC            = 70434,
    SPELL_EXPLOSIVE_BARRAGE_KRICK       = 69012,
    SPELL_EXPLOSIVE_BARRAGE_ICK         = 69263,
    SPELL_EXPLOSIVE_BARRAGE_SUMMON      = 69015,
    SPELL_EXPLODING_ORB_VISUAL          = 69017,
    SPELL_MARK_OF_RIMEFANG              = 69275,
    RIMEFANG_SPELL_HOARFROST            = 69246,
    RIMEFANG_SPELL_HOARFROST_HC         = 69245,
    RIMEFANG_SPELL_HOARFROST_HC2        = 69645,
};

class IckAndKrickTrigger : public Trigger
{
public:
    IckAndKrickTrigger(ShadowAI* ai) : Trigger(ai, "ick and krick") {}

    bool IsActive() override;
};

class TyrannusTrigger : public Trigger
{
public:
    TyrannusTrigger(ShadowAI* ai) : Trigger(ai, "tyrannus") {}

    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position ICKANDKRICK_TANK_POSITION = Position(816.8508f, 102.331505f, 509.1586f);

class IckAndKrickAction : public AttackAction
{
public:
    IckAndKrickAction(ShadowAI* ai) : AttackAction(ai, "ick and krick") {}
    bool Execute(Event event) override;

    bool TankPosition(Unit* boss);
    bool Pursuit(bool pursuit, Unit* boss);
    bool PoisonNova(bool poisonNova, Unit* boss);
    bool ExplosiveBarrage(bool explosiveBarrage, Unit* boss);
};

class TyrannusAction : public AttackAction
{
public:
    TyrannusAction(ShadowAI* ai) : AttackAction(ai, "tyrannus") {}
    bool Execute(Event event) override;

    bool RangedSpread(bool rangedSpread);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class IckAndKrickMultiplier : public Multiplier
{
    public:
    IckAndKrickMultiplier(ShadowAI* ai) : Multiplier(ai, "ick and krick") {}

    public:
        virtual float GetValue(Action* action);
};

class GarfrostMultiplier : public Multiplier
{
public:
    GarfrostMultiplier(ShadowAI* ai) : Multiplier(ai, "garfrost") { }

    float GetValue(Action* action) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonPoSStrategy : public Strategy
{
public:
    WotlkDungeonPoSStrategy(ShadowAI* ai) : Strategy(ai) {}
    std::string const getName() override { return "pit of saron"; }
    void InitTriggers(std::vector<TriggerNode*> &triggers) override;
    void InitMultipliers(std::vector<Multiplier*> &multipliers) override;

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonPoSActionContext : public NamedObjectContext<Action>
{
    public:
        WotlkDungeonPoSActionContext()
        {
            creators["ick and krick"] = &WotlkDungeonPoSActionContext::ick_and_krick;
            creators["tyrannus"] = &WotlkDungeonPoSActionContext::tyrannus;
        }
    private:
        static Action* ick_and_krick(ShadowAI* ai) { return new IckAndKrickAction(ai); }
        static Action* tyrannus(ShadowAI* ai) { return new TyrannusAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class WotlkDungeonPoSTriggerContext : public NamedObjectContext<Trigger>
{
public:
    WotlkDungeonPoSTriggerContext()
    {
        creators["ick and krick"] = &WotlkDungeonPoSTriggerContext::ick_and_krick;
        creators["tyrannus"] = &WotlkDungeonPoSTriggerContext::tyrannus;
    }

private:
    static Trigger* ick_and_krick(ShadowAI* ai) { return new IckAndKrickTrigger(ai); }
    static Trigger* tyrannus(ShadowAI* ai) { return new TyrannusTrigger(ai); }
};

#endif
