#include "UtgardeKeep.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool KelesethFrostTombTrigger::IsActive()
{
    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Unit* unit = botAI->GetUnit(member);
        if (unit && unit->HasAura(SPELL_FROST_TOMB))
        {
            return true;
        }
    }
    return false;
}

bool DalronnDpsTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "dalronn the controller");
    if (!boss || !boss->isTargetableForAttack()) { return false; }

    // This doesn't cause issues with healers currently and they will continue to heal even when included here
    return !botAI->IsTank(bot);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AttackFrostTombAction::isUseful() { return !botAI->IsHeal(bot); }
bool AttackFrostTombAction::Execute(Event event [[maybe_unused]])
{
    Unit* frostTomb = nullptr;

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->GetEntry() == NPC_FROST_TOMB)
        {
            frostTomb = unit;
            break;
        }
    }
    if (!frostTomb || AI_VALUE(Unit*, "current target") == frostTomb)
    {
        return false;
    }
    return Attack(frostTomb);
}

// TODO: Possibly add player stacking behaviour close to tank, to prevent Skarvald charging ranged
bool AttackDalronnAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "dalronn the controller");
    if (!boss) { return false; }

    if (AI_VALUE(Unit*, "current target") == boss)
    {
        return false;
    }
    return Attack(boss);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float PrinceKelesethMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "prince keleseth");
    if (!boss) { return 1.0f; }

    // Suppress auto-targeting behaviour only when a tomb is up
    if (dynamic_cast<DpsAssistAction*>(action))
    {
        GuidVector members = AI_VALUE(GuidVector, "group members");
        for (auto& member : members)
        {
            Unit* unit = botAI->GetUnit(member);
            if (unit && unit->HasAura(SPELL_FROST_TOMB))
            {
                return 0.0f;
            }
        }
    }
    return 1.0f;
}

float SkarvaldAndDalronnMultiplier::GetValue(Action* action)
{
    // Only need to deal with Dalronn here. If he's dead, just fall back to normal dps strat
    Unit* dalronn = AI_VALUE2(Unit*, "find target", "dalronn the controller");
    if (!dalronn) { return 1.0f; }

    // Only suppress DpsAssistAction if Dalronn is alive
    if (dalronn->isTargetableForAttack() && dynamic_cast<DpsAssistAction*>(action))
    {
        return 0.0f;
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonUKStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Prince Keleseth
    triggers.push_back(new TriggerNode("keleseth frost tomb",
             NextAction::array(0, new NextAction("attack frost tomb", ACTION_RAID + 1), nullptr)));

    // Skarvald the Constructor & Dalronn the Controller
    triggers.push_back(new TriggerNode("dalronn priority",
             NextAction::array(0, new NextAction("attack dalronn", ACTION_RAID + 1), nullptr)));

}

void WotlkDungeonUKStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new PrinceKelesethMultiplier(botAI));
    multipliers.push_back(new SkarvaldAndDalronnMultiplier(botAI));
}
