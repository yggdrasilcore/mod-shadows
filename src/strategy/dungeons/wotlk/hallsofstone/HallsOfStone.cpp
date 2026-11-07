#include "HallsOfStone.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool KrystallusGroundSlamTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "krystallus");
    if (!boss) { return false; }

    // Check both of these... the spell is applied first, debuff later.
    // Neither is active for the full duration so we need to trigger off both
    return bot->HasAura(SPELL_GROUND_SLAM) || bot->HasAura(DEBUFF_GROUND_SLAM);
}

bool SjonnirLightningRingTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sjonnir the ironshaper");
    if (!boss) { return false; }

    return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_LIGHTNING_RING);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool ShatterSpreadAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "krystallus");
    if (!boss) { return false; }

    float radius = 40.0f;
    Unit* closestMember = nullptr;

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Unit* unit = botAI->GetUnit(member);
        if (!unit || bot->GetGUID() == member)
        {
            continue;
        }
        if (!closestMember || bot->GetExactDist2d(unit) < bot->GetExactDist2d(closestMember))
        {
            closestMember = unit;
        }
    }

    if (closestMember && bot->GetExactDist2d(closestMember) < radius)
    {
        // Move in small increments so course can be corrected, otherwise two bots may
        // run in the same direction and not adjust until they reach the destination
        // return MoveAway(closestMember, radius - bot->GetExactDist2d(closestMember));
        return MoveAway(closestMember, 5.0f);
    }

    return false;
}

bool AvoidLightningRingAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sjonnir the ironshaper");
    if (!boss) { return false; }

    float distance = bot->GetExactDist2d(boss->GetPosition());
    float radius = 10.0f;
    float distanceExtra = 2.0f;

    if (distance < radius + distanceExtra)
    {
        return MoveAway(boss, radius + distanceExtra - distance);
    }

    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float KrystallusMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "krystallus");
    if (!boss) { return 1.0f; }

    // Check both of these... the spell is applied first, debuff later.
    // Neither is active for the full duration so we need to trigger off both
    if (bot->HasAura(SPELL_GROUND_SLAM) || bot->HasAura(DEBUFF_GROUND_SLAM))
    {
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<ShatterSpreadAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float SjonnirMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sjonnir the ironshaper");
    if (!boss) { return 1.0f; }

    if (boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_LIGHTNING_RING))
        {
            // Problematic since there's a lot of movement on this boss, will prevent players from positioning
            // well to deal with adds etc. during the channel period. Takes a bit of work to improve this though
            if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidLightningRingAction*>(action))
            {
                return 0.0f;
            }
        }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonHoSStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Maiden of Grief
    // TODO: Jump into damage during shock of sorrow?

    // Krystallus
    // TODO: I think bots need to dismiss pets on this, or they nuke players they are standing close to
    triggers.push_back(new TriggerNode("ground slam",
        NextAction::array(0, new NextAction("shatter spread", ACTION_RAID + 5), nullptr)));

    // Tribunal of Ages
    // Seems fine, maybe add focus targeting strat if needed on heroic.
    // Main issue is dps will immediately rambo in and sometimes die before tank gets aggro,
    // this is mostly an issue with the bot AI as they do it on every fight

    // Sjonnir The Ironshaper
    // Possibly tank in place in the middle of the room, assign a dps to adds?
    triggers.push_back(new TriggerNode("lightning ring",
        NextAction::array(0, new NextAction("avoid lightning ring", ACTION_RAID + 5), nullptr)));
}

void WotlkDungeonHoSStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new KrystallusMultiplier(botAI));
    multipliers.push_back(new SjonnirMultiplier(botAI));
}
