#include "RaidUlduar.h"

#include "ChatHelper.h"
#include "ObjectAccessor.h"
#include "GameObject.h"
#include "Group.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "World.h"
#include "EventMap.h"
#include "Object.h"
#include "SharedDefines.h"
#include "Trigger.h"
#include "Vehicle.h"
#include <MovementActions.h>
#include <FollowMasterStrategy.h>
#include <RtiTargetValue.h>
#include <CombatStrategy.h>
#include <cmath>
#include "AiObjectContext.h"
#include "DBCEnums.h"
#include "LastMovementValue.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "ShadowAIConfig.h"
#include "Position.h"
#include "RtiValue.h"
#include "ServerFacade.h"
#include "Unit.h"
#include <TankAssistStrategy.h>
#include "ChooseTargetActions.h"
#include "DKActions.h"
#include "DruidActions.h"
#include "DruidBearActions.h"
#include "FollowActions.h"
#include "GenericActions.h"
#include "GenericSpellActions.h"
#include "HunterActions.h"
#include "MageActions.h"
#include "MovementActions.h"
#include "PaladinActions.h"
#include "PriestActions.h"
#include "ReachTargetActions.h"
#include "RogueActions.h"
#include "ShamanActions.h"
#include "UseMeetingStoneAction.h"
#include "WarriorActions.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// Prevent harpoon spam
std::unordered_map<ObjectGuid, time_t> RazorscaleBossHelper::_harpoonCooldowns;
// Prevent role assignment spam
std::unordered_map<ObjectGuid, std::time_t> RazorscaleBossHelper::_lastRoleSwapTime;
const std::time_t RazorscaleBossHelper::_roleSwapCooldown;

bool RazorscaleBossHelper::UpdateBossAI()
{
    _boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (_boss)
    {
        Group* group = bot->GetGroup();
        if (group && !AreRolesAssigned())
        {
            AssignRolesBasedOnHealth();
        }
        return true;
    }
    return false;
}

Unit* RazorscaleBossHelper::GetBoss() const
{
    return _boss;
}

bool RazorscaleBossHelper::IsGroundPhase() const
{
    return _boss && _boss->IsAlive() &&
           (_boss->GetPositionZ() <= RAZORSCALE_FLYING_Z_THRESHOLD) &&
           (_boss->GetHealthPct() < 50.0f) &&
           !_boss->HasAura(SPELL_STUN_AURA);
}

bool RazorscaleBossHelper::IsFlyingPhase() const
{
    return _boss && (!IsGroundPhase() || _boss->GetPositionZ() >= RAZORSCALE_FLYING_Z_THRESHOLD);
}

bool RazorscaleBossHelper::IsHarpoonFired(uint32 chainSpellId) const
{
    return _boss && _boss->HasAura(chainSpellId);
}

bool RazorscaleBossHelper::IsHarpoonReady(GameObject* harpoonGO)
{
    if (!harpoonGO)
        return false;

    auto it = _harpoonCooldowns.find(harpoonGO->GetGUID());
    if (it != _harpoonCooldowns.end())
    {
        time_t currentTime = std::time(nullptr);
        time_t elapsedTime = currentTime - it->second;
        if (elapsedTime < HARPOON_COOLDOWN_DURATION)
            return false;
    }

    return harpoonGO->GetGoState() == GO_STATE_READY;
}

void RazorscaleBossHelper::SetHarpoonOnCooldown(GameObject* harpoonGO)
{
    if (!harpoonGO)
        return;

    time_t currentTime = std::time(nullptr);
    _harpoonCooldowns[harpoonGO->GetGUID()] = currentTime;
}

GameObject* RazorscaleBossHelper::FindNearestHarpoon(float x, float y, float z) const
{
    GameObject* nearestHarpoon = nullptr;
    float minDistanceSq = std::numeric_limits<float>::max();

    for (const auto& harpoon : GetHarpoonData())
    {
        if (GameObject* harpoonGO = bot->FindNearestGameObject(harpoon.gameObjectEntry, 200.0f))
        {
            float dx = harpoonGO->GetPositionX() - x;
            float dy = harpoonGO->GetPositionY() - y;
            float dz = harpoonGO->GetPositionZ() - z;
            float distanceSq = dx * dx + dy * dy + dz * dz;

            if (distanceSq < minDistanceSq)
            {
                minDistanceSq = distanceSq;
                nearestHarpoon = harpoonGO;
            }
        }
    }

    return nearestHarpoon;
}

const std::vector<RazorscaleBossHelper::HarpoonData>& RazorscaleBossHelper::GetHarpoonData()
{
    static const std::vector<HarpoonData> harpoonData =
    {
        { GO_RAZORSCALE_HARPOON_1, SPELL_CHAIN_1 },
        { GO_RAZORSCALE_HARPOON_2, SPELL_CHAIN_2 },
        { GO_RAZORSCALE_HARPOON_3, SPELL_CHAIN_3 },
        { GO_RAZORSCALE_HARPOON_4, SPELL_CHAIN_4 },
    };
    return harpoonData;
}

bool RazorscaleBossHelper::AreRolesAssigned() const
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Retrieve the group member slot list (GUID + flags + other info)
    Group::MemberSlotList const& slots = group->GetMemberSlots();
    for (auto const& slot : slots)
    {
        // Check if this member has the MAINTANK flag
        if (slot.flags & MEMBER_FLAG_MAINTANK)
        {
            return true;
        }
    }

    return false;
}

bool RazorscaleBossHelper::CanSwapRoles() const
{
    // Identify the GUID of the current bot
    ObjectGuid botGuid = bot->GetGUID();
    if (!botGuid)
        return false;

    // If no entry exists yet for this bot, initialize it to 0
    auto it = _lastRoleSwapTime.find(botGuid);
    if (it == _lastRoleSwapTime.end())
    {
        _lastRoleSwapTime[botGuid] = 0;
        it = _lastRoleSwapTime.find(botGuid);
    }

    // Compare the current time against the stored time
    std::time_t currentTime = std::time(nullptr);
    std::time_t lastSwapTime = it->second;

    return (currentTime - lastSwapTime) >= _roleSwapCooldown;
}


void RazorscaleBossHelper::AssignRolesBasedOnHealth()
{
    // Check if enough time has passed since last swap
    if (!CanSwapRoles())
        return;

    Group* group = bot->GetGroup();
    if (!group)
        return;

    // Gather all tank-capable players (bots + real players), excluding those with too many Fuse Armor stacks
    std::vector<Player*> tankCandidates;
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (!member || !botAI->IsTank(member, true) || !member->IsAlive())
            continue;

        Aura* fuseArmor = member->GetAura(SPELL_FUSEARMOR);
        if (fuseArmor && fuseArmor->GetStackAmount() >= FUSEARMOR_THRESHOLD)
            continue;

        tankCandidates.push_back(member);
    }

    // If there are no viable tanks, do nothing
    if (tankCandidates.empty())
        return;

    // Sort by highest max health first
    std::sort(tankCandidates.begin(), tankCandidates.end(),
        [](Player* a, Player* b)
        {
            return a->GetMaxHealth() > b->GetMaxHealth();
        }
    );

    // Pick the top candidate
    Player* newMainTank = tankCandidates[0];
    if (!newMainTank) // Safety check
        return;

    // Unflag everyone from main tank
    for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        Player* member = ref->GetSource();
        if (member && botAI->IsMainTank(member))
            group->SetGroupMemberFlag(member->GetGUID(), false, MEMBER_FLAG_MAINTANK);
    }

    // Assign the single main tank
    group->SetGroupMemberFlag(newMainTank->GetGUID(), true, MEMBER_FLAG_MAINTANK);

    // Yell a message regardless of whether the new main tank is a bot or a real player
    const std::string playerName = newMainTank->GetName();
    const std::string text = playerName + " set as main tank!";
    bot->Yell(text, LANG_UNIVERSAL);

    ObjectGuid botGuid = bot->GetGUID();
    if (!botGuid)
        return;

    // Set current time in the cooldown map for this bot to start cooldown
    _lastRoleSwapTime[botGuid] = std::time(nullptr);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const std::vector<uint32> availableVehicles = {NPC_VEHICLE_CHOPPER, NPC_SALVAGED_DEMOLISHER,
                                               NPC_SALVAGED_DEMOLISHER_TURRET, NPC_SALVAGED_SIEGE_ENGINE,
                                               NPC_SALVAGED_SIEGE_ENGINE_TURRET};

const std::vector<uint32> illusionMobs =
{
    NPC_INFLUENCE_TENTACLE,
    NPC_RUBY_CONSORT,
    NPC_AZURE_CONSORT,
    NPC_BRONZE_CONSORT,
    NPC_EMERALD_CONSORT,
    NPC_OBSIDIAN_CONSORT,
    NPC_ALEXTRASZA,
    NPC_MALYGOS_ILLUSION,
    NPC_NELTHARION,
    NPC_YSERA,
    NPC_DEATHSWORN_ZEALOT,
    NPC_LICH_KING_ILLUSION,
    NPC_IMMOLATED_CHAMPION,
    NPC_SUIT_OF_ARMOR,
    NPC_GARONA,
    NPC_KING_LLANE
};

bool FlameLeviathanOnVehicleTrigger::IsActive()
{
    Unit* vehicleBase = bot->GetVehicleBase();
    Vehicle* vehicle = bot->GetVehicle();
    if (!vehicleBase || !vehicle)
        return false;

    uint32 entry = vehicleBase->GetEntry();
    for (uint32 comp : availableVehicles)
    {
        if (entry == comp)
            return true;
    }
    return false;
}

bool FlameLeviathanVehicleNearTrigger::IsActive()
{
    if (bot->GetVehicle())
        return false;

    Player* master = botAI->GetMaster();
    if (!master)
        return false;

    if (!master->GetVehicle())
        return false;

    return true;
}

bool RazorscaleFlyingAloneTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
    {
        return false;
    }

    // Check if the boss is flying
    if (boss->GetPositionZ() < RazorscaleBossHelper::RAZORSCALE_FLYING_Z_THRESHOLD)
    {
        return false;
    }

    // Get the list of attackers
    GuidVector attackers = context->GetValue<GuidVector>("attackers")->Get();
    if (attackers.empty())
    {
        return true;  // No attackers implies flying alone
    }

    std::vector<Unit*> dark_rune_adds;

    // Loop through attackers to find dark rune adds
    for (ObjectGuid const& guid : attackers)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit)
            continue;

        uint32 entry = unit->GetEntry();

        // Check for valid dark rune entries
        if (entry == RazorscaleBossHelper::UNIT_DARK_RUNE_WATCHER ||
            entry == RazorscaleBossHelper::UNIT_DARK_RUNE_GUARDIAN ||
            entry == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL)
        {
            dark_rune_adds.push_back(unit);
        }
    }

    // Return whether there are no dark rune adds
    return dark_rune_adds.empty();
}

bool RazorscaleDevouringFlamesTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
        return false;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DEVOURING_FLAME)
        {
            return true;
        }
    }

    return false;
}

bool RazorscaleAvoidSentinelTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
        return false;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL)
        {
            return true;
        }
    }

    return false;
}

bool RazorscaleAvoidWhirlwindTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
        return false;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL &&
            (unit->HasAura(RazorscaleBossHelper::SPELL_SENTINEL_WHIRLWIND) ||
             unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL)))
        {
            return true;
        }
    }

    return false;
}

bool RazorscaleGroundedTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
    {
        return false;
    }

    // Check if the boss is flying
    if (boss->GetPositionZ() < RazorscaleBossHelper::RAZORSCALE_FLYING_Z_THRESHOLD)
    {
        return true;
    }
    return false;
}

bool RazorscaleHarpoonAvailableTrigger::IsActive()
{
    // Get harpoon data from the helper
    const std::vector<RazorscaleBossHelper::HarpoonData>& harpoonData = RazorscaleBossHelper::GetHarpoonData();

    // Get the boss entity
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Update the boss AI context in the helper
    RazorscaleBossHelper razorscaleHelper(botAI);

    if (!razorscaleHelper.UpdateBossAI())
    {
        return false;
    }

    // Check each harpoon entry
    for (const auto& harpoon : harpoonData)
    {
        // Skip harpoons whose chain spell is already active on the boss
        if (razorscaleHelper.IsHarpoonFired(harpoon.chainSpellId))
        {
            continue;
        }

        // Find the nearest harpoon GameObject within 200 yards
        if (GameObject* harpoonGO = bot->FindNearestGameObject(harpoon.gameObjectEntry, 200.0f))
        {
            if (RazorscaleBossHelper::IsHarpoonReady(harpoonGO))
            {
                return true;  // At least one harpoon is available and ready to be fired
            }
        }
    }

    // No harpoons are available or need to be fired
    return false;
}

bool RazorscaleFuseArmorTrigger::IsActive()
{
    // Get the boss entity
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Only proceed if this bot can actually tank
    if (!botAI->IsTank(bot))
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Iterate through group members to find the main tank with Fuse Armor
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !botAI->IsMainTank(member))
            continue;

        Aura* fuseArmor = member->GetAura(RazorscaleBossHelper::SPELL_FUSEARMOR);
        if (fuseArmor && fuseArmor->GetStackAmount() >= RazorscaleBossHelper::FUSEARMOR_THRESHOLD)
            return true;
    }

    return false;
}

bool IronAssemblyLightningTendrilsTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "stormcaller brundir");
    if (!boss || !boss->IsAlive())
        return false;

    // Check if bot is within 35 yards of the boss
    if (boss->GetDistance(bot) > 35.0f)
        return false;

    // Check if the boss has the Lightning Tendrils aura
    return boss->HasAura(SPELL_LIGHTNING_TENDRILS_10_MAN) || boss->HasAura(SPELL_LIGHTNING_TENDRILS_25_MAN);
}

bool IronAssemblyOverloadTrigger::IsActive()
{
    // Check if bot is tank
    if (botAI->IsTank(bot))
        return false;

    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "stormcaller brundir");
    if (!boss || !boss->IsAlive())
        return false;

    // Check if bot is within 35 yards of the boss
    if (boss->GetDistance(bot) > 35.0f)
        return false;

    // Check if the boss has the Overload aura
    return boss->HasAura(SPELL_OVERLOAD_10_MAN) || boss->HasAura(SPELL_OVERLOAD_25_MAN) ||
           boss->HasAura(SPELL_OVERLOAD_10_MAN_2) || boss->HasAura(SPELL_OVERLOAD_25_MAN_2);
}

bool IronAssemblyRuneOfPowerTrigger::IsActive()
{
    Unit* target = botAI->GetUnit(bot->GetTarget());
    if (!target || !target->IsAlive())
        return false;

    if (!target->HasAura(SPELL_RUNE_OF_POWER))
        return false;

    if (target->GetVictim() != bot)
        return false;

    return botAI->IsTank(bot);
}

bool KologarnMarkDpsTargetTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");
    if (!boss || !boss->IsAlive())
        return false;

    // Only tank bot can mark target
    if (!botAI->IsTank(bot))
        return false;

    // Get current raid dps target
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    int8 skullIndex = 7;
    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);
    Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);

    // Check that rubble is marked
    if (currentSkullUnit && currentSkullUnit->IsAlive() && currentSkullUnit->GetEntry() == NPC_RUBBLE)
    {
        return false;  // Skull marker is already set on rubble
    }

    // Check that there is rubble to mark
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target)
            continue;

        uint32 creatureId [[maybe_unused]] = target->GetEntry();
        if (target->GetEntry() == NPC_RUBBLE && target->IsAlive())
        {
            return true;  // Found a rubble to mark
        }
    }

    // Check that right arm is marked
    if (currentSkullUnit && currentSkullUnit->IsAlive() && currentSkullUnit->GetEntry() == NPC_RIGHT_ARM)
    {
        return false;  // Skull marker is already set on right arm
    }

    // Check that there is right arm to mark
    Unit* rightArm = AI_VALUE2(Unit*, "find target", "right arm");
    if (rightArm && rightArm->IsAlive())
    {
        return true;  // Found a right arm to mark
    }

    // Check that main body is marked
    if (currentSkullUnit && currentSkullUnit->IsAlive() && currentSkullUnit->GetEntry() == NPC_KOLOGARN)
    {
        return false;  // Skull marker is already set on main body
    }

    // Main body is not marked
    return true;
}

bool KologarnFallFromFloorTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Check if bot is on the floor
    return bot->GetPositionZ() < ULDUAR_KOLOGARN_AXIS_Z_PATHING_ISSUE_DETECT;
}

bool KologarnRubbleSlowdownTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
        return false;

    // Check if bot is hunter
    if (bot->getClass() != CLASS_HUNTER)
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    // Check that the current skull mark is set on rubble
    int8 skullIndex = 7;
    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);
    Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);
    if (!currentSkullUnit || !currentSkullUnit->IsAlive() || currentSkullUnit->GetEntry() != NPC_RUBBLE)
        return false;

    if (bot->HasSpellCooldown(SPELL_FROST_TRAP))
        return false;

    return true;
}

bool KologarnEyebeamTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
        return false;

    GuidVector triggers = AI_VALUE(GuidVector, "possible triggers");

    if (!triggers.empty())
    {
        for (ObjectGuid const guid : triggers)
        {
            if (Unit* unit = botAI->GetUnit(guid))
            {
                std::string triggerName = unit->GetNameForLocaleIdx(sWorld->GetDefaultDbcLocale());

                if (triggerName.rfind("Focused Eyebeam", 0) == 0 &&
                    bot->GetDistance2d(unit) < ULDUAR_KOLOGARN_EYEBEAM_RADIUS + 1.0f)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool KologarnAttackDpsTargetTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
        return false;

    // Get bot's current target
    Unit* currentTarget = botAI->GetUnit(bot->GetTarget());
    if (!currentTarget || !currentTarget->IsAlive())
        return false;

    // Get the current raid marker from the group
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    ObjectGuid skullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
    ObjectGuid crossTarget = group->GetTargetIcon(RtiTargetValue::crossIndex);

    if (crossTarget && (botAI->IsMainTank(bot) || botAI->IsAssistTankOfIndex(bot, 0)))
    {
        return currentTarget->GetGUID() != crossTarget;
    }
    else
    {
        return currentTarget->GetGUID() != skullTarget;
    }
}

bool KologarnRtiTargetTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
        return false;

    std::string rtiMark = AI_VALUE(std::string, "rti");

    if (botAI->IsMainTank(bot) || botAI->IsAssistTankOfIndex(bot, 0))
        return rtiMark != "cross";

    return rtiMark != "skull";
}

bool KologarnCrunchArmorTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
        return false;

    return bot->HasAura(SPELL_CRUNCH_ARMOR);
}

bool AuriayaFallFromFloorTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "auriaya");
    if (!boss || !boss->IsAlive())
        return false;

    // Check if bot is on the floor
    return bot->GetPositionZ() < ULDUAR_AURIAYA_AXIS_Z_PATHING_ISSUE_DETECT;
}

bool HodirBitingColdTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "hodir");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    Player* master = botAI->GetMaster();
    if (!master || !master->IsAlive())
        return false;

    return botAI->GetAura("biting cold", bot, false, false, 2) &&
           !botAI->GetAura("biting cold", master, false, false, 2);
}

// Snowpacked Icicle Target
bool HodirNearSnowpackedIcicleTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "hodir");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Check if boss is casting Flash Freeze
    if (!boss->HasUnitState(UNIT_STATE_CASTING) || !boss->FindCurrentSpellBySpellId(SPELL_FLASH_FREEZE))
    {
        return false;
    }

    // Find the nearest Snowpacked Icicle Target
    Creature* target = bot->FindNearestCreature(NPC_SNOWPACKED_ICICLE, 100.0f);
    if (!target)
        return false;

    // Check that bot is stacked on Snowpacked Icicle
    if (bot->GetDistance2d(target->GetPositionX(), target->GetPositionY()) <= 5.0f)
    {
        return false;
    }

    return true;
}

bool FreyaNearNatureBombTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "freya");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Find the nearest Nature Bomb
    GameObject* target = bot->FindNearestGameObject(GOBJECT_NATURE_BOMB, 12.0f);
    return target != nullptr;
}

