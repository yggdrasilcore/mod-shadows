#include "Oculus.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "Unit.h"
#include "LastSpellCastValue.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "FollowActions.h"
#include "ReachTargetActions.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool DrakosUnstableSphereTrigger::IsActive()
{
    // Doesn't seem to be much point trying to get melee to dodge this,
    // they get hit anyway and it just causes a lot of running around and chaos
    // if (botAI->IsMelee(bot)) { return false; }
    if (botAI->IsTank(bot)) { return false; }

    GuidVector targets = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (unit && unit->GetEntry() == NPC_UNSTABLE_SPHERE)
        {
            return true;
        }
    }
    return false;
}

bool DrakeMountTrigger::IsActive()
{
    Player* master = botAI->GetMaster();
    if (!master) { return false; }

    return master->GetVehicleBase() && !bot->GetVehicleBase();
}

bool DrakeDismountTrigger::IsActive()
{
    Player* master = botAI->GetMaster();
    if (!master) { return false; }

    return !master->GetVehicleBase() && bot->GetVehicleBase();
}

bool GroupFlyingTrigger::IsActive()
{
    Player* master = botAI->GetMaster();
    if (!master) { return false; }

    return master->GetVehicleBase() && bot->GetVehicleBase();
}

bool DrakeCombatTrigger::IsActive()
{
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    return !targets.empty();
}

bool VarosCloudstriderTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "varos cloudstrider");
    if (!boss) { return false; }

    return true;
}

bool UromArcaneExplosionTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "mage-lord urom");
    if (!boss) { return false; }

    return bool(boss->FindCurrentSpellBySpellId(SPELL_EMPOWERED_ARCANE_EXPLOSION));
}

bool UromTimeBombTrigger::IsActive()
{
    return bot->HasAura(SPELL_TIME_BOMB);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AvoidUnstableSphereAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "drakos the interrogator");
    if (!boss) { return false; }

    float radius = 12.0f;
    float extraDistance = 1.0f;
    Unit* closestSphere = nullptr;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == NPC_UNSTABLE_SPHERE && !unit->isMoving())
        {
            if (!closestSphere || bot->GetExactDist2d(unit) < bot->GetExactDist2d(closestSphere))
            {
                closestSphere = unit;
            }
        }
    }

    if (closestSphere && bot->GetExactDist2d(closestSphere) < radius + extraDistance)
    {
        return MoveAway(closestSphere, fmin(3.0f, bot->GetExactDist2d(closestSphere) - radius + extraDistance));
    }

    return false;
}

bool MountDrakeAction::isPossible() { return bot->GetMapId() == OCULUS_MAP_ID; }
bool MountDrakeAction::Execute(Event event [[maybe_unused]])
{
    std::map<int32, int32> drakeAssignments;
    // Composition can be adjusted - both 3/1/1 and 2/2/1 are good default comps
    // {Amber, Emerald, Ruby}
    std::vector<uint8> composition = {2, 2, 1};
    // std::vector<uint8> composition = {3, 1, 1};
    int32 myIndex = botAI->GetGroupSlotIndex(bot);

    Player* master = botAI->GetMaster();
    if (!master) { return false; }
    Unit* vehicle = master->GetVehicleBase();
    if (!vehicle) { return false; }

    // Subtract the player's chosen mount type from the composition so player can play whichever they prefer
    switch (vehicle->GetEntry())
    {
        case NPC_AMBER_DRAKE:
            composition[0]--;
            break;
        case NPC_EMERALD_DRAKE:
            composition[1]--;
            break;
        case NPC_RUBY_DRAKE:
            composition[2]--;
            break;
    }

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Player* player = botAI->GetPlayer(member);
        if (!player->GetSession()->IsBot()) { continue; }

        for (size_t i = 0; i < composition.size(); i++)
        {
            if (composition[i] > 0)
            {
                drakeAssignments[botAI->GetGroupSlotIndex(player)] = DRAKE_ITEMS[i];
                composition[i]--;
                break;
            }
        }
    }

    // Correct/update the drake items in inventories incase assignments have changed
    for (uint32 itemId : DRAKE_ITEMS)
    {
        Item* item = bot->GetItemByEntry(itemId);
        if (!item) { continue; }

        if (itemId == static_cast<uint32>(drakeAssignments[myIndex]))
        {
            // Use our assigned drake
            return UseItemAuto(item);
        }
        // Else assigned drake is different, destroy old drake
        uint32 count = 1;
        bot->DestroyItemCount(item, count, true);
        break;
    }

    // Bot does not have the correct drake item
    bot->AddItem(drakeAssignments[myIndex], 1);
    return false;
}

