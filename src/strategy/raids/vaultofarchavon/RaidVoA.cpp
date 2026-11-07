#include "RaidVoA.h"

#include "EventMap.h"
#include "Object.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "Define.h"
#include "Event.h"
#include "Group.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Unit.h"
#include "Action.h"
#include "Strategy.h"
#include "Trigger.h"
#include "vector"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool EmalonMarkBossTrigger::IsActive()
{
    // Only tank bot can mark target
    if (!botAI->IsTank(bot))
    {
        return false;
    }

    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "emalon the storm watcher");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Check if boss have skull mark
    Group* group = bot->GetGroup();
    int8 skullIndex = 7;  // Skull
    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);
    if (currentSkullTarget == boss->GetGUID())
    {
        return false;
    }

    // Check if there is any overcharged minion
    Unit* overchargedMinion = nullptr;
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit)
            continue;

        uint32 entry = unit->GetEntry();
        if (entry == NPC_TEMPEST_MINION && unit->HasAura(AURA_OVERCHARGE))
        {
            overchargedMinion = unit;
            break;
        }
    }
    if (overchargedMinion)
    {
        return false;
    }

    return true;
}

bool EmalonLightingNovaTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "emalon the storm watcher");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Tank dont need to move
    if (botAI->IsTank(bot))
    {
        return false;
    }

    // Check if boss is casting Lightning Nova
    bool isCasting = boss->HasUnitState(UNIT_STATE_CASTING);
    bool isLightingNova = boss->FindCurrentSpellBySpellId(SPELL_LIGHTNING_NOVA_10_MAN) ||
                          boss->FindCurrentSpellBySpellId(SPELL_LIGHTNING_NOVA_25_MAN);
    return isCasting && isLightingNova;
}

bool EmalonOverchargeTrigger::IsActive()
{
    // Only tank bot can mark target
    if (!botAI->IsTank(bot))
    {
        return false;
    }

    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "emalon the storm watcher");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Check if there is any overcharged minion
    Unit* overchargedMinion = nullptr;
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit)
            continue;

        uint32 entry = unit->GetEntry();
        if (entry == NPC_TEMPEST_MINION && unit->HasAura(AURA_OVERCHARGE))
        {
            overchargedMinion = unit;
            break;
        }
    }
    if (!overchargedMinion)
    {
        return false;
    }

    // Check if minion have skull mark
    Group* group = bot->GetGroup();
    int8 skullIndex = 7;  // Skull
    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);
    if (currentSkullTarget == overchargedMinion->GetGUID())
    {
        return false;
    }

    return true;
}

bool EmalonFallFromFloorTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "emalon the storm watcher");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Check if bot is on the floor
    return bot->GetPositionZ() < 80.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position VOA_EMALON_RESTORE_POSITION = Position(-221.8f, -243.8f, 96.8f, 4.7f);

bool EmalonMarkBossAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "emalon the storm watcher");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    bool isMainTank = botAI->IsMainTank(bot);
    Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
    Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;

    if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a real player
    {
        // Iterate through the first 3 bot tanks to assign the Skull marker
        for (int i = 0; i < 3; ++i)
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Bot is a valid tank
            {
                Group* group = bot->GetGroup();
                if (group && boss)
                {
                    int8 skullIndex = 7;  // Skull
                    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

                    // If there's no skull set yet, or the skull is on a different target, set boss
                    if (!currentSkullTarget || (boss->GetGUID() != currentSkullTarget))
                    {
                        group->SetTargetIcon(skullIndex, bot->GetGUID(), boss->GetGUID());
                        return true;
                    }
                }
                break;  // Stop after finding the first valid bot tank
            }
        }
    }
    else if (isMainTank)  // Bot is the main tank
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            int8 skullIndex = 7;  // Skull
            ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

            // If there's no skull set yet, or the skull is on a different target, set the Eonar's Gift
            if (!currentSkullTarget || (boss->GetGUID() != currentSkullTarget))
            {
                group->SetTargetIcon(skullIndex, bot->GetGUID(), boss->GetGUID());
                return true;
            }
        }
    }

    return false;
}

bool EmalonMarkBossAction::isUseful()
{
    EmalonMarkBossTrigger emalonMarkBossTrigger(botAI);
    return emalonMarkBossTrigger.IsActive();
}