bool FreyaMarkDpsTargetTrigger::IsActive()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "freya");
    if (!boss || !boss->IsAlive())
        return false;

    // Only tank bot can mark target
    if (!botAI->IsTank(bot))
        return false;

    // Get current raid dps target
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    int8 skullIndex = 7;
    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);
    Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);

    if (currentSkullUnit && !currentSkullUnit->IsAlive())
    {
        currentSkullUnit = nullptr;
    }

    // Check which adds is up
    Unit* eonarsGift = nullptr;
    Unit* ancientConservator = nullptr;
    Unit* snaplasher = nullptr;
    Unit* ancientWaterSpirit = nullptr;
    Unit* stormLasher = nullptr;
    Unit* firstDetonatingLasher = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_EONARS_GIFT)
        {
            eonarsGift = target;
        }
        else if (target->GetEntry() == NPC_ANCIENT_CONSERVATOR)
        {
            ancientConservator = target;
        }
        else if (target->GetEntry() == NPC_SNAPLASHER)
        {
            snaplasher = target;
        }
        else if (target->GetEntry() == NPC_ANCIENT_WATER_SPIRIT)
        {
            ancientWaterSpirit = target;
        }
        else if (target->GetEntry() == NPC_STORM_LASHER)
        {
            stormLasher = target;
        }
        else if (target->GetEntry() == NPC_DETONATING_LASHER && !firstDetonatingLasher)
        {
            firstDetonatingLasher = target;
        }
    }

    // Check that eonars gift is need to be mark
    if (eonarsGift && (!currentSkullUnit || currentSkullUnit->GetEntry() != eonarsGift->GetEntry()))
    {
        return true;
    }

    // Check that ancient conservator is need to be mark
    if (ancientConservator && (!currentSkullUnit || currentSkullUnit->GetEntry() != ancientConservator->GetEntry()))
    {
        return true;
    }

    // Check that trio of adds is need to be mark
    if (snaplasher || ancientWaterSpirit || stormLasher)
    {
        Unit* highestHealthUnit = nullptr;
        uint32 highestHealth = 0;

        if (snaplasher && snaplasher->GetHealth() > highestHealth)
        {
            highestHealth = snaplasher->GetHealth();
            highestHealthUnit = snaplasher;
        }
        if (ancientWaterSpirit && ancientWaterSpirit->GetHealth() > highestHealth)
        {
            highestHealth = ancientWaterSpirit->GetHealth();
            highestHealthUnit = ancientWaterSpirit;
        }
        if (stormLasher && stormLasher->GetHealth() > highestHealth)
        {
            highestHealthUnit = stormLasher;
        }

        // If the highest health unit is not already marked, mark it
        if (highestHealthUnit && (!currentSkullUnit || currentSkullUnit->GetEntry() != highestHealthUnit->GetEntry()))
        {
            return true;
        }
    }

    // Check that detonating lasher is need to be mark
    if (firstDetonatingLasher &&
        (!currentSkullUnit || currentSkullUnit->GetEntry() != firstDetonatingLasher->GetEntry()))
    {
        Map* map = bot->GetMap();
        if (!map || !map->IsRaid())
            return false;

        uint32 healthThreshold = map->Is25ManRaid() ? 7200 : 4900;  // Detonate maximum damage

        // Check that detonate lasher dont kill raid members
        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member || !member->IsAlive())
                continue;

            if (member->GetHealth() < healthThreshold)
                return false;
        }

        return true;
    }

    return false;
}

bool FreyaMoveToHealingSporeTrigger::IsActive()
{
    // Check for the Freya boss
    Unit* boss = AI_VALUE2(Unit*, "find target", "freya");
    if (!boss || !boss->IsAlive())
        return false;

    if (!botAI->IsRanged(bot))
        return false;

    Unit* conservatory = AI_VALUE2(Unit*, "find target", "ancient conservator");
    if (!conservatory || !conservatory->IsAlive())
        return false;

    GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
    float nearestDistance = std::numeric_limits<float>::max();
    bool foundSpore = false;

    // Iterate through all targets to find healthy spores
    for (const ObjectGuid& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        // Check if the unit is a healthy spore
        if (unit->GetEntry() == NPC_HEALTHY_SPORE)
        {
            foundSpore = true;
            float distance = bot->GetDistance(unit);
            if (distance < nearestDistance)
            {
                nearestDistance = distance;
            }
        }
    }

    // If no healthy spores are found, return false
    if (!foundSpore)
        return false;

    // If the nearest spore is farther than 6 yards, a move is required
    return nearestDistance > 6.0f;
}

bool ThorimUnbalancingStrikeTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive() || !boss->IsHostileTo(bot))
        return false;

    return bot->HasAura(SPELL_UNBALANCING_STRIKE);
}

bool ThorimMarkDpsTargetTrigger::IsActive()
{
    if (bot->GetDistance(ULDUAR_THORIM_NEAR_ARENA_CENTER) > 110.0f)
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    if (botAI->IsMainTank(bot))
    {
        ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
        Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);
        if (currentSkullUnit && !currentSkullUnit->IsAlive())
        {
            currentSkullUnit = nullptr;
        }

        Unit* acolyte = AI_VALUE2(Unit*, "find target", "dark rune acolyte");
        Unit* evoker = AI_VALUE2(Unit*, "find target", "dark rune evoker");

        if (acolyte && acolyte->IsAlive() && bot->GetDistance(acolyte) < 50.0f &&
            (!currentSkullUnit || currentSkullUnit->GetEntry() != acolyte->GetEntry()))
            return true;

        if (evoker && evoker->IsAlive() && bot->GetDistance(evoker) < 50.0f &&
            (!currentSkullUnit || currentSkullUnit->GetEntry() != evoker->GetEntry()))
            return true;

        Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");

        // Check boss and it is alive
        if (!boss || !boss->IsAlive() || !boss->IsHostileTo(bot))
            return false;


        if (boss->GetPositionZ() < ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD && (!currentSkullUnit || !currentSkullUnit->IsAlive()))
        {
            group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), boss->GetGUID());
            return true;
        }

        return false;
    }
    else if (botAI->IsAssistTankOfIndex(bot, 0))
    {
        Player* mainTank = nullptr;
        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (member && botAI->IsMainTank(member))
            {
                mainTank = member;
                break;
            }
        }

        if (mainTank && bot->GetDistance(mainTank) < 30.0f)
            return false;

        ObjectGuid currentCrossTarget = group->GetTargetIcon(RtiTargetValue::crossIndex);
        Unit* currentCrossUnit = botAI->GetUnit(currentCrossTarget);
        if (currentCrossUnit && !currentCrossUnit->IsAlive())
        {
            currentCrossUnit = nullptr;
        }

        Unit* acolyte = AI_VALUE2(Unit*, "find target", "dark rune acolyte");
        if (currentCrossUnit && currentCrossUnit->GetEntry() == NPC_DARK_RUNE_ACOLYTE_I)
            return false;

        Unit* runicColossus = AI_VALUE2(Unit*, "find target", "runic colossus");
        Unit* ancientRuneGiant = AI_VALUE2(Unit*, "find target", "ancient rune giant");
        Unit* ironHonorGuard [[maybe_unused]] = AI_VALUE2(Unit*, "find target", "iron ring guard");
        Unit* ironRingGuard [[maybe_unused]] = AI_VALUE2(Unit*, "find target", "iron honor guard");

        if (acolyte && acolyte->IsAlive() && (!currentCrossUnit || currentCrossUnit->GetEntry() != acolyte->GetEntry()))
            return true;

        if (currentCrossUnit && currentCrossUnit->GetEntry() == NPC_RUNIC_COLOSSUS)
            return false;
        if (runicColossus && runicColossus->IsAlive() &&
            (!currentCrossUnit || currentCrossUnit->GetEntry() != runicColossus->GetEntry()))
            return true;

        if (currentCrossUnit && currentCrossUnit->GetEntry() == NPC_ANCIENT_RUNE_GIANT)
            return false;
        if (ancientRuneGiant && ancientRuneGiant->IsAlive() &&
            (!currentCrossUnit || currentCrossUnit->GetEntry() != ancientRuneGiant->GetEntry()))
            return true;

        return false;
    }

    return false;
}

bool ThorimGauntletPositioningTrigger::IsActive()
{
    if (bot->GetDistance(ULDUAR_THORIM_NEAR_ARENA_CENTER) > 110.0f)
        return false;

    Difficulty raidDifficulty = bot->GetRaidDifficulty();

    Group* group = bot->GetGroup();
    if (!group)
        return false;
    uint32 requiredAssistTankQuantity = 1;
    uint32 requiredHealerQuantity = 0;
    uint32 requiredDpsQuantity = 0;

    if (raidDifficulty == Difficulty::RAID_DIFFICULTY_10MAN_NORMAL)
    {
        requiredDpsQuantity = 3;
        requiredHealerQuantity = 1;
    }
    else if (raidDifficulty == Difficulty::RAID_DIFFICULTY_25MAN_NORMAL)
    {
        requiredDpsQuantity = 7;
        requiredHealerQuantity = 2;
    }

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member)
            continue;

        if (requiredDpsQuantity > 0 && botAI->IsDps(member))
        {
            requiredDpsQuantity--;
            if (bot->GetGUID() == member->GetGUID())
                break;
        }

        if (requiredAssistTankQuantity > 0 && botAI->IsAssistTankOfIndex(member, 0))
        {
            requiredAssistTankQuantity--;
            if (bot->GetGUID() == member->GetGUID())
                break;
        }

        if (requiredHealerQuantity > 0 && botAI->IsHeal(member))
        {
            requiredHealerQuantity--;
            if (bot->GetGUID() == member->GetGUID())
                break;
        }

        if (requiredDpsQuantity == 0 && requiredAssistTankQuantity == 0 && requiredHealerQuantity == 0)
            return false;
    }

    Unit* master = botAI->GetMaster();
    if (master->GetDistance(ULDUAR_THORIM_NEAR_ENTRANCE_POSITION) < 10.0f && (bot->GetDistance2d(master) > 5.0f))
    {
        return true;
    }

    if ((master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1) < 6.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2) < 6.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1) < 5.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1) < 10.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2) < 10.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3) < 10.0f) &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1) > 6.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2) > 6.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1) > 5.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1) > 10.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2) > 10.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3) > 10.0f)
    {
        if (bot->GetPositionZ() > ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
            return false;

        return true;
    }

    if ((master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1) < 6.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2) < 6.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1) < 5.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1) < 10.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2) < 10.0f ||
         master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3) < 10.0f) &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1) > 6.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2) > 6.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1) > 5.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1) > 10.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2) > 10.0f &&
        bot->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3) > 10.0f)
    {
        if (bot->GetPositionZ() > ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
            return false;

        return true;
    }

    Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");
    if (boss && boss->IsAlive() && bot->GetPositionZ() > ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD &&
        boss->GetPositionZ() < ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
    {
        return true;
    }

    return false;
}

bool ThorimArenaPositioningTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive() || !boss->IsHostileTo(bot))
        return false;

    if (boss->GetPositionZ() < ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
        return false;

    Difficulty raidDifficulty = bot->GetRaidDifficulty();

    Group* group = bot->GetGroup();
    if (!group)
        return false;
    uint32 requiredAssistTankQuantity = 1;
    uint32 requiredHealerQuantity = 0;
    uint32 requiredDpsQuantity = 0;

    if (raidDifficulty == Difficulty::RAID_DIFFICULTY_10MAN_NORMAL)
    {
        requiredDpsQuantity = 3;
        requiredHealerQuantity = 1;
    }
    else if (raidDifficulty == Difficulty::RAID_DIFFICULTY_25MAN_NORMAL)
    {
        requiredDpsQuantity = 7;
        requiredHealerQuantity = 2;
    }

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member)
            continue;

        if (requiredDpsQuantity > 0 && botAI->IsDps(member))
        {
            requiredDpsQuantity--;
            if (bot->GetGUID() == member->GetGUID())
                return false;
        }

        if (requiredAssistTankQuantity > 0 && botAI->IsAssistTankOfIndex(member, 0))
        {
            requiredAssistTankQuantity--;
            if (bot->GetGUID() == member->GetGUID())
                return false;
        }

        if (requiredHealerQuantity > 0 && botAI->IsHeal(member))
        {
            requiredHealerQuantity--;
            if (bot->GetGUID() == member->GetGUID())
                return false;
        }

        if (requiredDpsQuantity == 0 && requiredAssistTankQuantity == 0 && requiredHealerQuantity == 0)
            break;
    }

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        uint32 entry = target->GetEntry();

        if (entry == NPC_DARK_RUNE_ACOLYTE_I || entry == NPC_CAPTURED_MERCENARY_SOLDIER_ALLY ||
            entry == NPC_CAPTURED_MERCENARY_SOLDIER_HORDE || entry == NPC_CAPTURED_MERCENARY_CAPTAIN_ALLY ||
            entry == NPC_CAPTURED_MERCENARY_CAPTAIN_HORDE || entry == NPC_JORMUNGAR_BEHEMOT ||
            entry == NPC_DARK_RUNE_WARBRINGER || entry == NPC_DARK_RUNE_EVOKER || entry == NPC_DARK_RUNE_CHAMPION ||
            entry == NPC_DARK_RUNE_COMMONER)
            return false;
    }

    if (bot && bot->GetDistance(ULDUAR_THORIM_NEAR_ARENA_CENTER) > 5.0f)
        return true;

    return false;
}

bool ThorimFallFromFloorTrigger::IsActive()
{
    if (bot->GetDistance(ULDUAR_THORIM_NEAR_ARENA_CENTER) > 110.0f)
        return false;

    // Check if bot is on the floor
    return bot->GetPositionZ() < ULDUAR_THORIM_AXIS_Z_PATHING_ISSUE_DETECT;
}

bool ThorimPhase2PositioningTrigger::IsActive()
{
    if (!botAI->IsRanged(bot) && !botAI->IsMainTank(bot))
        return false;

    Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive() || !boss->IsHostileTo(bot))
        return false;

    if (boss->GetPositionZ() > ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
        return false;

    if (botAI->IsMainTank(bot))
    {
        if (bot->GetDistance(ULDUAR_THORIM_PHASE2_TANK_SPOT) > 1.0f && boss->GetVictim() == bot)
            return true;

        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    uint32 memberPositionNumber = 0;
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member)
            continue;

        if (botAI->IsRanged(member))
        {
            if (bot->GetGUID() == member->GetGUID())
                break;

            memberPositionNumber++;

            if (memberPositionNumber == 3)
                memberPositionNumber = 0;
        }
    }

    if (memberPositionNumber == 0 && bot->GetDistance(ULDUAR_THORIM_PHASE2_RANGE1_SPOT) > 1.0f)
        return true;

    if (memberPositionNumber == 1 && bot->GetDistance(ULDUAR_THORIM_PHASE2_RANGE2_SPOT) > 1.0f)
        return true;

    if (memberPositionNumber == 2 && bot->GetDistance(ULDUAR_THORIM_PHASE2_RANGE3_SPOT) > 1.0f)
        return true;

    return false;
}

bool MimironShockBlastTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "leviathan mk ii");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    if (!boss->HasUnitState(UNIT_STATE_CASTING) || !boss->FindCurrentSpellBySpellId(SPELL_SHOCK_BLAST))
    {
        return false;
    }

    if (botAI->IsMelee(bot))
    {
        return true;
    }
    else
    {
        return bot->GetDistance2d(boss) < 15.0f;
    }
}

bool MimironPhase1PositioningTrigger::IsActive()
{
    if (!botAI->IsRanged(bot))
    {
        return false;
    }

    Unit* leviathanMkII = nullptr;
    Unit* vx001 [[maybe_unused]] = nullptr;
    Unit* aerialCommandUnit [[maybe_unused]] = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            return false;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            return false;
        }
    }

    if (!leviathanMkII || !leviathanMkII->IsAlive())
    {
        return false;
    }

    return AI_VALUE(float, "disperse distance") != 6.0f;
}

bool MimironP3Wx2LaserBarrageTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "vx-001");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    bool isCasting = boss->HasUnitState(UNIT_STATE_CASTING);
    bool isP3WX2LaserBarrage = boss->FindCurrentSpellBySpellId(SPELL_SPINNING_UP) ||
        boss->FindCurrentSpellBySpellId(SPELL_P3WX2_LASER_BARRAGE_1) ||
        boss->FindCurrentSpellBySpellId(SPELL_P3WX2_LASER_BARRAGE_2) ||
        boss->FindCurrentSpellBySpellId(SPELL_P3WX2_LASER_BARRAGE_AURA_1) ||
        boss->FindCurrentSpellBySpellId(SPELL_P3WX2_LASER_BARRAGE_AURA_2) ||
        boss->FindCurrentSpellBySpellId(SPELL_P3WX2_LASER_BARRAGE_3);
    bool hasP3WX2LaserBarrageAura =
        boss->HasAura(SPELL_P3WX2_LASER_BARRAGE_AURA_1) || boss->HasAura(SPELL_P3WX2_LASER_BARRAGE_AURA_2);

    if ((!isCasting && !hasP3WX2LaserBarrageAura) || !isP3WX2LaserBarrage)
    {
        return false;
    }

    return true;
}

bool MimironRapidBurstTrigger::IsActive()
{
    Unit* leviathanMkII = nullptr;
    Unit* vx001 = nullptr;
    Unit* aerialCommandUnit = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            vx001 = target;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            aerialCommandUnit = target;
        }
    }

    if (!vx001 || !vx001->IsAlive())
    {
        return false;
    }

    if (leviathanMkII && leviathanMkII->HasUnitState(UNIT_STATE_CASTING) &&
        leviathanMkII->FindCurrentSpellBySpellId(SPELL_SHOCK_BLAST))
    {
        return false;
    }

    if (botAI->IsMainTank(bot) && leviathanMkII && leviathanMkII->IsAlive() && leviathanMkII->GetVictim() != bot)
    {
        return false;
    }

    if (botAI->IsMelee(bot) && !botAI->IsMainTank(bot) && leviathanMkII && aerialCommandUnit)
    {
        return false;
    }

    MimironP3Wx2LaserBarrageTrigger mimironP3Wx2LaserBarrageTrigger(botAI);
    if (mimironP3Wx2LaserBarrageTrigger.IsActive())
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    uint32 memberSpotNumber = 0;
    Position memberPosition;
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member)
            continue;

        if (bot->GetGUID() == member->GetGUID())
        {
            if (botAI->IsRanged(bot))
            {
                switch (memberSpotNumber)
                {
                    case 0:
                        memberPosition = ULDUAR_MIMIRON_PHASE2_SIDE1RANGE_SPOT;
                        break;
                    case 1:
                        memberPosition = ULDUAR_MIMIRON_PHASE2_SIDE2RANGE_SPOT;
                        break;
                    case 2:
                        memberPosition = ULDUAR_MIMIRON_PHASE2_SIDE3RANGE_SPOT;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                if (botAI->IsMainTank(bot) && leviathanMkII)
                {
                    memberPosition = ULDUAR_MIMIRON_PHASE4_TANK_SPOT;
                }
                else
                {
                    switch (memberSpotNumber)
                    {
                        case 0:
                            memberPosition = ULDUAR_MIMIRON_PHASE2_SIDE1MELEE_SPOT;
                            break;
                        case 1:
                            memberPosition = ULDUAR_MIMIRON_PHASE2_SIDE2MELEE_SPOT;
                            break;
                        case 2:
                            memberPosition = ULDUAR_MIMIRON_PHASE2_SIDE3MELEE_SPOT;
                            break;
                        default:
                            break;
                    }
                }
            }
            break;
        }

        memberSpotNumber++;

        if (memberSpotNumber == 3)
        {
            memberSpotNumber = 0;
        }
    }

    GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
    float nearestRocketStrikeDistance = std::numeric_limits<float>::max();
    bool rocketStrikeDetected = false;

    for (const ObjectGuid& guid : npcs)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit)
            continue;

        if (unit->GetEntry() == NPC_ROCKET_STRIKE_N)
        {
            rocketStrikeDetected = true;
            float distance = bot->GetDistance2d(memberPosition.GetPositionX(), memberPosition.GetPositionY());
            if (distance < nearestRocketStrikeDistance)
            {
                nearestRocketStrikeDistance = distance;
            }
        }
    }

    return (bot->GetDistance(memberPosition) > 7.0f && !rocketStrikeDetected) ||
           bot->GetDistance(memberPosition) > 20.0f;
}

bool MimironAerialCommandUnitTrigger::IsActive()
{
    Unit* leviathanMkII = nullptr;
    Unit* vx001 = nullptr;
    Unit* aerialCommandUnit = nullptr;
    //Unit* bombBot = nullptr;
    Unit* assaultBot = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            vx001 = target;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            aerialCommandUnit = target;
        }
        //else if (target->GetEntry() == NPC_BOMB_BOT)
        //{
        //    bombBot = target;
        //}
        else if (target->GetEntry() == NPC_ASSAULT_BOT)
        {
            assaultBot = target;
        }
    }

    if (!aerialCommandUnit || !aerialCommandUnit->IsAlive() || leviathanMkII || vx001)
    {
        return false;
    }

    if (!botAI->IsRanged(bot) && !botAI->IsMainTank(bot) && !botAI->IsAssistTankOfIndex(bot, 0))
    {
        return false;
    }

    if (botAI->IsMainTank(bot) || botAI->IsAssistTankOfIndex(bot, 0))
    {
        Group* group = bot->GetGroup();
        if (!group)
        {
            return false;
        }

        ObjectGuid skullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
        ObjectGuid crossTarget = group->GetTargetIcon(RtiTargetValue::crossIndex);

        //if (bombBot && bombBot->GetGUID() != crossTarget)
        //{
        //    return true;
        //}
        if (!crossTarget || aerialCommandUnit->GetGUID() != crossTarget)
        {
            return true;
        }
        else if (assaultBot && (!skullTarget || assaultBot->GetGUID() != skullTarget))
        {
            return true;
        }

        return false;
    }

    std::string rtiMark = AI_VALUE(std::string, "rti");
    if (rtiMark != "cross")
    {
        return true;
    }

    return false;
}

