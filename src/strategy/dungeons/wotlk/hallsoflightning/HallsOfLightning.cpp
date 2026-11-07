#include "HallsOfLightning.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"
#include "WarriorActions.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool StormforgedLieutenantTrigger::IsActive()
{
    if (!botAI->IsDps(bot)) { return false; }

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");

    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->IsInCombat() &&
            unit->GetEntry() == NPC_STORMFORGED_LIEUTENANT)
        {
            return true;
        }
    }
    return false;
}

bool BjarngrimWhirlwindTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "general bjarngrim");
    if (!boss) { return false; }

    return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_WHIRLWIND_BJARNGRIM);
}

bool VolkhanTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "volkhan");
    return boss && !botAI->IsTank(bot) && !botAI->IsHeal(bot);
}

bool IonarStaticOverloadTrigger::IsActive()
{
    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Unit* unit = botAI->GetUnit(member);
        if (unit && unit->HasAura(SPELL_STATIC_OVERLOAD))
        {
            return true;
        }
    }
    return false;
}

bool IonarBallLightningTrigger::IsActive()
{
    if (botAI->IsMelee(bot)) { return false; }

    Unit* boss = AI_VALUE2(Unit*, "find target", "ionar");
    if (!boss) { return false; }

    return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_BALL_LIGHTNING);
}

bool IonarTankAggroTrigger::IsActive()
{
    if (!botAI->IsTank(bot)) { return false; }

    Unit* boss = AI_VALUE2(Unit*, "find target", "ionar");
    if (!boss) { return false; }

    return AI_VALUE2(bool, "has aggro", "current target");
}

bool IonarDisperseTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ionar");
    if (!boss) { return false; }

    return !bot->CanSeeOrDetect(boss) || boss->FindCurrentSpellBySpellId(SPELL_DISPERSE);
}

bool LokenRangedTrigger::IsActive()
{
    return !botAI->IsMelee(bot) && AI_VALUE2(Unit*, "find target", "loken");
}

bool LokenLightningNovaTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "loken");
    if (!boss) { return false; }

    return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_LIGHTNING_NOVA);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool BjarngrimTargetAction::Execute(Event event [[maybe_unused]])
{
    Unit* target = nullptr;

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector npcs = AI_VALUE(GuidVector, "possible targets");

    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == NPC_STORMFORGED_LIEUTENANT)
        {
            target = unit;
            break;
        }
    }

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    // There are two, we don't want to ping-pong between them if we're attacking one already
    if (target && currentTarget && currentTarget->GetEntry() == NPC_STORMFORGED_LIEUTENANT)
    {
        return false;
    }

    if (AI_VALUE(Unit*, "current target") == target)
    {
        return false;
    }

    return Attack(target);
}

bool AvoidWhirlwindAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "general bjarngrim");
    if (!boss) { return false; }

    float distance = bot->GetExactDist2d(boss->GetPosition());
    float radius = 8.0f;
    float distanceExtra = 2.0f;

    if (distance < radius + distanceExtra)
    {
        return MoveAway(boss, radius + distanceExtra - distance);
    }

    return false;
}

bool VolkhanTargetAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "volkhan");
    if (!boss || AI_VALUE(Unit*, "current target") == boss)
    {
        return false;
    }

    return Attack(boss);
}

bool StaticOverloadSpreadAction::Execute(Event event [[maybe_unused]])
{
    float radius = 8.0f;
    float distanceExtra = 2.0f;

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        if (bot->GetGUID() == member)
        {
            continue;
        }

        Unit* unit = botAI->GetUnit(member);
        if (unit && unit->HasAura(SPELL_STATIC_OVERLOAD)
            && bot->GetExactDist2d(unit) < radius)
        {
            return MoveAway(unit, radius + distanceExtra - bot->GetExactDist2d(unit));
        }
    }
    return false;
}

bool BallLightningSpreadAction::Execute(Event event [[maybe_unused]])
{
    float radius = 6.0f;
    float distanceExtra = 1.0f;

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        if (bot->GetGUID() == member)
        {
            continue;
        }
        Unit* unit = botAI->GetUnit(member);
        if (unit && bot->GetExactDist2d(unit) < radius + distanceExtra)
        {
            return MoveAway(unit, radius + distanceExtra - bot->GetExactDist2d(unit));
        }
    }
    return false;
}

bool IonarTankPositionAction::isUseful() { return bot->GetExactDist2d(IONAR_TANK_POSITION) > 10.0f; }
bool IonarTankPositionAction::Execute(Event event [[maybe_unused]])
{
    return MoveTo(bot->GetMapId(), IONAR_TANK_POSITION.GetPositionX(), IONAR_TANK_POSITION.GetPositionY(), IONAR_TANK_POSITION.GetPositionZ(),
                  false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
}

bool DispersePositionAction::isUseful() { return bot->GetExactDist2d(DISPERSE_POSITION) > 8.0f; }
bool DispersePositionAction::Execute(Event event [[maybe_unused]])
{
    return MoveTo(bot->GetMapId(), DISPERSE_POSITION.GetPositionX(), DISPERSE_POSITION.GetPositionY(), DISPERSE_POSITION.GetPositionZ(),
                  false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
}

bool LokenStackAction::isUseful()
{
    // Minimum hunter range is 5, but values too close to this seem to cause issues..
    // Hunter bots will try and melee in between ranged attacks, or just melee entirely at 5 as they are in range.
    // 6.5 or 7.0 solves this for this boss.
    if(bot->getClass() == CLASS_HUNTER)
    {
        return AI_VALUE2(float, "distance", "current target") > 6.5f;
    }
    // else
    return AI_VALUE2(float, "distance", "current target") > 2.0f;
}
bool LokenStackAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "loken");
    if (!boss) { return false; }

    float maxMovement = 10.0f;
    if (!boss->HasUnitState(UNIT_STATE_CASTING))
    {
        if(bot->getClass() == CLASS_HUNTER)
        {
            return Move(bot->GetAngle(boss), fmin(bot->GetExactDist2d(boss) - 6.5f, maxMovement));
        }
        // else
        return Move(bot->GetAngle(boss), fmin(bot->GetExactDist2d(boss), maxMovement));
    }

    return false;
}


