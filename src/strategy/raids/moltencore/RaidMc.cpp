#include "RaidMc.h"

#include "SharedDefines.h"
#include "Shadows.h"
#include "Strategy.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool McLivingBombDebuffTrigger::IsActive()
{
    // if bot has barron geddon's living bomb, we need to add strat, otherwise we need to remove
    // only do when fighting baron geddon (to avoid modifying strat set by player outside this fight)
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "baron geddon"))
    {
        if (boss->IsInCombat())
            return bot->HasAura(20475) != botAI->HasStrategy("move from group", BotState::BOT_STATE_COMBAT);
    }
    return false;
}

bool McBaronGeddonInfernoTrigger::IsActive()
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "baron geddon"))
        return boss->HasAura(19695);
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool McCheckShouldMoveFromGroupAction::Execute(Event event [[maybe_unused]])
{
    if (bot->HasAura(20475))  // barron geddon's living bomb
    {
        if (!botAI->HasStrategy("move from group", BotState::BOT_STATE_COMBAT))
        {
            // add/remove from both for now as it will make it more obvious to
            // player if this strat remains on after fight somehow
            botAI->ChangeStrategy("+move from group", BOT_STATE_NON_COMBAT);
            botAI->ChangeStrategy("+move from group", BOT_STATE_COMBAT);
            return true;
        }
    }
    else if (botAI->HasStrategy("move from group", BotState::BOT_STATE_COMBAT))
    {
        // add/remove from both for now as it will make it more obvious to
        // player if this strat remains on after fight somehow
        botAI->ChangeStrategy("-move from group", BOT_STATE_NON_COMBAT);
        botAI->ChangeStrategy("-move from group", BOT_STATE_COMBAT);
        return true;
    }
    return false;
}

bool McMoveFromBaronGeddonAction::Execute(Event event [[maybe_unused]])
{
    const float radius = 25.0f;  // more than should be needed but bots keep trying to run back in
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "baron geddon"))
    {
        long distToTravel = radius - bot->GetDistance(boss);
        if (distToTravel > 0)
        {
            // float angle = bot->GetAngle(boss) + M_PI;
            // return Move(angle, distToTravel);
            return MoveAway(boss, distToTravel);
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidMcStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("mc living bomb debuff",
                        NextAction::array(0, new NextAction("mc check should move from group", ACTION_RAID), nullptr)));
    triggers.push_back(
        new TriggerNode("mc baron geddon inferno",
                        NextAction::array(0, new NextAction("mc move from baron geddon", ACTION_RAID), nullptr)));
}