bool MimironRocketStrikeTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "vx-001");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    Creature* rocketStrikeN = bot->FindNearestCreature(NPC_ROCKET_STRIKE_N, 100.0f);

    if (!rocketStrikeN)
    {
        return false;
    }

    return bot->GetDistance2d(rocketStrikeN->GetPositionX(), rocketStrikeN->GetPositionY()) <= 10.0f;
}

bool MimironPhase4MarkDpsTrigger::IsActive()
{
    Unit* leviathanMkII = nullptr;
    Unit* vx001 = nullptr;
    Unit* aerialCommandUnit = nullptr;

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            vx001 = target;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            aerialCommandUnit = target;
        }
    }

    if (!leviathanMkII || !vx001 || !aerialCommandUnit)
    {
        return false;
    }

    if (botAI->IsMainTank(bot))
    {
        Unit* highestHealthUnit = nullptr;
        uint32 highestHealth = 0;

        if (leviathanMkII && leviathanMkII->GetHealth() > highestHealth)
        {
            highestHealth = leviathanMkII->GetHealth();
            highestHealthUnit = leviathanMkII;
        }
        if (vx001 && vx001->GetHealth() > highestHealth)
        {
            highestHealth = vx001->GetHealth();
            highestHealthUnit = vx001;
        }
        if (aerialCommandUnit && aerialCommandUnit->GetHealth() > highestHealth)
        {
            highestHealthUnit = aerialCommandUnit;
        }

        ObjectGuid skullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
        if (!skullTarget)
        {
            return true;
        }

        return highestHealthUnit->GetGUID() != skullTarget;
    }
    else
    {
        return AI_VALUE(std::string, "rti") != "skull";
    }

    return false;
}

bool MimironCheatTrigger::IsActive()
{
    if (!botAI->HasCheat(BotCheatMask::raid))
    {
        return false;
    }

    if (!botAI->IsMainTank(bot))
    {
        return false;
    }

    GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
    for (const ObjectGuid& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        if (unit->GetEntry() == NPC_PROXIMITY_MINE)
        {
            return true;
        }
        else if (unit->GetEntry() == NPC_BOMB_BOT)
        {
            return true;
        }
    }

    return false;
}

bool VezaxCheatTrigger::IsActive()
{
    if (!botAI->HasCheat(BotCheatMask::raid))
    {
        return false;
    }

    Unit* boss = AI_VALUE2(Unit*, "find target", "general vezax");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    if (!AI_VALUE2(bool, "has mana", "self target"))
    {
        return false;
    }

    return AI_VALUE2(uint8, "mana", "self target") < sShadowAIConfig->lowMana;
}

bool VezaxShadowCrashTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "general vezax");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    return botAI->HasAura(SPELL_SHADOW_CRASH, bot);
}

bool VezaxMarkOfTheFacelessTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "general vezax");

    // Check boss and it is alive
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    if (!botAI->HasAura(SPELL_MARK_OF_THE_FACELESS, bot))
    {
        return false;
    }

    float distance = bot->GetDistance2d(ULDUAR_VEZAX_MARK_OF_THE_FACELESS_SPOT.GetPositionX(),
                                        ULDUAR_VEZAX_MARK_OF_THE_FACELESS_SPOT.GetPositionY());

    return distance > 2.0f;
}

Unit* YoggSaronTrigger::GetSaraIfAlive()
{
    Unit* sara = AI_VALUE2(Unit*, "find target", "sara");
    if (!sara || !sara->IsAlive())
    {
        return nullptr;
    }
    return sara;
}

bool YoggSaronTrigger::IsPhase2()
{
    Creature* target = bot->FindNearestCreature(NPC_YOGG_SARON, 200.0f, true);

    return target && target->IsAlive() && target->HasAura(SPELL_SHADOW_BARRIER);
}

bool YoggSaronTrigger::IsPhase3()
{
    Creature* target = bot->FindNearestCreature(NPC_YOGG_SARON, 200.0f, true);
    Creature* guardian = bot->FindNearestCreature(NPC_GUARDIAN_OF_YS, 200.0f, true);

    return target && target->IsAlive() && !target->HasAura(SPELL_SHADOW_BARRIER) && !guardian;
}

bool YoggSaronTrigger::IsInBrainLevel()
{
    return bot->GetPositionZ() > 230.0f && bot->GetPositionZ() < 250.0f;
}

bool YoggSaronTrigger::IsYoggSaronFight()
{
    Unit* sara = AI_VALUE2(Unit*, "find target", "sara");
    Unit* yoggsaron = AI_VALUE2(Unit*, "find target", "yogg-saron");

    if ((sara && sara->IsAlive()) || (yoggsaron && yoggsaron->IsAlive()))
    {
        return true;
    }

    return false;
}

bool YoggSaronTrigger::IsInIllusionRoom()
{
    if (!IsInBrainLevel())
    {
        return false;
    }

    if (IsInStormwindKeeperIllusion())
    {
        return true;
    }

    if (IsInIcecrownKeeperIllusion())
    {
        return true;
    }

    if (IsInChamberOfTheAspectsIllusion())
    {
        return true;
    }

    return false;
}

bool YoggSaronTrigger::IsInStormwindKeeperIllusion()
{
    return bot->GetDistance2d(ULDUAR_YOGG_SARON_STORMWIND_KEEPER_MIDDLE.GetPositionX(),
                              ULDUAR_YOGG_SARON_STORMWIND_KEEPER_MIDDLE.GetPositionY()) <
           ULDUAR_YOGG_SARON_STORMWIND_KEEPER_RADIUS;
}

bool YoggSaronTrigger::IsInIcecrownKeeperIllusion()
{
    return bot->GetDistance2d(ULDUAR_YOGG_SARON_ICECROWN_CITADEL_MIDDLE.GetPositionX(),
                              ULDUAR_YOGG_SARON_ICECROWN_CITADEL_MIDDLE.GetPositionY()) <
           ULDUAR_YOGG_SARON_ICECROWN_CITADEL_RADIUS;
}

bool YoggSaronTrigger::IsInChamberOfTheAspectsIllusion()
{
    return bot->GetDistance2d(ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_MIDDLE.GetPositionX(),
                              ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_MIDDLE.GetPositionY()) <
           ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_RADIUS;
}

bool YoggSaronTrigger::IsMasterIsInIllusionGroup()
{
    Player* master = botAI->GetMaster();
    return master && !botAI->IsTank(master);
}

bool YoggSaronTrigger::IsMasterIsInBrainRoom()
{
    Player* master = botAI->GetMaster();

    if (!master)
    {
        return false;
    }

    return master->GetDistance2d(ULDUAR_YOGG_SARON_BRAIN_ROOM_MIDDLE.GetPositionX(),
                                 ULDUAR_YOGG_SARON_BRAIN_ROOM_MIDDLE.GetPositionY()) <
               ULDUAR_YOGG_SARON_BRAIN_ROOM_RADIUS &&
           master->GetPositionZ() > 230.0f && master->GetPositionZ() < 250.0f;
}

Position YoggSaronTrigger::GetIllusionRoomEntrancePosition()
{
    if (IsInChamberOfTheAspectsIllusion())
    {
        return ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_ENTRANCE;
    }
    else if (IsInIcecrownKeeperIllusion())
    {
        return ULDUAR_YOGG_SARON_ICECROWN_CITADEL_ENTRANCE;
    }
    else if (IsInStormwindKeeperIllusion())
    {
        return ULDUAR_YOGG_SARON_STORMWIND_KEEPER_ENTRANCE;
    }
    else
    {
        return Position();
    }
}

Unit* YoggSaronTrigger::GetIllusionRoomRtiTarget()
{
    Group* group = bot->GetGroup();
    if (!group)
    {
        return nullptr;
    }

    int32 rtiIndex = RtiTargetValue::GetRtiIndex(AI_VALUE(std::string, "rti"));
    if (rtiIndex == -1)
    {
        return nullptr;  // Invalid RTI mark
    }

    ObjectGuid currentRtiTarget = group->GetTargetIcon(rtiIndex);
    Unit* currentRtiTargetUnit = botAI->GetUnit(currentRtiTarget);
    if (!currentRtiTargetUnit || !currentRtiTargetUnit->IsAlive())
    {
        currentRtiTargetUnit = nullptr;
    }

    return currentRtiTargetUnit;
}

Unit* YoggSaronTrigger::GetNextIllusionRoomRtiTarget()
{
    float detectionRadius = 0.0f;
    if (IsInStormwindKeeperIllusion())
    {
        detectionRadius = ULDUAR_YOGG_SARON_STORMWIND_KEEPER_RADIUS;
    }
    else if (IsInIcecrownKeeperIllusion())
    {
        detectionRadius = ULDUAR_YOGG_SARON_ICECROWN_CITADEL_RADIUS;
    }
    else if (IsInChamberOfTheAspectsIllusion())
    {
        detectionRadius = ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_RADIUS;
    }
    else
    {
        return nullptr;
    }

    GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");

    if (botAI->HasCheat(BotCheatMask::raid))
    {
        for (const ObjectGuid& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (unit && unit->IsAlive() && unit->GetEntry() == NPC_LAUGHING_SKULL)
            {
                return unit;
            }
        }
    }

    float nearestDistance = std::numeric_limits<float>::max();
    Unit* nextIllusionRoomRtiTarget = nullptr;

    for (const uint32& creatureId : illusionMobs)
    {
        for (const ObjectGuid& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (unit && unit->IsAlive() && unit->GetEntry() == creatureId)
            {
                float distance = bot->GetDistance(unit);
                if (distance < nearestDistance)
                {
                    nextIllusionRoomRtiTarget = unit;
                    nearestDistance = distance;
                }
            }
        }
    }

    if (nextIllusionRoomRtiTarget)
    {
        return nextIllusionRoomRtiTarget;
    }

    if (IsInStormwindKeeperIllusion())
    {
        Creature* target = bot->FindNearestCreature(NPC_SUIT_OF_ARMOR, detectionRadius, true);
        if (target)
        {
            return target;
        }
    }

    return nullptr;
}

bool YoggSaronOminousCloudCheatTrigger::IsActive()
{
    if (!botAI->HasCheat(BotCheatMask::raid))
    {
        return false;
    }

    Unit* boss = GetSaraIfAlive();
    if (!boss)
    {
        return false;
    }

    if (!botAI->IsBotMainTank(bot))
    {
        return false;
    }

    if (bot->GetDistance2d(boss->GetPositionX(), boss->GetPositionY()) > 50.0f)
    {
        return false;
    }

    Creature* target = boss->FindNearestCreature(NPC_OMINOUS_CLOUD, 25.0f, true);

    return target;
}

bool YoggSaronGuardianPositioningTrigger::IsActive()
{
    if (!GetSaraIfAlive())
    {
        return false;
    }

    if (!botAI->IsTank(bot))
    {
        return false;
    }

    GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
    bool thereIsAnyGuardian = false;

    for (const ObjectGuid& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
        {
            continue;
        }

        if (unit->GetEntry() == NPC_GUARDIAN_OF_YS)
        {
            thereIsAnyGuardian = true;
            ObjectGuid unitTargetGuid = unit->GetTarget();
            Player* targetedPlayer = botAI->GetPlayer(unitTargetGuid);
            if (!targetedPlayer || !botAI->IsTank(targetedPlayer))
            {
                return false;
            }
        }
    }

    return thereIsAnyGuardian &&
           bot->GetDistance2d(ULDUAR_YOGG_SARON_MIDDLE.GetPositionX(), ULDUAR_YOGG_SARON_MIDDLE.GetPositionY()) > 1.0f;
}

bool YoggSaronSanityTrigger::IsActive()
{
    Aura* sanityAura = bot->GetAura(SPELL_SANITY);

    if (!sanityAura)
    {
        return false;
    }

    int sanityAuraStacks = sanityAura->GetStackAmount();

    Creature* sanityWell = bot->FindNearestCreature(NPC_SANITY_WELL, 200.0f);

    if (!sanityWell)
    {
        return false;
    }

    float distanceToSanityWell = bot->GetDistance(sanityWell);

    return (distanceToSanityWell >= 1.0f && sanityAuraStacks < 40) ||
           (distanceToSanityWell < 1.0f && sanityAuraStacks < 100);
}

bool YoggSaronDeathOrbTrigger::IsActive()
{
    TooCloseToCreatureTrigger tooCloseToDeathOrbTrigger(botAI);
    return IsPhase2() && tooCloseToDeathOrbTrigger.TooCloseToCreature(NPC_DEATH_ORB, 10.0f);
}

bool YoggSaronMaladyOfTheMindTrigger::IsActive()
{
    TooCloseToPlayerWithDebuffTrigger tooCloseToPlayerWithDebuffTrigger(botAI);
    return IsPhase2() && tooCloseToPlayerWithDebuffTrigger.TooCloseToPlayerWithDebuff(SPELL_MALADY_OF_THE_MIND, 15.0f) && botAI->CanMove();
}

bool YoggSaronMarkTargetTrigger::IsActive()
{
    if (!IsYoggSaronFight())
    {
        return false;
    }

    if (!botAI->IsBotMainTank(bot))
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    if (IsPhase2())
    {
        ObjectGuid currentMoonTarget = group->GetTargetIcon(RtiTargetValue::moonIndex);
        Creature* yogg_saron = bot->FindNearestCreature(NPC_YOGG_SARON, 200.0f, true);
        if (!currentMoonTarget || currentMoonTarget != yogg_saron->GetGUID())
        {
            return true;
        }

        ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);

        Creature* nextPossibleTarget = bot->FindNearestCreature(NPC_CONSTRICTOR_TENTACLE, 200.0f, true);
        if (!nextPossibleTarget)
        {
            nextPossibleTarget = bot->FindNearestCreature(NPC_CORRUPTOR_TENTACLE, 200.0f, true);
            if (!nextPossibleTarget)
            {
                return false;
            }
        }

        if (currentSkullTarget)
        {
            Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);

            if (!currentSkullUnit)
            {
                return true;
            }

            if (currentSkullUnit->IsAlive() && currentSkullUnit->GetGUID() == nextPossibleTarget->GetGUID())
            {
                return false;
            }
        }

        return true;
    }
    else if (IsPhase3())
    {
        ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
        Unit* currentSkullUnit = nullptr;
        if (currentSkullTarget)
        {
            currentSkullUnit = botAI->GetUnit(currentSkullTarget);
        }

        if (currentSkullUnit &&
            (currentSkullUnit->GetEntry() == NPC_IMMORTAL_GUARDIAN ||
             currentSkullUnit->GetEntry() == NPC_MARKED_IMMORTAL_GUARDIAN) &&
            currentSkullUnit->GetHealthPct() > 10)
        {
            return false;
        }

        GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
        for (const ObjectGuid& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive())
            {
                continue;
            }

            if ((unit->GetEntry() == NPC_IMMORTAL_GUARDIAN || unit->GetEntry() == NPC_MARKED_IMMORTAL_GUARDIAN) &&
                unit->GetHealthPct() > 10)
            {
                return true;
            }
        }

        if (!currentSkullUnit || currentSkullUnit->GetEntry() != NPC_YOGG_SARON)
        {
            return true;
        }

        return false;
    }

    return false;
}

bool YoggSaronBrainLinkTrigger::IsActive()
{
    TooFarFromPlayerWithAuraTrigger tooFarFromPlayerWithAuraTrigger(botAI);
    return IsPhase2() && bot->HasAura(SPELL_BRAIN_LINK) &&
           tooFarFromPlayerWithAuraTrigger.TooFarFromPlayerWithAura(SPELL_BRAIN_LINK, 20.0f, false);
}

bool YoggSaronMoveToEnterPortalTrigger::IsActive()
{
    if (!IsPhase2())
    {
        return false;
    }

    Creature* portal = bot->FindNearestCreature(NPC_DESCEND_INTO_MADNESS, 100.0f, true);
    if (!portal)
    {
        return false;
    }

    if (bot->GetDistance2d(portal->GetPositionX(), portal->GetPositionY()) < 2.0f)
    {
        return false;
    }

    if (AI_VALUE(std::string, "rti") != "skull")
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    int brainRoomTeamCount = 10;
    if (bot->GetRaidDifficulty() == Difficulty::RAID_DIFFICULTY_10MAN_NORMAL)
    {
        brainRoomTeamCount = 4;
    }

    if (IsMasterIsInIllusionGroup())
    {
        brainRoomTeamCount--;
    }

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !member->IsAlive() || botAI->IsTank(member))
        {
            continue;
        }

        if (member->GetGUID() == bot->GetGUID())
        {
            return true;
        }

        brainRoomTeamCount--;
        if (brainRoomTeamCount == 0)
        {
            break;
        }
    }

    return false;
}

bool YoggSaronFallFromFloorTrigger::IsActive()
{
    if (!IsYoggSaronFight())
    {
        return false;
    }

    std::string rtiMark = AI_VALUE(std::string, "rti");

    if (rtiMark == "skull" && bot->GetPositionZ() < ULDUAR_YOGG_SARON_BOSS_ROOM_AXIS_Z_PATHING_ISSUE_DETECT)
    {
        return true;
    }
    if ((rtiMark == "cross" || rtiMark == "circle" || rtiMark == "star") && bot->GetPositionZ() < ULDUAR_YOGG_SARON_BRAIN_ROOM_AXIS_Z_PATHING_ISSUE_DETECT)
    {
        return true;
    }

    return false;
}

bool YoggSaronBossRoomMovementCheatTrigger::IsActive()
{
    if (!IsYoggSaronFight() || !IsPhase2())
    {
        return false;
    }

    FollowMasterStrategy followMasterStrategy(botAI);
    if (botAI->HasStrategy(followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT))
    {
        return true;
    }

    if (!botAI->HasCheat(BotCheatMask::raid))
    {
        return false;
    }

    if (AI_VALUE(std::string, "rti") != "skull")
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }
    ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);

    if (!currentSkullTarget)
    {
        return false;
    }

    Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);

    if (!currentSkullUnit || !currentSkullUnit->IsAlive() || bot->GetDistance2d(currentSkullUnit->GetPositionX(), currentSkullUnit->GetPositionY()) < 40.0f)
    {
        return false;
    }

    return true;
}

bool YoggSaronUsePortalTrigger::IsActive()
{
    if (!IsPhase2())
    {
        return false;
    }

    if (AI_VALUE(std::string, "rti") != "diamond")
    {
        return false;
    }

    return bot->FindNearestCreature(NPC_DESCEND_INTO_MADNESS, 2.0f, true) != nullptr;
}

bool YoggSaronIllusionRoomTrigger::IsActive()
{
    if (!IsYoggSaronFight() || !IsInIllusionRoom() || AI_VALUE(std::string, "rti") == "square")
    {
        return false;
    }

    if (SetRtiMarkRequired())
    {
        return true;
    }

    if (SetRtiTargetRequired())
    {
        return true;
    }

    if (GoToBrainRoomRequired())
    {
        return true;
    }

    return false;
}

bool YoggSaronIllusionRoomTrigger::GoToBrainRoomRequired()
{
    if (AI_VALUE(std::string, "rti") == "square")
    {
        return false;
    }

    return IsMasterIsInBrainRoom();
}

bool YoggSaronIllusionRoomTrigger::SetRtiMarkRequired()
{
    return AI_VALUE(std::string, "rti") == "diamond";
}

bool YoggSaronIllusionRoomTrigger::SetRtiTargetRequired()
{
    Unit const* currentRtiTarget = GetIllusionRoomRtiTarget();
    if (currentRtiTarget != nullptr)
    {
        return false;
    }

    return GetNextIllusionRoomRtiTarget() != nullptr;
}

bool YoggSaronMoveToExitPortalTrigger::IsActive()
{
    if (!IsYoggSaronFight() || !IsInBrainLevel())
    {
        return false;
    }

    Creature const* brain = bot->FindNearestCreature(NPC_BRAIN, 60.0f, true);
    if (!brain || !brain->IsAlive())
    {
        return false;
    }

    if (brain->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* induceMadnessSpell = brain->GetCurrentSpell(CURRENT_GENERIC_SPELL);

        if (induceMadnessSpell && induceMadnessSpell->m_spellInfo->Id == SPELL_INDUCE_MADNESS)
        {
            uint32 castingTimeLeft = induceMadnessSpell->GetCastTimeRemaining();
            if ((botAI->HasCheat(BotCheatMask::raid) && castingTimeLeft < 6000) ||
                (!botAI->HasCheat(BotCheatMask::raid) && castingTimeLeft < 8000))
            {
                return true;
            }
        }
    }
    else if (brain->GetHealth() < brain->GetMaxHealth() * 0.3f)
    {
        return true;
    }

    return false;
}