bool AvoidLightningNovaAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "loken");
    if (!boss) { return false; }

    float distance = bot->GetExactDist2d(boss->GetPosition());
    float radius = 20.0f;
    float distanceExtra = 2.0f;

    if (distance < radius + distanceExtra)
    {
        return MoveAway(boss, radius + distanceExtra - distance);
    }

    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float BjarngrimMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "general bjarngrim");
    if (!boss || botAI->IsHeal(bot)) { return 1.0f; }

    if (boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_WHIRLWIND_BJARNGRIM))
    {
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidWhirlwindAction*>(action))
        {
            return 0.0f;
        }
    }

    // Detect boss adds this way as sometimes they don't get added to threat table on dps bots,
    // and some dps just stand at range and don't engage the boss at all as they can't find the adds
    // Unit* boss_add = AI_VALUE2(Unit*, "find target", "stormforged lieutenant");
    Unit* boss_add = nullptr;
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_STORMFORGED_LIEUTENANT)
        {
            boss_add = unit;
            break;
        }
    }

    if (!boss_add || botAI->IsTank(bot)) { return 1.0f; }

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

float VolkhanMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "volkhan");
    if (!boss || botAI->IsTank(bot) || botAI->IsHeal(bot)) { return 1.0f; }

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

float IonarMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ionar");
    if (!boss) { return 1.0f; }

    // Check if the boss has dispersed into Sparks (not visible).
    if (!bot->CanSeeOrDetect(boss))
    {
        // Block MovementActions except for specific exceptions.
        if (dynamic_cast<MovementAction*>(action)
            && !dynamic_cast<DispersePositionAction*>(action)
            && !dynamic_cast<StaticOverloadSpreadAction*>(action))
        {
            return 0.0f;
        }
    }

    if (boss->FindCurrentSpellBySpellId(SPELL_DISPERSE))
    {
        // Explicitly block the CastChargeAction during dispersal.
        if (dynamic_cast<CastChargeAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}
float LokenMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "loken");
    if (!boss) { return 1.0f; }

    // Prevent FleeAction from being executed.
    if (dynamic_cast<FleeAction*>(action)) { return 0.0f; }

    // Prevent MovementActions during Lightning Nova unless it's AvoidLightningNovaAction.
    if (boss->FindCurrentSpellBySpellId(SPELL_LIGHTNING_NOVA))
    {
        if (dynamic_cast<MovementAction*>(action)
            && !dynamic_cast<AvoidLightningNovaAction*>(action))
        {
            return 0.0f;
        }

        // Specifically prevent Charge during Lightning Nova.
        if (dynamic_cast<CastChargeAction*>(action))
        {
            return 0.0f;
        }
    }

    return 1.0f; // Default multiplier value for other cases.
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonHoLStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // General Bjarngrim
    triggers.push_back(new TriggerNode("stormforged lieutenant",
        NextAction::array(0, new NextAction("bjarngrim target", ACTION_RAID + 5), nullptr)));
    triggers.push_back(new TriggerNode("whirlwind",
        NextAction::array(0, new NextAction("avoid whirlwind", ACTION_RAID + 4), nullptr)));

    // Volkhan
    triggers.push_back(new TriggerNode("volkhan",
        NextAction::array(0, new NextAction("volkhan target", ACTION_RAID + 5), nullptr)));

    // Ionar
    triggers.push_back(new TriggerNode("ionar disperse",
        NextAction::array(0, new NextAction("disperse position", ACTION_MOVE + 5), nullptr)));
    triggers.push_back(new TriggerNode("ionar tank aggro",
        NextAction::array(0, new NextAction("ionar tank position", ACTION_MOVE + 4), nullptr)));
    triggers.push_back(new TriggerNode("static overload",
        NextAction::array(0, new NextAction("static overload spread", ACTION_MOVE + 3), nullptr)));
    // TODO: Targeted player can dodge the ball, but a single player soaking it isn't too bad to heal
    triggers.push_back(new TriggerNode("ball lightning",
        NextAction::array(0, new NextAction("ball lightning spread", ACTION_MOVE + 2), nullptr)));

    // Loken
    triggers.push_back(new TriggerNode("lightning nova",
        NextAction::array(0, new NextAction("avoid lightning nova", ACTION_MOVE + 5), nullptr)));
    triggers.push_back(new TriggerNode("loken ranged",
        NextAction::array(0, new NextAction("loken stack", ACTION_MOVE + 4), nullptr)));
}

void WotlkDungeonHoLStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new BjarngrimMultiplier(botAI));
    multipliers.push_back(new VolkhanMultiplier(botAI));
    multipliers.push_back(new IonarMultiplier(botAI));
    multipliers.push_back(new LokenMultiplier(botAI));
}
