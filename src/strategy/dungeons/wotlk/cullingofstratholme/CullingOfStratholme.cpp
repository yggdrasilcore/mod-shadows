#include "CullingOfStratholme.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool ExplodeGhoulTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "salramm the fleshcrafter");
    if (!boss) { return false; }

    float distance = 10.0f;
    float distanceExtra = 2.0f;
    GuidVector corpses = AI_VALUE(GuidVector, "nearest corpses");
    for (auto i = corpses.begin(); i != corpses.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_RISEN_GHOUL)
        {
            if (bot->GetExactDist2d(unit) < distance + distanceExtra)
            {
                return true;
            }
        }
    }
    return false;
}

bool EpochRangedTrigger::IsActive()
{
    return !botAI->IsMelee(bot) && AI_VALUE2(Unit*, "find target", "chrono-lord epoch");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool ExplodeGhoulSpreadAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "salramm the fleshcrafter");
    if (!boss) { return false; }

    float distance = 10.0f;
    float distanceExtra = 2.0f;
    GuidVector corpses = AI_VALUE(GuidVector, "nearest corpses");
    for (auto i = corpses.begin(); i != corpses.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_GHOUL_MINION)
        {
            float currentDistance = bot->GetExactDist2d(unit);
            if (currentDistance < distance + distanceExtra)
            {
                return MoveAway(unit, distance + distanceExtra - currentDistance);
            }
        }
    }
    return false;
}

bool EpochStackAction::isUseful()
{
    // Minimum hunter range is 5, but values too close to this seem to cause issues..
    // Hunter bots will try and melee in between ranged attacks, or just melee entirely at 5 as they are in range.
    // 7.5 or 8.0 solves this for this boss.
    // Unfortunately at this range the boss will charge. So I guess just don't stack as a hunter..
    // if(bot->getClass() == CLASS_HUNTER)
    // {
    //     return AI_VALUE2(float, "distance", "current target") > 7.5f;
    // }
    // else
    return !(bot->getClass() == CLASS_HUNTER) && AI_VALUE2(float, "distance", "current target") > 5.0f;
}
bool EpochStackAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "chrono-lord epoch");
    if (!boss) { return false; }

    float maxMovement = 10.0f;
    // if(bot->getClass() == CLASS_HUNTER)
    // {
    //     return Move(bot->GetAngle(boss), fmin(bot->GetExactDist2d(boss) - 6.5f, maxMovement));
    // }
    // else
    return Move(bot->GetAngle(boss), fmin(bot->GetExactDist2d(boss), maxMovement));
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float EpochMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "chrono-lord epoch");
    if (!boss) { return 1.0f; }

    if (bot->getClass() == CLASS_HUNTER) { return 1.0f; }

    if (dynamic_cast<FleeAction*>(action)) { return 0.0f; }

    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonCoSStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Meathook
    // Can tank this in a fixed position to allow healer to LoS the stun, probably not necessary

    // Salramm the Fleshcrafter
    triggers.push_back(new TriggerNode("explode ghoul",
        NextAction::array(0, new NextAction("explode ghoul spread", ACTION_MOVE + 5), nullptr)));

    // Chrono-Lord Epoch
    // Not sure if this actually works, I think I've seen him charge melee characters..?
    triggers.push_back(new TriggerNode("epoch ranged",
        NextAction::array(0, new NextAction("epoch stack", ACTION_MOVE + 5), nullptr)));

    // Mal'Ganis

    // Infinite Corruptor (Heroic only)
}

void WotlkDungeonCoSStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new EpochMultiplier(botAI));
}