bool YoggSaronLunaticGazeTrigger::IsActive()
{
    Unit* yoggsaron = AI_VALUE2(Unit*, "find target", "yogg-saron");

    if (yoggsaron && yoggsaron->IsAlive() && yoggsaron->HasUnitState(UNIT_STATE_CASTING))
    {
        Spell* currentSpell = yoggsaron->GetCurrentSpell(CURRENT_CHANNELED_SPELL);
        if (currentSpell && currentSpell->m_spellInfo->Id == SPELL_LUNATIC_GAZE_YS)
        {
            return true;
        }
    }

    return false;
}

bool YoggSaronPhase3PositioningTrigger::IsActive()
{
    if (!IsYoggSaronFight() || !IsPhase3())
    {
        return false;
    }

    YoggSaronSanityTrigger sanityTrigger(botAI);
    if (sanityTrigger.IsActive())
    {
        return false;
    }

    if (botAI->IsRanged(bot) && bot->GetDistance2d(ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionX(),
                                                   ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionY()) > 15.0f)
    {
        return true;
    }

    if (botAI->IsMelee(bot) && !botAI->IsTank(bot) &&
        bot->GetDistance2d(ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionX(),
            ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionY()) > 15.0f)
    {
        return true;
    }

    if (botAI->IsTank(bot))
    {
        if (bot->GetDistance(ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT) > 30.0f)
        {
            return true;
        }

        GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
        bool thereIsAnyGuardian = false;

        for (const ObjectGuid& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive())
            {
                continue;
            }

            if (unit->GetEntry() == NPC_IMMORTAL_GUARDIAN || unit->GetEntry() == NPC_MARKED_IMMORTAL_GUARDIAN)
            {
                thereIsAnyGuardian = true;
                ObjectGuid unitTargetGuid = unit->GetTarget();
                Player* targetedPlayer = botAI->GetPlayer(unitTargetGuid);
                if (!targetedPlayer || !botAI->IsTank(targetedPlayer))
                {
                    return false;
                }
            }
        }

        if (thereIsAnyGuardian && bot->GetDistance2d(ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionX(),
                                                     ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionY()) > 3.0f)
        {
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const std::string ADD_STRATEGY_CHAR = "+";
const std::string REMOVE_STRATEGY_CHAR = "-";

const std::vector<Position> corners = {
    {183.53f, 66.53f, 409.80f}, {383.03f, 75.10f, 411.71f}, {379.74f, -133.05f, 410.88f}, {158.67f, -137.54f, 409.80f}};

const Position ULDUAR_KOLOGARN_RESTORE_POSITION = Position(1764.3749f, -24.02903f, 448.0f, 0.00087690353f);
const Position ULDUAR_KOLOGARN_EYEBEAM_LEFT_POSITION = Position(1781.2051f, 9.34402f, 449.0f, 0.00087690353f);
const Position ULDUAR_KOLOGARN_EYEBEAM_RIGHT_POSITION = Position(1763.2561f, -24.44305f, 449.0f, 0.00087690353f);
const Position ULDUAR_THORIM_JUMP_START_POINT = Position(2137.137f, -291.19025f, 438.24753f, 1.7059844f);
const Position ULDUAR_YOGG_SARON_BOSS_ROOM_RESTORE_POINT = Position(1928.8923f, -24.871964f, 324.88956f, 6.247805f);

const Position yoggPortalLoc[] = {
    {1970.48f, -9.75f, 325.5f},  {1992.76f, -10.21f, 325.5f}, {1995.53f, -39.78f, 325.5f}, {1969.25f, -42.00f, 325.5f},
    {1960.62f, -32.00f, 325.5f}, {1981.98f, -5.69f, 325.5f},  {1982.78f, -45.73f, 325.5f}, {2000.66f, -29.68f, 325.5f},
    {1999.88f, -19.61f, 325.5f}, {1961.37f, -19.54f, 325.5f}};

bool FlameLeviathanVehicleAction::Execute(Event event [[maybe_unused]])
{
    vehicleBase_ = bot->GetVehicleBase();
    vehicle_ = bot->GetVehicle();
    if (!vehicleBase_ || !vehicle_)
        return false;

    GuidVector attackers = context->GetValue<GuidVector>("attackers")->Get();
    Unit* target = nullptr;
    Unit* flame = nullptr;
    for (auto i = attackers.begin(); i != attackers.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (!unit)
            continue;
        if (unit->GetEntry() == 33139)  // Flame Leviathan Turret
            continue;
        if (unit->GetEntry() == 33142)  // Leviathan Defense Turret
            continue;
        if (unit->GetEntry() == 33113)  // Flame Leviathan
        {
            flame = unit;
            continue;
        }
        if (!target || bot->GetExactDist(target) > bot->GetExactDist(unit))
        {
            target = unit;
        }
    }
    // Flame Leviathan is chasing me
    if (flame && flame->GetVictim() == vehicleBase_)
        if (MoveAvoidChasing(flame))
            return true;

    uint32 entry = vehicleBase_->GetEntry();
    switch (entry)
    {
        case NPC_SALVAGED_DEMOLISHER:
            return DemolisherAction(flame ? flame : target);
        case NPC_SALVAGED_DEMOLISHER_TURRET:
            return DemolisherTurretAction(target ? target: flame);
        case NPC_SALVAGED_SIEGE_ENGINE:
            return SiegeEngineAction(flame ? flame : target);
        case NPC_SALVAGED_SIEGE_ENGINE_TURRET:
            return SiegeEngineTurretAction(target ? target: flame);
        case NPC_VEHICLE_CHOPPER:
            return ChopperAction(target ? target: flame);
        default:
            break;
    }
    return false;
}

bool FlameLeviathanVehicleAction::MoveAvoidChasing(Unit* target)
{
    if (!target)
        return false;
    if (avoidChaseIdx == -1)
    {
        for (size_t i = 0; i < corners.size(); i++)
        {
            if (bot->GetExactDist(corners[i]) > target->GetExactDist(corners[i]))
                continue;
            if (avoidChaseIdx == -1 || bot->GetExactDist(corners[i]) > bot->GetExactDist(corners[avoidChaseIdx]))
                avoidChaseIdx = i;
        }
        if (avoidChaseIdx == -1)
            avoidChaseIdx = 0;
    }
    else
    {
        if (bot->GetExactDist(corners[avoidChaseIdx]) < 5.0f && target->GetExactDist(bot) < 50.0f)
            avoidChaseIdx = (avoidChaseIdx + 1) % corners.size();
    }
    const Position& to = corners[avoidChaseIdx];
    return MoveTo(bot->GetMap()->GetId(), to.GetPositionX(), to.GetPositionY(), to.GetPositionZ(), false, false, false,
                  false, MovementPriority::MOVEMENT_COMBAT);
}

bool FlameLeviathanVehicleAction::DemolisherAction(Unit* target)
{
    if (!target)
        return false;
    Aura* bluePyrite = target->GetAura(68605);
    if (!bluePyrite || (vehicleBase_->GetPower(POWER_ENERGY) >= 20) || bluePyrite->GetDuration() <= 5000)
    {
        uint32 spellId = 62490;
        if (botAI->CanCastVehicleSpell(spellId, target))
            if (botAI->CastVehicleSpell(spellId, target))
            {
                vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
                return true;
            }
    }
    uint32 spellId = 62306;
    if (botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
            return true;
        }
    return false;
}

bool FlameLeviathanVehicleAction::DemolisherTurretAction(Unit* target)
{
    int32 liquidCount = 0;
    {
        GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
        for (auto i = npcs.begin(); i != npcs.end(); i++)
        {
            Unit* unit = botAI->GetUnit(*i);
            if (!unit)
                continue;
            if (unit->GetEntry() != 33189)
                continue;
            if (unit->GetDistance(bot) >= 49.0f)
                continue;
            ++liquidCount;
            if (vehicleBase_->GetPower(POWER_ENERGY) <= 25) // Liquid Pyrite
            {
                uint32 spellId = 62479;
                if (botAI->CanCastVehicleSpell(spellId, unit))
                    if (botAI->CastVehicleSpell(spellId, unit))
                    {
                        vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
                        return true;
                    }
            }
        }
    }
    if (liquidCount <= 10)
    {
        GuidVector targets = AI_VALUE(GuidVector, "possible targets");
        for (auto i = targets.begin(); i != targets.end(); i++)
        {
            Unit* unit = botAI->GetUnit(*i);
            if (!unit)
                continue;
            if (unit->GetEntry() == 33214) // Mechanolift 304-A
            {
                uint32 spellId = 64979;
                if (botAI->CanCastVehicleSpell(spellId, unit))
                    if (botAI->CastVehicleSpell(spellId, unit))
                    {
                        vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
                        return true;
                    }
            }
        }
    }
    if (!target)
        return false;
    uint32 spellId = 62634;
    if (botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
            return true;
        }
    return false;
}

bool FlameLeviathanVehicleAction::SiegeEngineAction(Unit* target)
{
    if (!target)
        return false;
    if (target->GetCurrentSpell(CURRENT_CHANNELED_SPELL) || target->HasAura(62396))
    {
        uint32 spellId = 62522;
        if (botAI->CanCastVehicleSpell(spellId, target))
            if (botAI->CastVehicleSpell(spellId, target))
            {
                vehicleBase_->AddSpellCooldown(spellId, 0, 10000);
                return true;
            }
    }
    uint32 spellId = 62345;
    if (vehicleBase_->GetPower(POWER_ENERGY) >= 80 && botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
            return true;
        }
    return false;
}

bool FlameLeviathanVehicleAction::SiegeEngineTurretAction(Unit* target)
{
    if (!target)
        return false;
    uint32 spellId = 62358;
    if (botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
            return true;
        }
    return false;
}

bool FlameLeviathanVehicleAction::ChopperAction(Unit* target)
{
    if (!target)
        return false;
    uint32 spellId = 62286;
    if (botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase_->AddSpellCooldown(spellId, 0, 15000);
            return true;
        }
    spellId = 62974;
    if (botAI->CanCastVehicleSpell(spellId, target))
        if (botAI->CastVehicleSpell(spellId, target))
        {
            vehicleBase_->AddSpellCooldown(spellId, 0, 1000);
            return true;
        }
    return false;
}

bool FlameLeviathanEnterVehicleAction::Execute(Event event [[maybe_unused]])
{
    // do not switch vehicles yet
    if (bot->GetVehicle())
        return false;
    Unit* vehicleToEnter = nullptr;
    GuidVector npcs = AI_VALUE(GuidVector, "nearest vehicles far");
    for (GuidVector::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* vehicleBase = botAI->GetUnit(*i);
        if (!vehicleBase)
            continue;

        if (vehicleBase->HasUnitFlag(UNIT_FLAG_NOT_SELECTABLE))
            continue;

        if (!ShouldEnter(vehicleBase))
            continue;

        if (!vehicleToEnter || bot->GetExactDist(vehicleToEnter) > bot->GetExactDist(vehicleBase))
            vehicleToEnter = vehicleBase;
    }

    if (!vehicleToEnter)
        return false;

    if (EnterVehicle(vehicleToEnter, true))
        return true;

    return false;
}

bool FlameLeviathanEnterVehicleAction::EnterVehicle(Unit* vehicleBase, bool moveIfFar)
{
    float dist = bot->GetDistance(vehicleBase);

    if (dist > INTERACTION_DISTANCE && !moveIfFar)
        return false;

    if (dist > INTERACTION_DISTANCE)
        return MoveTo(vehicleBase);

    botAI->RemoveShapeshift();
    // Use HandleSpellClick instead of Unit::EnterVehicle to handle special vehicle script (ulduar)
    vehicleBase->HandleSpellClick(bot);

    if (!bot->IsOnVehicle(vehicleBase))
        return false;

    // dismount because bots can enter vehicle on mount
    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
    return true;
}

bool FlameLeviathanEnterVehicleAction::ShouldEnter(Unit* target)
{
    Vehicle* vehicleKit = target->GetVehicleKit();
    if (!vehicleKit)
        return false;

    bool isMelee = botAI->IsMelee(bot);
    bool allMain = AllMainVehiclesOnUse();
    bool inUse = vehicleKit->IsVehicleInUse();
    int32 entry = target->GetEntry();
    if (entry != NPC_SALVAGED_DEMOLISHER && entry != NPC_SALVAGED_SIEGE_ENGINE && entry != NPC_VEHICLE_CHOPPER)
        return false;
    // two phase enter (make all main vehicles in use -> next player enter)
    if (!allMain)
    {
        if (inUse)
            return false;
        if (entry != NPC_SALVAGED_DEMOLISHER && entry != NPC_SALVAGED_SIEGE_ENGINE)
            return false;
        if (entry == NPC_SALVAGED_DEMOLISHER && isMelee)
            return false;
        if (entry == NPC_SALVAGED_SIEGE_ENGINE && !isMelee)
            return false;
        return true;
    }

    if (!vehicleKit->GetAvailableSeatCount())
        return false;

    // do not enter useless seat
    if (entry == NPC_SALVAGED_SIEGE_ENGINE)
    {
        Unit* turret = vehicleKit->GetPassenger(7);
        if (!turret)
            return false;
        Vehicle* turretVehicle = turret->GetVehicleKit();
        if (!turretVehicle)
            return false;
        if (turretVehicle->IsVehicleInUse())
            return false;
        return true;
    }

    if (entry == NPC_SALVAGED_DEMOLISHER)
    {
        if (vehicleKit->GetPassenger(0))
        {
            Unit* target2 = vehicleKit->GetPassenger(1);
            if (!target2)
                return false;
            Vehicle* vehicle2 = target2->GetVehicleKit();
            if (!vehicle2)
                return false;
            if (vehicle2->GetPassenger(0))
                return false;
        }
        return true;
    }

    if (entry == NPC_VEHICLE_CHOPPER && vehicleKit->GetAvailableSeatCount() <= 1)
        return false;

    return true;
}

bool FlameLeviathanEnterVehicleAction::AllMainVehiclesOnUse()
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;
    int demolisher = 0;
    int siege = 0;
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* player = gref->GetSource();
        if (!player)
            continue;
        Unit* vehicleBase = player->GetVehicleBase();
        if (!vehicleBase)
            continue;
        if (vehicleBase->GetEntry() == NPC_SALVAGED_DEMOLISHER)
            ++demolisher;
        else if (vehicleBase->GetEntry() == NPC_SALVAGED_SIEGE_ENGINE)
            ++siege;
    }
    Difficulty diff = bot->GetRaidDifficulty();
    int maxC = (diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC) ? 2 : 5;
    return demolisher >= maxC && siege >= maxC;
}

bool RazorscaleAvoidDevouringFlameAction::Execute(Event event [[maybe_unused]])
{
    RazorscaleBossHelper razorscaleHelper(botAI);

    if (!razorscaleHelper.UpdateBossAI())
    {
        return false;
    }

    bool isMainTank = botAI->IsMainTank(bot);
    const float flameRadius = 3.5f;

    // Main tank moves further so they can hold adds away from flames, but only during the air phases
    const float safeDistanceMultiplier = (isMainTank && !razorscaleHelper.IsGroundPhase()) ? 2.3f : 1.0f;
    const float safeDistance = flameRadius * safeDistanceMultiplier;

    // Get the boss
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
    {
        return false;
    }

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    Unit* closestFlame = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    // Find the closest Devouring Flame
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DEVOURING_FLAME)
        {
            float distance = bot->GetDistance2d(unit);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestFlame = unit;
            }
        }
    }

    // Off tanks are following the main tank during grounded and should prioritise stacking
    if (razorscaleHelper.IsGroundPhase() && (botAI->IsTank(bot) && !botAI->IsMainTank(bot)))
    {
        return false;
    }

    // Handle movement from flames
    if (closestDistance < safeDistance)
    {
        return MoveAway(closestFlame, safeDistance);
    }
    return false;
}

bool RazorscaleAvoidDevouringFlameAction::isUseful()
{
    bool isMainTank = botAI->IsMainTank(bot);

    const float flameRadius = 3.5f;
    const float safeDistanceMultiplier = isMainTank ? 2.3f : 1.0f;
    const float safeDistance = flameRadius * safeDistanceMultiplier;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DEVOURING_FLAME)
        {
            float distance = bot->GetDistance2d(unit);
            if (distance < safeDistance)
            {
                return true;  // Bot is within the danger distance
            }
        }
    }

    return false;  // No nearby flames or bot is at a safe distance
}

bool RazorscaleAvoidSentinelAction::Execute(Event event [[maybe_unused]])
{
    bool isMainTank = botAI->IsMainTank(bot);
    bool isRanged = botAI->IsRanged(bot);
    const float radius = 8.0f;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    Unit* lowestHealthSentinel = nullptr;
    uint32 lowestHealth = UINT32_MAX;
    bool movedAway = false;

    // Iterate through all nearby NPCs
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL)
        {
            // Check if this sentinel has the lowest health
            if (unit->GetHealth() < lowestHealth)
            {
                lowestHealth = unit->GetHealth();
                lowestHealthSentinel = unit;
            }

            // Move away if ranged and too close
            if (isRanged && bot->GetDistance2d(unit) < radius)
            {
                movedAway = MoveAway(unit, radius) || movedAway;
            }
        }
    }

    // Check if the main tank is a human player
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
                if (group && lowestHealthSentinel)
                {
                    int8 skullIndex = 7;  // Skull
                    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

                    // If there's no skull set yet, or the skull is on a different target, set the sentinel
                    if (!currentSkullTarget || (lowestHealthSentinel->GetGUID() != currentSkullTarget))
                    {
                        group->SetTargetIcon(skullIndex, bot->GetGUID(), lowestHealthSentinel->GetGUID());
                    }
                }
                break;  // Stop after finding the first valid bot tank
            }
        }
    }
    else if (isMainTank && lowestHealthSentinel)  // Bot is the main tank
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            int8 skullIndex = 7;  // Skull
            ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

            // If there's no skull set yet, or the skull is on a different target, set the sentinel
            if (!currentSkullTarget || (lowestHealthSentinel->GetGUID() != currentSkullTarget))
            {
                group->SetTargetIcon(skullIndex, bot->GetGUID(), lowestHealthSentinel->GetGUID());
            }
        }
    }

    return movedAway;  // Return true if moved
}

bool RazorscaleAvoidSentinelAction::isUseful()
{
    bool isMainTank = botAI->IsMainTank(bot);
    Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
    Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;

    // If this bot is the main tank, it should always try to mark
    if (isMainTank)
    {
        return true;
    }

    // If the main tank is a human, check if this bot is one of the first three valid bot tanks
    if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a human player
    {
        for (int i = 0; i < 3; ++i)
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Bot is a valid tank
            {
                return true;  // This bot should assist with marking
            }
        }
    }

    bool isRanged = botAI->IsRanged(bot);
    const float radius = 8.0f;

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL)
        {
            if (isRanged && bot->GetDistance2d(unit) < radius)
            {
                return true;
            }
        }
    }

    return false;
}

bool RazorscaleAvoidWhirlwindAction::Execute(Event event [[maybe_unused]])
{
    if (botAI->IsTank(bot))
    {
        return false;
    }

    const float radius = 8.0f;
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL)
        {
            float currentDistance = bot->GetDistance2d(unit);
            if (currentDistance < radius)
            {
                return MoveAway(unit, radius);
            }
        }
    }
    return false;
}

