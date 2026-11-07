#include "RaidOs.h"

#include "SharedDefines.h"
#include "Shadows.h"
#include "ChooseTargetActions.h"
#include "DKActions.h"
#include "DruidActions.h"
#include "DruidBearActions.h"
#include "FollowActions.h"
#include "GenericActions.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "PaladinActions.h"
#include "ReachTargetActions.h"
#include "ScriptedCreature.h"
#include "WarriorActions.h"
#include "Strategy.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool SartharionTankTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }

    return botAI->IsTank(bot);
}

bool FlameTsunamiTrigger::IsActive()
{
    if (botAI->IsTank(bot)) { return false; }

    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }


    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit)
        {
            if (unit->GetEntry() == NPC_FLAME_TSUNAMI)
            {
                return true;
            }
        }
    }

    return false;
}

bool TwilightFissureTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }


    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit)
        {
            if (unit->GetEntry() == NPC_TWILIGHT_FISSURE)
            {
                return true;
            }
        }
    }

    return false;
}

bool SartharionDpsTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }

    return botAI->IsDps(bot);
}

bool SartharionMeleePositioningTrigger::IsActive()
{
    if (!botAI->IsMelee(bot) || !botAI->IsDps(bot)) { return false; }

    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }

    Unit* shadron = AI_VALUE2(Unit*, "find target", "shadron");
    Unit* tenebron = AI_VALUE2(Unit*, "find target", "tenebron");
    Unit* vesperon = AI_VALUE2(Unit*, "find target", "vesperon");

    return !(shadron || tenebron || vesperon);
}

bool TwilightPortalEnterTrigger::IsActive()
{
    if (botAI->IsMainTank(bot) || botAI->IsHealAssistantOfIndex(bot, 0)) { return false; }

    // In 25-man, take two healers in. Otherwise just take one
    // if (bot->GetRaidDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
    // {
    //     if (botAI->IsHealAssistantOfIndex(bot, 0) || botAI->IsHealAssistantOfIndex(bot, 1))
    //     {
    //         return false;
    //     }
    // }
    // else
    // {
    //     if (botAI->IsHealAssistantOfIndex(bot, 0))
    //     {
    //         return false;
    //     }
    // }


    // Don't enter portal until drakes are dead
    if (bot->HasAura(SPELL_POWER_OF_SHADRON) ||
        bot->HasAura(SPELL_POWER_OF_TENEBRON) ||
        bot->HasAura(SPELL_POWER_OF_VESPERON))
    {
        return false;
    }

    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }

    // GuidVector objects = AI_VALUE(GuidVector, "nearest game objects no los");
    // for (auto& object : objects)
    // {
    //     GameObject* go = botAI->GetGameObject(object);
    //     if (go && go->GetEntry() == GO_TWILIGHT_PORTAL)
    //     {
    //         return true;
    //     }
    // }
    return bool(bot->FindNearestGameObject(GO_TWILIGHT_PORTAL, 100.0f));
}