bool DismountDrakeAction::Execute(Event event [[maybe_unused]])
{
    if (bot->GetVehicle())
    {
        bot->ExitVehicle();
        return true;
    }
    return false;
}

bool OccFlyDrakeAction::Execute(Event event [[maybe_unused]])
{
    Player* master = botAI->GetMaster();
    if (!master) { return false; }
    Unit* masterVehicle = master->GetVehicleBase();
    Unit* vehicleBase = bot->GetVehicleBase();
    if (!vehicleBase || !masterVehicle) { return false; }

    MotionMaster* mm = vehicleBase->GetMotionMaster();
    Unit* boss = AI_VALUE2(Unit*, "find target", "ley-guardian eregos");
    if (boss && !boss->HasAura(SPELL_PLANAR_SHIFT))
    {
        // Handle as boss encounter instead of formation flight
        mm->Clear(false);
        float distance = vehicleBase->GetExactDist(boss);
        float range = 55.0f;    // Drake range is 60yd
        if (distance > range)
        {
            mm->MoveForwards(boss, range - distance);
            vehicleBase->SendMovementFlagUpdate();
            return true;
        }

        vehicleBase->SetFacingToObject(boss);
        mm->MoveIdle();
        vehicleBase->SendMovementFlagUpdate();
        return false;
    }

    if (vehicleBase->GetExactDist(masterVehicle) > 20.0f)
    {
        // 3/4 of a circle, with frontal cone 90 deg unobstructed
        float angle = botAI->GetGroupSlotIndex(bot) * (2*M_PI - M_PI_2)/5 + M_PI_2;
        vehicleBase->SetCanFly(true);
        mm->MoveFollow(masterVehicle, 15.0f, angle);
        vehicleBase->SendMovementFlagUpdate();
        return true;
    }
    return false;
}

bool OccDrakeAttackAction::Execute(Event event [[maybe_unused]])
{
    vehicleBase = bot->GetVehicleBase();
    if (!vehicleBase) { return false; }

    Unit* target = AI_VALUE(Unit*, "current target");

    if (!target)
    {
        GuidVector npcs = AI_VALUE(GuidVector, "possible targets");
        for (auto& npc : npcs)
        {
            Unit* unit = botAI->GetUnit(npc);
            if (!unit || !unit->IsInCombat()) { continue; }

            target = unit;
            break;
        }
    }
    // Check this again to see if a target was assigned
    if (!target) { return false; }

    switch (vehicleBase->GetEntry())
    {
        case NPC_AMBER_DRAKE:
            return AmberDrakeAction(target);
        case NPC_EMERALD_DRAKE:
            return EmeraldDrakeAction(target);
        case NPC_RUBY_DRAKE:
            return RubyDrakeAction(target);
        default:
            break;
    }
    return false;
}

bool OccDrakeAttackAction::CastDrakeSpellAction(Unit* target, uint32 spellId, uint32 cooldown)
{
    if (botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase->AddSpellCooldown(spellId, 0, cooldown);
            return true;
        }
    return false;
}

bool OccDrakeAttackAction::AmberDrakeAction(Unit* target)
{
    Aura* shockCharges = target->GetAura(SPELL_SHOCK_CHARGE, vehicleBase->GetGUID());
    if (shockCharges && shockCharges->GetStackAmount() > 8)
    {
        // At 9 charges, better to detonate and re-channel rather than stacking the last charge due to gcd
        // If stacking Amber drakes, may need to drop this even lower as the charges stack so fast
        return CastDrakeSpellAction(target, SPELL_SHOCK_LANCE, 0);
    }

    // Deal with enrage after shock charges, as Stop Time adds 5 charges and they may get wasted
    if (target->HasAura(SPELL_ENRAGED_ASSAULT) &&
        !target->HasAura(SPELL_STOP_TIME) &&
        !vehicleBase->HasSpellCooldown(SPELL_STOP_TIME))
    {
        return CastDrakeSpellAction(target, SPELL_STOP_TIME, 60000);
    }

    if (!vehicleBase->FindCurrentSpellBySpellId(SPELL_TEMPORAL_RIFT))
    {
        return CastDrakeSpellAction(target, SPELL_TEMPORAL_RIFT, 0);
    }

    return false;
}