bool RazorscaleAvoidWhirlwindAction::isUseful()
{
    // Tanks do not avoid Whirlwind
    if (botAI->IsTank(bot))
    {
        return false;
    }

    const float radius = 8.0f;
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto& npc : npcs)
    {
        Unit* unit = botAI->GetUnit(npc);
        if (unit && unit->GetEntry() == RazorscaleBossHelper::UNIT_DARK_RUNE_SENTINEL)
        {
            if (unit->HasAura(RazorscaleBossHelper::SPELL_SENTINEL_WHIRLWIND) ||
                unit->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            {
                if (bot->GetDistance2d(unit) < radius)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool RazorscaleIgnoreBossAction::isUseful()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
    {
        return false;
    }

    // Check if the boss is flying
    if (boss->GetPositionZ() >= RazorscaleBossHelper::RAZORSCALE_FLYING_Z_THRESHOLD)
    {
        // Check if the bot is outside the designated area
        if (bot->GetDistance2d(RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                               RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y) >
            RazorscaleBossHelper::RAZORSCALE_ARENA_RADIUS + 25.0f)
        {
            return true;  // Movement to the center is the top priority for all bots
        }

        if (!botAI->IsTank(bot))
        {
            return false;
        }

        Group* group = bot->GetGroup();
        if (!group)
        {
            return false;
        }

        // Check if the boss is already set as the moon marker
        int8 moonIndex = 4;  // Moon marker index
        ObjectGuid currentMoonTarget = group->GetTargetIcon(moonIndex);
        if (currentMoonTarget == boss->GetGUID())
        {
            return false;  // Moon marker is already correctly set, no further action needed
        }

        // Proceed to tank-specific logic
        Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
        Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;

        // If this bot is the main tank, it needs to set the moon marker
        if (mainTankUnit == bot)
        {
            return true;
        }

        // If the main tank is a human, check if this bot is the lowest-indexed bot tank
        if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a human player
        {
            for (int i = 0; i < 3; ++i)  // Only iterate through the first 3 indexes
            {
                if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Valid bot tank
                {
                    return true;  // This bot should assign the marker
                }
            }
        }
    }

    return false;
}

bool RazorscaleIgnoreBossAction::Execute(Event event [[maybe_unused]])
{
    if (!bot)
    {
        return false;
    }

    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss)
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    // Check if the bot is outside the designated area and move inside first
    if (bot->GetDistance2d(RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                           RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y) >
        RazorscaleBossHelper::RAZORSCALE_ARENA_RADIUS + 25.0f)
    {
        return MoveInside(ULDUAR_MAP_ID, RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                          RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y, bot->GetPositionZ(),
                          RazorscaleBossHelper::RAZORSCALE_ARENA_RADIUS - 10.0f, MovementPriority::MOVEMENT_NORMAL);
    }

    if (!botAI->IsTank(bot))
    {
        return false;
    }

    // Check if the boss is already set as the moon marker
    int8 moonIndex = 4;
    ObjectGuid currentMoonTarget = group->GetTargetIcon(moonIndex);
    if (currentMoonTarget == boss->GetGUID())
    {
        return false;  // Moon marker is already correctly set
    }

    // Get the main tank and determine role
    Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
    Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;

    // If the main tank is a human, assign the moon marker using the lowest-indexed bot tank
    if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a real player
    {
        for (int i = 0; i < 3; ++i)  // Only iterate through the first 3 indexes
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Bot is a valid tank
            {
                group->SetTargetIcon(moonIndex, bot->GetGUID(), boss->GetGUID());
                SetNextMovementDelay(1000);
                break;  // Assign the moon marker and stop
            }
        }
    }
    else if (mainTankUnit == bot)  // If this bot is the main tank
    {
        group->SetTargetIcon(moonIndex, bot->GetGUID(), boss->GetGUID());
        SetNextMovementDelay(1000);
    }

    // Tanks move inside the arena
    return MoveInside(ULDUAR_MAP_ID, RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                      RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y, bot->GetPositionZ(),
                      RazorscaleBossHelper::RAZORSCALE_ARENA_RADIUS - 10.0f, MovementPriority::MOVEMENT_NORMAL);
}

bool RazorscaleGroundedAction::isUseful()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss || !boss->IsAlive() || boss->GetPositionZ() > RazorscaleBossHelper::RAZORSCALE_FLYING_Z_THRESHOLD)
    {
        return false;
    }

    if (botAI->IsMainTank(bot))
    {
        Group* group = bot->GetGroup();
        if (!group)
            return false;

        // Check if the boss is marked with Moon
        int8 moonIndex = 4;
        ObjectGuid currentMoonTarget = group->GetTargetIcon(moonIndex);

        // Useful only if the boss is currently marked with Moon
        return currentMoonTarget == boss->GetGUID();
    }

    if (botAI->IsTank(bot) && !botAI->IsMainTank(bot))
    {
        Group* group = bot->GetGroup();
        if (!group)
            return false;

        // Find the main tank
        Player* mainTank = nullptr;
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            Player* member = ref->GetSource();
            if (member && botAI->IsMainTank(member))
            {
                mainTank = member;
                break;
            }
        }

        if (mainTank)
        {
            constexpr float maxDistance = 2.0f;
            float distanceToMainTank = bot->GetDistance2d(mainTank);
            return (distanceToMainTank > maxDistance);
        }
    }

    if (botAI->IsMelee(bot))
    {
        return false;
    }

    if (botAI->IsRanged(bot))
    {
        constexpr float landingX = 588.0f;
        constexpr float landingY = -166.0f;
        constexpr float landingZ = 391.1f;

        float bossX = boss->GetPositionX();
        float bossY = boss->GetPositionY();
        float bossZ = boss->GetPositionZ();

        bool atInitialLandingPosition =
            (fabs(bossX - landingX) < 2.0f) && (fabs(bossY - landingY) < 2.0f) && (fabs(bossZ - landingZ) < 1.0f);

        constexpr float initialLandingRadius = 14.0f;
        constexpr float normalRadius = 12.0f;

        if (atInitialLandingPosition)
        {
            float adjustedCenterX = RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X;
            float adjustedCenterY = RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y - 20.0f;

            float distanceToAdjustedCenter = bot->GetDistance2d(adjustedCenterX, adjustedCenterY);
            return distanceToAdjustedCenter > initialLandingRadius;
        }

        float distanceToCenter = bot->GetDistance2d(RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                                                    RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y);
        return distanceToCenter > normalRadius;
    }

    return false;
}

bool RazorscaleGroundedAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "razorscale");
    if (!boss || !boss->IsAlive() || boss->GetPositionZ() > RazorscaleBossHelper::RAZORSCALE_FLYING_Z_THRESHOLD)
        return false;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
    Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;

    if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a human player
    {
        // Iterate through the first 3 bot tanks to handle the moon marker
        for (int i = 0; i < 3; ++i)
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Bot is a valid tank
            {
                int8 moonIndex = 4;
                ObjectGuid currentMoonTarget = group->GetTargetIcon(moonIndex);

                // If the moon marker is set to the boss, reset it
                if (currentMoonTarget == boss->GetGUID())
                {
                    group->SetTargetIcon(moonIndex, bot->GetGUID(), ObjectGuid::Empty);
                    SetNextMovementDelay(1000);
                    return true;
                }
            }
        }
    }
    else if (botAI->IsMainTank(bot))  // Bot is the main tank
    {
        int8 moonIndex = 4;
        ObjectGuid currentMoonTarget = group->GetTargetIcon(moonIndex);

        // If the moon marker is set to the boss, reset it
        if (currentMoonTarget == boss->GetGUID())
        {
            group->SetTargetIcon(moonIndex, bot->GetGUID(), ObjectGuid::Empty);
            SetNextMovementDelay(1000);
            return true;
        }
    }

    if (mainTank && (botAI->IsTank(bot) && !botAI->IsMainTank(bot)))
    {
        constexpr float followDistance = 2.0f;
        return MoveNear(mainTank, followDistance, MovementPriority::MOVEMENT_COMBAT);
    }

    if (botAI->IsRanged(bot))
    {
        constexpr float landingX = 588.0f;
        constexpr float landingY = -166.0f;
        constexpr float landingZ = 391.1f;

        float bossX = boss->GetPositionX();
        float bossY = boss->GetPositionY();
        float bossZ = boss->GetPositionZ();

        bool atInitialLandingPosition =
            (fabs(bossX - landingX) < 2.0f) && (fabs(bossY - landingY) < 2.0f) && (fabs(bossZ - landingZ) < 1.0f);

        if (atInitialLandingPosition)
        {
            // If at the initial landing position, use 12-yard radius with a
            // 20 yard offset on the Y axis so everyone is behind the boss
            return MoveInside(ULDUAR_MAP_ID, RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                              RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y - 20.0f, bot->GetPositionZ(),
                              RazorscaleBossHelper::RAZORSCALE_ARENA_RADIUS - 12.0f, MovementPriority::MOVEMENT_COMBAT);
        }

        // Otherwise, move inside a 12-yard radius around the arena center
        return MoveInside(ULDUAR_MAP_ID, RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_X,
                          RazorscaleBossHelper::RAZORSCALE_ARENA_CENTER_Y, bot->GetPositionZ(), 12.0f,
                          MovementPriority::MOVEMENT_COMBAT);
    }
    return false;
}

bool RazorscaleHarpoonAction::Execute(Event event [[maybe_unused]])
{
    if (!bot)
    {
        return false;
    }

    RazorscaleBossHelper razorscaleHelper(botAI);

    // Update the boss AI context
    if (!razorscaleHelper.UpdateBossAI())
        return false;

    Unit* boss = razorscaleHelper.GetBoss();
    if (!boss || !boss->IsAlive())
        return false;

    // Retrieve harpoon data from the helper
    const std::vector<RazorscaleBossHelper::HarpoonData>& harpoonData = razorscaleHelper.GetHarpoonData();

    GameObject* closestHarpoon = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    // Find the nearest harpoon that hasn't been fired and is not on cooldown
    for (const auto& harpoon : harpoonData)
    {
        if (razorscaleHelper.IsHarpoonFired(harpoon.chainSpellId))
            continue;

        if (GameObject* harpoonGO = bot->FindNearestGameObject(harpoon.gameObjectEntry, 200.0f))
        {
            if (RazorscaleBossHelper::IsHarpoonReady(harpoonGO))
            {
                float distance = bot->GetDistance2d(harpoonGO);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closestHarpoon = harpoonGO;
                }
            }
        }
    }

    if (!closestHarpoon)
        return false;

    // Find the nearest ranged DPS (not a healer) to the harpoon
    Player* closestRangedDPS = nullptr;
    minDistance = std::numeric_limits<float>::max();
    GuidVector groupBots = AI_VALUE(GuidVector, "group members");

    for (auto& guid : groupBots)
    {
        Player* member = ObjectAccessor::FindPlayer(guid);
        if (member && member->IsAlive() && botAI->IsRanged(member) && botAI->IsDps(member) && !botAI->IsHeal(member))
        {
            float distance = member->GetDistance2d(closestHarpoon);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestRangedDPS = member;
            }
        }
    }

    // Only proceed if this bot is the closest ranged DPS
    if (closestRangedDPS != bot)
        return false;

    float botDist = bot->GetDistance(closestHarpoon);
    if (botDist > INTERACTION_DISTANCE - 1.0f)
    {
        return MoveTo(bot->GetMapId(), closestHarpoon->GetPositionX(), closestHarpoon->GetPositionY(),
                      closestHarpoon->GetPositionZ());
    }

    SetNextMovementDelay(1000);

    // Interact with the harpoon
    {
        WorldPacket usePacket(CMSG_GAMEOBJ_USE);
        usePacket << closestHarpoon->GetGUID();
        bot->GetSession()->HandleGameObjectUseOpcode(usePacket);
    }

    {
        WorldPacket reportPacket(CMSG_GAMEOBJ_REPORT_USE);
        reportPacket << closestHarpoon->GetGUID();
        bot->GetSession()->HandleGameobjectReportUse(reportPacket);
    }

    RazorscaleBossHelper::SetHarpoonOnCooldown(closestHarpoon);

    return true;
}

bool RazorscaleHarpoonAction::isUseful()
{
    RazorscaleBossHelper razorscaleHelper(botAI);

    // Update the boss AI context to ensure we have the latest info
    if (!razorscaleHelper.UpdateBossAI())
        return false;

    Unit* boss = razorscaleHelper.GetBoss();
    if (!boss || !boss->IsAlive())
        return false;

    const std::vector<RazorscaleBossHelper::HarpoonData>& harpoonData = razorscaleHelper.GetHarpoonData();

    for (const auto& harpoon : harpoonData)
    {
        if (razorscaleHelper.IsHarpoonFired(harpoon.chainSpellId))
            continue;

        if (GameObject* harpoonGO = bot->FindNearestGameObject(harpoon.gameObjectEntry, 200.0f))
        {
            if (RazorscaleBossHelper::IsHarpoonReady(harpoonGO))
            {
                // Check if this bot is a ranged DPS (not a healer)
                if (botAI->IsRanged(bot) && botAI->IsDps(bot) && !botAI->IsHeal(bot))
                    return true;
            }
        }
    }

    return false;
}

bool RazorscaleFuseArmorAction::isUseful()
{
    // If this bot cannot tank at all, no need to do anything
    if (!botAI->IsTank(bot))
        return false;

    // If this bot is the main tank AND has Fuse Armor at the threshold, return true immediately
    if (botAI->IsMainTank(bot))
    {
        Aura* fuseArmor = bot->GetAura(RazorscaleBossHelper::SPELL_FUSEARMOR);
        if (fuseArmor && fuseArmor->GetStackAmount() >= RazorscaleBossHelper::FUSEARMOR_THRESHOLD)
            return true;
    }

    // Otherwise, check if there's any other main tank with high Fuse Armor
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member)
            continue;

        if (botAI->IsMainTank(member) && member != bot)
        {
            Aura* fuseArmor = member->GetAura(RazorscaleBossHelper::SPELL_FUSEARMOR);
            if (fuseArmor && fuseArmor->GetStackAmount() >= RazorscaleBossHelper::FUSEARMOR_THRESHOLD)
            {
                // There is another main tank with high Fuse Armor
                return true;
            }
        }
    }

    return false;
}

bool RazorscaleFuseArmorAction::Execute(Event event [[maybe_unused]])
{
    // We already know from isUseful() that:
    //  1) This bot can tank, AND
    //  2) There is at least one main tank (possibly this bot) with Fuse Armor >= threshold.

    RazorscaleBossHelper bossHelper(botAI);

    // Attempt to reassign the roles based on health/Fuse Armor debuff
    bossHelper.AssignRolesBasedOnHealth();
    return true;
}

bool IronAssemblyLightningTendrilsAction::isUseful()
{
    IronAssemblyLightningTendrilsTrigger ironAssemblyLightningTendrilsTrigger(botAI);
    return ironAssemblyLightningTendrilsTrigger.IsActive();
}

bool IronAssemblyLightningTendrilsAction::Execute(Event event [[maybe_unused]])
{
    const float radius = 18.0f + 10.0f;  // 18 yards + 10 yards for safety

    Unit* boss = AI_VALUE2(Unit*, "find target", "stormcaller brundir");
    if (!boss)
        return false;

    float currentDistance = bot->GetDistance2d(boss);

    if (currentDistance < radius)
    {
        return MoveAway(boss, radius - currentDistance);
    }

    return false;
}

bool IronAssemblyOverloadAction::isUseful()
{
    IronAssemblyOverloadTrigger ironAssemblyOverloadTrigger(botAI);
    return ironAssemblyOverloadTrigger.IsActive();
}

bool IronAssemblyOverloadAction::Execute(Event event [[maybe_unused]])
{
    const float radius = 20.0f + 5.0f;  // 20 yards + 5 yards for safety

    Unit* boss = AI_VALUE2(Unit*, "find target", "stormcaller brundir");
    if (!boss)
        return false;

    float currentDistance = bot->GetDistance2d(boss);

    if (currentDistance < radius)
    {
        return MoveAway(boss, radius - currentDistance);
    }

    return false;
}

bool IronAssemblyRuneOfPowerAction::isUseful()
{
    IronAssemblyRuneOfPowerTrigger ironAssemblyRuneOfPowerTrigger(botAI);
    return ironAssemblyRuneOfPowerTrigger.IsActive();
}

bool IronAssemblyRuneOfPowerAction::Execute(Event event [[maybe_unused]])
{
    Unit* target = botAI->GetUnit(bot->GetTarget());
    if (!target || !target->IsAlive())
        return false;

    return MoveAway(target, 10.0f, true);
}

bool KologarnMarkDpsTargetAction::isUseful()
{
    KologarnMarkDpsTargetTrigger kologarnMarkDpsTargetTrigger(botAI);
    return kologarnMarkDpsTargetTrigger.IsActive();
}

bool KologarnMarkDpsTargetAction::Execute(Event event [[maybe_unused]])
{
    Unit* targetToMark = nullptr;
    Unit* additionalTargetToMark = nullptr;
    Unit* targetToCcMark = nullptr;
    int8 skullIndex = 7;  // Skull
    int8 crossIndex = 6;  // Cross
    int8 moonIndex = 4;   // Moon

    Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");
    if (!boss || !boss->IsAlive())
        return false;

    // Check that there is rubble to mark
    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target)
            continue;

        uint32 creatureId [[maybe_unused]] = target->GetEntry();
        if (target->GetEntry() == NPC_RUBBLE && target->IsAlive())
        {
            targetToMark = target;
            additionalTargetToMark = boss;
        }
    }

    if (!targetToMark)
    {
        Unit* rightArm = AI_VALUE2(Unit*, "find target", "right arm");
        if (rightArm && rightArm->IsAlive())
        {
            targetToMark = rightArm;
            additionalTargetToMark = boss;
        }
    }

    if (!targetToMark)
    {
        Unit* boss = AI_VALUE2(Unit*, "find target", "kologarn");
        if (boss && boss->IsAlive())
        {
            targetToMark = boss;
        }
    }

    if (!targetToMark)
    {
        return false;  // No target to mark
    }

    Unit* leftArm = AI_VALUE2(Unit*, "find target", "left arm");
    if (leftArm && leftArm->IsAlive())
    {
        targetToCcMark = leftArm;
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
                if (group)
                {
                    group->SetTargetIcon(skullIndex, bot->GetGUID(), targetToMark->GetGUID());
                    if (targetToCcMark)
                    {
                        group->SetTargetIcon(moonIndex, bot->GetGUID(), targetToCcMark->GetGUID());
                    }
                    if (additionalTargetToMark)
                    {
                        group->SetTargetIcon(crossIndex, bot->GetGUID(), additionalTargetToMark->GetGUID());
                    }

                    return true;
                }
                break;  // Stop after finding the first valid bot tank
            }
        }
    }
    else if (isMainTank && bot->IsAlive())  // Bot is the main tank
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            group->SetTargetIcon(skullIndex, bot->GetGUID(), targetToMark->GetGUID());
            if (targetToCcMark)
            {
                group->SetTargetIcon(moonIndex, bot->GetGUID(), targetToCcMark->GetGUID());
            }
            if (additionalTargetToMark)
            {
                group->SetTargetIcon(crossIndex, bot->GetGUID(), additionalTargetToMark->GetGUID());
            }
            return true;
        }
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot) && bot->IsAlive())  // Bot is a valid tank
            {
                Group* group = bot->GetGroup();
                if (group)
                {
                    group->SetTargetIcon(skullIndex, bot->GetGUID(), targetToMark->GetGUID());
                    if (targetToCcMark)
                    {
                        group->SetTargetIcon(moonIndex, bot->GetGUID(), targetToCcMark->GetGUID());
                    }
                    if (additionalTargetToMark)
                    {
                        group->SetTargetIcon(crossIndex, bot->GetGUID(), additionalTargetToMark->GetGUID());
                    }
                    return true;
                }
                break;  // Stop after finding the first valid bot tank
            }
        }
    }

    return false;
}

bool KologarnFallFromFloorAction::Execute(Event event [[maybe_unused]])
{
    return bot->TeleportTo(bot->GetMapId(), ULDUAR_KOLOGARN_RESTORE_POSITION.GetPositionX(),
                           ULDUAR_KOLOGARN_RESTORE_POSITION.GetPositionY(),
                           ULDUAR_KOLOGARN_RESTORE_POSITION.GetPositionZ(),
                           ULDUAR_KOLOGARN_RESTORE_POSITION.GetOrientation());
}

bool KologarnFallFromFloorAction::isUseful()
{
    KologarnFallFromFloorTrigger kologarnFallFromFloorTrigger(botAI);
    return kologarnFallFromFloorTrigger.IsActive();
}

bool KologarnRubbleSlowdownAction::Execute(Event event [[maybe_unused]])
{
    Group* group = bot->GetGroup();
    if (!group)
        return false;

    int8 skullIndex = 7;
    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);
    Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);
    if (!currentSkullUnit || !currentSkullUnit->IsAlive() || currentSkullUnit->GetEntry() != NPC_RUBBLE)
        return false;

    return botAI->CastSpell("frost trap", currentSkullUnit);
}

bool KologarnEyebeamAction::Execute(Event event [[maybe_unused]])
{
    float distanceToLeftPoint = bot->GetExactDist(ULDUAR_KOLOGARN_EYEBEAM_LEFT_POSITION);
    float distanceToRightPoint = bot->GetExactDist(ULDUAR_KOLOGARN_EYEBEAM_RIGHT_POSITION);

    bool runToLeftSide;
    if (!distanceToLeftPoint)
    {
        runToLeftSide = true;
    }
    else if (!distanceToRightPoint)
    {
        runToLeftSide = false;
    }
    else
    {
        runToLeftSide = distanceToRightPoint > distanceToLeftPoint;
    }

    bool teleportedToPoint;
    KologarnEyebeamTrigger kologarnEyebeamTrigger(botAI);
    if (runToLeftSide)
    {
        teleportedToPoint = bot->TeleportTo(bot->GetMapId(), ULDUAR_KOLOGARN_EYEBEAM_LEFT_POSITION.GetPositionX(),
                                            ULDUAR_KOLOGARN_EYEBEAM_LEFT_POSITION.GetPositionY(),
                                            ULDUAR_KOLOGARN_EYEBEAM_LEFT_POSITION.GetPositionZ(),
                                            ULDUAR_KOLOGARN_EYEBEAM_LEFT_POSITION.GetOrientation());
    }
    else
    {
        teleportedToPoint = bot->TeleportTo(bot->GetMapId(), ULDUAR_KOLOGARN_EYEBEAM_RIGHT_POSITION.GetPositionX(),
                                            ULDUAR_KOLOGARN_EYEBEAM_RIGHT_POSITION.GetPositionY(),
                                            ULDUAR_KOLOGARN_EYEBEAM_RIGHT_POSITION.GetPositionZ(),
                                            ULDUAR_KOLOGARN_EYEBEAM_RIGHT_POSITION.GetOrientation());
    }

    if (teleportedToPoint)
        SetNextMovementDelay(5000);

    return teleportedToPoint;
}