bool TwilightPortalExitTrigger::IsActive()
{
    return bot->HasAura(SPELL_TWILIGHT_SHIFT) && !AI_VALUE2(Unit*, "find target", "acolyte of shadron");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool SartharionTankPositionAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return false; }

    // Unit* shadron = AI_VALUE2(Unit*, "find target", "shadron");
    // Unit* tenebron = AI_VALUE2(Unit*, "find target", "tenebron");
    // Unit* vesperon = AI_VALUE2(Unit*, "find target", "vesperon");
    Unit* shadron = nullptr;
    Unit* tenebron = nullptr;
    Unit* vesperon = nullptr;

    // Detect incoming drakes before they are on aggro table
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");
    for (auto& target : targets)
    {
        Unit* unit = botAI->GetUnit(target);
        if (!unit) { continue; }

        switch (unit->GetEntry())
        {
            case NPC_SHADRON:
                shadron = unit;
                continue;
            case NPC_TENEBRON:
                tenebron = unit;
                continue;
            case NPC_VESPERON:
                vesperon = unit;
                continue;
            default:
                continue;
        }
    }

    Position currentPos = bot->GetPosition();
    // Adjustable, this is the acceptable distance to stack point that will be accepted as "safe"
    float looseDistance = 12.0f;

    if (botAI->IsMainTank(bot))
    {
        if (bot->GetExactDist2d(SARTHARION_MAINTANK_POSITION.first, SARTHARION_MAINTANK_POSITION.second) > looseDistance)
        {
            return MoveTo(OS_MAP_ID, SARTHARION_MAINTANK_POSITION.first, SARTHARION_MAINTANK_POSITION.second, currentPos.GetPositionZ(),
                false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
        }
    }
    // Offtank grab drakes
    else if (shadron || tenebron || vesperon)
    {
        float triggerDistance = 100.0f;
        // Prioritise threat before positioning
        if (tenebron && bot->GetExactDist2d(tenebron) < triggerDistance &&
            tenebron->GetTarget() != bot->GetGUID() && AI_VALUE(Unit*, "current target") != tenebron)
        {
            return Attack(tenebron);
        }
        if (shadron && bot->GetExactDist2d(shadron) < triggerDistance &&
            shadron->GetTarget() != bot->GetGUID() && AI_VALUE(Unit*, "current target") != shadron)
        {
            return Attack(shadron);
        }
        if (vesperon && bot->GetExactDist2d(vesperon) < triggerDistance &&
            vesperon->GetTarget() != bot->GetGUID() && AI_VALUE(Unit*, "current target") != vesperon)
        {
            return Attack(vesperon);
        }

        bool drakeInCombat = (tenebron && bot->GetExactDist2d(tenebron) < triggerDistance) ||
                                (shadron && bot->GetExactDist2d(shadron) < triggerDistance) ||
                                (vesperon && bot->GetExactDist2d(vesperon) < triggerDistance);
        // Offtank has threat on drakes, check positioning
        if (drakeInCombat && bot->GetExactDist2d(SARTHARION_OFFTANK_POSITION.first, SARTHARION_OFFTANK_POSITION.second) > looseDistance)
        {
            return MoveTo(OS_MAP_ID, SARTHARION_OFFTANK_POSITION.first, SARTHARION_OFFTANK_POSITION.second, currentPos.GetPositionZ(),
                false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
        }
    }
    return false;
}

bool AvoidTwilightFissureAction::Execute(Event event [[maybe_unused]])
{
    const float radius = 5.0f;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == NPC_TWILIGHT_FISSURE)
        {
            float currentDistance = bot->GetDistance2d(unit);
            if (currentDistance < radius)
            {
                return MoveAway(unit, radius - currentDistance);
            }
        }
    }
    return false;
}

bool AvoidFlameTsunamiAction::Execute(Event event [[maybe_unused]])
{
    // Adjustable, this is the acceptable distance to stack point that will be accepted as "safe"
    float looseDistance = 4.0f;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == NPC_FLAME_TSUNAMI)
        {
            Position currentPos = bot->GetPosition();

            // I think these are centrepoints for the wave segments. Either way they uniquely identify the wave
            // direction as they have different coords for the left and right waves
            // int casting is not a mistake, need to avoid FP errors somehow.
            // I always saw these accurate to around 6 decimal places, but if there are issues,
            // can switch this to abs comparison of floats which would technically be more robust.
            int posY = (int) unit->GetPositionY();
            if (posY == 505 || posY == 555)     // RIGHT WAVE
            {
                bool wavePassed = currentPos.GetPositionX() > unit->GetPositionX();
                if (wavePassed)
                {
                    return false;
                }

                if (bot->GetExactDist2d(currentPos.GetPositionX(), TSUNAMI_RIGHT_SAFE_ALL) > looseDistance)
                {
                    return MoveTo(OS_MAP_ID, currentPos.GetPositionX(), TSUNAMI_RIGHT_SAFE_ALL, currentPos.GetPositionZ(),
                        false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
                }
            }
            else    // LEFT WAVE
            {
                bool wavePassed = currentPos.GetPositionX() < unit->GetPositionX();
                if (wavePassed)
                {
                    return false;
                }

                if (botAI->IsMelee(bot))
                {
                    if (bot->GetExactDist2d(currentPos.GetPositionX(), TSUNAMI_LEFT_SAFE_MELEE) > looseDistance)
                    {
                        return MoveTo(OS_MAP_ID, currentPos.GetPositionX(), TSUNAMI_LEFT_SAFE_MELEE, currentPos.GetPositionZ(),
                            false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
                    }
                }
                else    // Ranged/healers
                {
                    if (bot->GetExactDist2d(currentPos.GetPositionX(), TSUNAMI_LEFT_SAFE_RANGED) > looseDistance)
                    {
                        return MoveTo(OS_MAP_ID, currentPos.GetPositionX(), TSUNAMI_LEFT_SAFE_RANGED, currentPos.GetPositionZ(),
                            false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
                    }
                }
            }
        }
    }
    return false;
}

bool SartharionAttackPriorityAction::Execute(Event event [[maybe_unused]])
{
    Unit* sartharion = AI_VALUE2(Unit*, "find target", "sartharion");
    Unit* shadron = AI_VALUE2(Unit*, "find target", "shadron");
    Unit* tenebron = AI_VALUE2(Unit*, "find target", "tenebron");
    Unit* vesperon = AI_VALUE2(Unit*, "find target", "vesperon");
    Unit* acolyte = AI_VALUE2(Unit*, "find target", "acolyte of shadron");

    Unit* target = nullptr;

    if (acolyte)
    {
        target = acolyte;
    }
    else if (vesperon)
    {
        target = vesperon;
    }
    else if (tenebron)
    {
        target = tenebron;
    }
    else if (shadron)
    {
        target = shadron;
    }
    else if (sartharion)
    {
        target = sartharion;
    }

    if (target && AI_VALUE(Unit*, "current target") != target)
    {
        return Attack(target);
    }

    return false;
}

bool EnterTwilightPortalAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss || !boss->HasAura(SPELL_GIFT_OF_TWILIGHT_FIRE)) { return false; }

    GameObject* portal = bot->FindNearestGameObject(GO_TWILIGHT_PORTAL, 100.0f);
    if (!portal) { return false; }

    if (!portal->IsAtInteractDistance(bot))
    {
        return MoveTo(portal, fmaxf(portal->GetInteractionDistance() - 1.0f, 0.0f));
    }

    // Go through portal
    WorldPacket data1(CMSG_GAMEOBJ_USE);
    data1 << portal->GetGUID();
    bot->GetSession()->HandleGameObjectUseOpcode(data1);

    return true;
}

