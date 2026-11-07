#include "OldKingdom.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool NadoxGuardianTrigger::IsActive()
{
    if (botAI->IsHeal(bot)) { return false; }

    Unit* boss = AI_VALUE2(Unit*, "find target", "elder nadox");
    Unit* guardian = AI_VALUE2(Unit*, "find target", "ahn'kahar guardian");

    return boss && guardian;
}

bool JedogaVolunteerTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "jedoga shadowseeker");
    if (!boss) { return false; }

    // Volunteer is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_TWILIGHT_VOLUNTEER)
        {
            return true;
        }
    }
    return false;
}

bool ShadowCrashTrigger::IsActive()
{
    Unit* unit = AI_VALUE2(Unit*, "find target", "forgotten one");
    if (!unit) { return false; }

    return !botAI->IsMelee(bot);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AttackNadoxGuardianAction::Execute(Event event [[maybe_unused]])
{
    Unit* target = AI_VALUE2(Unit*, "find target", "ahn'kahar guardian");
    if (!target || AI_VALUE(Unit*, "current target") == target)
    {
        return false;
    }
    return Attack(target);
}

bool AttackJedogaVolunteerAction::Execute(Event event [[maybe_unused]])
{
    Unit* target = nullptr;
    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (!unit) // Null check for safety
        {
            continue; // Skip null or invalid units
        }
        if (unit && unit->GetEntry() == NPC_TWILIGHT_VOLUNTEER)
        {
            target = unit;
            break;
        }
    }

    if (!target || AI_VALUE(Unit*, "current target") == target)
    {
        return false;
    }
    return Attack(target);
}

bool AvoidShadowCrashAction::Execute(Event event [[maybe_unused]])
{
    // Could check all enemy units in range as it's possible to pull multiple of these mobs.
    // They should really be killed 1 by 1, multipulls are messy so we just handle singles for now
    Unit* unit = AI_VALUE2(Unit*, "find target", "forgotten one");
    if (!unit) { return false; }

    Unit* victim = nullptr;
    float radius = 10.0f;
    float targetDist = radius + 2.0f;

    // Actively move if targeted by a shadow crash.
    // Spell check not needed, they don't have any other non-instant casts
    if (unit->HasUnitState(UNIT_STATE_CASTING)) // && unit->FindCurrentSpellBySpellId(SPELL_SHADOW_CRASH))
    {
        // This doesn't seem to avoid casts very well, perhaps because this isn't checked while allies are casting.
        // TODO: Revisit if this is an issue in heroics, otherwise ignore shadow crashes for the most part.
        victim = botAI->GetUnit(unit->GetTarget());
        float distance = bot->GetExactDist2d(victim->GetPosition());

        if (victim && distance < radius)
        {
            return MoveAway(victim, targetDist - distance);
        }
    }

    // Otherwise ranged members passively spread, to avoid AoE overlap
    if (botAI->IsMelee(bot)) { return false; }

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Unit* unit = botAI->GetUnit(member);
        if (!unit || bot->GetGUID() == member)
        {
            continue;
        }
        float currentDist = bot->GetExactDist2d(botAI->GetUnit(member));
        if (currentDist < radius)
        {
            return MoveAway(botAI->GetUnit(member), targetDist - currentDist);
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float ElderNadoxMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "elder nadox");
    if (!boss) { return 1.0f; }

    Unit* guardian = AI_VALUE2(Unit*, "find target", "ahn'kahar guardian");
    if (guardian)
    {
        if (dynamic_cast<DpsAssistAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float JedogaShadowseekerMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "jedoga shadowseeker");
    if (!boss) { return 1.0f; }

    Unit* volunteer = nullptr;
    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_TWILIGHT_VOLUNTEER)
        {
            volunteer = unit;
            break;
        }
    }

    if (volunteer)
    {
        if (dynamic_cast<DpsAssistAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float ForgottenOneMultiplier::GetValue(Action* action)
{
    Unit* unit = AI_VALUE2(Unit*, "find target", "forgotten one");
    if (!unit) { return 1.0f; }

    if (bot->isMoving())
    {
        if (dynamic_cast<MovementAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonOKStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Elder Nadox
    triggers.push_back(new TriggerNode("nadox guardian",
        NextAction::array(0, new NextAction("attack nadox guardian", ACTION_RAID + 5), nullptr)));

    // Prince Taldaram
    // Flame Orb spawns in melee, doesn't have a clear direction until it starts moving.
    // Maybe not worth trying to avoid and just heal through. Only consideration is not to have ranged
    // players anywhere near melee when it spawns

    // Jedoga Shadowseeker
    triggers.push_back(new TriggerNode("jedoga volunteer",
        NextAction::array(0, new NextAction("attack jedoga volunteer", ACTION_RAID + 5), nullptr)));

    // Herald Volazj
    // Trash mobs before him have a big telegraphed shadow crash spell,
    // this can be avoided and is intended to be dodged
    triggers.push_back(new TriggerNode("shadow crash",
        NextAction::array(0, new NextAction("avoid shadow crash", ACTION_MOVE + 5), nullptr)));
    // Volazj is not implemented properly in AC, insanity phase does nothing.

    // Amanitar (Heroic Only)
    // TODO: once I get to heroics
}

void WotlkDungeonOKStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new ElderNadoxMultiplier(botAI));
    multipliers.push_back(new JedogaShadowseekerMultiplier(botAI));
    multipliers.push_back(new ForgottenOneMultiplier(botAI));
}
