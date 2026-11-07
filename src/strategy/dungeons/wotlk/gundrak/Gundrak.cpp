#include "Gundrak.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool SladranPoisonNovaTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "slad'ran");
    if (!boss) { return false; }

    return bool(boss->FindCurrentSpellBySpellId(SPELL_POISON_NOVA));
}

bool SladranSnakeWrapTrigger::IsActive()
{
    if (!botAI->IsDps(bot)) { return false; }

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");

    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->GetEntry() == NPC_SNAKE_WRAP)
        {
            return true;
        }
    }
    return false;
}

bool GaldarahWhirlingSlashTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "gal'darah");
    return boss && boss->HasAura(SPELL_WHIRLING_SLASH);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AvoidPoisonNovaAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "slad'ran");
    if (!boss) { return false; }

    float distance = bot->GetExactDist2d(boss->GetPosition());
    float radius = 15.0f;
    float distanceExtra = 2.0f;

    if (distance < radius + distanceExtra)
    {
        return MoveAway(boss, radius + distanceExtra - distance);
    }

    return false;
}

bool AttackSnakeWrapAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "slad'ran");
    if (!boss) { return false; }

    Unit* snakeWrap [[maybe_unused]] = nullptr;
    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->GetEntry() == NPC_SNAKE_WRAP)
        {
            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (!currentTarget || currentTarget->GetEntry() != NPC_SNAKE_WRAP)
            {
            return Attack(unit);
            }
        }
    }

    return false;
}

bool AvoidWhirlingSlashAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "gal'darah");
    if (!boss) { return false; }

    float distance = bot->GetExactDist2d(boss->GetPosition());
    float radius = 5.0f;
    float distanceExtra = 2.0f;

    if (distance < radius + distanceExtra)
    {
        if (botAI->IsTank(bot))
        {
            // The boss chases tank during this, leads to jittery stutter-stepping
            // by the tank if we don't pre-move additional range. 2*radius seems ok
            return MoveAway(boss, (2.0f * radius) + distanceExtra - distance);
        }
        // else
        return MoveAway(boss, radius + distanceExtra - distance);
    }

    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float SladranMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "slad'ran");
    if (!boss) { return 1.0f; }

    if (boss->FindCurrentSpellBySpellId(SPELL_POISON_NOVA))
    {
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidPoisonNovaAction*>(action))
        {
            return 0.0f;
        }
    }

    if (!botAI->IsDps(bot)) { return 1.0f; }

    if (action->getThreatType() == Action::ActionThreatType::Aoe)
    {
        return 0.0f;
    }

    Unit* snakeWrap = nullptr;
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");
    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->GetEntry() == NPC_SNAKE_WRAP)
        {
            snakeWrap = unit;
            break;
        }
    }
    // Prevent auto-target acquisition during snake wraps
    if (snakeWrap && dynamic_cast<DpsAssistAction*>(action))
    {
        return 0.0f;
    }

    return 1.0f;
}

float GaldarahMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "gal'darah");
    if (!boss) { return 1.0f; }

    if (boss->HasAura(SPELL_WHIRLING_SLASH))
        {
            if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidWhirlingSlashAction*>(action))
            {
                return 0.0f;
            }
        }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonGDStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Moorabi

    // Drakkari Colossus

    // Slad'ran
    // TODO: Might need to add target priority for heroic on the snakes or to burn down boss.
    // Will re-test in heroic, decent dps groups should be able to blast him down with no funky strats.
    triggers.push_back(new TriggerNode("poison nova",
        NextAction::array(0, new NextAction("avoid poison nova", ACTION_RAID + 5), nullptr)));
    triggers.push_back(new TriggerNode("snake wrap",
        NextAction::array(0, new NextAction("attack snake wrap", ACTION_RAID + 4), nullptr)));

    // Gal'darah
    triggers.push_back(new TriggerNode("whirling slash",
        NextAction::array(0, new NextAction("avoid whirling slash", ACTION_RAID + 5), nullptr)));

    // Eck the Ferocious (Heroic only)
}

void WotlkDungeonGDStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new SladranMultiplier(botAI));
    multipliers.push_back(new GaldarahMultiplier(botAI));
}
