#include "DrakTharonKeep.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool CorpseExplodeTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "trollgore");
    if (!boss) { return false; }

    float distance = 6.0f;  // 5 unit radius, 1 unit added as buffer
    GuidVector corpses = AI_VALUE(GuidVector, "nearest corpses");
    for (auto i = corpses.begin(); i != corpses.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_DRAKKARI_INVADER)
        {
            if (bot->GetExactDist2d(unit) < distance)
            {
                return true;
            }
        }
    }
    return false;
}

bool ArcaneFieldTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "novos the summoner");
    if (boss)
    {
        return boss->HasUnitState(UNIT_STATE_CASTING) && boss->FindCurrentSpellBySpellId(SPELL_ARCANE_FIELD);
    }
    return false;
}

// bool CrystalHandlerTrigger::IsActive()
// {
//     Unit* boss = AI_VALUE2(Unit*, "find target", "novos the summoner");
//     if (!boss) { return false; }

//     // Target is not findable from threat table using AI_VALUE2(),
//     // therefore need to search manually for the unit name
//     GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

//     for (auto i = targets.begin(); i != targets.end(); ++i)
//     {
//         Unit* unit = botAI->GetUnit(*i);
//         if (unit && unit->GetEntry() == NPC_CRYSTAL_HANDLER)
//         {
//             return true;
//         }
//     }
//     return false;
// }

bool GiftOfTharonjaTrigger::IsActive()
{
    return bot->HasAura(SPELL_GIFT_OF_THARONJA);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool CorpseExplodeSpreadAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "trollgore");
    if (!boss) { return false; }

    float distance = 5.0f;
    float distanceExtra = 2.0f;
    GuidVector corpses = AI_VALUE(GuidVector, "nearest corpses");
    for (auto i = corpses.begin(); i != corpses.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_DRAKKARI_INVADER)
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

bool AvoidArcaneFieldAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "novos the summoner");
    if (!boss) { return false; }

    float distance = 12.0f;  // 11 unit radius, 1 unit added as buffer
    if (bot->GetExactDist2d(boss) < distance)
    {
        return MoveAway(boss, distance - bot->GetExactDist2d(boss));
    }
    return false;
}

bool NovosDefaultPositionAction::isUseful()
{
    // Distance to tether to centre of room
    float threshold = 15.0f;
    return bot->GetDistance(NOVOS_PARTY_POSITION) > threshold;
}
bool NovosDefaultPositionAction::Execute(Event event [[maybe_unused]])
{
    float clusterDistance = 4.0f;
    // Only reposition if we're not killing anything
    if (!bot->GetTarget())
    {
        return MoveNear(bot->GetMap()->GetId(),
            NOVOS_PARTY_POSITION.GetPositionX(),
            NOVOS_PARTY_POSITION.GetPositionY(),
            NOVOS_PARTY_POSITION.GetPositionZ(),
            clusterDistance, MovementPriority::MOVEMENT_NORMAL);
    }
    return false;
}