bool OccDrakeAttackAction::EmeraldDrakeAction(Unit* target)
{
    Aura* poisonStacks = target->GetAura(SPELL_LEECHING_POISON, vehicleBase->GetGUID());
    if (!poisonStacks || (poisonStacks->GetStackAmount() < 3 ||
                         poisonStacks->GetDuration() < 4000))
    {
        return CastDrakeSpellAction(target, SPELL_LEECHING_POISON, 0);
    }

    if (!vehicleBase->HasSpellCooldown(SPELL_TOUCH_THE_NIGHTMARE) &&
        (!target->HasAura(SPELL_TOUCH_THE_NIGHTMARE) || vehicleBase->HealthAbovePct(90)))
    {
        return CastDrakeSpellAction(target, SPELL_TOUCH_THE_NIGHTMARE, 10000);
    }

    Unit* healingTarget = nullptr;
    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Unit* unit = botAI->GetUnit(member);
        if (!unit || bot->GetGUID() == member)
        {
            continue;
        }

        Unit* drake = unit->GetVehicleBase();
        if (!drake || drake->IsFullHealth()) { continue; }

        if (!healingTarget || drake->GetHealthPct() < healingTarget->GetHealthPct() - 15.0f)
        {
            healingTarget = drake;
        }
    }

    Spell* currentSpell = vehicleBase->FindCurrentSpellBySpellId(SPELL_DREAM_FUNNEL);
    if (healingTarget)
    {
        if (!currentSpell || currentSpell->m_targets.GetUnitTarget() != healingTarget)
        {
            float distance = vehicleBase->GetExactDist(healingTarget);
            float range = 55.0f;
            if (distance > range)
            {
                MotionMaster* mm = vehicleBase->GetMotionMaster();
                mm->Clear(false);
                mm->MoveForwards(healingTarget, distance - range - 10.0f);
                vehicleBase->SendMovementFlagUpdate();
                return false;
            }
            return CastDrakeSpellAction(healingTarget, SPELL_DREAM_FUNNEL, 0);
        }
    }
    // Fill GCDs with Leeching Poison to refresh timer, rather than idling
    if (!currentSpell)
    {
        return CastDrakeSpellAction(target, SPELL_LEECHING_POISON, 0);
    }

    return false;
}

bool OccDrakeAttackAction::RubyDrakeAction(Unit* target)
{
    Aura* evasiveCharges = vehicleBase->GetAura(SPELL_EVASIVE_CHARGES);
    Aura* evasiveManeuvers = vehicleBase->GetAura(SPELL_EVASIVE_MANEUVERS);

    if (evasiveCharges)
    {
        if (evasiveManeuvers &&
            !vehicleBase->HasSpellCooldown(SPELL_MARTYR) &&
            evasiveManeuvers->GetDuration() > 10000 &&
            evasiveCharges->GetStackAmount() >= 5)
        {
            return CastDrakeSpellAction(vehicleBase, SPELL_MARTYR, 10000);
        }

        if (!vehicleBase->HasSpellCooldown(SPELL_EVASIVE_MANEUVERS) &&
            evasiveCharges->GetStackAmount() >= 10)
        {
            return CastDrakeSpellAction(vehicleBase, SPELL_EVASIVE_MANEUVERS, 5000);
        }
    }

    return CastDrakeSpellAction(target, SPELL_SEARING_WRATH, 0);
}

bool AvoidArcaneExplosionAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "mage-lord urom");
    if (!boss) { return false; }

    const Position* closestPos = nullptr;

    for (auto& position : uromSafePositions)
    {
        if (!closestPos || bot->GetExactDist(position) < bot->GetExactDist(closestPos))
            {
                closestPos = &position;
            }
    }

    if (!closestPos) { return false; }

    return MoveNear(bot->GetMapId(), closestPos->GetPositionX(), closestPos->GetPositionY(), closestPos->GetPositionZ(), 2.0f, MovementPriority::MOVEMENT_COMBAT);
}