bool EmalonLightingNovaAction::Execute(Event event [[maybe_unused]])
{
    const float radius = 25.0f;  // 20 yards + 5 yard for safety for 10 man. For 25man there is no maximum range but 25 yards should be ok

    Unit* boss = AI_VALUE2(Unit*, "find target", "emalon the storm watcher");
    if (!boss)
        return false;

    float currentDistance = bot->GetDistance2d(boss);

    if (currentDistance < radius)
    {
        return MoveAway(boss, radius - currentDistance);
    }

    return false;
}

bool EmalonLightingNovaAction::isUseful()
{
    EmalonLightingNovaTrigger emalonLightingNovaTrigger(botAI);
    return emalonLightingNovaTrigger.IsActive();
}

bool EmalonOverchargeAction::Execute(Event event [[maybe_unused]])
{
    // Check if there is any overcharged minion
    Unit* minion = nullptr;
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (!unit)
            continue;

        uint32 entry = unit->GetEntry();
        if (entry == NPC_TEMPEST_MINION && unit->HasAura(AURA_OVERCHARGE))
        {
            minion = unit;
            break;
        }
    }
    if (!minion)
    {
        return false;
    }

    bool isMainTank = botAI->IsMainTank(bot);
    Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
    Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;

    if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a real player
    {
        // Iterate through the first 3 bot tanks to assign the Skull marker
        for (int i = 0; i < 3; ++i)
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Bot is a valid tank
            {
                Group* group = bot->GetGroup();
                if (group && minion)
                {
                    int8 skullIndex = 7;  // Skull
                    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

                    // If there's no skull set yet, or the skull is on a different target, set Tempest Minion
                    if (!currentSkullTarget || (minion->GetGUID() != currentSkullTarget))
                    {
                        group->SetTargetIcon(skullIndex, bot->GetGUID(), minion->GetGUID());
                        return true;
                    }
                }
                break;  // Stop after finding the first valid bot tank
            }
        }
    }
    else if (isMainTank)  // Bot is the main tank
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            int8 skullIndex = 7;  // Skull
            ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

            // If there's no skull set yet, or the skull is on a different target, set the Eonar's Gift
            if (!currentSkullTarget || (minion->GetGUID() != currentSkullTarget))
            {
                group->SetTargetIcon(skullIndex, bot->GetGUID(), minion->GetGUID());
                return true;
            }
        }
    }

    return false;
}

bool EmalonOverchargeAction::isUseful()
{
    EmalonOverchargeTrigger emalonOverchargeTrigger(botAI);
    return emalonOverchargeTrigger.IsActive();
}

bool EmalonFallFromFloorAction::Execute(Event event [[maybe_unused]])
{
    return bot->TeleportTo(bot->GetMapId(), VOA_EMALON_RESTORE_POSITION.GetPositionX(),
                           VOA_EMALON_RESTORE_POSITION.GetPositionY(), VOA_EMALON_RESTORE_POSITION.GetPositionZ(),
                           VOA_EMALON_RESTORE_POSITION.GetOrientation());
}

bool EmalonFallFromFloorAction::isUseful()
{
    EmalonFallFromFloorTrigger emalonFallFromFloorTrigger(botAI);
    return emalonFallFromFloorTrigger.IsActive();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidVoAStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    //
    // Emalon the Storm Watcher
    //
    triggers.push_back(new TriggerNode(
        "emalon lighting nova trigger",
        NextAction::array(0, new NextAction("emalon lighting nova action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "emalon mark boss trigger",
        NextAction::array(0, new NextAction("emalon mark boss action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "emalon overcharge trigger",
        NextAction::array(0, new NextAction("emalon overcharge action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "emalon fall from floor trigger",
        NextAction::array(0, new NextAction("emalon fall from floor action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "emalon nature resistance trigger",
        NextAction::array(0, new NextAction("emalon nature resistance action", ACTION_RAID), nullptr)));

    //
    // Koralon the Flame Watcher
    //

    triggers.push_back(new TriggerNode(
        "koralon fire resistance trigger",
        NextAction::array(0, new NextAction("koralon fire resistance action", ACTION_RAID), nullptr)));
}