bool KologarnEyebeamAction::isUseful()
{
    KologarnEyebeamTrigger kologarnEyebeamTrigger(botAI);
    if (!kologarnEyebeamTrigger.IsActive())
    {
        return false;
    }

    return botAI->HasCheat(BotCheatMask::raid);
}

bool KologarnRtiTargetAction::isUseful()
{
    KologarnRtiTargetTrigger kologarnRtiTargetTrigger(botAI);
    return kologarnRtiTargetTrigger.IsActive();
}

bool KologarnRtiTargetAction::Execute(Event event [[maybe_unused]])
{
    if (botAI->IsMainTank(bot) || botAI->IsAssistTankOfIndex(bot, 0))
    {
        context->GetValue<std::string>("rti")->Set("cross");
        return true;
    }

    context->GetValue<std::string>("rti")->Set("skull");
    return true;
}

bool KologarnCrunchArmorAction::isUseful()
{
    KologarnCrunchArmorTrigger kologarnCrunchArmorTrigger(botAI);
    if (!kologarnCrunchArmorTrigger.IsActive())
    {
        return false;
    }

    return botAI->HasCheat(BotCheatMask::raid);
}

bool KologarnCrunchArmorAction::Execute(Event event [[maybe_unused]])
{
    bot->RemoveAura(SPELL_CRUNCH_ARMOR);
    return true;
}

bool AuriayaFallFromFloorAction::Execute(Event event [[maybe_unused]])
{
    Player* master = botAI->GetMaster();

    if (!master)
        return false;

    return bot->TeleportTo(bot->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(),
                           master->GetOrientation());
}

bool AuriayaFallFromFloorAction::isUseful()
{
    AuriayaFallFromFloorTrigger auriayaFallFromFloorTrigger(botAI);
    return auriayaFallFromFloorTrigger.IsActive();
}

bool HodirMoveSnowpackedIcicleAction::isUseful()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "hodir");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Check if boss is casting Flash Freeze
    if (!boss->HasUnitState(UNIT_STATE_CASTING) || !boss->FindCurrentSpellBySpellId(SPELL_FLASH_FREEZE))
    {
        return false;
    }

    // Find the nearest Snowpacked Icicle Target
    Creature* target = bot->FindNearestCreature(NPC_SNOWPACKED_ICICLE, 100.0f);
    if (!target)
        return false;

    // Check that bot is stacked on Snowpacked Icicle
    if (bot->GetDistance2d(target->GetPositionX(), target->GetPositionY()) <= 5.0f)
    {
        return false;
    }

    return true;
}

bool HodirMoveSnowpackedIcicleAction::Execute(Event event [[maybe_unused]])
{
    Creature* target = bot->FindNearestCreature(NPC_SNOWPACKED_ICICLE, 100.0f);
    if (!target)
        return false;

    return MoveTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), false,
                  false, false, true, MovementPriority::MOVEMENT_NORMAL, true);
}

bool HodirBitingColdJumpAction::Execute(Event event [[maybe_unused]])
{
    bot->RemoveAurasDueToSpell(SPELL_BITING_COLD_PLAYER_AURA);

    return true;

    // Backup when the overall strategy without cheat will be more vialable

    // int mapId = bot->GetMap()->GetId();
    // int x = bot->GetPositionX();
    // int y = bot->GetPositionY();
    // int z = bot->GetPositionZ() + 3.98f;
    // float speed = 7.96f;

    // UpdateMovementState();
    // if (!IsMovingAllowed(mapId, x, y, z))
    //{
    //     return false;
    // }
    // MovementPriority priority;
    // if (IsWaitingForLastMove(priority))
    //{
    //     return false;
    // }

    // MotionMaster& mm = *bot->GetMotionMaster();
    // mm.Clear();
    // mm.MoveJump(x, y, z, speed, speed, 1, AI_VALUE(Unit*, "current target"));
    // mm.MoveFall(0, true);
    // AI_VALUE(LastMovement&, "last movement").Set(mapId, x, y, z, bot->GetOrientation(), 1000, priority);

    // return true;
}

bool HodirBitingColdJumpAction::isUseful()
{
    return botAI->HasCheat(BotCheatMask::raid);
}

bool FreyaMoveAwayNatureBombAction::isUseful()
{
    // Check boss and it is alive
    Unit* boss = AI_VALUE2(Unit*, "find target", "freya");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Find the nearest Nature Bomb
    GameObject* target = bot->FindNearestGameObject(GOBJECT_NATURE_BOMB, 12.0f);
    if (!target)
        return false;

    return true;
}

bool FreyaMoveAwayNatureBombAction::Execute(Event event [[maybe_unused]])
{
    GameObject* target = bot->FindNearestGameObject(GOBJECT_NATURE_BOMB, 12.0f);
    if (!target)
        return false;

    return FleePosition(target->GetPosition(), 13.0f);
}

bool FreyaMarkDpsTargetAction::isUseful()
{
    FreyaMarkDpsTargetTrigger freyaMarkDpsTargetTrigger(botAI);
    return freyaMarkDpsTargetTrigger.IsActive();
}

bool FreyaMarkDpsTargetAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "freya");
    if (!boss || !boss->IsAlive())
        return false;

    Unit* targetToMark = nullptr;

    // Check which adds is up
    Unit* eonarsGift = nullptr;
    Unit* ancientConservator = nullptr;
    Unit* snaplasher = nullptr;
    Unit* ancientWaterSpirit = nullptr;
    Unit* stormLasher = nullptr;
    Unit* firstDetonatingLasher = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_EONARS_GIFT)
        {
            eonarsGift = target;
        }
        else if (target->GetEntry() == NPC_ANCIENT_CONSERVATOR)
        {
            ancientConservator = target;
        }
        else if (target->GetEntry() == NPC_SNAPLASHER)
        {
            snaplasher = target;
        }
        else if (target->GetEntry() == NPC_ANCIENT_WATER_SPIRIT)
        {
            ancientWaterSpirit = target;
        }
        else if (target->GetEntry() == NPC_STORM_LASHER)
        {
            stormLasher = target;
        }
        else if (target->GetEntry() == NPC_DETONATING_LASHER && !firstDetonatingLasher)
        {
            firstDetonatingLasher = target;
        }
    }

    // Check that eonars gift is need to be mark
    if (eonarsGift)
    {
        targetToMark = eonarsGift;
    }

    // Check that ancient conservator is need to be mark
    if (ancientConservator && !targetToMark)
    {
        targetToMark = ancientConservator;
    }

    // Check that trio of adds is need to be mark
    if ((snaplasher || ancientWaterSpirit || stormLasher) && !targetToMark)
    {
        Unit* highestHealthUnit = nullptr;
        uint32 highestHealth = 0;

        if (snaplasher && snaplasher->GetHealth() > highestHealth)
        {
            highestHealth = snaplasher->GetHealth();
            highestHealthUnit = snaplasher;
        }
        if (ancientWaterSpirit && ancientWaterSpirit->GetHealth() > highestHealth)
        {
            highestHealth = ancientWaterSpirit->GetHealth();
            highestHealthUnit = ancientWaterSpirit;
        }
        if (stormLasher && stormLasher->GetHealth() > highestHealth)
        {
            highestHealthUnit = stormLasher;
        }

        // If the highest health unit is not already marked, mark it
        if (highestHealthUnit)
        {
            targetToMark = highestHealthUnit;
        }
    }

    // Check that detonating lasher is need to be mark
    if (firstDetonatingLasher && !targetToMark)
    {
        targetToMark = firstDetonatingLasher;
    }

    if (!targetToMark)
    {
        return false;  // No target to mark
    }

    bool isMainTank = botAI->IsMainTank(bot);
    Unit* mainTankUnit = AI_VALUE(Unit*, "main tank");
    Player* mainTank = mainTankUnit ? mainTankUnit->ToPlayer() : nullptr;
    int8 squareIndex = 5;  // Square
    int8 skullIndex = 7;   // Skull

    if (mainTank && !GET_SHADOW_AI(mainTank))  // Main tank is a real player
    {
        // Iterate through the first 3 bot tanks to assign the Skull marker
        for (int i = 0; i < 3; ++i)
        {
            if (botAI->IsAssistTankOfIndex(bot, i) && GET_SHADOW_AI(bot))  // Bot is a valid tank
            {
                Group* group = bot->GetGroup();
                if (group)
                {
                    ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

                    if (!currentSkullTarget || (targetToMark->GetGUID() != currentSkullTarget))
                    {
                        group->SetTargetIcon(skullIndex, bot->GetGUID(), targetToMark->GetGUID());
                        group->SetTargetIcon(squareIndex, bot->GetGUID(), boss->GetGUID());
                        return true;
                    }
                }
                break;
            }
        }
    }
    else if (isMainTank)  // Bot is the main tank
    {
        Group* group = bot->GetGroup();
        if (group)
        {
            ObjectGuid currentSkullTarget = group->GetTargetIcon(skullIndex);

            if (!currentSkullTarget || (targetToMark->GetGUID() != currentSkullTarget))
            {
                group->SetTargetIcon(skullIndex, bot->GetGUID(), targetToMark->GetGUID());
                group->SetTargetIcon(squareIndex, bot->GetGUID(), boss->GetGUID());
                botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("square");
                return true;
            }
        }
    }

    return false;
}

bool FreyaMoveToHealingSporeAction::isUseful()
{
    FreyaMoveToHealingSporeTrigger freyaMoveToHealingSporeTrigger(botAI);
    return freyaMoveToHealingSporeTrigger.IsActive();
}

bool FreyaMoveToHealingSporeAction::Execute(Event event [[maybe_unused]])
{
    GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
    Creature* nearestSpore = nullptr;
    float nearestDistance = std::numeric_limits<float>::max();

    for (auto guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit)
            continue;

        // Check if this unit is a healthy spore and alive
        if (unit->GetEntry() != NPC_HEALTHY_SPORE || !unit->IsAlive())
            continue;

        float distance = bot->GetDistance2d(unit);
        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestSpore = static_cast<Creature*>(unit);
        }
    }

    if (!nearestSpore)
        return false;

    return MoveTo(nearestSpore->GetMapId(), nearestSpore->GetPositionX(), nearestSpore->GetPositionY(),
                  nearestSpore->GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_COMBAT);
}

bool ThorimUnbalancingStrikeAction::isUseful()
{
    ThorimUnbalancingStrikeTrigger thorimUnbalancingStrikeTrigger(botAI);
    if (!thorimUnbalancingStrikeTrigger.IsActive())
    {
        return false;
    }

    return botAI->HasCheat(BotCheatMask::raid);
}

bool ThorimUnbalancingStrikeAction::Execute(Event event [[maybe_unused]])
{
    bot->RemoveAura(SPELL_UNBALANCING_STRIKE);
    return true;
}

bool ThorimMarkDpsTargetAction::isUseful()
{
    ThorimMarkDpsTargetTrigger thorimMarkDpsTargetTrigger(botAI);
    return thorimMarkDpsTargetTrigger.IsActive();
}

bool ThorimMarkDpsTargetAction::Execute(Event event [[maybe_unused]])
{
    Unit* targetToMark = nullptr;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    ObjectGuid currentMoonTarget = group->GetTargetIcon(RtiTargetValue::moonIndex);
    Unit* currentMoonUnit = botAI->GetUnit(currentMoonTarget);
    Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");
    if (!currentMoonUnit && boss && boss->IsAlive() && boss->GetPositionZ() > ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
    {
        group->SetTargetIcon(RtiTargetValue::moonIndex, bot->GetGUID(), boss->GetGUID());
    }

    if (currentMoonUnit && boss && currentMoonUnit->GetEntry() == boss->GetEntry() &&
        boss->GetPositionZ() < ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
    {
        group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), boss->GetGUID());
        return true;
    }

    if (botAI->IsMainTank(bot))
    {
        ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
        Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);
        if (currentSkullUnit && !currentSkullUnit->IsAlive())
        {
            currentSkullUnit = nullptr;
        }

        Unit* acolyte = AI_VALUE2(Unit*, "find target", "dark rune acolyte");
        Unit* evoker = AI_VALUE2(Unit*, "find target", "dark rune evoker");

        if (acolyte && acolyte->IsAlive() && bot->GetDistance(acolyte) < 50.0f &&
            (!currentSkullUnit || currentSkullUnit->GetEntry() != acolyte->GetEntry()))
            targetToMark = acolyte;
        else if (evoker && evoker->IsAlive() && bot->GetDistance(evoker) < 50.0f &&
                 (!currentSkullUnit || currentSkullUnit->GetEntry() != evoker->GetEntry()))
            targetToMark = evoker;
        else
            return false;
    }
    else if (botAI->IsAssistTankOfIndex(bot, 0))
    {
        ObjectGuid currentCrossTarget = group->GetTargetIcon(RtiTargetValue::crossIndex);
        Unit* currentCrossUnit = botAI->GetUnit(currentCrossTarget);
        if (currentCrossUnit && !currentCrossUnit->IsAlive())
        {
            currentCrossUnit = nullptr;
        }

        Unit* acolyte = AI_VALUE2(Unit*, "find target", "dark rune acolyte");
        Unit* runicColossus = AI_VALUE2(Unit*, "find target", "runic colossus");
        Unit* ancientRuneGiant = AI_VALUE2(Unit*, "find target", "ancient rune giant");
        Unit* ironHonorGuard = AI_VALUE2(Unit*, "find target", "iron ring guard");
        Unit* ironRingGuard = AI_VALUE2(Unit*, "find target", "iron honor guard");

        if (acolyte && acolyte->IsAlive() && (!currentCrossUnit || currentCrossUnit->GetEntry() != acolyte->GetEntry()))
            targetToMark = acolyte;
        else if (runicColossus && runicColossus->IsAlive() &&
                 (!currentCrossUnit || currentCrossUnit->GetEntry() != runicColossus->GetEntry()))
            targetToMark = runicColossus;
        else if (ancientRuneGiant && ancientRuneGiant->IsAlive() &&
                 (!currentCrossUnit || currentCrossUnit->GetEntry() != ancientRuneGiant->GetEntry()))
            targetToMark = ancientRuneGiant;
        else if (ironHonorGuard && ironHonorGuard->IsAlive() &&
                 (!currentCrossUnit || currentCrossUnit->GetEntry() != ironHonorGuard->GetEntry()))
            targetToMark = ironHonorGuard;
        else if (ironRingGuard && ironRingGuard->IsAlive() &&
                 (!currentCrossUnit || currentCrossUnit->GetEntry() != ironRingGuard->GetEntry()))
            targetToMark = ironRingGuard;
        else
            return false;
    }

    if (!targetToMark)
        return false;  // No target to mark

    if (botAI->IsMainTank(bot))
    {
        group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), targetToMark->GetGUID());
        return true;
    }

    if (botAI->IsAssistTankOfIndex(bot, 0))
    {
        group->SetTargetIcon(RtiTargetValue::crossIndex, bot->GetGUID(), targetToMark->GetGUID());
        return true;
    }

    return false;
}

bool ThorimArenaPositioningAction::isUseful()
{
    ThorimArenaPositioningTrigger thorimArenaPositioningTrigger(botAI);
    return thorimArenaPositioningTrigger.IsActive();
}

bool ThorimArenaPositioningAction::Execute(Event event [[maybe_unused]])
{
    FollowMasterStrategy followMasterStrategy(botAI);

    MoveTo(bot->GetMapId(), ULDUAR_THORIM_NEAR_ARENA_CENTER.GetPositionX(),
           ULDUAR_THORIM_NEAR_ARENA_CENTER.GetPositionY(), ULDUAR_THORIM_NEAR_ARENA_CENTER.GetPositionZ(), false, false,
           false, true, MovementPriority::MOVEMENT_COMBAT, true);

    if (botAI->HasStrategy(followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT))
    {
        botAI->ChangeStrategy(REMOVE_STRATEGY_CHAR + followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT);
    }

    return true;
}

bool ThorimGauntletPositioningAction::isUseful()
{
    ThorimGauntletPositioningTrigger thorimGauntletPositioningTrigger(botAI);
    return thorimGauntletPositioningTrigger.IsActive();
}

bool ThorimGauntletPositioningAction::Execute(Event event [[maybe_unused]])
{
    FollowMasterStrategy followMasterStrategy(botAI);

    Unit* master = botAI->GetMaster();

    std::string const rti = AI_VALUE(std::string, "rti");
    if (rti != "cross")
    {
        botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("cross");
    }

    if (master->GetDistance(ULDUAR_THORIM_NEAR_ENTRANCE_POSITION) < 10.0f && (bot->GetDistance2d(master) > 5.0f))
    {
        if (MoveTo(bot->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(), false,
                   false, false, true, MovementPriority::MOVEMENT_NORMAL, true))
        {
            if (!botAI->HasStrategy(followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT))
            {
                botAI->ChangeStrategy(ADD_STRATEGY_CHAR + followMasterStrategy.getName(),
                                      BotState::BOT_STATE_NON_COMBAT);
            }

            return true;
        }
    }

    if (master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1) < 6.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2) < 6.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1) < 5.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1) < 10.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2) < 10.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3) < 10.0f)
    {
        float distance1 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1);
        float distance2 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2);
        float distance3 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1);
        float distance4 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1);
        float distance5 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2);
        float distance6 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3);

        float smallestDistance = std::min({distance1, distance2, distance3, distance4, distance5, distance6});

        Position targetPosition;

        if (smallestDistance == distance1)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance2)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_COMBAT);
        }
        else if (smallestDistance == distance3)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance4)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance5)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance6)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else
            return false;
    }

    if (master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1) < 6.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2) < 6.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1) < 5.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1) < 10.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2) < 10.0f ||
        master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3) < 10.0f)
    {
        float distance1 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1);
        float distance2 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2);
        float distance3 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1);
        float distance4 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1);
        float distance5 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2);
        float distance6 = master->GetDistance(ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3);

        float smallestDistance = std::min({distance1, distance2, distance3, distance4, distance5, distance6});

        Position targetPosition;

        if (smallestDistance == distance1)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance2)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_COMBAT);
        }
        else if (smallestDistance == distance3)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance4)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance5)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else if (smallestDistance == distance6)
        {
            return MoveTo(bot->GetMapId(), ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3.GetPositionX(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3.GetPositionY(),
                          ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3.GetPositionZ(), false, false, false, true,
                          MovementPriority::MOVEMENT_NORMAL, true);
        }
        else
            return false;
    }

    Unit* boss = AI_VALUE2(Unit*, "find target", "thorim");
    if (boss && boss->IsAlive() && bot->GetPositionZ() > ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD &&
        boss->GetPositionZ() < ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD)
    {
        MoveTo(bot->GetMapId(), ULDUAR_THORIM_JUMP_START_POINT.GetPositionX(),
               ULDUAR_THORIM_JUMP_START_POINT.GetPositionY(), ULDUAR_THORIM_JUMP_START_POINT.GetPositionZ(), false,
               false, false, true, MovementPriority::MOVEMENT_NORMAL, true);

        if (bot->GetDistance(ULDUAR_THORIM_JUMP_START_POINT) > 0.5f)
            return false;

        JumpTo(bot->GetMapId(), ULDUAR_THORIM_JUMP_END_POINT.GetPositionX(),
               ULDUAR_THORIM_JUMP_END_POINT.GetPositionY(), ULDUAR_THORIM_JUMP_END_POINT.GetPositionZ(),
               MovementPriority::MOVEMENT_COMBAT);
    }

    return false;
}

bool ThorimFallFromFloorAction::Execute(Event event [[maybe_unused]])
{
    Player* master = botAI->GetMaster();

    if (!master)
        return false;

    return bot->TeleportTo(bot->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(),
                           master->GetOrientation());
}

bool ThorimFallFromFloorAction::isUseful()
{
    ThorimFallFromFloorTrigger thorimFallFromFloorTrigger(botAI);
    return thorimFallFromFloorTrigger.IsActive();
}