bool TimeBombSpreadAction::Execute(Event event [[maybe_unused]])
{
    float radius = 10.0f;
    float distanceExtra = 2.0f;

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        if (bot->GetGUID() == member)
        {
            continue;
        }

        Unit* unit = botAI->GetUnit(member);
        if (unit && bot->GetExactDist2d(unit) < radius)
        {
            return MoveAway(unit, radius + distanceExtra - bot->GetExactDist2d(unit));
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float MountingDrakeMultiplier::GetValue(Action* action)
{
    // P.I.T.A bug where the bots will somehow interrupt their item spell use,
    // even though the 0.5 sec cast goes off, it puts the drake essence on 15 sec cd
    // and no drake comes down.
    // It seems like this is due to moving/other actions being processed during the 0.5 secs.
    // If we suppress everything, they seem to mount properly. A bit of a ham-fisted solution but it works
    Player* master = botAI->GetMaster();
    if (!master) { return 1.0f; }

    if (bot->GetMapId() != OCULUS_MAP_ID || !master->GetVehicleBase() || bot->GetVehicleBase()) { return 1.0f; }

    if (!dynamic_cast<MountDrakeAction*>(action))
    {
        return 0.0f;
    }
    return 1.0f;
}

float OccFlyingMultiplier::GetValue(Action* action)
{
    if (bot->GetMapId() != OCULUS_MAP_ID || !bot->GetVehicleBase()) { return 1.0f; }

    // Suppresses FollowAction as well as some attack-based movements
    if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<OccFlyDrakeAction*>(action))
    {
        return 0.0f;
    }
    return 1.0f;
}

float UromMultiplier::GetValue(Action* action)
{
    if(GetPhaseByCurrentPosition(bot) < 3)
    {
        Unit* target = action->GetTarget();
        if (target && target->GetEntry() == NPC_MAGE_LORD_UROM)
        {
            return 0.0f;
        }
    }

    Unit* boss = AI_VALUE2(Unit*, "find target", "mage-lord urom");
    if (!boss) { return 1.0f; }

    // REAL BOSS FIGHT
    if (boss->HasUnitState(UNIT_STATE_CASTING) &&
        boss->FindCurrentSpellBySpellId(SPELL_EMPOWERED_ARCANE_EXPLOSION))
    {
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidArcaneExplosionAction*>(action))
        {
            return 0.0f;
        }
    }

    // Don't bother avoiding Frostbomb for melee
    if (botAI->IsMelee(bot))
    {
        if (dynamic_cast<AvoidAoeAction*>(action))
        {
            return 0.0f;
        }
    }

    if (bot->HasAura(SPELL_TIME_BOMB))
    {
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<TimeBombSpreadAction*>(action))
        {
            return 0.0f;
        }
    }

    return 1.0f;
}

uint8 UromMultiplier::GetPhaseByCurrentPosition(Unit* unit)
{
    // Distance to return a positive match for spawn platforms, tweak slightly if needed/
    // Make sure this doesn't get too large and reach the central ring as well
    float distance = 60.0f;

    for (uint8 i = 0; i < 3; ++i)
    {
        if (unit->GetDistance(uromCoords[i][0], uromCoords[i][1], uromCoords[i][2]) < distance)
        {
            return i;
        }
    }
    return 3;
}

float EregosMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ley-guardian eregos");
    if (!boss) { return 1.0f; }

    if (boss->HasAura(SPELL_PLANAR_SHIFT) && dynamic_cast<OccDrakeAttackAction*>(action))
    {
        return 0.0f;
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonOccStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Drakos the Interrogator
    // TODO: May need work, TBA.
    triggers.push_back(new TriggerNode("unstable sphere",
        NextAction::array(0, new NextAction("avoid unstable sphere", ACTION_MOVE + 5), nullptr)));

    // DRAKES
    triggers.push_back(new TriggerNode("drake mount",
        NextAction::array(0, new NextAction("mount drake", ACTION_RAID + 5), nullptr)));
    triggers.push_back(new TriggerNode("drake dismount",
        NextAction::array(0, new NextAction("dismount drake", ACTION_RAID + 5), nullptr)));
    triggers.push_back(new TriggerNode("group flying",
        NextAction::array(0, new NextAction("occ fly drake", ACTION_NORMAL + 1), nullptr)));
    triggers.push_back(new TriggerNode("drake combat",
        NextAction::array(0, new NextAction("occ drake attack", ACTION_NORMAL + 5), nullptr)));

    // Varos Cloudstrider
    // Seems to be no way to identify the marked cores, may need to hook boss AI..
    // triggers.push_back(new TriggerNode("varos cloudstrider",
    //     NextAction::array(0, new NextAction("avoid energize cores", ACTION_RAID + 5), nullptr)));

    // Mage-Lord Urom
    triggers.push_back(new TriggerNode("arcane explosion",
        NextAction::array(0, new NextAction("avoid arcane explosion", ACTION_MOVE + 5), nullptr)));
    triggers.push_back(new TriggerNode("time bomb",
        NextAction::array(0, new NextAction("time bomb spread", ACTION_MOVE + 4), nullptr)));

    // Ley-Guardian Eregos
}

void WotlkDungeonOccStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new MountingDrakeMultiplier(botAI));
    multipliers.push_back(new OccFlyingMultiplier(botAI));
    multipliers.push_back(new UromMultiplier(botAI));
    multipliers.push_back(new EregosMultiplier(botAI));
}
