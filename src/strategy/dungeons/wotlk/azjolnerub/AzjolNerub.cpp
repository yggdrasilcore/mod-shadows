#include "AzjolNerub.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool KrikthirWebWrapTrigger::IsActive()
{
    if (!botAI->IsDps(bot)) { return false; }

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_WEB_WRAP)
        {
            return true;
        }
    }

    return false;
}

bool KrikthirWatchersTrigger::IsActive()
{
    if (!botAI->IsDps(bot)) { return false; }

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_KRIKTHIR)
        {
            return true;
        }
    }
    return false;
}

// bool AnubarakImpaleTrigger::IsActive()
// {
//     Unit* boss = AI_VALUE2(Unit*, "find target", "anub'arak");
//     if (!boss) { return false; }
//     GuidVector triggers = AI_VALUE(GuidVector, "possible triggers");
//     for (auto i = triggers.begin(); i != triggers.end(); i++)
//     {
//         Unit* unit = botAI->GetUnit(*i);

//         if (unit)
//         {
//             bot->Yell("TRIGGER="+unit->GetName(), LANG_UNIVERSAL);
//         }
//     }
//     return false;
// }

bool AnubarakPoundTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "anub'arak");
    if (!boss) { return false; }

    return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_POUND);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AttackWebWrapAction::isUseful() { return !botAI->IsHeal(bot); }
bool AttackWebWrapAction::Execute(Event event [[maybe_unused]])
{
    Unit* webWrap = nullptr;

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_WEB_WRAP)
        {
            webWrap = unit;
            break;
        }
    }
    if (!webWrap || AI_VALUE(Unit*, "current target") == webWrap)
    {
        return false;
    }

    return Attack(webWrap);
}

bool WatchersTargetAction::isUseful() { return !botAI->IsHeal(bot); }
bool WatchersTargetAction::Execute(Event event [[maybe_unused]])
{
    // Always prioritise web wraps
    Unit* currTarget = AI_VALUE(Unit*, "current target");
    if (currTarget && currTarget->GetEntry() == NPC_WEB_WRAP) { return false; }

    // Do not search all units in range!
    // There are many adds we don't want to aggro in close proximity,
    // only check in-combat adds now.
    GuidVector attackers = AI_VALUE(GuidVector, "attackers");
    Unit* priorityTargets[4] = {nullptr, nullptr, nullptr, nullptr};

    for (auto& attacker : attackers)
    {
        Unit* npc = botAI->GetUnit(attacker);
        if (!npc)
        {
            continue;
        }
        switch (npc->GetEntry())
        {
            // Focus skirmishers first
            case NPC_WATCHER_SKIRMISHER:
                priorityTargets[0] = npc;
                break;
            // Then shadowcaster. This doesn't work so well for the shadowcaster
            // + skirmisher pack - ideally we would kill the watcher second.
            // But don't want to make this unnecessarily complex and rigid...
            // Will revisit if this causes problems in heroic.
            case NPC_WATCHER_SHADOWCASTER:
                priorityTargets[1] = npc;
                break;
            // Named watcher next
            case NPC_WATCHER_SILTHIK:
            case NPC_WATCHER_GASHRA:
            case NPC_WATCHER_NARJIL:
                priorityTargets[2] = npc;
                break;
            // Warrior last
            case NPC_WATCHER_WARRIOR:
                priorityTargets[3] = npc;
                break;
        }
    }

    for (Unit* target : priorityTargets)
    {
        // Attack the first valid split target in the priority list
        if (target)
        {
            if (currTarget != target)
            {
                // bot->Yell("ATTACKING "+target->GetName(), LANG_UNIVERSAL);
                return Attack(target);
            }
            // Don't continue loop here, the target exists so we don't
            // want to move down the prio list. We just don't need to send attack
            // command again, just return false and exit the loop that way
            return false;
        }
    }

    return false;
}

bool AnubarakDodgePoundAction::isUseful() { return !AI_VALUE2(bool, "behind", "current target"); }
bool AnubarakDodgePoundAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "anub'arak");
    if (!boss) { return false; }

    float distance = bot->GetExactDist2d(boss->GetPosition());
    // Extra units to move into the boss, instead of being just 1 pixel past his midpoint.
    // Can be adjusted - this value tends to mirror how a human would play,
    // and visibly ensures you won't get hit while not creating excessive movements.
    float distanceExtra = 2.0f;
    return Move(bot->GetAngle(boss), distance + distanceExtra);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float KrikthirMultiplier::GetValue(Action* action)
{
    if (!botAI->IsDps(bot)) { return 1.0f; }

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    Unit* boss = nullptr;
    Unit* watcher = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");
    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (!unit) { continue; }

        switch (unit->GetEntry())
        {
            case NPC_KRIKTHIR:
                boss = unit;
                continue;
            case NPC_WATCHER_SILTHIK:
            case NPC_WATCHER_GASHRA:
            case NPC_WATCHER_NARJIL:
            case NPC_WATCHER_SKIRMISHER:
            case NPC_WATCHER_SHADOWCASTER:
            case NPC_WATCHER_WARRIOR:
                watcher = unit;
                continue;
        }
    }

    if (boss && watcher)
    {
        // Do not target swap
        if (dynamic_cast<DpsAssistAction*>(action))
        {
            return 0.0f;
        }

        if (action->getThreatType() == Action::ActionThreatType::Aoe)
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonANStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Krik'thir the Gatewatcher
    // TODO: Add CC trigger while web wraps are casting?
    // TODO: Bring healer closer than ranged dps to avoid fixates?
    triggers.push_back(new TriggerNode("krik'thir web wrap",
        NextAction::array(0, new NextAction("attack web wrap", ACTION_RAID + 5), nullptr)));
    triggers.push_back(new TriggerNode("krik'thir watchers",
        NextAction::array(0, new NextAction("krik'thir priority", ACTION_RAID + 4), nullptr)));

    // Hadronox
    // The core AC triggers are very buggy with this boss, but default strat seems to play correctly

    //Anub'arak
    // TODO: No clear way to track these spikes. They don't seem to appear as gameobjects or triggers,
    // and cast time is instant so no way to check currently casting location.
    // May need to hook boss AI.. might be able to just heal through it for now.
    // triggers.push_back(new TriggerNode("anub'arak impale",
    //     NextAction::array(0, new NextAction("TODO", ACTION_MOVE + 5), nullptr)));
    triggers.push_back(new TriggerNode("anub'arak pound",
        NextAction::array(0, new NextAction("dodge pound", ACTION_MOVE + 5), nullptr)));
}

void WotlkDungeonANStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new KrikthirMultiplier(botAI));
}
