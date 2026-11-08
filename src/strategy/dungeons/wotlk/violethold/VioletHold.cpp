#include "VioletHold.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool ErekemTargetTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "erekem");
    if (!boss) { return false; }

    return botAI->IsDps(bot);
}

bool IchoronTargetTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ichoron");
    if (!boss) { return false; }

    return !botAI->IsHeal(bot);
}

bool VoidShiftTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "zuramat the obliterator");
    if (!boss) { return false; }

    return bot->HasAura(SPELL_VOID_SHIFTED) && !botAI->IsHeal(bot);
}

bool ShroudOfDarknessTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "zuramat the obliterator");
    if (!boss) { return false; }

    return boss->HasAura(SPELL_SHROUD_OF_DARKNESS);
}

bool CyanigosaPositioningTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "cyanigosa");
    if (!boss) { return false; }

    // Include healers here for now, otherwise they stand in things
    return !botAI->IsTank(bot) && !botAI->IsRangedDps(bot);
    // return botAI->IsMelee(bot) && !botAI->IsTank(bot);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AttackErekemAction::Execute(Event event [[maybe_unused]])
{
    // Focus boss first, adds after
    Unit* boss = AI_VALUE2(Unit*, "find target", "erekem");
    if (!boss) { return false; }

    if (AI_VALUE(Unit*, "current target") != boss)
        {
            return Attack(boss);
        }
    return false;
}

bool AttackIchorGlobuleAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ichoron");
    if (!boss) { return false; }

    Unit* currentTarget = AI_VALUE(Unit*, "current target");

    // Tank prioritise boss if it's up
    if (botAI->IsTank(bot) && !boss->HasAura(SPELL_DRAINED))
    {
        if (AI_VALUE(Unit*, "current target") != boss)
        {
            return Attack(boss);
        }
        return false;
    }

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_ICHOR_GLOBULE)
        {
            // Check IDs here, NOT Unit* pointers:
            // Don't keep swapping between sentries.
            // If we're already attacking one, don't retarget another
            if (currentTarget && currentTarget->GetEntry() == NPC_ICHOR_GLOBULE)
            {
                return false;
            }
            return Attack(unit);
        }
    }
    // No ichor globules left alive, fall back to targeting boss
    if (currentTarget != boss)
    {
        return Attack(boss);
    }

    return false;
}

bool AttackVoidSentryAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "zuramat the obliterator");
    if (!boss) { return false; }

    Unit* currentTarget = AI_VALUE(Unit*, "current target");

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    // GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_VOID_SENTRY)
        {
            // Check IDs here, NOT Unit* pointers:
            // Don't keep swapping between sentries.
            // If we're already attacking one, don't retarget another
            if (currentTarget && currentTarget->GetEntry() == NPC_VOID_SENTRY)
            {
                return false;
            }
            return Attack(unit);
        }
    }
    // No void sentries left alive, fall back to targeting boss
    if (currentTarget != boss)
    {
        return Attack(boss);
    }

    return false;
}

bool StopAttackAction::Execute(Event event [[maybe_unused]])
{
    return bot->AttackStop();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float ErekemMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "erekem");
    if (!boss || !botAI->IsDps(bot)) { return 1.0f; }

    if (dynamic_cast<DpsAssistAction*>(action))
    {
        return 0.0f;
    }
    if (action->getThreatType() == Action::ActionThreatType::Aoe)
    {
        return 0.0f;
    }
    return 1.0f;
}

float IchoronMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ichoron");
    if (!boss) { return 1.0f; }

    if (dynamic_cast<DpsAssistAction*>(action)
        || dynamic_cast<TankAssistAction*>(action)
        || dynamic_cast<DropTargetAction*>(action))
    {
        return 0.0f;
    }
    return 1.0f;
}

float ZuramatMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "zuramat the obliterator");
    if (!boss) { return 1.0f; }

    if (bot->HasAura(SPELL_VOID_SHIFTED))
    {
        if (dynamic_cast<DpsAssistAction*>(action) || dynamic_cast<TankAssistAction*>(action))
        {
            return 0.0f;
        }
    }

    if (boss->HasAura(SPELL_SHROUD_OF_DARKNESS) && dynamic_cast<AttackAction*>(action))
    {
        return 0.0f;
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonVHStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Erekem
    // This boss has many purgable buffs, purging/dispels could be merged into generic strats though
    triggers.push_back(new TriggerNode("erekem target",
        NextAction::array(0, new NextAction("attack erekem", ACTION_RAID + 1), nullptr)));

    // Moragg
    // TODO: This guy has Optic Link which may require moving, add if needed

    // Ichoron
    triggers.push_back(new TriggerNode("ichoron target",
        NextAction::array(0, new NextAction("attack ichor globule", ACTION_RAID + 1), nullptr)));

    // Xevozz
    // TODO: Revisit in heroics, waypoints back and forth on stairs. Need to test with double beacon spawn

    // Lavanthor
    // Tank & spank

    // Zuramat the Obliterator
    triggers.push_back(new TriggerNode("shroud of darkness",
        NextAction::array(0, new NextAction("stop attack", ACTION_HIGH + 5), nullptr)));
    triggers.push_back(new TriggerNode("void shift",
        NextAction::array(0, new NextAction("attack void sentry", ACTION_RAID + 1), nullptr)));

    // Cyanigosa
    triggers.push_back(new TriggerNode("cyanigosa positioning",
        NextAction::array(0, new NextAction("rear flank", ACTION_MOVE + 5), nullptr)));
}

void WotlkDungeonVHStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new ErekemMultiplier(botAI));
    multipliers.push_back(new IchoronMultiplier(botAI));
    multipliers.push_back(new ZuramatMultiplier(botAI));
}