bool ThorimPhase2PositioningAction::Execute(Event event [[maybe_unused]])
{
    Position targetPosition;
    bool backward = false;

    if (botAI->IsMainTank(bot))
    {
        targetPosition = ULDUAR_THORIM_PHASE2_TANK_SPOT;
        backward = true;
    }
    else
    {
        Group* group = bot->GetGroup();
        if (!group)
            return false;

        uint32 memberPositionNumber = 0;
        for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            if (!member)
                continue;

            if (botAI->IsRanged(member) || botAI->IsHeal(member))
            {
                if (bot->GetGUID() == member->GetGUID())
                    break;

                memberPositionNumber++;

                if (memberPositionNumber == 3)
                    memberPositionNumber = 0;
            }
        }

        if (memberPositionNumber == 0)
            targetPosition = ULDUAR_THORIM_PHASE2_RANGE1_SPOT;

        if (memberPositionNumber == 1)
            targetPosition = ULDUAR_THORIM_PHASE2_RANGE2_SPOT;

        if (memberPositionNumber == 2)
            targetPosition = ULDUAR_THORIM_PHASE2_RANGE3_SPOT;
    }

    MoveTo(bot->GetMapId(), targetPosition.GetPositionX(), targetPosition.GetPositionY(), targetPosition.GetPositionZ(),
           false, false, false, true, MovementPriority::MOVEMENT_COMBAT, true, backward);

    if (bot->GetDistance(targetPosition) > 1.0f)
        return false;

    return true;
}

bool ThorimPhase2PositioningAction::isUseful()
{
    ThorimPhase2PositioningTrigger thorimPhase2PositioningTrigger(botAI);
    return thorimPhase2PositioningTrigger.IsActive();
}

bool MimironShockBlastAction::Execute(Event event [[maybe_unused]])
{
    Unit* leviathanMkII = nullptr;
    Unit* vx001 = nullptr;
    Unit* aerialCommandUnit = nullptr;

    float radius = 20.0f;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            vx001 = target;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            aerialCommandUnit = target;
        }
    }

    if (!leviathanMkII)
    {
        return false;
    }

    if (!vx001 && !aerialCommandUnit)
    {
        float currentDistance = bot->GetDistance2d(leviathanMkII);

        MoveAway(leviathanMkII, radius - currentDistance);

        if (botAI->IsMelee(bot))
        {
            botAI->SetNextCheckDelay(100);
        }
        return true;
    }
    else
    {
        float init_angle = leviathanMkII->GetAngle(bot);
        float distance = radius - bot->GetDistance2d(leviathanMkII);
        for (float delta = 0; delta <= M_PI / 2; delta += M_PI / 8)
        {
            float angle = init_angle + delta;
            float dx = bot->GetPositionX() + cos(angle) * distance;
            float dy = bot->GetPositionY() + sin(angle) * distance;
            float dz = bot->GetPositionZ();
            if (bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
                                                               bot->GetPositionZ(), dx, dy, dz))
            {
                bot->TeleportTo(target->GetMapId(), dx, dy, dz, target->GetOrientation());
                return true;
            }
        }
        return false;
    }
}

bool MimironShockBlastAction::isUseful()
{
    MimironShockBlastTrigger mimironShockBlastTrigger(botAI);
    return mimironShockBlastTrigger.IsActive();
}

bool MimironPhase1PositioningAction::Execute(Event event [[maybe_unused]])
{
    SET_AI_VALUE(float, "disperse distance", 6.0f);
    return true;
}

bool MimironPhase1PositioningAction::isUseful()
{
    MimironPhase1PositioningTrigger mimironPhase1PositioningTrigger(botAI);
    return mimironPhase1PositioningTrigger.IsActive();
}

bool MimironP3Wx2LaserBarrageAction::Execute(Event event [[maybe_unused]])
{
    auto master = botAI->GetMaster();
    if (!master || !master->IsAlive())
    {
        return false;
    }

    if (bot->GetDistance2d(master) > 15.0f)
    {
        return bot->TeleportTo(master->GetMapId(), master->GetPositionX(), master->GetPositionY(),
                               master->GetPositionZ(), master->GetOrientation());
    }

    return MoveTo(master->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(), false,
                  false, false, true, MovementPriority::MOVEMENT_COMBAT, true);
}

bool MimironRapidBurstAction::isUseful()
{
    MimironRapidBurstTrigger mimironRapidBurstTrigger(botAI);
    return mimironRapidBurstTrigger.IsActive();
}

bool MimironRapidBurstAction::Execute(Event event [[maybe_unused]])
{
    Unit* leviathanMkII = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
    }

    Position targetPosition;

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    uint32 memberSpotNumber = 0;
    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member)
            continue;

        if (bot->GetGUID() == member->GetGUID())
        {
            if (botAI->IsRanged(bot))
            {
                switch (memberSpotNumber)
                {
                    case 0:
                        targetPosition = ULDUAR_MIMIRON_PHASE2_SIDE1RANGE_SPOT;
                        break;
                    case 1:
                        targetPosition = ULDUAR_MIMIRON_PHASE2_SIDE2RANGE_SPOT;
                        break;
                    case 2:
                        targetPosition = ULDUAR_MIMIRON_PHASE2_SIDE3RANGE_SPOT;
                        break;
                    default:
                        break;
                }
            }
            else if (botAI->IsMainTank(bot) && leviathanMkII)
            {
                targetPosition = ULDUAR_MIMIRON_PHASE4_TANK_SPOT;
            }
            else
            {
                switch (memberSpotNumber)
                {
                    case 0:
                        targetPosition = ULDUAR_MIMIRON_PHASE2_SIDE1MELEE_SPOT;
                        break;
                    case 1:
                        targetPosition = ULDUAR_MIMIRON_PHASE2_SIDE2MELEE_SPOT;
                        break;
                    case 2:
                        targetPosition = ULDUAR_MIMIRON_PHASE2_SIDE3MELEE_SPOT;
                        break;
                    default:
                        break;
                }
            }
        }

        memberSpotNumber++;

        if (memberSpotNumber == 3)
        {
            memberSpotNumber = 0;
        }
    }

    MoveTo(bot->GetMapId(), targetPosition.GetPositionX(), targetPosition.GetPositionY(), targetPosition.GetPositionZ(),
           false, false, false, true, MovementPriority::MOVEMENT_FORCED, true, false);

    if (AI_VALUE(float, "disperse distance") != 0.0f)
    {
        SET_AI_VALUE(float, "disperse distance", 0.0f);
    }

    TankFaceStrategy tankFaceStrategy(botAI);
    if (botAI->HasStrategy(tankFaceStrategy.getName(), BotState::BOT_STATE_COMBAT))
    {
        botAI->ChangeStrategy(REMOVE_STRATEGY_CHAR + tankFaceStrategy.getName(), BotState::BOT_STATE_COMBAT);
    }
    if (botAI->HasStrategy(tankFaceStrategy.getName(), BotState::BOT_STATE_NON_COMBAT))
    {
        botAI->ChangeStrategy(REMOVE_STRATEGY_CHAR + tankFaceStrategy.getName(), BotState::BOT_STATE_NON_COMBAT);
    }

    if (bot->GetDistance(targetPosition) > 1.0f)
        return false;

    return true;
}

bool MimironAerialCommandUnitAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = nullptr;
    Unit* bombBot = nullptr;
    Unit* assaultBot = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            boss = target;
        }
        else if (target->GetEntry() == NPC_BOMB_BOT)
        {
            bombBot = target;
        }
        else if (target->GetEntry() == NPC_ASSAULT_BOT)
        {
            assaultBot = target;
        }
    }

    if (botAI->IsMainTank(bot) || botAI->IsAssistTankOfIndex(bot, 0))
    {
        Group* group = bot->GetGroup();
        if (!group)
        {
            return false;
        }

        if (bombBot)
        {
            group->SetTargetIcon(RtiTargetValue::crossIndex, bot->GetGUID(), bombBot->GetGUID());
        }
        else if (boss)
        {
            group->SetTargetIcon(RtiTargetValue::crossIndex, bot->GetGUID(), boss->GetGUID());
        }

        if (assaultBot)
        {
            ObjectGuid skullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
            Unit* skullUnit = botAI->GetUnit(skullTarget);
            if (!skullTarget || !skullUnit || !skullUnit->IsAlive())
            {
                group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), assaultBot->GetGUID());
            }
        }

        return true;
    }

    if (AI_VALUE(float, "disperse distance") != 5.0f)
    {
        SET_AI_VALUE(float, "disperse distance", 5.0f);
    }
    botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("cross");
    return true;
}

bool MimironRocketStrikeAction::isUseful()
{
    MimironRocketStrikeTrigger mimironRocketStrikeTrigger(botAI);
    return mimironRocketStrikeTrigger.IsActive();
}

bool MimironRocketStrikeAction::Execute(Event event [[maybe_unused]])
{
    Unit* leviathanMkII [[maybe_unused]] = nullptr;
    Unit* vx001 = nullptr;
    Unit* aerialCommandUnit = nullptr;

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            vx001 = target;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            aerialCommandUnit = target;
        }
    }

    Creature* rocketStrikeN = bot->FindNearestCreature(NPC_ROCKET_STRIKE_N, 100.0f);

    if (!rocketStrikeN)
    {
        return false;
    }

    if (!vx001 && !aerialCommandUnit)
    {
        MoveAway(rocketStrikeN, 10.0f);
        return true;
    }
    else
    {
        float init_angle = rocketStrikeN->GetAngle(bot);
        float distance = 10.0f - bot->GetDistance2d(rocketStrikeN);
        for (float delta = 0; delta <= M_PI / 2; delta += M_PI / 8)
        {
            float angle = init_angle + delta;
            float dx = bot->GetPositionX() + cos(angle) * distance;
            float dy = bot->GetPositionY() + sin(angle) * distance;
            float dz = bot->GetPositionZ();
            if (bot->GetMap()->CheckCollisionAndGetValidCoords(bot, bot->GetPositionX(), bot->GetPositionY(),
                                                               bot->GetPositionZ(), dx, dy, dz))
            {
                bot->TeleportTo(target->GetMapId(), dx, dy, dz, target->GetOrientation());
                return true;
            }
        }
        return false;
    }
}

bool MimironPhase4MarkDpsAction::Execute(Event event [[maybe_unused]])
{
    Unit* leviathanMkII = nullptr;
    Unit* vx001 = nullptr;
    Unit* aerialCommandUnit = nullptr;

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    GuidVector targets = AI_VALUE(GuidVector, "possible targets");
    Unit* target = nullptr;
    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        target = botAI->GetUnit(*i);
        if (!target || !target->IsAlive())
            continue;

        if (target->GetEntry() == NPC_LEVIATHAN_MKII)
        {
            leviathanMkII = target;
        }
        else if (target->GetEntry() == NPC_VX001)
        {
            vx001 = target;
        }
        else if (target->GetEntry() == NPC_AERIAL_COMMAND_UNIT)
        {
            aerialCommandUnit = target;
        }
    }

    if (!leviathanMkII || !vx001 || !aerialCommandUnit)
    {
        return false;
    }

    if (botAI->IsMainTank(bot))
    {
        Unit* highestHealthUnit = nullptr;
        uint32 highestHealth = 0;
        if (leviathanMkII && leviathanMkII->GetHealth() > highestHealth)
        {
            highestHealth = leviathanMkII->GetHealth();
            highestHealthUnit = leviathanMkII;
        }
        if (vx001 && vx001->GetHealth() > highestHealth)
        {
            highestHealth = vx001->GetHealth();
            highestHealthUnit = vx001;
        }
        if (aerialCommandUnit && aerialCommandUnit->GetHealth() > highestHealth)
        {
            highestHealthUnit = aerialCommandUnit;
        }

        group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), highestHealthUnit->GetGUID());
        if (highestHealthUnit == leviathanMkII)
        {
            if (AI_VALUE(std::string, "rti") == "skull")
            {
                botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("skull");
            }
        }
        else
        {
            group->SetTargetIcon(RtiTargetValue::crossIndex, bot->GetGUID(), leviathanMkII->GetGUID());
            if (AI_VALUE(std::string, "rti") != "cross")
            {
                botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("cross");
            }
        }

        botAI->DoSpecificAction("attack rti target");
        return true;
    }
    else
    {
        /*if (AI_VALUE(float, "disperse distance") != 0.0f)
        {
            SET_AI_VALUE(float, "disperse distance", 0.0f);
        }*/
        botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("skull");
        return true;
    }
}

bool MimironCheatAction::Execute(Event event [[maybe_unused]])
{
    GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");
    for (const ObjectGuid& guid : targets)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || !unit->IsAlive())
            continue;

        if (unit->GetEntry() == NPC_PROXIMITY_MINE)
        {
            unit->Kill(bot, unit);
        }
        else if (unit->GetEntry() == NPC_BOMB_BOT)
        {
            unit->Kill(bot, unit);
        }
    }

    return true;
}

bool VezaxCheatAction::Execute(Event event [[maybe_unused]])
{
    // Restore bot's mana to full
    uint32 maxMana = bot->GetMaxPower(POWER_MANA);
    if (maxMana > 0)
    {
        bot->SetPower(POWER_MANA, maxMana);
    }

    return true;
}

bool VezaxShadowCrashAction::Execute(Event event [[maybe_unused]])
{
    // Find General Vezax boss
    Unit* boss = AI_VALUE2(Unit*, "find target", "general vezax");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }

    // Get bot's current position relative to boss
    float bossX = boss->GetPositionX();
    float bossY = boss->GetPositionY();
    float bossZ = boss->GetPositionZ();

    float botX = bot->GetPositionX();
    float botY = bot->GetPositionY();

    // Calculate current angle and distance from boss
    float currentAngle = atan2(botY - bossY, botX - bossX);
    float currentDistance = bot->GetDistance2d(boss);

    // Set desired distance from boss (stay close enough for melee, far enough for ranged)
    float desiredDistance = 15.0f;

    // If too close or too far, adjust distance first
    if (currentDistance < desiredDistance - 2.0f || currentDistance > desiredDistance + 2.0f)
    {
        currentDistance = desiredDistance;
    }

    // Calculate movement increment - move in increments around the boss
    float angleIncrement = M_PI / 10;
    float newAngle = currentAngle + angleIncrement;

    // Calculate new position
    float newX = bossX + currentDistance * cos(newAngle);
    float newY = bossY + currentDistance * sin(newAngle);
    float newZ = bossZ;  // Keep same Z level as boss

    // Move to the new position
    return MoveTo(boss->GetMapId(), newX, newY, newZ, false, false, false, true, MovementPriority::MOVEMENT_COMBAT,
                  true);
}

bool VezaxMarkOfTheFacelessAction::Execute(Event event [[maybe_unused]])
{
    return MoveTo(bot->GetMapId(), ULDUAR_VEZAX_MARK_OF_THE_FACELESS_SPOT.GetPositionX(),
                  ULDUAR_VEZAX_MARK_OF_THE_FACELESS_SPOT.GetPositionY(),
                  ULDUAR_VEZAX_MARK_OF_THE_FACELESS_SPOT.GetPositionZ(), false, false, false, true,
                  MovementPriority::MOVEMENT_FORCED, true, false);
}

bool YoggSaronOminousCloudCheatAction::Execute(Event event [[maybe_unused]])
{
    YoggSaronTrigger yoggSaronTrigger(botAI);

    Unit* boss = yoggSaronTrigger.GetSaraIfAlive();
    if (!boss)
    {
        return false;
    }

    Creature* target = boss->FindNearestCreature(NPC_OMINOUS_CLOUD, 25.0f);
    if (!target || !target->IsAlive())
    {
        return false;
    }

    target->Kill(bot, target);
    return true;
}

bool YoggSaronGuardianPositioningAction::Execute(Event event [[maybe_unused]])
{
    return MoveTo(bot->GetMapId(), ULDUAR_YOGG_SARON_MIDDLE.GetPositionX(), ULDUAR_YOGG_SARON_MIDDLE.GetPositionY(),
                  ULDUAR_YOGG_SARON_MIDDLE.GetPositionZ(), false, false, false, true,
                  MovementPriority::MOVEMENT_FORCED, true, false);
}

bool YoggSaronSanityAction::Execute(Event event [[maybe_unused]])
{
    Creature* sanityWell = bot->FindNearestCreature(NPC_SANITY_WELL, 200.0f);

    return MoveTo(bot->GetMapId(), sanityWell->GetPositionX(), sanityWell->GetPositionY(), sanityWell->GetPositionZ(),
                  false, false, false, true, MovementPriority::MOVEMENT_FORCED,
                  true, false);
}

bool YoggSaronMarkTargetAction::Execute(Event event [[maybe_unused]])
{
    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    YoggSaronTrigger yoggSaronTrigger(botAI);
    if (yoggSaronTrigger.IsPhase2())
    {
        if (botAI->HasCheat(BotCheatMask::raid))
        {
            Unit* crusherTentacle = bot->FindNearestCreature(NPC_CRUSHER_TENTACLE, 200.0f, true);
            if (crusherTentacle)
            {
                crusherTentacle->Kill(bot, crusherTentacle);
            }
        }

        ObjectGuid currentMoonTarget = group->GetTargetIcon(RtiTargetValue::moonIndex);
        Creature* yogg_saron = bot->FindNearestCreature(NPC_YOGG_SARON, 200.0f, true);
        if (!currentMoonTarget || currentMoonTarget != yogg_saron->GetGUID())
        {
            group->SetTargetIcon(RtiTargetValue::moonIndex, bot->GetGUID(), yogg_saron->GetGUID());
            return true;
        }

        ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);

        Creature* nextPossibleTarget = bot->FindNearestCreature(NPC_CONSTRICTOR_TENTACLE, 200.0f, true);
        if (!nextPossibleTarget)
        {
            nextPossibleTarget = bot->FindNearestCreature(NPC_CORRUPTOR_TENTACLE, 200.0f, true);
            if (!nextPossibleTarget)
            {
                return false;
            }
        }

        if (currentSkullTarget)
        {
            Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);

            if (currentSkullUnit && currentSkullUnit->IsAlive() &&
                currentSkullUnit->GetGUID() == nextPossibleTarget->GetGUID())
            {
                return false;
            }
        }

        group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), nextPossibleTarget->GetGUID());
    }
    else if (yoggSaronTrigger.IsPhase3())
    {
        TankFaceStrategy tankFaceStrategy(botAI);
        if (botAI->HasStrategy(tankFaceStrategy.getName(), BotState::BOT_STATE_COMBAT))
        {
            botAI->ChangeStrategy(REMOVE_STRATEGY_CHAR + tankFaceStrategy.getName(), BotState::BOT_STATE_COMBAT);
        }

        TankAssistStrategy tankAssistStrategy(botAI);
        if (!botAI->HasStrategy(tankAssistStrategy.getName(), BotState::BOT_STATE_COMBAT))
        {
            botAI->ChangeStrategy(ADD_STRATEGY_CHAR + tankAssistStrategy.getName(), BotState::BOT_STATE_COMBAT);
        }

        GuidVector targets = AI_VALUE(GuidVector, "nearest npcs");

        int lowestHealth = std::numeric_limits<int>::max();
        Unit* lowestHealthUnit = nullptr;
        for (const ObjectGuid& guid : targets)
        {
            Unit* unit = botAI->GetUnit(guid);
            if (!unit || !unit->IsAlive())
            {
                continue;
            }

            if ((unit->GetEntry() == NPC_IMMORTAL_GUARDIAN || unit->GetEntry() == NPC_MARKED_IMMORTAL_GUARDIAN) &&
                unit->GetHealthPct() > 10)
            {
                if (unit->GetHealth() < static_cast<uint32>(lowestHealth))
                {
                    lowestHealth = unit->GetHealth();
                    lowestHealthUnit = unit;
                }
            }
        }

        if (lowestHealthUnit)
        {
            // Added because lunatic gaze freeze all bots and they can't attack
            // If someone fix it then this cheat can be removed
            if (botAI->HasCheat(BotCheatMask::raid))
            {
                lowestHealthUnit->Kill(bot, lowestHealthUnit);
            }
            else
            {
                group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), lowestHealthUnit->GetGUID());
            }

            return true;
        }

        ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);
        Unit* currentSkullUnit = nullptr;
        if (currentSkullTarget)
        {
            currentSkullUnit = botAI->GetUnit(currentSkullTarget);
        }

        if (!currentSkullUnit || currentSkullUnit->GetEntry() != NPC_YOGG_SARON)
        {
            Unit* yoggsaron = AI_VALUE2(Unit*, "find target", "yogg-saron");
            if (yoggsaron && yoggsaron->IsAlive())
            {
                group->SetTargetIcon(RtiTargetValue::skullIndex, bot->GetGUID(), yoggsaron->GetGUID());
                return true;
            }
        }

        return false;
    }

    return false;
}

bool YoggSaronBrainLinkAction::Execute(Event event [[maybe_unused]])
{
    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* player = gref->GetSource();
        if (player && player->IsAlive() && player->HasAura(SPELL_BRAIN_LINK) && player->GetGUID() != bot->GetGUID())
        {
            return MoveNear(player, 10.0f, MovementPriority::MOVEMENT_FORCED);
        }
    }

    return false;
}

