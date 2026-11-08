#include "RaidOnyxia.h"

#include "GenericTriggers.h"
#include "ObjectAccessor.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "strategy/values/NearestNpcsValue.h"
#include "GenericSpellActions.h"
#include "LastMovementValue.h"
#include "MovementActions.h"
#include "PositionAction.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

OnyxiaDeepBreathTrigger::OnyxiaDeepBreathTrigger(ShadowAI* botAI) : Trigger(botAI, "ony deep breath warning") {}

bool OnyxiaDeepBreathTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !boss->HasUnitState(UNIT_STATE_CASTING))
        return false;

    // Check if Onyxia is casting
    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);

    if (!currentSpell)
        return false;

    uint32 spellId = currentSpell->m_spellInfo->Id;

    if (spellId == 17086 ||  // North to South
        spellId == 18351 ||  // South to North
        spellId == 18576 ||  // East to West
        spellId == 18609 ||  // West to East
        spellId == 18564 ||  // Southeast to Northwest
        spellId == 18584 ||  // Northwest to Southeast
        spellId == 18596 ||  // Southwest to Northeast
        spellId == 18617     // Northeast to Southwest
    )
    {
        return true;
    }

    return false;
}

OnyxiaNearTailTrigger::OnyxiaNearTailTrigger(ShadowAI* botAI) : Trigger(botAI, "ony near tail") {}

bool OnyxiaNearTailTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || botAI->IsTank(bot))
        return false;

    // Skip if Onyxia is in air or transitioning
    if (!boss->IsInCombat() || boss->IsFlying() || !boss->GetVictim())
        return false;

    return true;
}
RaidOnyxiaFireballSplashTrigger::RaidOnyxiaFireballSplashTrigger(ShadowAI* botAI)
    : Trigger(botAI, "ony fireball splash incoming")
{
}

bool RaidOnyxiaFireballSplashTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !boss->HasUnitState(UNIT_STATE_CASTING))
        return false;

    // Check if Onyxia is casting Fireball
    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!currentSpell || currentSpell->m_spellInfo->Id != 18392)  // 18392 is the classic Fireball ID
        return false;

    GuidVector nearbyUnits = AI_VALUE(GuidVector, "nearest friendly players");

    for (ObjectGuid guid : nearbyUnits)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == bot || !unit->IsAlive())
            continue;

        if (bot->GetDistance(unit) < 8.0f)
            return true;
    }

    return false;
}

RaidOnyxiaWhelpsSpawnTrigger::RaidOnyxiaWhelpsSpawnTrigger(ShadowAI* botAI) : Trigger(botAI, "ony whelps spawn") {}

bool RaidOnyxiaWhelpsSpawnTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    return !botAI->IsHeal(bot) && boss->IsFlying();  // DPS + Tanks only
}

OnyxiaAvoidEggsTrigger::OnyxiaAvoidEggsTrigger(ShadowAI* botAI) : Trigger(botAI, "ony avoid eggs") {}

bool OnyxiaAvoidEggsTrigger::IsActive()
{
    Position botPos = Position(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());

    if (botPos.GetExactDist2d(-35.0f, -165.0f) <= 5.0f)
        return true;

    if (botPos.GetExactDist2d(-35.0f, -260.0f) <= 5.0f)
        return true;

    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------



bool RaidOnyxiaMoveToSideAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    float angleToBot = boss->GetAngle(bot);
    float bossFacing = boss->GetOrientation();
    float diff = fabs(angleToBot - bossFacing);
    if (diff > M_PI)
        diff = 2 * M_PI - diff;

    float distance = bot->GetDistance(boss);

    // Too close (30 yards) and either in front or behind
    if (distance <= 30.0f && (diff < M_PI / 4 || diff > 3 * M_PI / 4))
    {
        float offsetAngle = bossFacing + M_PI_2;  // 90° to the right
        float offsetDist = 15.0f;

        float sideX = boss->GetPositionX() + offsetDist * cos(offsetAngle);
        float sideY = boss->GetPositionY() + offsetDist * sin(offsetAngle);

        // bot->Yell("Too close to front or tail — moving to side of Onyxia!", LANG_UNIVERSAL);
        return MoveTo(boss->GetMapId(), sideX, sideY, boss->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_COMBAT);
    }

    return false;
}

bool RaidOnyxiaSpreadOutAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");

    if (!boss)
        return false;

    Player* target = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_targets.GetUnitTarget()->ToPlayer();
    if (target != bot)
        return false;

    // bot->Yell("Spreading out — I'm the Fireball target!", LANG_UNIVERSAL);
    return MoveFromGroup(9.0f);  // move 9 yards
}

bool RaidOnyxiaMoveToSafeZoneAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!currentSpell)
        return false;

    uint32 spellId = currentSpell->m_spellInfo->Id;

    std::vector<SafeZone> safeZones = GetSafeZonesForBreath(spellId);
    if (safeZones.empty())
        return false;

    // Find closest safe zone
    SafeZone* bestZone = nullptr;
    float bestDist = std::numeric_limits<float>::max();

    for (auto& zone : safeZones)
    {
        float dist = bot->GetExactDist2d(zone.pos.GetPositionX(), zone.pos.GetPositionY());
        if (dist < bestDist)
        {
            bestDist = dist;
            bestZone = &zone;
        }
    }

    if (!bestZone)
        return false;

    if (bot->IsWithinDist2d(bestZone->pos.GetPositionX(), bestZone->pos.GetPositionY(), bestZone->radius))
        return false;  // Already safe

    // bot->Yell("Moving to Safe Zone!", LANG_UNIVERSAL);
    return MoveTo(bot->GetMapId(), bestZone->pos.GetPositionX(), bestZone->pos.GetPositionY(), bot->GetPositionZ(),
                  false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
}

bool RaidOnyxiaKillWhelpsAction::Execute(Event event [[maybe_unused]])
{
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    // If already attacking a whelp, don't swap targets
    if (currentTarget && currentTarget->GetEntry() == 11262)
    {
        return false;
    }
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    for (ObjectGuid guid : targets)
    {
        Creature* unit = botAI->GetCreature(guid);
        if (!unit || !unit->IsAlive() || !unit->IsInWorld())
            continue;

        if (unit->GetEntry() == 11262)  // Onyxia Whelp
        {
            // bot->Yell("Attacking Whelps!", LANG_UNIVERSAL);
            return Attack(unit);
        }
    }
    return false;
}

bool OnyxiaAvoidEggsAction::Execute(Event event [[maybe_unused]])
{
    Position botPos = Position(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());

    float x, y;

    // get safe zone slightly away from eggs (Can this be dynamic?)
    if (botPos.GetExactDist2d(-36.0f, -164.0f) <= 5.0f)
    {
        x = -10.0f;
        y = -180.0f;
    }
    else if (botPos.GetExactDist2d(-34.0f, -262.0f) <= 5.0f)
    {
        x = -16.0f;
        y = -250.0f;
    }
    else
    {
        return false;  // Not in danger zone
    }

    // bot->Yell("Too close to eggs — backing off!", LANG_UNIVERSAL);

    return MoveTo(bot->GetMapId(), x, y, bot->GetPositionZ(), false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidOnyxiaStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // ----------- Phase 1 (100% - 65%) -----------

    triggers.push_back(new TriggerNode(
        "ony near tail", NextAction::array(0, new NextAction("ony move to side", ACTION_RAID + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "ony avoid eggs", NextAction::array(0, new NextAction("ony avoid eggs move", ACTION_EMERGENCY + 5), nullptr)));

    // ----------- Phase 2 (65% - 40%) -----------

    triggers.push_back(
        new TriggerNode("ony deep breath warning",
                        NextAction::array(0, new NextAction("ony move to safe zone", ACTION_EMERGENCY + 5), nullptr)));

    triggers.push_back(
        new TriggerNode("ony fireball splash incoming",
                        NextAction::array(0, new NextAction("ony spread out", ACTION_EMERGENCY + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "ony whelps spawn", NextAction::array(0, new NextAction("ony kill whelps", ACTION_RAID + 1), nullptr)));
}

void RaidOnyxiaStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers [[maybe_unused]])
{
    // Empty for now
}