bool NovosTargetPriorityAction::Execute(Event event [[maybe_unused]])
{
    // TODO: This can be improved, some parts are still buggy.
    // But it works for now and this fight is very easy

    // Designate a dps char to handle the stairs adds.
    // This is probably better as a melee, so just pick the first
    // melee dps in the party. If none exist, pick the first ranged.

    // TODO: Switch to botAI->Index instead, cleaner
    Player* stairsDps = nullptr;
    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Player* groupMember = botAI->GetPlayer(member);
        if (!groupMember) { continue; }

        if (botAI->IsDps(groupMember))
        {
            if (botAI->IsMelee(groupMember))
            {
                // Found our first melee dps, grab handle and break
                stairsDps = groupMember;
                break;
            }
            else
            {
                // Ranged dps, only set if none already assigned.
                // Don't break, we want to keep searching for a melee instead.
                if (!stairsDps)
                {
                    stairsDps = groupMember;
                }
            }
        }
    }

    Unit* selectedTargets[2] = {nullptr, nullptr};
    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (!unit) { continue; }
        uint32 creatureId = unit->GetEntry();

        // Tank priority:
        // Hulking Corpse -> Crystal Handler
        if (botAI->IsTank(bot))
        {
            if (creatureId == NPC_HULKING_CORPSE)
            {
                selectedTargets[0] = unit;
            }
            else if (creatureId == NPC_CRYSTAL_HANDLER)
            {
                selectedTargets[1] = unit;
            }
        }
        // Dedicated stairs dps is assigned.
        // Priority: Risen Shadowcaster -> Fetid Troll Corpse
        else if (stairsDps && bot == stairsDps)
        {
            if (creatureId == NPC_RISEN_SHADOWCASTER)
            {
                if (!selectedTargets[0] || bot->GetDistance(unit) < bot->GetDistance(selectedTargets[0]) - 5.0f)
                {
                    selectedTargets[0] = unit;
                }

            }
            else if (creatureId == NPC_FETID_TROLL_CORPSE)
            {
                if (!selectedTargets[1] || bot->GetDistance(unit) < bot->GetDistance(selectedTargets[1]) - 5.0f)
                {
                    selectedTargets[1] = unit;
                }
            }
        }
        // All other dps priority:
        // Crystal Handler -> Hulking Corpse
        else if (botAI->IsDps(bot))
        {
            if (creatureId == NPC_CRYSTAL_HANDLER)
            {
                selectedTargets[0] = unit;
            }
            else if (creatureId == NPC_HULKING_CORPSE)
            {
                selectedTargets[1] = unit;
            }
        }
    }

    for (Unit* primaryTarget : selectedTargets)
    {
        // Attack the first valid target in the priority list
        if (primaryTarget)
        {
            if (AI_VALUE(Unit*, "current target") != primaryTarget)
            {
                // bot->Yell(primaryTarget->GetName(), LANG_UNIVERSAL);
                return Attack(primaryTarget);
            }
            // Don't continue loop here, the target exists so we don't
            // want to move down the prio list. We just don't need to send attack
            // command again, just return false and exit the loop that way
            return false;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float NovosMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "novos the summoner");
    if (!boss) { return 1.0f; }

    if (boss->FindCurrentSpellBySpellId(SPELL_ARCANE_FIELD) && bot->GetTarget())
    {
        if (dynamic_cast<DpsAssistAction*>(action)
            || dynamic_cast<TankAssistAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float TharonjaMultiplier::GetValue(Action* action)
{
    if (!bot->HasAura(SPELL_GIFT_OF_THARONJA)) { return 1.0f; }

    // Suppress all skills that are not enabled in skeleton form.
    // Still allow non-ability actions such as movement
    if (dynamic_cast<CastSpellAction*>(action)
        && !dynamic_cast<CastSlayingStrikeAction*>(action)
        && !dynamic_cast<CastTauntAction*>(action)
        && !dynamic_cast<CastBoneArmorAction*>(action)
        && !dynamic_cast<CastTouchOfLifeAction*>(action))
    {
        return 0.0f;
    }
    // Also suppress FleeAction to prevent ranged characters from avoiding melee range
    if (dynamic_cast<FleeAction*>(action))
    {
        return 0.0f;
    }

    // Tanks should only taunt, no slaying strike
    if (botAI->IsTank(bot))
    {
        if (dynamic_cast<CastSlayingStrikeAction*>(action))
        {
            return 0.0f;
        }
    }
    // Dps & healer should not taunt
    else
    {
        if (dynamic_cast<CastTauntAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonDTKStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Trollgore
    triggers.push_back(new TriggerNode("corpse explode",
        NextAction::array(0, new NextAction("corpse explode spread", ACTION_MOVE + 5), nullptr)));

    // Novos the Summoner
    // TODO: Can be improved - it's a pretty easy fight but complex to program, revisit if needed
    triggers.push_back(new TriggerNode("arcane field",
        NextAction::array(0, new NextAction("avoid arcane field", ACTION_MOVE + 5), nullptr)));
    triggers.push_back(new TriggerNode("arcane field",
        NextAction::array(0, new NextAction("novos positioning", ACTION_MOVE + 4), nullptr)));
    triggers.push_back(new TriggerNode("arcane field",
        NextAction::array(0, new NextAction("novos target priority", ACTION_NORMAL + 1), nullptr)));

    // King Dred
    // TODO: Fear ward / tremor totem, or general anti-fear strat development

    //The Prophet Tharon'ja
    triggers.push_back(new TriggerNode("gift of tharon'ja",
        NextAction::array(0, new NextAction("touch of life", ACTION_NORMAL + 5), nullptr)));
    triggers.push_back(new TriggerNode("gift of tharon'ja",
        NextAction::array(0, new NextAction("bone armor", ACTION_NORMAL + 4), nullptr)));
    // Run ranged chars (who would normally stand at range) into melee, to dps in skeleton form
    triggers.push_back(new TriggerNode("tharon'ja out of melee",
        NextAction::array(0, new NextAction("reach melee", ACTION_NORMAL + 3), nullptr)));
    triggers.push_back(new TriggerNode("gift of tharon'ja",
        NextAction::array(0, new NextAction("taunt", ACTION_NORMAL + 2), nullptr)));
    triggers.push_back(new TriggerNode("gift of tharon'ja",
        NextAction::array(0, new NextAction("slaying strike", ACTION_NORMAL + 2), nullptr)));
}

void WotlkDungeonDTKStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new NovosMultiplier(botAI));
    multipliers.push_back(new TharonjaMultiplier(botAI));
}