bool YoggSaronMoveToEnterPortalAction::Execute(Event event [[maybe_unused]])
{
    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    bool isInBrainRoomTeam = false;
    int portalNumber = 0;
    int brainRoomTeamCount = 10;
    if (bot->GetRaidDifficulty() == Difficulty::RAID_DIFFICULTY_10MAN_NORMAL)
    {
        brainRoomTeamCount = 4;
    }

    Player* master = botAI->GetMaster();
    if (master && !botAI->IsTank(master))
    {
        portalNumber++;
        brainRoomTeamCount--;
    }

    for (GroupReference* gref = group->GetFirstMember(); gref; gref = gref->next())
    {
        Player* member = gref->GetSource();
        if (!member || !member->IsAlive() || botAI->IsTank(member) || botAI->GetMaster()->GetGUID() == member->GetGUID())
        {
            continue;
        }

        portalNumber++;
        if (member->GetGUID() == bot->GetGUID())
        {
            isInBrainRoomTeam = true;
            break;
        }

        brainRoomTeamCount--;
        if (brainRoomTeamCount == 0)
        {
            break;
        }
    }

    if (!isInBrainRoomTeam)
    {
        return false;
    }

    Position assignedPortalPosition = yoggPortalLoc[portalNumber - 1];

    botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("diamond");

    if (botAI->HasCheat(BotCheatMask::raid))
    {
        return bot->TeleportTo(bot->GetMapId(), assignedPortalPosition.GetPositionX(),
                                      assignedPortalPosition.GetPositionY(),
                        assignedPortalPosition.GetPositionZ(), bot->GetOrientation());
    }
    else
    {
        return MoveNear(bot->GetMapId(), assignedPortalPosition.GetPositionX(),
                               assignedPortalPosition.GetPositionY(),
                 assignedPortalPosition.GetPositionZ(), sShadowAIConfig->contactDistance,
                 MovementPriority::MOVEMENT_FORCED);
    }
}

bool YoggSaronFallFromFloorAction::Execute(Event event [[maybe_unused]])
{
    std::string rtiMark = AI_VALUE(std::string, "rti");
    if (rtiMark == "skull")
    {
        return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_BOSS_ROOM_RESTORE_POINT.GetPositionX(),
                               ULDUAR_YOGG_SARON_BOSS_ROOM_RESTORE_POINT.GetPositionY(),
                               ULDUAR_YOGG_SARON_BOSS_ROOM_RESTORE_POINT.GetPositionZ(),
                               ULDUAR_YOGG_SARON_BOSS_ROOM_RESTORE_POINT.GetOrientation());
    }
    if (rtiMark == "cross")
    {
        return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_STORMWIND_KEEPER_MIDDLE.GetPositionX(),
                               ULDUAR_YOGG_SARON_STORMWIND_KEEPER_MIDDLE.GetPositionY(),
                               ULDUAR_YOGG_SARON_STORMWIND_KEEPER_MIDDLE.GetPositionZ(),
                               bot->GetOrientation());
    }
    if (rtiMark == "circle")
    {
        return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_ICECROWN_CITADEL_MIDDLE.GetPositionX(),
                               ULDUAR_YOGG_SARON_ICECROWN_CITADEL_MIDDLE.GetPositionY(),
                               ULDUAR_YOGG_SARON_ICECROWN_CITADEL_MIDDLE.GetPositionZ(), bot->GetOrientation());
    }
    if (rtiMark == "star")
    {
        return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_MIDDLE.GetPositionX(),
                               ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_MIDDLE.GetPositionY(),
                               ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_MIDDLE.GetPositionZ(), bot->GetOrientation());
    }
    return false;
}

bool YoggSaronBossRoomMovementCheatAction::Execute(Event event [[maybe_unused]])
{
    FollowMasterStrategy followMasterStrategy(botAI);
    if (botAI->HasStrategy(followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT))
    {
        botAI->ChangeStrategy(REMOVE_STRATEGY_CHAR + followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT);
    }

    if (!botAI->HasCheat(BotCheatMask::raid))
    {
        return false;
    }

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }
    ObjectGuid currentSkullTarget = group->GetTargetIcon(RtiTargetValue::skullIndex);

    if (!currentSkullTarget)
    {
        return false;
    }

    Unit* currentSkullUnit = botAI->GetUnit(currentSkullTarget);

    if (!currentSkullUnit || !currentSkullUnit->IsAlive())
    {
        return false;
    }

    return bot->TeleportTo(bot->GetMapId(), currentSkullUnit->GetPositionX(), currentSkullUnit->GetPositionY(),
                           currentSkullUnit->GetPositionZ(), bot->GetOrientation());
}

bool YoggSaronUsePortalAction::Execute(Event event [[maybe_unused]])
{
     Creature* assignedPortal = bot->FindNearestCreature(NPC_DESCEND_INTO_MADNESS, 2.0f, true);
     if (!assignedPortal)
     {
         return false;
     }

     FollowMasterStrategy followMasterStrategy(botAI);
     if (botAI->HasStrategy(followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT))
     {
         botAI->ChangeStrategy(ADD_STRATEGY_CHAR + followMasterStrategy.getName(), BotState::BOT_STATE_NON_COMBAT);
     }

     return assignedPortal->HandleSpellClick(bot);
}

bool YoggSaronIllusionRoomAction::Execute(Event event [[maybe_unused]])
{
    YoggSaronTrigger yoggSaronTrigger(botAI);

    bool resultSetRtiMark = SetRtiMark(yoggSaronTrigger);
    bool resultSetIllusionRtiTarget = SetIllusionRtiTarget(yoggSaronTrigger);
    bool resultSetBrainRtiTarget = SetBrainRtiTarget(yoggSaronTrigger);

    return resultSetRtiMark || resultSetIllusionRtiTarget || resultSetBrainRtiTarget;
}

bool YoggSaronIllusionRoomAction::SetRtiMark(YoggSaronTrigger yoggSaronTrigger)
{
    if (AI_VALUE(std::string, "rti") == "diamond")
    {
        if (yoggSaronTrigger.IsInStormwindKeeperIllusion())
        {
            botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("cross");
            return true;
        }
        else if (yoggSaronTrigger.IsInIcecrownKeeperIllusion())
        {
            botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("circle");
            return true;
        }
        else if (yoggSaronTrigger.IsInChamberOfTheAspectsIllusion())
        {
            botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("star");
            return true;
        }
    }
    return false;
}

bool YoggSaronIllusionRoomAction::SetIllusionRtiTarget(YoggSaronTrigger yoggSaronTrigger)
{
    Unit* currentRtiTarget = yoggSaronTrigger.GetIllusionRoomRtiTarget();
    if (currentRtiTarget)
    {
        return false;
    }

    Unit* nextRtiTarget = yoggSaronTrigger.GetNextIllusionRoomRtiTarget();
    if (!nextRtiTarget)
    {
        return false;
    }

    // If proper adds handling in illusion room will be implemented, then this can be removed
    if (botAI->HasCheat(BotCheatMask::raid))
    {
        bot->TeleportTo(bot->GetMapId(), nextRtiTarget->GetPositionX(), nextRtiTarget->GetPositionY(),
                        nextRtiTarget->GetPositionZ(), bot->GetOrientation());

        Unit::DealDamage(bot->GetSession()->GetPlayer(), nextRtiTarget, nextRtiTarget->GetHealth(), nullptr,
                         DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false, true);
    }
    else
    {
        Group* group = bot->GetGroup();
        if (!group)
        {
            return false;
        }

        uint8 rtiIndex = RtiTargetValue::GetRtiIndex(AI_VALUE(std::string, "rti"));
        group->SetTargetIcon(rtiIndex, bot->GetGUID(), nextRtiTarget->GetGUID());
    }

    return true;
}

bool YoggSaronIllusionRoomAction::SetBrainRtiTarget(YoggSaronTrigger yoggSaronTrigger)
{
    if (AI_VALUE(std::string, "rti") == "square" || !yoggSaronTrigger.IsMasterIsInBrainRoom())
    {
        return false;
    }

    botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("square");

    Group* group = bot->GetGroup();
    if (!group)
    {
        return false;
    }

    Creature* brain = bot->FindNearestCreature(NPC_BRAIN, 200.0f, true);
    if (!brain)
    {
        return false;
    }

    group->SetTargetIcon(RtiTargetValue::squareIndex, bot->GetGUID(), brain->GetGUID());

    Position entrancePosition = yoggSaronTrigger.GetIllusionRoomEntrancePosition();

    if (botAI->HasCheat(BotCheatMask::raid))
    {
        if (Unit const* master = botAI->GetMaster())
        {
            Position masterPosition = master->GetPosition();
            bot->TeleportTo(bot->GetMapId(), masterPosition.GetPositionX(), masterPosition.GetPositionY(),
                            masterPosition.GetPositionZ(), bot->GetOrientation());
        }
        else
        {
            bot->TeleportTo(bot->GetMapId(), entrancePosition.GetPositionX(), entrancePosition.GetPositionY(),
                            entrancePosition.GetPositionZ(), bot->GetOrientation());
        }
    }
    else
    {
        MoveTo(bot->GetMapId(), entrancePosition.GetPositionX(), entrancePosition.GetPositionY(),
            entrancePosition.GetPositionZ(), false, false, false, true, MovementPriority::MOVEMENT_FORCED, true,
            false);
    }

    botAI->DoSpecificAction("attack rti target");
    return true;
}

bool YoggSaronMoveToExitPortalAction::Execute(Event event [[maybe_unused]])
{
    GameObject* portal = bot->FindNearestGameObject(GO_FLEE_TO_THE_SURFACE_PORTAL, 100.0f);
    if (!portal)
    {
        return false;
    }

    if (botAI->HasCheat(BotCheatMask::raid))
    {
        bot->TeleportTo(bot->GetMapId(), portal->GetPositionX(), portal->GetPositionY(), portal->GetPositionZ(),
                               bot->GetOrientation());
    }
    else
    {
        MoveTo(bot->GetMapId(), portal->GetPositionX(), portal->GetPositionY(), portal->GetPositionZ(), false,
                      false, false, true, MovementPriority::MOVEMENT_FORCED,
                      true, false);
    }

    if (bot->GetDistance2d(portal) > 2.0f)
    {
        return false;
    }

    portal->Use(bot);

    botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("skull");
    return true;
}

bool YoggSaronLunaticGazeAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "yogg-saron");
    if (!boss || !boss->IsAlive())
    {
        return false;
    }
    float angle = bot->GetAngle(boss);
    float newAngle = Position::NormalizeOrientation(angle + M_PI);  // Add 180 degrees (PI radians)
    bot->SetFacingTo(newAngle);

    if (botAI->IsRangedDps(bot))
    {
        if (AI_VALUE(std::string, "rti") != "cross")
        {
            botAI->GetAiObjectContext()->GetValue<std::string>("rti")->Set("cross");
        }
    }

    return true;
}

bool YoggSaronPhase3PositioningAction::Execute(Event event [[maybe_unused]])
{
    if (botAI->IsRanged(bot))
    {
        if (botAI->HasCheat(BotCheatMask::raid))
        {
            return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionX(),
                            ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionY(),
                            ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionZ(),
                            bot->GetOrientation());
        }
        else
        {
            return MoveTo(bot->GetMapId(), ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionX(),
                   ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionY(),
                   ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT.GetPositionZ(), false,
                   false, false, true, MovementPriority::MOVEMENT_FORCED, true, false);
        }
    }

    if (botAI->IsMelee(bot) && !botAI->IsTank(bot))
    {
        if (botAI->HasCheat(BotCheatMask::raid))
        {
            return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionX(),
                            ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionY(),
                            ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionZ(), bot->GetOrientation());
        }
        else
        {
            return MoveTo(bot->GetMapId(), ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionX(),
                   ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionY(),
                   ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionZ(), false, false, false, true,
                   MovementPriority::MOVEMENT_FORCED, true, false);
        }
    }

    if (botAI->IsTank(bot))
    {
        if (bot->GetDistance(ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT) > 30.0f)
        {
            if (botAI->HasCheat(BotCheatMask::raid))
            {
                return bot->TeleportTo(bot->GetMapId(), ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionX(),
                                       ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionY(),
                                       ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionZ(), bot->GetOrientation());
            }
        }

        return MoveTo(bot->GetMapId(), ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionX(),
                      ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionY(),
                      ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT.GetPositionZ(), false, false, false, true,
                      MovementPriority::MOVEMENT_FORCED, true, false);
    }

    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float FlameLeviathanMultiplier::GetValue(Action* action [[maybe_unused]])
{
    // if (dynamic_cast<FleeAction*>(action))
    //     return 0.0f;
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidUlduarStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    //
    // Flame Leviathan
    //
    triggers.push_back(new TriggerNode(
        "flame leviathan vehicle near",
        NextAction::array(0, new NextAction("flame leviathan enter vehicle", ACTION_RAID + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "flame leviathan on vehicle",
        NextAction::array(0, new NextAction("flame leviathan vehicle", ACTION_RAID + 1), nullptr)));

    //
    // Razorscale
    //
    triggers.push_back(new TriggerNode(
        "razorscale avoid devouring flames",
        NextAction::array(0, new NextAction("razorscale avoid devouring flames", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale avoid sentinel",
        NextAction::array(0, new NextAction("razorscale avoid sentinel", ACTION_RAID + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale flying alone",
    NextAction::array(0, new NextAction("razorscale ignore flying alone", ACTION_MOVE + 5), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale avoid whirlwind",
        NextAction::array(0, new NextAction("razorscale avoid whirlwind", ACTION_RAID + 3), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale grounded",
        NextAction::array(0, new NextAction("razorscale grounded", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale harpoon trigger",
        NextAction::array(0, new NextAction("razorscale harpoon action", ACTION_MOVE), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale fuse armor trigger",
        NextAction::array(0, new NextAction("razorscale fuse armor action", ACTION_RAID + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "razorscale fire resistance trigger",
        NextAction::array(0, new NextAction("razorscale fire resistance action", ACTION_RAID), nullptr)));

    //
    // Ignis
    //
    triggers.push_back(new TriggerNode(
        "ignis fire resistance trigger",
        NextAction::array(0, new NextAction("ignis fire resistance action", ACTION_RAID), nullptr)));

    //
    // Iron Assembly
    //
    triggers.push_back(new TriggerNode(
        "iron assembly lightning tendrils trigger",
        NextAction::array(0, new NextAction("iron assembly lightning tendrils action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "iron assembly overload trigger",
        NextAction::array(0, new NextAction("iron assembly overload action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "iron assembly rune of power trigger",
        NextAction::array(0, new NextAction("iron assembly rune of power action", ACTION_RAID), nullptr)));

    //
    // Kologarn
    //
    triggers.push_back(new TriggerNode(
        "kologarn fall from floor trigger",
        NextAction::array(0, new NextAction("kologarn fall from floor action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn rti target trigger",
        NextAction::array(0, new NextAction("kologarn rti target action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn eyebeam trigger",
        NextAction::array(0, new NextAction("kologarn eyebeam action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn attack dps target trigger",
        NextAction::array(0, new NextAction("attack rti target", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn mark dps target trigger",
        NextAction::array(0, new NextAction("kologarn mark dps target action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn nature resistance trigger",
        NextAction::array(0, new NextAction("kologarn nature resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn rubble slowdown trigger",
        NextAction::array(0, new NextAction("kologarn rubble slowdown action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "kologarn crunch armor trigger",
        NextAction::array(0, new NextAction("kologarn crunch armor action", ACTION_RAID), nullptr)));

    //
    // Auriaya
    //
    triggers.push_back(new TriggerNode(
        "auriaya fall from floor trigger",
        NextAction::array(0, new NextAction("auriaya fall from floor action", ACTION_RAID), nullptr)));

    //
    // Hodir
    //
    triggers.push_back(new TriggerNode(
        "hodir near snowpacked icicle",
        NextAction::array(0, new NextAction("hodir move snowpacked icicle", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "hodir biting cold",
        NextAction::array(0, new NextAction("hodir biting cold jump", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "hodir frost resistance trigger",
        NextAction::array(0, new NextAction("hodir frost resistance action", ACTION_RAID), nullptr)));

    //
    // Freya
    //
    triggers.push_back(new TriggerNode(
        "freya near nature bomb",
        NextAction::array(0, new NextAction("freya move away nature bomb", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "freya nature resistance trigger",
        NextAction::array(0, new NextAction("freya nature resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "freya fire resistance trigger",
        NextAction::array(0, new NextAction("freya fire resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "freya mark dps target trigger",
        NextAction::array(0, new NextAction("freya mark dps target action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "freya move to healing spore trigger",
        NextAction::array(0, new NextAction("freya move to healing spore action", ACTION_RAID), nullptr)));

    //
    // Thorim
    //
    triggers.push_back(new TriggerNode(
        "thorim nature resistance trigger",
        NextAction::array(0, new NextAction("thorim nature resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim frost resistance trigger",
        NextAction::array(0, new NextAction("thorim frost resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim unbalancing strike trigger",
        NextAction::array(0, new NextAction("thorim unbalancing strike action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim mark dps target trigger",
        NextAction::array(0, new NextAction("thorim mark dps target action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim gauntlet positioning trigger",
        NextAction::array(0, new NextAction("thorim gauntlet positioning action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim arena positioning trigger",
        NextAction::array(0, new NextAction("thorim arena positioning action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim fall from floor trigger",
        NextAction::array(0, new NextAction("thorim fall from floor action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "thorim phase 2 positioning trigger",
        NextAction::array(0, new NextAction("thorim phase 2 positioning action", ACTION_RAID), nullptr)));

    //
    // Mimiron
    //
    triggers.push_back(new TriggerNode(
        "mimiron p3wx2 laser barrage trigger",
        NextAction::array(0, new NextAction("mimiron p3wx2 laser barrage action", ACTION_RAID + 2), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron shock blast trigger",
        NextAction::array(0, new NextAction("mimiron shock blast action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron fire resistance trigger",
        NextAction::array(0, new NextAction("mimiron fire resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron phase 1 positioning trigger",
        NextAction::array(0, new NextAction("mimiron phase 1 positioning action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron rapid burst trigger",
        NextAction::array(0, new NextAction("mimiron rapid burst action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron aerial command unit trigger",
        NextAction::array(0, new NextAction("mimiron aerial command unit action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron rocket strike trigger",
        NextAction::array(0, new NextAction("mimiron rocket strike action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron phase 4 mark dps trigger",
        NextAction::array(0, new NextAction("mimiron phase 4 mark dps action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "mimiron cheat trigger",
        NextAction::array(0, new NextAction("mimiron cheat action", ACTION_RAID), nullptr)));

    //
    // General Vezax
    //
    triggers.push_back(new TriggerNode(
        "vezax cheat trigger",
        NextAction::array(0, new NextAction("vezax cheat action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "vezax shadow crash trigger",
        NextAction::array(0, new NextAction("vezax shadow crash action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "vezax mark of the faceless trigger",
        NextAction::array(0, new NextAction("vezax mark of the faceless action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "vezax shadow resistance trigger",
        NextAction::array(0, new NextAction("vezax shadow resistance action", ACTION_RAID), nullptr)));

    //
    // Yogg-Saron
    //
    triggers.push_back(new TriggerNode(
        "sara shadow resistance trigger",
        NextAction::array(0, new NextAction("sara shadow resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron shadow resistance trigger",
        NextAction::array(0, new NextAction("yogg-saron shadow resistance action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron ominous cloud cheat trigger",
        NextAction::array(0, new NextAction("yogg-saron ominous cloud cheat action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron guardian positioning trigger",
        NextAction::array(0, new NextAction("yogg-saron guardian positioning action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron sanity trigger",
        NextAction::array(0, new NextAction("yogg-saron sanity action", ACTION_RAID + 1), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron death orb trigger",
        NextAction::array(0, new NextAction("yogg-saron death orb action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron malady of the mind trigger",
        NextAction::array(0, new NextAction("yogg-saron malady of the mind action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron mark target trigger",
        NextAction::array(0, new NextAction("yogg-saron mark target action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron brain link trigger",
        NextAction::array(0, new NextAction("yogg-saron brain link action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron move to enter portal trigger",
        NextAction::array(0, new NextAction("yogg-saron move to enter portal action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron use portal trigger",
        NextAction::array(0, new NextAction("yogg-saron use portal action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron fall from floor trigger",
        NextAction::array(0, new NextAction("yogg-saron fall from floor action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron boss room movement cheat trigger",
        NextAction::array(0, new NextAction("yogg-saron boss room movement cheat action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron illusion room trigger",
        NextAction::array(0, new NextAction("yogg-saron illusion room action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron move to exit portal trigger",
        NextAction::array(0, new NextAction("yogg-saron move to exit portal action", ACTION_RAID), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron lunatic gaze trigger",
        NextAction::array(0, new NextAction("yogg-saron lunatic gaze action", ACTION_EMERGENCY), nullptr)));

    triggers.push_back(new TriggerNode(
        "yogg-saron phase 3 positioning trigger",
        NextAction::array(0, new NextAction("yogg-saron phase 3 positioning action", ACTION_RAID), nullptr)));
}

void RaidUlduarStrategy::InitMultipliers(std::vector<Multiplier*>& multipliers)
{
    multipliers.push_back(new FlameLeviathanMultiplier(botAI));
}