bool ExitTwilightPortalAction::Execute(Event event [[maybe_unused]])
{
    GameObject* portal = bot->FindNearestGameObject(GO_NORMAL_PORTAL, 100.0f);
    if (!portal) { return false; }

    if (!portal->IsAtInteractDistance(bot))
    {
        return MoveTo(portal, fmaxf(portal->GetInteractionDistance() - 1.0f, 0.0f));
    }

    // Go through portal
    WorldPacket data1(CMSG_GAMEOBJ_USE);
    data1 << portal->GetGUID();
    bot->GetSession()->HandleGameObjectUseOpcode(data1);

    return true;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float SartharionMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "sartharion");
    if (!boss) { return 1.0f; }

    Unit* target = action->GetTarget();

    if (botAI->IsMainTank(bot) && dynamic_cast<TankFaceAction*>(action))
    {
        // return 0.0f;
    }

    if (botAI->IsDps(bot) && dynamic_cast<DpsAssistAction*>(action))
    {
        return 0.0f;
    }

    if (botAI->IsMainTank(bot) && target && target != boss &&
        (dynamic_cast<TankAssistAction*>(action) || dynamic_cast<CastTauntAction*>(action) || dynamic_cast<CastDarkCommandAction*>(action) ||
         dynamic_cast<CastHandOfReckoningAction*>(action) || dynamic_cast<CastGrowlAction*>(action)))
    {
        return 0.0f;
    }

    if (botAI->IsAssistTank(bot) && target && target == boss &&
        (dynamic_cast<CastTauntAction*>(action) || dynamic_cast<CastDarkCommandAction*>(action) ||
         dynamic_cast<CastHandOfReckoningAction*>(action) || dynamic_cast<CastGrowlAction*>(action)))
    {
        return 0.0f;
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidOsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("sartharion tank",
                        NextAction::array(0, new NextAction("sartharion tank position", ACTION_MOVE), nullptr)));
    triggers.push_back(
        new TriggerNode("twilight fissure",
                        NextAction::array(0, new NextAction("avoid twilight fissure", ACTION_RAID + 2), nullptr)));
    triggers.push_back(
        new TriggerNode("flame tsunami",
                        NextAction::array(0, new NextAction("avoid flame tsunami", ACTION_RAID + 1), nullptr)));
    triggers.push_back(
        new TriggerNode("sartharion dps",
                        NextAction::array(0, new NextAction("sartharion attack priority", ACTION_RAID), nullptr)));
    // Flank dragon positioning
    triggers.push_back(new TriggerNode("sartharion melee positioning",
        NextAction::array(0, new NextAction("rear flank", ACTION_MOVE + 4), nullptr)));

    triggers.push_back(new TriggerNode("twilight portal enter",
        NextAction::array(0, new NextAction("enter twilight portal", ACTION_RAID + 1), nullptr)));
    triggers.push_back(new TriggerNode("twilight portal exit",
        NextAction::array(0, new NextAction("exit twilight portal", ACTION_RAID + 1), nullptr)));
}

void RaidOsStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new SartharionMultiplier(botAI));
}
