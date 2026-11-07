#ifndef _SHADOW_RAIDULDUAR_H
#define _SHADOW_RAIDULDUAR_H

#include "../../../../src/server/scripts/Northrend/Ulduar/Ulduar/ulduar.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <ctime>
#include <limits>
#include "AiObject.h"
#include "AiObjectContext.h"
#include "EventMap.h"
#include "Log.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "ShadowAI.h"
#include "Shadows.h"
#include "ScriptedCreature.h"
#include "SharedDefines.h"
#include "GenericTriggers.h"
#include "ShadowAIConfig.h"
#include "Trigger.h"
#include "Action.h"
#include "AttackAction.h"
#include "GenericActions.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"
#include "Vehicle.h"
#include "Multiplier.h"
#include "Strategy.h"
#include "NamedObjectContext.h"
#include "BossAuraActions.h"
#include "BossAuraTriggers.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const uint32 ULDUAR_MAP_ID = 603;

class RazorscaleBossHelper : public AiObject
{
public:
    // Enums and constants specific to Razorscale
    enum RazorscaleUnits : uint32
    {
        UNIT_RAZORSCALE          = 33186,
        UNIT_DARK_RUNE_SENTINEL  = 33846,
        UNIT_DARK_RUNE_WATCHER   = 33453,
        UNIT_DARK_RUNE_GUARDIAN  = 33388,
        UNIT_DEVOURING_FLAME     = 34188,
    };

    enum RazorscaleGameObjects : uint32
    {
        GO_RAZORSCALE_HARPOON_1 = 194519,
        GO_RAZORSCALE_HARPOON_2 = 194541,
        GO_RAZORSCALE_HARPOON_3 = 194542,
        GO_RAZORSCALE_HARPOON_4 = 194543,
    };

    enum RazorscaleSpells : uint32
    {
        SPELL_CHAIN_1           = 49679,
        SPELL_CHAIN_2           = 49682,
        SPELL_CHAIN_3           = 49683,
        SPELL_CHAIN_4           = 49684,
        SPELL_SENTINEL_WHIRLWIND = 63806,
        SPELL_STUN_AURA         = 62794,
        SPELL_FUSEARMOR         = 64771
    };

    static constexpr uint32 FUSEARMOR_THRESHOLD = 2;

    // Constants for arena parameters
    static constexpr float RAZORSCALE_FLYING_Z_THRESHOLD = 440.0f;
    static constexpr float RAZORSCALE_ARENA_CENTER_X = 587.54f;
    static constexpr float RAZORSCALE_ARENA_CENTER_Y = -175.04f;
    static constexpr float RAZORSCALE_ARENA_RADIUS = 30.0f;

    // Harpoon cooldown (seconds)
    static constexpr time_t HARPOON_COOLDOWN_DURATION = 5;

    // Structure for harpoon data
    struct HarpoonData
    {
        uint32 gameObjectEntry;
        uint32 chainSpellId;
    };

    explicit RazorscaleBossHelper(ShadowAI* botAI)
        : AiObject(botAI), _boss(nullptr) {}

    bool UpdateBossAI();
    Unit* GetBoss() const;

    bool IsGroundPhase() const;
    bool IsFlyingPhase() const;

    bool IsHarpoonFired(uint32 chainSpellId) const;
    static bool IsHarpoonReady(GameObject* harpoonGO);
    static void SetHarpoonOnCooldown(GameObject* harpoonGO);
    GameObject* FindNearestHarpoon(float x, float y, float z) const;

    static const std::vector<HarpoonData>& GetHarpoonData();

    void AssignRolesBasedOnHealth();
    bool AreRolesAssigned() const;
    bool CanSwapRoles() const;

private:
    Unit* _boss;

    // A map to track the last role swap *per bot* by their GUID
    static std::unordered_map<ObjectGuid, std::time_t> _lastRoleSwapTime;

    // The cooldown that applies to every bot
    static const std::time_t _roleSwapCooldown = 10;

    static std::unordered_map<ObjectGuid, time_t> _harpoonCooldowns;
};

// template <class BossAiType>
// class GenericBossHelper : public AiObject
// {
// public:
//     GenericBossHelper(ShadowAI* botAI, std::string name) : AiObject(botAI), _name(name) {}
//     virtual bool UpdateBossAI()
//     {
//         if (!bot->IsInCombat())
//         {
//             _unit = nullptr;
//         }
//         if (_unit && (!_unit->IsInWorld() || !_unit->IsAlive()))
//         {
//             _unit = nullptr;
//         }
//         if (!_unit)
//         {
//             _unit = AI_VALUE2(Unit*, "find target", _name);
//             if (!_unit)
//             {
//                 return false;
//             }
//             _target = _unit->ToCreature();
//             if (!_target)
//             {
//                 return false;
//             }
//             _ai = dynamic_cast<BossAiType*>(_target->GetAI());
//             if (!_ai)
//             {
//                 return false;
//             }
//             _event_map = &_ai->events;
//             if (!_event_map)
//             {
//                 return false;
//             }
//         }
//         if (!_event_map)
//         {
//             return false;
//         }
//         _timer = _event_map->GetTimer();
//         return true;
//     }
//     virtual void Reset()
//     {
//         _unit = nullptr;
//         _target = nullptr;
//         _ai = nullptr;
//         _event_map = nullptr;
//         _timer = 0;
//     }

// protected:
//     std::string _name;
//     Unit* _unit = nullptr;
//     Creature* _target = nullptr;
//     BossAiType* _ai = nullptr;
//     EventMap* _event_map = nullptr;
//     uint32 _timer = 0;
// };

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum UlduarIDs
{
    // Iron Assembly
    SPELL_LIGHTNING_TENDRILS_10_MAN = 61887,
    SPELL_LIGHTNING_TENDRILS_25_MAN = 63486,
    SPELL_OVERLOAD_10_MAN = 61869,
    SPELL_OVERLOAD_25_MAN = 63481,
    SPELL_OVERLOAD_10_MAN_2 = 63485,
    SPELL_OVERLOAD_25_MAN_2 = 61886,
    SPELL_RUNE_OF_POWER = 64320,

    // Kologarn
    NPC_RIGHT_ARM = 32934,
    NPC_RUBBLE = 33768,
    SPELL_CRUNCH_ARMOR = 64002,

    SPELL_FOCUSED_EYEBEAM_10_2 = 63346,
    SPELL_FOCUSED_EYEBEAM_10 = 63347,
    SPELL_FOCUSED_EYEBEAM_25_2 = 63976,
    SPELL_FOCUSED_EYEBEAM_25 = 63977,

    // Hodir
    NPC_SNOWPACKED_ICICLE = 33174,
    NPC_TOASTY_FIRE = 33342,
    SPELL_FLASH_FREEZE = 61968,
    SPELL_BITING_COLD_PLAYER_AURA = 62039,

    // Freya
    NPC_SNAPLASHER = 32916,
    NPC_STORM_LASHER = 32919,
    NPC_DETONATING_LASHER = 32918,
    NPC_ANCIENT_WATER_SPIRIT = 33202,
    NPC_ANCIENT_CONSERVATOR = 33203,
    NPC_HEALTHY_SPORE = 33215,
    NPC_EONARS_GIFT = 33228,
    GOBJECT_NATURE_BOMB = 194902,

    // Thorim
    NPC_DARK_RUNE_ACOLYTE_I = 32886,
    NPC_CAPTURED_MERCENARY_SOLDIER_ALLY = 32885,
    NPC_CAPTURED_MERCENARY_SOLDIER_HORDE = 32883,
    NPC_CAPTURED_MERCENARY_CAPTAIN_ALLY = 32908,
    NPC_CAPTURED_MERCENARY_CAPTAIN_HORDE = 32907,
    NPC_JORMUNGAR_BEHEMOT = 32882,
    NPC_DARK_RUNE_WARBRINGER = 32877,
    NPC_DARK_RUNE_EVOKER = 32878,
    NPC_DARK_RUNE_CHAMPION = 32876,
    NPC_DARK_RUNE_COMMONER = 32904,
    NPC_IRON_RING_GUARD = 32874,
    NPC_RUNIC_COLOSSUS = 32872,
    NPC_ANCIENT_RUNE_GIANT = 32873,
    NPC_DARK_RUNE_ACOLYTE_G = 33110,
    NPC_IRON_HONOR_GUARD = 32875,
    SPELL_UNBALANCING_STRIKE = 62130,

    // Mimiron
    NPC_LEVIATHAN_MKII = 33432,
    NPC_VX001 = 33651,
    NPC_AERIAL_COMMAND_UNIT = 33670,
    NPC_BOMB_BOT = 33836,
    NPC_ROCKET_STRIKE_N = 34047,
    NPC_ASSAULT_BOT = 34057,
    NPC_PROXIMITY_MINE = 34362,
    SPELL_P3WX2_LASER_BARRAGE_1 = 63293,
    SPELL_P3WX2_LASER_BARRAGE_2 = 63297,
    SPELL_SPINNING_UP = 63414,
    SPELL_SHOCK_BLAST = 63631,
    SPELL_P3WX2_LASER_BARRAGE_3 = 64042,
    SPELL_P3WX2_LASER_BARRAGE_AURA_1 = 63274,
    SPELL_P3WX2_LASER_BARRAGE_AURA_2 = 63300,

    // General Vezax
    SPELL_MARK_OF_THE_FACELESS = 63276,
    SPELL_SHADOW_CRASH = 63277,

    // Yogg-Saron
    ACTION_ILLUSION_DRAGONS = 1,
    ACTION_ILLUSION_ICECROWN = 2,
    ACTION_ILLUSION_STORMWIND = 3,
    NPC_GUARDIAN_OF_YS = 33136,
    NPC_YOGG_SARON = 33288,
    NPC_OMINOUS_CLOUD = 33292,
    NPC_RUBY_CONSORT = 33716,
    NPC_AZURE_CONSORT = 33717,
    NPC_BRONZE_CONSORT = 33718,
    NPC_EMERALD_CONSORT = 33719,
    NPC_OBSIDIAN_CONSORT = 33720,
    NPC_ALEXTRASZA = 33536,
    NPC_MALYGOS_ILLUSION = 33535,
    NPC_NELTHARION = 33523,
    NPC_YSERA = 33495,
    GO_DRAGON_SOUL = 194462,
    NPC_SARA_PHASE_1 = 33134,
    NPC_LICH_KING_ILLUSION = 33441,
    NPC_IMMOLATED_CHAMPION = 33442,
    NPC_SUIT_OF_ARMOR = 33433,
    NPC_GARONA = 33436,
    NPC_KING_LLANE = 33437,
    NPC_DEATHSWORN_ZEALOT = 33567,
    NPC_INFLUENCE_TENTACLE = 33943,
    NPC_DEATH_ORB = 33882,
    NPC_BRAIN = 33890,
    NPC_CRUSHER_TENTACLE = 33966,
    NPC_CONSTRICTOR_TENTACLE = 33983,
    NPC_CORRUPTOR_TENTACLE = 33985,
    NPC_IMMORTAL_GUARDIAN = 33988,
    NPC_LAUGHING_SKULL = 33990,
    NPC_SANITY_WELL = 33991,
    NPC_DESCEND_INTO_MADNESS = 34072,
    NPC_MARKED_IMMORTAL_GUARDIAN = 36064,
    SPELL_SANITY = 63050,
    SPELL_BRAIN_LINK = 63802,
    SPELL_MALADY_OF_THE_MIND = 63830,
    SPELL_SHADOW_BARRIER = 63894,
    SPELL_TELEPORT_TO_CHAMBER = 63997,
    SPELL_TELEPORT_TO_ICECROWN = 63998,
    SPELL_TELEPORT_TO_STORMWIND = 63989,
    SPELL_TELEPORT_BACK = 63992,
    SPELL_CANCEL_ILLUSION_AURA = 63993,
    SPELL_INDUCE_MADNESS = 64059,
    SPELL_LUNATIC_GAZE_YS = 64163,
    GO_FLEE_TO_THE_SURFACE_PORTAL = 194625,

    // Buffs
    SPELL_FROST_TRAP = 13809
};

const float ULDUAR_KOLOGARN_AXIS_Z_PATHING_ISSUE_DETECT = 420.0f;
const float ULDUAR_KOLOGARN_EYEBEAM_RADIUS = 3.0f;
const float ULDUAR_THORIM_AXIS_Z_FLOOR_THRESHOLD = 429.6094f;
const float ULDUAR_THORIM_AXIS_Z_PATHING_ISSUE_DETECT = 410.0f;
const float ULDUAR_AURIAYA_AXIS_Z_PATHING_ISSUE_DETECT = 410.0f;
const float ULDUAR_YOGG_SARON_BOSS_ROOM_AXIS_Z_PATHING_ISSUE_DETECT = 300.0f;
const float ULDUAR_YOGG_SARON_BRAIN_ROOM_AXIS_Z_PATHING_ISSUE_DETECT = 200.0f;
const float ULDUAR_YOGG_SARON_STORMWIND_KEEPER_RADIUS = 150.0f;
const float ULDUAR_YOGG_SARON_ICECROWN_CITADEL_RADIUS = 150.0f;
const float ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_RADIUS = 150.0f;
const float ULDUAR_YOGG_SARON_BRAIN_ROOM_RADIUS = 50.0f;

const Position ULDUAR_THORIM_NEAR_ARENA_CENTER = Position(2134.9854f, -263.11853f, 419.8465f);
const Position ULDUAR_THORIM_NEAR_ENTRANCE_POSITION = Position(2172.4355f, -258.27957f, 418.47162f);
const Position ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_1 = Position(2237.6187f, -265.08844f, 412.17548f);
const Position ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_6_YARDS_2 = Position(2237.2498f, -275.81122f, 412.17548f);
const Position ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_5_YARDS_1 = Position(2236.895f, -294.62448f, 412.1348f);
const Position ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_1 = Position(2242.1162f, -310.15308f, 412.1348f);
const Position ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_2 = Position(2242.018f, -318.66003f, 412.1348f);
const Position ULDUAR_THORIM_GAUNTLET_LEFT_SIDE_10_YARDS_3 = Position(2242.1904f, -329.0533f, 412.1348f);
const Position ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_1 = Position(2219.5417f, -264.77167f, 412.17548f);
const Position ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_6_YARDS_2 = Position(2217.446f, -275.85248f, 412.17548f);
const Position ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_5_YARDS_1 = Position(2217.8877f, -295.01193f, 412.13434f);
const Position ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_1 = Position(2212.193f, -307.44992f, 412.1348f);
const Position ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_2 = Position(2212.1353f, -318.20795f, 412.1348f);
const Position ULDUAR_THORIM_GAUNTLET_RIGHT_SIDE_10_YARDS_3 = Position(2212.1956f, -328.0144f, 412.1348f);
const Position ULDUAR_THORIM_JUMP_END_POINT = Position(2137.8818f, -278.18942f, 419.66653f);
const Position ULDUAR_THORIM_PHASE2_TANK_SPOT = Position(2134.8572f, -287.0291f, 419.4935f);
const Position ULDUAR_THORIM_PHASE2_RANGE1_SPOT = Position(2112.8752f, -267.69305f, 419.52814f);
const Position ULDUAR_THORIM_PHASE2_RANGE2_SPOT = Position(2134.1296f, -257.3316f, 419.8462f);
const Position ULDUAR_THORIM_PHASE2_RANGE3_SPOT = Position(2156.798f, -267.57434f, 419.52722f);
const Position ULDUAR_MIMIRON_PHASE2_SIDE1RANGE_SPOT = Position(2753.708f, 2583.9617f, 364.31357f);
const Position ULDUAR_MIMIRON_PHASE2_SIDE1MELEE_SPOT = Position(2746.9792f, 2573.6716f, 364.31357f);
const Position ULDUAR_MIMIRON_PHASE2_SIDE2RANGE_SPOT = Position(2727.7224f, 2569.527f, 364.31357f);
const Position ULDUAR_MIMIRON_PHASE2_SIDE2MELEE_SPOT = Position(2739.4746f, 2569.4106f, 364.31357f);
const Position ULDUAR_MIMIRON_PHASE2_SIDE3RANGE_SPOT = Position(2754.1294f, 2553.9954f, 364.31357f);
const Position ULDUAR_MIMIRON_PHASE2_SIDE3MELEE_SPOT = Position(2746.8513f, 2565.4263f, 364.31357f);
const Position ULDUAR_MIMIRON_PHASE4_TANK_SPOT = Position(2744.5754f, 2570.8657f, 364.3138f);
const Position ULDUAR_VEZAX_MARK_OF_THE_FACELESS_SPOT = Position(1913.6501f, 122.93989f, 342.38083f);
const Position ULDUAR_YOGG_SARON_MIDDLE = Position(1980.28f, -25.5868f, 329.397f);
const Position ULDUAR_YOGG_SARON_STORMWIND_KEEPER_MIDDLE = Position(1927.1511f, 68.507256f, 242.37657f);
const Position ULDUAR_YOGG_SARON_ICECROWN_CITADEL_MIDDLE = Position(1925.6553f, -121.59296f, 239.98965f);
const Position ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_MIDDLE = Position(2104.5667f, -25.509348f, 242.64679f);
const Position ULDUAR_YOGG_SARON_BRAIN_ROOM_MIDDLE = Position(1980.1971f, -27.854689f, 236.06789f);
const Position ULDUAR_YOGG_SARON_STORMWIND_KEEPER_ENTRANCE = Position(1954.06f, 21.66f, 239.71f);
const Position ULDUAR_YOGG_SARON_ICECROWN_CITADEL_ENTRANCE = Position(1950.11f, -79.284f, 239.98982f);
const Position ULDUAR_YOGG_SARON_CHAMBER_OF_ASPECTS_ENTRANCE = Position(2048.63f, -25.5f, 239.72f);
const Position ULDUAR_YOGG_SARON_PHASE_3_MELEE_SPOT = Position(1998.5377f, -22.90317f, 324.8895f);
const Position ULDUAR_YOGG_SARON_PHASE_3_RANGED_SPOT = Position(2018.7628f, -18.896868f, 327.07245f);

//
// Flame Levi
//
class FlameLeviathanOnVehicleTrigger : public Trigger
{
public:
    FlameLeviathanOnVehicleTrigger(ShadowAI* ai) : Trigger(ai, "flame leviathan on vehicle") {}
    bool IsActive() override;
};

class FlameLeviathanVehicleNearTrigger : public Trigger
{
public:
    FlameLeviathanVehicleNearTrigger(ShadowAI* ai) : Trigger(ai, "flame leviathan vehicle near") {}
    bool IsActive() override;
};

//
// Razorscale
//
class RazorscaleFlyingAloneTrigger : public Trigger
{
public:
    RazorscaleFlyingAloneTrigger(ShadowAI* ai) : Trigger(ai, "razorscale flying alone") {}
    bool IsActive() override;
};

class RazorscaleDevouringFlamesTrigger : public Trigger
{
public:
    RazorscaleDevouringFlamesTrigger(ShadowAI* ai) : Trigger(ai, "razorscale avoid devouring flames") {}
    bool IsActive() override;
};

class RazorscaleAvoidSentinelTrigger : public Trigger
{
public:
    RazorscaleAvoidSentinelTrigger(ShadowAI* ai) : Trigger(ai, "razorscale avoid sentinel") {}
    bool IsActive() override;
};

class RazorscaleAvoidWhirlwindTrigger : public Trigger
{
public:
    RazorscaleAvoidWhirlwindTrigger(ShadowAI* ai) : Trigger(ai, "razorscale avoid whirlwind") {}
    bool IsActive() override;
};

class RazorscaleGroundedTrigger : public Trigger
{
public:
    RazorscaleGroundedTrigger(ShadowAI* ai) : Trigger(ai, "razorscale grounded") {}
    bool IsActive() override;
};

class RazorscaleHarpoonAvailableTrigger : public Trigger
{
public:
    RazorscaleHarpoonAvailableTrigger(ShadowAI* ai) : Trigger(ai, "razorscale harpoon trigger") {}
    bool IsActive() override;
};

class RazorscaleFuseArmorTrigger : public Trigger
{
public:
    RazorscaleFuseArmorTrigger(ShadowAI* ai) : Trigger(ai, "razorscale fuse armor trigger") {}
    bool IsActive() override;
};

//
// Iron Assembly
//
class IronAssemblyLightningTendrilsTrigger : public Trigger
{
public:
    IronAssemblyLightningTendrilsTrigger(ShadowAI* ai) : Trigger(ai, "iron assembly lightning tendrils trigger") {}
    bool IsActive() override;
};

class IronAssemblyOverloadTrigger : public Trigger
{
public:
    IronAssemblyOverloadTrigger(ShadowAI* ai) : Trigger(ai, "iron assembly overload trigger") {}
    bool IsActive() override;
};

class IronAssemblyRuneOfPowerTrigger : public Trigger
{
public:
    IronAssemblyRuneOfPowerTrigger(ShadowAI* ai) : Trigger(ai, "iron assembly rune of power trigger") {}
    bool IsActive() override;
};

//
// Kologarn
//
class KologarnMarkDpsTargetTrigger : public Trigger
{
public:
    KologarnMarkDpsTargetTrigger(ShadowAI* ai) : Trigger(ai, "kologarn mark dps target trigger") {}
    bool IsActive() override;
};

class KologarnFallFromFloorTrigger : public Trigger
{
public:
    KologarnFallFromFloorTrigger(ShadowAI* ai) : Trigger(ai, "kologarn fall from floor trigger") {}
    bool IsActive() override;
};

class KologarnRubbleSlowdownTrigger : public Trigger
{
public:
    KologarnRubbleSlowdownTrigger(ShadowAI* ai) : Trigger(ai, "kologarn rubble slowdown trigger") {}
    bool IsActive() override;
};

class KologarnEyebeamTrigger : public Trigger
{
public:
    KologarnEyebeamTrigger(ShadowAI* ai) : Trigger(ai, "kologarn eyebeam trigger") {}
    bool IsActive() override;
};

class KologarnAttackDpsTargetTrigger : public Trigger
{
public:
    KologarnAttackDpsTargetTrigger(ShadowAI* ai) : Trigger(ai, "kologarn attack dps target trigger") {}
    bool IsActive() override;
};

class KologarnRtiTargetTrigger : public Trigger
{
public:
    KologarnRtiTargetTrigger(ShadowAI* ai) : Trigger(ai, "kologarn rti target trigger") {}
    bool IsActive() override;
};

class KologarnCrunchArmorTrigger : public Trigger
{
public:
    KologarnCrunchArmorTrigger(ShadowAI* ai) : Trigger(ai, "kologarn crunch armor trigger") {}
    bool IsActive() override;
};

//
// Auriaya
//
class AuriayaFallFromFloorTrigger : public Trigger
{
public:
    AuriayaFallFromFloorTrigger(ShadowAI* ai) : Trigger(ai, "auriaya fall from floor trigger") {}
    bool IsActive() override;
};

//
// Hodir
//
class HodirBitingColdTrigger : public Trigger
{
public:
    HodirBitingColdTrigger(ShadowAI* ai) : Trigger(ai, "hodir biting cold") {}
    bool IsActive() override;
};

class HodirNearSnowpackedIcicleTrigger : public Trigger
{
public:
    HodirNearSnowpackedIcicleTrigger(ShadowAI* ai) : Trigger(ai, "hodir near snowpacked icicle") {}
    bool IsActive() override;
};

//
// Freya
//
class FreyaNearNatureBombTrigger : public Trigger
{
public:
    FreyaNearNatureBombTrigger(ShadowAI* ai) : Trigger(ai, "freya near nature bomb") {}
    bool IsActive() override;
};

class FreyaMarkDpsTargetTrigger : public Trigger
{
public:
    FreyaMarkDpsTargetTrigger(ShadowAI* ai) : Trigger(ai, "freya mark dps target trigger") {}
    bool IsActive() override;
};

class FreyaMoveToHealingSporeTrigger : public Trigger
{
public:
    FreyaMoveToHealingSporeTrigger(ShadowAI* ai) : Trigger(ai, "freya move to healing spore trigger") {}
    bool IsActive() override;
};

//
// Thorim
//
class ThorimUnbalancingStrikeTrigger : public Trigger
{
public:
    ThorimUnbalancingStrikeTrigger(ShadowAI* ai) : Trigger(ai, "thorim unbalancing strike trigger") {}
    bool IsActive() override;
};

class ThorimMarkDpsTargetTrigger : public Trigger
{
public:
    ThorimMarkDpsTargetTrigger(ShadowAI* ai) : Trigger(ai, "thorim mark dps target trigger") {}
    bool IsActive() override;
};

class ThorimGauntletPositioningTrigger : public Trigger
{
public:
    ThorimGauntletPositioningTrigger(ShadowAI* ai) : Trigger(ai, "thorim gauntlet positioning trigger") {}
    bool IsActive() override;
};

class ThorimArenaPositioningTrigger : public Trigger
{
public:
    ThorimArenaPositioningTrigger(ShadowAI* ai) : Trigger(ai, "thorim arena positioning trigger") {}
    bool IsActive() override;
};

class ThorimFallFromFloorTrigger : public Trigger
{
public:
    ThorimFallFromFloorTrigger(ShadowAI* ai) : Trigger(ai, "thorim fall from floor trigger") {}
    bool IsActive() override;
};

class ThorimPhase2PositioningTrigger : public Trigger
{
public:
    ThorimPhase2PositioningTrigger(ShadowAI* ai) : Trigger(ai, "thorim phase 2 positioning trigger") {}
    bool IsActive() override;
};

//
// Mimiron
//
class MimironShockBlastTrigger : public Trigger
{
public:
    MimironShockBlastTrigger(ShadowAI* ai) : Trigger(ai, "mimiron shock blast trigger") {}
    bool IsActive() override;
};

class MimironPhase1PositioningTrigger : public Trigger
{
public:
    MimironPhase1PositioningTrigger(ShadowAI* ai) : Trigger(ai, "mimiron phase 1 positioning trigger") {}
    bool IsActive() override;
};

class MimironP3Wx2LaserBarrageTrigger : public Trigger
{
public:
    MimironP3Wx2LaserBarrageTrigger(ShadowAI* ai) : Trigger(ai, "mimiron p3wx2 laser barrage trigger") {}
    bool IsActive() override;
};

class MimironRapidBurstTrigger : public Trigger
{
public:
    MimironRapidBurstTrigger(ShadowAI* ai) : Trigger(ai, "mimiron rapid burst trigger") {}
    bool IsActive() override;
};

class MimironAerialCommandUnitTrigger : public Trigger
{
public:
    MimironAerialCommandUnitTrigger(ShadowAI* ai) : Trigger(ai, "mimiron aerial command unit trigger") {}
    bool IsActive() override;
};

class MimironRocketStrikeTrigger : public Trigger
{
public:
    MimironRocketStrikeTrigger(ShadowAI* ai) : Trigger(ai, "mimiron rocket strike trigger") {}
    bool IsActive() override;
};

class MimironPhase4MarkDpsTrigger : public Trigger
{
public:
    MimironPhase4MarkDpsTrigger(ShadowAI* ai) : Trigger(ai, "mimiron phase 4 mark dps trigger") {}
    bool IsActive() override;
};

class MimironCheatTrigger : public Trigger
{
public:
    MimironCheatTrigger(ShadowAI* ai) : Trigger(ai, "mimiron cheat trigger") {}
    bool IsActive() override;
};

//
// General Vezax
//
class VezaxCheatTrigger : public Trigger
{
public:
    VezaxCheatTrigger(ShadowAI* ai) : Trigger(ai, "vezax cheat trigger") {}
    bool IsActive() override;
};

class VezaxShadowCrashTrigger : public Trigger
{
public:
    VezaxShadowCrashTrigger(ShadowAI* ai) : Trigger(ai, "vezax shadow crash trigger") {}
    bool IsActive() override;
};

class VezaxMarkOfTheFacelessTrigger : public Trigger
{
public:
    VezaxMarkOfTheFacelessTrigger(ShadowAI* ai) : Trigger(ai, "vezax mark of the faceless trigger") {}
    bool IsActive() override;
};

//
// Yogg-Saron
//
class YoggSaronTrigger : public Trigger
{
public:
    YoggSaronTrigger(ShadowAI* ai, std::string const name = "yogg saron trigger", int32 checkInteval = 1)
        : Trigger(ai, name, checkInteval) {}

    bool IsYoggSaronFight();
    bool IsPhase2();
    bool IsPhase3();
    bool IsInBrainLevel();
    bool IsInIllusionRoom();
    bool IsInStormwindKeeperIllusion();
    bool IsInIcecrownKeeperIllusion();
    bool IsInChamberOfTheAspectsIllusion();
    bool IsMasterIsInIllusionGroup();
    bool IsMasterIsInBrainRoom();
    Position GetIllusionRoomEntrancePosition();
    Unit* GetIllusionRoomRtiTarget();
    Unit* GetNextIllusionRoomRtiTarget();
    Unit* GetSaraIfAlive();
};

class YoggSaronOminousCloudCheatTrigger : public YoggSaronTrigger
{
public:
    YoggSaronOminousCloudCheatTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron ominous cloud cheat trigger") {}
    bool IsActive() override;
};

class YoggSaronGuardianPositioningTrigger : public YoggSaronTrigger
{
public:
    YoggSaronGuardianPositioningTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron guardian positioning trigger") {}
    bool IsActive() override;
};

class YoggSaronSanityTrigger : public YoggSaronTrigger
{
public:
    YoggSaronSanityTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron sanity trigger") {}
    bool IsActive() override;
};

class YoggSaronDeathOrbTrigger : public YoggSaronTrigger
{
public:
    YoggSaronDeathOrbTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron death orb trigger") {}
    bool IsActive() override;
};

class YoggSaronMaladyOfTheMindTrigger : public YoggSaronTrigger
{
public:
    YoggSaronMaladyOfTheMindTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron malady of the mind trigger") {}
    bool IsActive() override;
};

class YoggSaronMarkTargetTrigger : public YoggSaronTrigger
{
public:
    YoggSaronMarkTargetTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron mark target trigger") {}
    bool IsActive() override;
};

class YoggSaronBrainLinkTrigger : public YoggSaronTrigger
{
public:
    YoggSaronBrainLinkTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron brain link trigger") {}
    bool IsActive() override;
};

class YoggSaronMoveToEnterPortalTrigger : public YoggSaronTrigger
{
public:
    YoggSaronMoveToEnterPortalTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron move to enter portal trigger") {}
    bool IsActive() override;
};

class YoggSaronFallFromFloorTrigger : public YoggSaronTrigger
{
public:
    YoggSaronFallFromFloorTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron fall from floor trigger") {}
    bool IsActive() override;
};

class YoggSaronBossRoomMovementCheatTrigger : public YoggSaronTrigger
{
public:
    YoggSaronBossRoomMovementCheatTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron boss room movement cheat trigger") {}
    bool IsActive() override;
};

class YoggSaronUsePortalTrigger : public YoggSaronTrigger
{
public:
    YoggSaronUsePortalTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron use portal trigger") {}
    bool IsActive() override;
};

class YoggSaronIllusionRoomTrigger : public YoggSaronTrigger
{
public:
    YoggSaronIllusionRoomTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron illusion room trigger") {}
    bool IsActive() override;

private:
    bool GoToBrainRoomRequired();
    bool SetRtiMarkRequired();
    bool SetRtiTargetRequired();
};

class YoggSaronMoveToExitPortalTrigger : public YoggSaronTrigger
{
public:
    YoggSaronMoveToExitPortalTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron move to exit portal trigger") {}
    bool IsActive() override;
};

class YoggSaronLunaticGazeTrigger : public YoggSaronTrigger
{
public:
    YoggSaronLunaticGazeTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron lunatic gaze trigger") {}
    bool IsActive() override;
};

class YoggSaronPhase3PositioningTrigger : public YoggSaronTrigger
{
public:
    YoggSaronPhase3PositioningTrigger(ShadowAI* ai) : YoggSaronTrigger(ai, "yogg-saron phase 3 positioning trigger") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//
//  Flame Leviathan
//

class FlameLeviathanVehicleAction : public MovementAction
{
public:
    FlameLeviathanVehicleAction(ShadowAI* botAI) : MovementAction(botAI, "flame leviathan vehicle") {}
    bool Execute(Event event) override;

protected:
    bool MoveAvoidChasing(Unit* target);
    bool DemolisherAction(Unit* target);
    bool DemolisherTurretAction(Unit* target);
    bool SiegeEngineAction(Unit* target);
    bool SiegeEngineTurretAction(Unit* target);
    bool ChopperAction(Unit* target);
    Unit* GetAttacker();
    Unit* vehicleBase_;
    Vehicle* vehicle_;
    int avoidChaseIdx = -1;
};

class FlameLeviathanEnterVehicleAction : public MovementAction
{
public:
    FlameLeviathanEnterVehicleAction(ShadowAI* botAI) : MovementAction(botAI, "flame leviathan enter vehicle") {}
    bool Execute(Event event);

protected:
    bool EnterVehicle(Unit* vehicleBase, bool moveIfFar);
    bool ShouldEnter(Unit* vehicleBase);
    bool AllMainVehiclesOnUse();
};

//
//  Razorscale
//

class RazorscaleAvoidDevouringFlameAction : public MovementAction
{
public:
    RazorscaleAvoidDevouringFlameAction(ShadowAI* botAI) : MovementAction(botAI, "razorscale avoid devouring flames") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RazorscaleAvoidSentinelAction : public MovementAction
{
public:
    RazorscaleAvoidSentinelAction(ShadowAI* botAI) : MovementAction(botAI, "razorscale avoid sentinel") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RazorscaleIgnoreBossAction : public AttackAction
{
public:
    RazorscaleIgnoreBossAction(ShadowAI* botAI) : AttackAction(botAI, "razorscale ignore flying alone") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RazorscaleAvoidWhirlwindAction : public MovementAction
{
public:
    RazorscaleAvoidWhirlwindAction(ShadowAI* botAI) : MovementAction(botAI, "razorscale avoid whirlwind") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RazorscaleGroundedAction : public AttackAction
{
public:
    RazorscaleGroundedAction(ShadowAI* botAI) : AttackAction(botAI, "razorscale grounded") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RazorscaleHarpoonAction : public MovementAction
{
public:
    RazorscaleHarpoonAction(ShadowAI* botAI) : MovementAction(botAI, "razorscale harpoon action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class RazorscaleFuseArmorAction : public MovementAction
{
public:
    RazorscaleFuseArmorAction(ShadowAI* botAI) : MovementAction(botAI, "razorscale fuse armor action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class HodirMoveSnowpackedIcicleAction : public MovementAction
{
public:
    HodirMoveSnowpackedIcicleAction(ShadowAI* botAI) : MovementAction(botAI, "hodir move snowpacked icicle") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class IronAssemblyLightningTendrilsAction : public MovementAction
{
public:
    IronAssemblyLightningTendrilsAction(ShadowAI* botAI) : MovementAction(botAI, "iron assembly lightning tendrils action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class IronAssemblyOverloadAction : public MovementAction
{
public:
    IronAssemblyOverloadAction(ShadowAI* botAI) : MovementAction(botAI, "iron assembly overload action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class IronAssemblyRuneOfPowerAction : public MovementAction
{
public:
    IronAssemblyRuneOfPowerAction(ShadowAI* botAI) : MovementAction(botAI, "iron assembly rune of power action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class KologarnMarkDpsTargetAction : public Action
{
public:
    KologarnMarkDpsTargetAction(ShadowAI* botAI) : Action(botAI, "kologarn mark dps target action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class KologarnFallFromFloorAction : public Action
{
public:
    KologarnFallFromFloorAction(ShadowAI* botAI) : Action(botAI, "kologarn fall from floor action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class KologarnRubbleSlowdownAction : public Action
{
public:
    KologarnRubbleSlowdownAction(ShadowAI* botAI) : Action(botAI, "kologarn rubble slowdown action") {}
    bool Execute(Event event) override;
};

class KologarnEyebeamAction : public MovementAction
{
public:
    KologarnEyebeamAction(ShadowAI* botAI) : MovementAction(botAI, "kologarn eyebeam action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class KologarnRtiTargetAction : public Action
{
public:
    KologarnRtiTargetAction(ShadowAI* botAI) : Action(botAI, "kologarn rti target action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class KologarnCrunchArmorAction : public Action
{
public:
    KologarnCrunchArmorAction(ShadowAI* botAI) : Action(botAI, "kologarn crunch armor action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class AuriayaFallFromFloorAction : public Action
{
public:
    AuriayaFallFromFloorAction(ShadowAI* botAI) : Action(botAI, "auriaya fall from floor action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class HodirBitingColdJumpAction : public MovementAction
{
public:
    HodirBitingColdJumpAction(ShadowAI* ai) : MovementAction(ai, "hodir biting cold jump") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class FreyaMoveAwayNatureBombAction : public MovementAction
{
public:
    FreyaMoveAwayNatureBombAction(ShadowAI* botAI) : MovementAction(botAI, "freya move away nature bomb") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class FreyaMarkDpsTargetAction : public MovementAction
{
public:
    FreyaMarkDpsTargetAction(ShadowAI* botAI) : MovementAction(botAI, "freya mark dps target action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class FreyaMoveToHealingSporeAction : public MovementAction
{
public:
    FreyaMoveToHealingSporeAction(ShadowAI* ai) : MovementAction(ai, "freya move to healing spore action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class ThorimUnbalancingStrikeAction : public Action
{
public:
    ThorimUnbalancingStrikeAction(ShadowAI* ai) : Action(ai, "thorim unbalancing strike action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class ThorimMarkDpsTargetAction : public Action
{
public:
    ThorimMarkDpsTargetAction(ShadowAI* ai) : Action(ai, "thorim mark dps target action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class ThorimArenaPositioningAction : public MovementAction
{
public:
    ThorimArenaPositioningAction(ShadowAI* ai) : MovementAction(ai, "thorim arena positioning action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class ThorimGauntletPositioningAction : public MovementAction
{
public:
    ThorimGauntletPositioningAction(ShadowAI* ai) : MovementAction(ai, "thorim gauntlet positioning action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class ThorimFallFromFloorAction : public Action
{
public:
    ThorimFallFromFloorAction(ShadowAI* botAI) : Action(botAI, "thorim fall from floor action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class ThorimPhase2PositioningAction : public MovementAction
{
public:
    ThorimPhase2PositioningAction(ShadowAI* ai) : MovementAction(ai, "thorim phase 2 positioning action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class MimironShockBlastAction : public MovementAction
{
public:
    MimironShockBlastAction(ShadowAI* ai) : MovementAction(ai, "mimiron shock blast action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class MimironPhase1PositioningAction : public MovementAction
{
public:
    MimironPhase1PositioningAction(ShadowAI* ai) : MovementAction(ai, "mimiron phase 1 positioning action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class MimironP3Wx2LaserBarrageAction : public MovementAction
{
public:
    MimironP3Wx2LaserBarrageAction(ShadowAI* ai, float distance = 24.0f, float delta_angle = M_PI / 8)
        : MovementAction(ai, "mimiron p3wx2 laser barrage action")
    {
        this->distance = distance;
        this->delta_angle = delta_angle;
    }
    virtual bool Execute(Event event);

protected:
    float distance, delta_angle;
};

class MimironRapidBurstAction : public MovementAction
{
public:
    MimironRapidBurstAction(ShadowAI* ai) : MovementAction(ai, "mimiron rapid burst action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class MimironAerialCommandUnitAction : public Action
{
public:
    MimironAerialCommandUnitAction(ShadowAI* ai) : Action(ai, "mimiron aerial command unit action") {}

    bool Execute(Event event) override;
};

class MimironRocketStrikeAction : public MovementAction
{
public:
    MimironRocketStrikeAction(ShadowAI* ai) : MovementAction(ai, "mimiron rocket strike action") {}

    bool Execute(Event event) override;
    bool isUseful() override;
};

class MimironPhase4MarkDpsAction : public Action
{
public:
    MimironPhase4MarkDpsAction(ShadowAI* ai) : Action(ai, "mimiron phase 4 mark dps action") {}

    bool Execute(Event event) override;
};

class MimironCheatAction : public Action
{
public:
    MimironCheatAction(ShadowAI* ai) : Action(ai, "mimiron cheat action") {}

    bool Execute(Event event) override;
};

class VezaxCheatAction : public Action
{
public:
    VezaxCheatAction(ShadowAI* ai) : Action(ai, "vezax cheat action") {}

    bool Execute(Event event) override;
};

class VezaxShadowCrashAction : public MovementAction
{
public:
    VezaxShadowCrashAction(ShadowAI* ai) : MovementAction(ai, "vezax shadow crash action") {}

    bool Execute(Event event) override;
};

class VezaxMarkOfTheFacelessAction : public MovementAction
{
public:
    VezaxMarkOfTheFacelessAction(ShadowAI* ai) : MovementAction(ai, "vezax mark of the faceless action") {}

    bool Execute(Event event) override;
};

class YoggSaronOminousCloudCheatAction : public Action
{
public:
    YoggSaronOminousCloudCheatAction(ShadowAI* ai) : Action(ai, "yogg-saron ominous cloud cheat action") {}

    bool Execute(Event event) override;
};

class YoggSaronGuardianPositioningAction : public MovementAction
{
public:
    YoggSaronGuardianPositioningAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron guardian positioning action") {}

    bool Execute(Event event) override;
};

class YoggSaronSanityAction : public MovementAction
{
public:
    YoggSaronSanityAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron sanity action") {}

    bool Execute(Event event) override;
};

class YoggSaronDeathOrbAction : public MoveAwayFromCreatureAction
{
public:
    YoggSaronDeathOrbAction(ShadowAI* ai) : MoveAwayFromCreatureAction(ai, "yogg-saron death orb action", NPC_DEATH_ORB, 10.0f) {}
};

class YoggSaronMaladyOfTheMindAction : public MoveAwayFromPlayerWithDebuffAction
{
public:
    YoggSaronMaladyOfTheMindAction(ShadowAI* ai) : MoveAwayFromPlayerWithDebuffAction(ai, "yogg-saron malady of the mind action", SPELL_MALADY_OF_THE_MIND, 15.0f) {}
};

class YoggSaronMarkTargetAction : public Action
{
public:
    YoggSaronMarkTargetAction(ShadowAI* ai) : Action(ai, "yogg-saron mark target action") {}

    bool Execute(Event event) override;
};

class YoggSaronBrainLinkAction : public MovementAction
{
public:
    YoggSaronBrainLinkAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron brain link action") {}

    bool Execute(Event event) override;
};

class YoggSaronMoveToEnterPortalAction : public MovementAction
{
public:
    YoggSaronMoveToEnterPortalAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron move to enter portal action") {}

    bool Execute(Event event) override;
};

class YoggSaronFallFromFloorAction : public MovementAction
{
public:
    YoggSaronFallFromFloorAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron fall from floor action") {}

    bool Execute(Event event) override;
};

class YoggSaronBossRoomMovementCheatAction : public MovementAction
{
public:
    YoggSaronBossRoomMovementCheatAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron boss room movement cheat action") {}

    bool Execute(Event event) override;
};

class YoggSaronUsePortalAction : public Action
{
public:
    YoggSaronUsePortalAction(ShadowAI* ai) : Action(ai, "yogg-saron use portal action") {}

    bool Execute(Event event) override;
};

class YoggSaronIllusionRoomAction : public MovementAction
{
public:
    YoggSaronIllusionRoomAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron illusion room action") {}

    bool Execute(Event event) override;

private:
    bool SetRtiMark(YoggSaronTrigger yoggSaronTrigger);
    bool SetIllusionRtiTarget(YoggSaronTrigger yoggSaronTrigger);
    bool SetBrainRtiTarget(YoggSaronTrigger yoggSaronTrigger);
};

class YoggSaronMoveToExitPortalAction : public MovementAction
{
public:
    YoggSaronMoveToExitPortalAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron move to exit portal action") {}

    bool Execute(Event event) override;
};

class YoggSaronLunaticGazeAction : public MovementAction
{
public:
    YoggSaronLunaticGazeAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron lunatic gaze action") {}

    bool Execute(Event event) override;
};

class YoggSaronPhase3PositioningAction : public MovementAction
{
public:
    YoggSaronPhase3PositioningAction(ShadowAI* ai) : MovementAction(ai, "yogg-saron phase 3 positioning action") {}

    bool Execute(Event event) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class FlameLeviathanMultiplier : public Multiplier
{
public:
    FlameLeviathanMultiplier(ShadowAI* ai) : Multiplier(ai, "flame leviathan") {}

public:
    virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidUlduarStrategy : public Strategy
{
public:
    RaidUlduarStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "uld"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*>& multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// /*
//  * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
//  and/or modify it under version 2 of the License, or (at your option), any later version.
//  */



class RaidUlduarActionContext : public NamedObjectContext<Action>
{
public:
    RaidUlduarActionContext()
    {
        creators["flame leviathan vehicle"] = &RaidUlduarActionContext::flame_leviathan_vehicle;
        creators["flame leviathan enter vehicle"] = &RaidUlduarActionContext::flame_leviathan_enter_vehicle;
        creators["razorscale avoid devouring flames"] = &RaidUlduarActionContext::razorscale_avoid_devouring_flames;
        creators["razorscale avoid sentinel"] = &RaidUlduarActionContext::razorscale_avoid_sentinel;
        creators["razorscale ignore flying alone"] = &RaidUlduarActionContext::razorscale_ignore_flying_alone;
        creators["razorscale avoid whirlwind"] = &RaidUlduarActionContext::razorscale_avoid_whirlwind;
        creators["razorscale grounded"] = &RaidUlduarActionContext::razorscale_grounded;
        creators["razorscale harpoon action"] = &RaidUlduarActionContext::razorscale_harpoon_action;
        creators["razorscale fuse armor action"] = &RaidUlduarActionContext::razorscale_fuse_armor_action;
        creators["razorscale fire resistance action"] = &RaidUlduarActionContext::razorscale_fire_resistance_action;
        creators["ignis fire resistance action"] = &RaidUlduarActionContext::ignis_fire_resistance_action;
        creators["iron assembly lightning tendrils action"] = &RaidUlduarActionContext::iron_assembly_lightning_tendrils_action;
        creators["iron assembly overload action"] = &RaidUlduarActionContext::iron_assembly_overload_action;
        creators["iron assembly rune of power action"] = &RaidUlduarActionContext::iron_assembly_rune_of_power_action;
        creators["kologarn mark dps target action"] = &RaidUlduarActionContext::kologarn_mark_dps_target_action;
        creators["kologarn fall from floor action"] = &RaidUlduarActionContext::kologarn_fall_from_floor_action;
        creators["kologarn nature resistance action"] = &RaidUlduarActionContext::kologarn_nature_resistance_action;
        creators["kologarn rubble slowdown action"] = &RaidUlduarActionContext::kologarn_rubble_slowdown_action;
        creators["kologarn eyebeam action"] = &RaidUlduarActionContext::kologarn_eyebeam_action;
        creators["kologarn rti target action"] = &RaidUlduarActionContext::kologarn_rti_target_action;
        creators["kologarn crunch armor action"] = &RaidUlduarActionContext::kologarn_crunch_armor_action;
        creators["auriaya fall from floor action"] = &RaidUlduarActionContext::auriaya_fall_from_floor_action;
        creators["hodir move snowpacked icicle"] = &RaidUlduarActionContext::hodir_move_snowpacked_icicle;
        creators["hodir biting cold jump"] = &RaidUlduarActionContext::hodir_biting_cold_jump;
        creators["hodir frost resistance action"] = &RaidUlduarActionContext::hodir_frost_resistance_action;
        creators["freya move away nature bomb"] = &RaidUlduarActionContext::freya_move_away_nature_bomb;
        creators["freya fire resistance action"] = &RaidUlduarActionContext::freya_fire_resistance_action;
        creators["freya nature resistance action"] = &RaidUlduarActionContext::freya_nature_resistance_action;
        creators["freya mark dps target action"] = &RaidUlduarActionContext::freya_mark_dps_target_action;
        creators["freya move to healing spore action"] = &RaidUlduarActionContext::freya_move_to_healing_spore_action;
        creators["thorim frost resistance action"] = &RaidUlduarActionContext::thorim_frost_resistance_action;
        creators["thorim nature resistance action"] = &RaidUlduarActionContext::thorim_nature_resistance_action;
        creators["thorim unbalancing strike action"] = &RaidUlduarActionContext::thorim_unbalancing_strike_action;
        creators["thorim mark dps target action"] = &RaidUlduarActionContext::thorim_mark_dps_target_action;
        creators["thorim arena positioning action"] = &RaidUlduarActionContext::thorim_arena_positioning_action;
        creators["thorim gauntlet positioning action"] = &RaidUlduarActionContext::thorim_gauntlet_positioning_action;
        creators["thorim phase 2 positioning action"] = &RaidUlduarActionContext::thorim_phase2_positioning_action;
        creators["mimiron fire resistance action"] = &RaidUlduarActionContext::mimiron_fire_resistance_action;
        creators["mimiron shock blast action"] = &RaidUlduarActionContext::mimiron_shock_blast_action;
        creators["mimiron phase 1 positioning action"] = &RaidUlduarActionContext::mimiron_phase_1_positioning_action;
        creators["mimiron p3wx2 laser barrage action"] = &RaidUlduarActionContext::mimiron_p3wx2_laser_barrage_action;
        creators["mimiron rapid burst action"] = &RaidUlduarActionContext::mimiron_rapid_burst_action;
        creators["mimiron aerial command unit action"] = &RaidUlduarActionContext::mimiron_aerial_command_unit_action;
        creators["mimiron rocket strike action"] = &RaidUlduarActionContext::mimiron_rocket_strike_action;
        creators["mimiron phase 4 mark dps action"] = &RaidUlduarActionContext::mimiron_phase_4_mark_dps_action;
        creators["mimiron cheat action"] = &RaidUlduarActionContext::mimiron_cheat_action;
        creators["vezax cheat action"] = &RaidUlduarActionContext::vezax_cheat_action;
        creators["vezax shadow crash action"] = &RaidUlduarActionContext::vezax_shadow_crash_action;
        creators["vezax mark of the faceless action"] = &RaidUlduarActionContext::vezax_mark_of_the_faceless_action;
        creators["vezax shadow resistance action"] = &RaidUlduarActionContext::vezax_shadow_resistance_action;
        creators["sara shadow resistance action"] = &RaidUlduarActionContext::sara_shadow_resistance_action;
        creators["yogg-saron shadow resistance action"] = &RaidUlduarActionContext::yogg_saron_shadow_resistance_action;
        creators["yogg-saron ominous cloud cheat action"] = &RaidUlduarActionContext::yogg_saron_ominous_cloud_cheat_action;
        creators["yogg-saron guardian positioning action"] = &RaidUlduarActionContext::yogg_saron_guardian_positioning_action;
        creators["yogg-saron sanity action"] = &RaidUlduarActionContext::yogg_saron_sanity_action;
        creators["yogg-saron death orb action"] = &RaidUlduarActionContext::yogg_saron_death_orb_action;
        creators["yogg-saron malady of the mind action"] = &RaidUlduarActionContext::yogg_saron_malady_of_the_mind_action;
        creators["yogg-saron mark target action"] = &RaidUlduarActionContext::yogg_saron_mark_target_action;
        creators["yogg-saron brain link action"] = &RaidUlduarActionContext::yogg_saron_brain_link_action;
        creators["yogg-saron move to enter portal action"] = &RaidUlduarActionContext::yogg_saron_move_to_enter_portal_action;
        creators["yogg-saron use portal action"] = &RaidUlduarActionContext::yogg_saron_use_portal_action;
        creators["yogg-saron fall from floor action"] = &RaidUlduarActionContext::yogg_saron_fall_from_floor_action;
        creators["yogg-saron boss room movement cheat action"] = &RaidUlduarActionContext::yogg_saron_boss_room_movement_cheat_action;
        creators["yogg-saron illusion room action"] = &RaidUlduarActionContext::yogg_saron_illusion_room_action;
        creators["yogg-saron move to exit portal action"] = &RaidUlduarActionContext::yogg_saron_move_to_exit_portal_action;
        creators["yogg-saron lunatic gaze action"] = &RaidUlduarActionContext::yogg_saron_lunatic_gaze_action;
        creators["yogg-saron phase 3 positioning action"] = &RaidUlduarActionContext::yogg_saron_phase_3_positioning_action;
    }

private:
    static Action* flame_leviathan_vehicle(ShadowAI* ai) { return new FlameLeviathanVehicleAction(ai); }
    static Action* flame_leviathan_enter_vehicle(ShadowAI* ai) { return new FlameLeviathanEnterVehicleAction(ai); }
    static Action* razorscale_avoid_devouring_flames(ShadowAI* ai) { return new RazorscaleAvoidDevouringFlameAction(ai); }
    static Action* razorscale_avoid_sentinel(ShadowAI* ai) { return new RazorscaleAvoidSentinelAction(ai); }
    static Action* razorscale_ignore_flying_alone(ShadowAI* ai) { return new RazorscaleIgnoreBossAction(ai); }
    static Action* razorscale_avoid_whirlwind(ShadowAI* ai) { return new RazorscaleAvoidWhirlwindAction(ai); }
    static Action* razorscale_grounded(ShadowAI* ai) { return new RazorscaleGroundedAction(ai); }
    static Action* razorscale_harpoon_action(ShadowAI* ai) { return new RazorscaleHarpoonAction(ai); }
    static Action* razorscale_fuse_armor_action(ShadowAI* ai) { return new RazorscaleFuseArmorAction(ai); }
    static Action* razorscale_fire_resistance_action(ShadowAI* ai) { return new BossFireResistanceAction(ai, "razorscale"); }
    static Action* ignis_fire_resistance_action(ShadowAI* ai) { return new BossFireResistanceAction(ai, "ignis the furnace master"); }
    static Action* iron_assembly_lightning_tendrils_action(ShadowAI* ai) { return new IronAssemblyLightningTendrilsAction(ai); }
    static Action* iron_assembly_overload_action(ShadowAI* ai) { return new IronAssemblyOverloadAction(ai); }
    static Action* iron_assembly_rune_of_power_action(ShadowAI* ai) { return new IronAssemblyRuneOfPowerAction(ai); }
    static Action* kologarn_mark_dps_target_action(ShadowAI* ai) { return new KologarnMarkDpsTargetAction(ai); }
    static Action* kologarn_fall_from_floor_action(ShadowAI* ai) { return new KologarnFallFromFloorAction(ai); }
    static Action* kologarn_nature_resistance_action(ShadowAI* ai) { return new BossNatureResistanceAction(ai, "kologarn"); }
    static Action* kologarn_rubble_slowdown_action(ShadowAI* ai) { return new KologarnRubbleSlowdownAction(ai); }
    static Action* kologarn_eyebeam_action(ShadowAI* ai) { return new KologarnEyebeamAction(ai); }
    static Action* kologarn_rti_target_action(ShadowAI* ai) { return new KologarnRtiTargetAction(ai); }
    static Action* kologarn_crunch_armor_action(ShadowAI* ai) { return new KologarnCrunchArmorAction(ai); }
    static Action* auriaya_fall_from_floor_action(ShadowAI* ai) { return new AuriayaFallFromFloorAction(ai); }
    static Action* hodir_move_snowpacked_icicle(ShadowAI* ai) { return new HodirMoveSnowpackedIcicleAction(ai); }
    static Action* hodir_biting_cold_jump(ShadowAI* ai) { return new HodirBitingColdJumpAction(ai); }
    static Action* hodir_frost_resistance_action(ShadowAI* ai) { return new BossFrostResistanceAction(ai, "hodir"); }
    static Action* freya_move_away_nature_bomb(ShadowAI* ai) { return new FreyaMoveAwayNatureBombAction(ai); }
    static Action* freya_fire_resistance_action(ShadowAI* ai) { return new BossFireResistanceAction(ai, "freya"); }
    static Action* freya_nature_resistance_action(ShadowAI* ai) { return new BossNatureResistanceAction(ai, "freya"); }
    static Action* freya_mark_dps_target_action(ShadowAI* ai) { return new FreyaMarkDpsTargetAction(ai); }
    static Action* freya_move_to_healing_spore_action(ShadowAI* ai) { return new FreyaMoveToHealingSporeAction(ai); }
    static Action* thorim_frost_resistance_action(ShadowAI* ai) { return new BossFrostResistanceAction(ai, "thorim"); }
    static Action* thorim_nature_resistance_action(ShadowAI* ai) { return new BossNatureResistanceAction(ai, "thorim"); }
    static Action* thorim_unbalancing_strike_action(ShadowAI* ai) { return new ThorimUnbalancingStrikeAction(ai); }
    static Action* thorim_mark_dps_target_action(ShadowAI* ai) { return new ThorimMarkDpsTargetAction(ai); }
    static Action* thorim_arena_positioning_action(ShadowAI* ai) { return new ThorimArenaPositioningAction(ai); }
    static Action* thorim_gauntlet_positioning_action(ShadowAI* ai) { return new ThorimGauntletPositioningAction(ai); }
    static Action* thorim_phase2_positioning_action(ShadowAI* ai) { return new ThorimPhase2PositioningAction(ai); }
    static Action* mimiron_fire_resistance_action(ShadowAI* ai) { return new BossFireResistanceAction(ai, "mimiron"); }
    static Action* mimiron_shock_blast_action(ShadowAI* ai) { return new MimironShockBlastAction(ai); }
    static Action* mimiron_phase_1_positioning_action(ShadowAI* ai) { return new MimironPhase1PositioningAction(ai); }
    static Action* mimiron_p3wx2_laser_barrage_action(ShadowAI* ai) { return new MimironP3Wx2LaserBarrageAction(ai); }
    static Action* mimiron_rapid_burst_action(ShadowAI* ai) { return new MimironRapidBurstAction(ai); }
    static Action* mimiron_aerial_command_unit_action(ShadowAI* ai) { return new MimironAerialCommandUnitAction(ai); }
    static Action* mimiron_rocket_strike_action(ShadowAI* ai) { return new MimironRocketStrikeAction(ai); }
    static Action* mimiron_phase_4_mark_dps_action(ShadowAI* ai) { return new MimironPhase4MarkDpsAction(ai); }
    static Action* mimiron_cheat_action(ShadowAI* ai) { return new MimironCheatAction(ai); }
    static Action* vezax_cheat_action(ShadowAI* ai) { return new VezaxCheatAction(ai); }
    static Action* vezax_shadow_crash_action(ShadowAI* ai) { return new VezaxShadowCrashAction(ai); }
    static Action* vezax_mark_of_the_faceless_action(ShadowAI* ai) { return new VezaxMarkOfTheFacelessAction(ai); }
    static Action* vezax_shadow_resistance_action(ShadowAI* ai) { return new BossShadowResistanceAction(ai, "general vezax"); }
    static Action* sara_shadow_resistance_action(ShadowAI* ai) { return new BossShadowResistanceAction(ai, "sara"); }
    static Action* yogg_saron_shadow_resistance_action(ShadowAI* ai) { return new BossShadowResistanceAction(ai, "yogg-saron"); }
    static Action* yogg_saron_ominous_cloud_cheat_action(ShadowAI* ai) { return new YoggSaronOminousCloudCheatAction(ai); }
    static Action* yogg_saron_guardian_positioning_action(ShadowAI* ai) { return new YoggSaronGuardianPositioningAction(ai); }
    static Action* yogg_saron_sanity_action(ShadowAI* ai) { return new YoggSaronSanityAction(ai); }
    static Action* yogg_saron_death_orb_action(ShadowAI* ai) { return new YoggSaronDeathOrbAction(ai); }
    static Action* yogg_saron_malady_of_the_mind_action(ShadowAI* ai) { return new YoggSaronMaladyOfTheMindAction(ai); }
    static Action* yogg_saron_mark_target_action(ShadowAI* ai) { return new YoggSaronMarkTargetAction(ai); }
    static Action* yogg_saron_brain_link_action(ShadowAI* ai) { return new YoggSaronBrainLinkAction(ai); }
    static Action* yogg_saron_move_to_enter_portal_action(ShadowAI* ai) { return new YoggSaronMoveToEnterPortalAction(ai); }
    static Action* yogg_saron_use_portal_action(ShadowAI* ai) { return new YoggSaronUsePortalAction(ai); }
    static Action* yogg_saron_fall_from_floor_action(ShadowAI* ai) { return new YoggSaronFallFromFloorAction(ai); }
    static Action* yogg_saron_boss_room_movement_cheat_action(ShadowAI* ai) { return new YoggSaronBossRoomMovementCheatAction(ai); }
    static Action* yogg_saron_illusion_room_action(ShadowAI* ai) { return new YoggSaronIllusionRoomAction(ai); }
    static Action* yogg_saron_move_to_exit_portal_action(ShadowAI* ai) { return new YoggSaronMoveToExitPortalAction(ai); }
    static Action* yogg_saron_lunatic_gaze_action(ShadowAI* ai) { return new YoggSaronLunaticGazeAction(ai); }
    static Action* yogg_saron_phase_3_positioning_action(ShadowAI* ai) { return new YoggSaronPhase3PositioningAction(ai); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// /*
//  * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
//  and/or modify it under version 2 of the License, or (at your option), any later version.
//  */



class RaidUlduarTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidUlduarTriggerContext()
    {
        creators["flame leviathan on vehicle"] = &RaidUlduarTriggerContext::flame_leviathan_on_vehicle;
        creators["flame leviathan vehicle near"] = &RaidUlduarTriggerContext::flame_leviathan_vehicle_near;
        creators["razorscale flying alone"] = &RaidUlduarTriggerContext::razorscale_flying_alone;
        creators["razorscale avoid devouring flames"] = &RaidUlduarTriggerContext::razorscale_avoid_devouring_flames;
        creators["razorscale avoid sentinel"] = &RaidUlduarTriggerContext::razorscale_avoid_sentinel;
        creators["razorscale avoid whirlwind"] = &RaidUlduarTriggerContext::razorscale_avoid_whirlwind;
        creators["razorscale grounded"] = &RaidUlduarTriggerContext::razorscale_grounded;
        creators["razorscale harpoon trigger"] = &RaidUlduarTriggerContext::razorscale_harpoon_trigger;
        creators["razorscale fuse armor trigger"] = &RaidUlduarTriggerContext::razorscale_fuse_armor_trigger;
        creators["razorscale fire resistance trigger"] = &RaidUlduarTriggerContext::razorscale_fire_resistance_trigger;
        creators["ignis fire resistance trigger"] = &RaidUlduarTriggerContext::ignis_fire_resistance_trigger;
        creators["iron assembly lightning tendrils trigger"] = &RaidUlduarTriggerContext::iron_assembly_lightning_tendrils_trigger;
        creators["iron assembly overload trigger"] = &RaidUlduarTriggerContext::iron_assembly_overload_trigger;
        creators["iron assembly rune of power trigger"] = &RaidUlduarTriggerContext::iron_assembly_rune_of_power_trigger;
        creators["kologarn mark dps target trigger"] = &RaidUlduarTriggerContext::kologarn_mark_dps_target_trigger;
        creators["kologarn fall from floor trigger"] = &RaidUlduarTriggerContext::kologarn_fall_from_floor_trigger;
        creators["kologarn nature resistance trigger"] = &RaidUlduarTriggerContext::kologarn_nature_resistance_trigger;
        creators["kologarn rubble slowdown trigger"] = &RaidUlduarTriggerContext::kologarn_rubble_slowdown_trigger;
        creators["kologarn eyebeam trigger"] = &RaidUlduarTriggerContext::kologarn_eyebeam_trigger;
        creators["kologarn rti target trigger"] = &RaidUlduarTriggerContext::kologarn_rti_target_trigger;
        creators["kologarn crunch armor trigger"] = &RaidUlduarTriggerContext::kologarn_crunch_armor_trigger;
        creators["kologarn attack dps target trigger"] = &RaidUlduarTriggerContext::kologarn_attack_dps_target_trigger;
        creators["auriaya fall from floor trigger"] = &RaidUlduarTriggerContext::auriaya_fall_from_floor_trigger;
        creators["hodir biting cold"] = &RaidUlduarTriggerContext::hodir_biting_cold;
        creators["hodir near snowpacked icicle"] = &RaidUlduarTriggerContext::hodir_near_snowpacked_icicle;
        creators["hodir frost resistance trigger"] = &RaidUlduarTriggerContext::hodir_frost_resistance_trigger;
        creators["freya near nature bomb"] = &RaidUlduarTriggerContext::freya_near_nature_bomb;
        creators["freya fire resistance trigger"] = &RaidUlduarTriggerContext::freya_fire_resistance_trigger;
        creators["freya nature resistance trigger"] = &RaidUlduarTriggerContext::freya_nature_resistance_trigger;
        creators["freya mark dps target trigger"] = &RaidUlduarTriggerContext::freya_mark_dps_target_trigger;
        creators["freya move to healing spore trigger"] = &RaidUlduarTriggerContext::freya_move_to_healing_spore_trigger;
        creators["thorim frost resistance trigger"] = &RaidUlduarTriggerContext::thorim_frost_resistance_trigger;
        creators["thorim nature resistance trigger"] = &RaidUlduarTriggerContext::thorim_nature_resistance_trigger;
        creators["thorim unbalancing strike trigger"] = &RaidUlduarTriggerContext::thorim_unbalancing_strike_trigger;
        creators["thorim mark dps target trigger"] = &RaidUlduarTriggerContext::thorim_mark_dps_target_trigger;
        creators["thorim arena positioning trigger"] = &RaidUlduarTriggerContext::thorim_arena_positioning_trigger;
        creators["thorim gauntlet positioning trigger"] = &RaidUlduarTriggerContext::thorim_gauntlet_positioning_trigger;
        creators["thorim fall from floor trigger"] = &RaidUlduarTriggerContext::thorim_fall_from_floor_trigger;
        creators["thorim phase 2 positioning trigger"] = &RaidUlduarTriggerContext::thorim_phase2_positioning_trigger;
        creators["mimiron fire resistance trigger"] = &RaidUlduarTriggerContext::mimiron_fire_resistance_trigger;
        creators["mimiron shock blast trigger"] = &RaidUlduarTriggerContext::mimiron_shock_blast_trigger;
        creators["mimiron phase 1 positioning trigger"] = &RaidUlduarTriggerContext::mimiron_phase_1_positioning_trigger;
        creators["mimiron p3wx2 laser barrage trigger"] = &RaidUlduarTriggerContext::mimiron_p3wx2_laser_barrage_trigger;
        creators["mimiron rapid burst trigger"] = &RaidUlduarTriggerContext::mimiron_rapid_burst_trigger;
        creators["mimiron aerial command unit trigger"] = &RaidUlduarTriggerContext::mimiron_aerial_command_unit_trigger;
        creators["mimiron rocket strike trigger"] = &RaidUlduarTriggerContext::mimiron_rocket_strike_trigger;
        creators["mimiron phase 4 mark dps trigger"] = &RaidUlduarTriggerContext::mimiron_phase_4_mark_dps_trigger;
        creators["mimiron cheat trigger"] = &RaidUlduarTriggerContext::mimiron_cheat_trigger;
        creators["vezax cheat trigger"] = &RaidUlduarTriggerContext::vezax_cheat_trigger;
        creators["vezax shadow crash trigger"] = &RaidUlduarTriggerContext::vezax_shadow_crash_trigger;
        creators["vezax mark of the faceless trigger"] = &RaidUlduarTriggerContext::vezax_mark_of_the_faceless_trigger;
        creators["vezax shadow resistance trigger"] = &RaidUlduarTriggerContext::vezax_shadow_resistance_trigger;
        creators["sara shadow resistance trigger"] = &RaidUlduarTriggerContext::sara_shadow_resistance_trigger;
        creators["yogg-saron shadow resistance triggerr"] = &RaidUlduarTriggerContext::yogg_saron_shadow_resistance_trigger;
        creators["yogg-saron ominous cloud cheat trigger"] = &RaidUlduarTriggerContext::yogg_saron_ominous_cloud_cheat_trigger;
        creators["yogg-saron guardian positioning trigger"] = &RaidUlduarTriggerContext::yogg_saron_guardian_positioning_trigger;
        creators["yogg-saron sanity trigger"] = &RaidUlduarTriggerContext::yogg_saron_sanity_trigger;
        creators["yogg-saron death orb trigger"] = &RaidUlduarTriggerContext::yogg_saron_death_orb_trigger;
        creators["yogg-saron malady of the mind trigger"] = &RaidUlduarTriggerContext::yogg_saron_malady_of_the_mind_trigger;
        creators["yogg-saron mark target trigger"] = &RaidUlduarTriggerContext::yogg_saron_mark_target_trigger;
        creators["yogg-saron brain link trigger"] = &RaidUlduarTriggerContext::yogg_saron_brain_link_trigger;
        creators["yogg-saron move to enter portal trigger"] = &RaidUlduarTriggerContext::yogg_saron_move_to_enter_portal_trigger;
        creators["yogg-saron use portal trigger"] = &RaidUlduarTriggerContext::yogg_saron_use_portal_trigger;
        creators["yogg-saron fall from floor trigger"] = &RaidUlduarTriggerContext::yogg_saron_fall_from_floor_trigger;
        creators["yogg-saron boss room movement cheat trigger"] = &RaidUlduarTriggerContext::yogg_saron_boss_room_movement_cheat_trigger;
        creators["yogg-saron illusion room trigger"] = &RaidUlduarTriggerContext::yogg_saron_illusion_room_trigger;
        creators["yogg-saron move to exit portal trigger"] = &RaidUlduarTriggerContext::yogg_saron_move_to_exit_portal_trigger;
        creators["yogg-saron lunatic gaze trigger"] = &RaidUlduarTriggerContext::yogg_saron_lunatic_gaze_trigger;
        creators["yogg-saron phase 3 positioning trigger"] = &RaidUlduarTriggerContext::yogg_saron_phase_3_positioning_trigger;
    }

private:
    static Trigger* flame_leviathan_on_vehicle(ShadowAI* ai) { return new FlameLeviathanOnVehicleTrigger(ai); }
    static Trigger* flame_leviathan_vehicle_near(ShadowAI* ai) { return new FlameLeviathanVehicleNearTrigger(ai); }
    static Trigger* razorscale_flying_alone(ShadowAI* ai) { return new RazorscaleFlyingAloneTrigger(ai); }
    static Trigger* razorscale_avoid_devouring_flames(ShadowAI* ai) { return new RazorscaleDevouringFlamesTrigger(ai); }
    static Trigger* razorscale_avoid_sentinel(ShadowAI* ai) { return new RazorscaleAvoidSentinelTrigger(ai); }
    static Trigger* razorscale_avoid_whirlwind(ShadowAI* ai) { return new RazorscaleAvoidWhirlwindTrigger(ai); }
    static Trigger* razorscale_grounded(ShadowAI* ai) { return new RazorscaleGroundedTrigger(ai); }
    static Trigger* razorscale_harpoon_trigger(ShadowAI* ai) { return new RazorscaleHarpoonAvailableTrigger(ai); }
    static Trigger* razorscale_fuse_armor_trigger(ShadowAI* ai) { return new RazorscaleFuseArmorTrigger(ai); }
    static Trigger* razorscale_fire_resistance_trigger(ShadowAI* ai) { return new BossFireResistanceTrigger(ai, "razorscale"); }
    static Trigger* ignis_fire_resistance_trigger(ShadowAI* ai) { return new BossFireResistanceTrigger(ai, "ignis the furnace master"); }
    static Trigger* iron_assembly_lightning_tendrils_trigger(ShadowAI* ai) { return new IronAssemblyLightningTendrilsTrigger(ai); }
    static Trigger* iron_assembly_overload_trigger(ShadowAI* ai) { return new IronAssemblyOverloadTrigger(ai); }
    static Trigger* iron_assembly_rune_of_power_trigger(ShadowAI* ai) { return new IronAssemblyRuneOfPowerTrigger(ai); }
    static Trigger* kologarn_mark_dps_target_trigger(ShadowAI* ai) { return new KologarnMarkDpsTargetTrigger(ai); }
    static Trigger* kologarn_fall_from_floor_trigger(ShadowAI* ai) { return new KologarnFallFromFloorTrigger(ai); }
    static Trigger* kologarn_nature_resistance_trigger(ShadowAI* ai) { return new BossNatureResistanceTrigger(ai, "kologarn"); }
    static Trigger* kologarn_rubble_slowdown_trigger(ShadowAI* ai) { return new KologarnRubbleSlowdownTrigger(ai); }
    static Trigger* kologarn_eyebeam_trigger(ShadowAI* ai) { return new KologarnEyebeamTrigger(ai); }
    static Trigger* kologarn_rti_target_trigger(ShadowAI* ai) { return new KologarnRtiTargetTrigger(ai); }
    static Trigger* kologarn_crunch_armor_trigger(ShadowAI* ai) { return new KologarnCrunchArmorTrigger(ai); }
    static Trigger* kologarn_attack_dps_target_trigger(ShadowAI* ai) { return new KologarnAttackDpsTargetTrigger(ai); }
    static Trigger* auriaya_fall_from_floor_trigger(ShadowAI* ai) { return new AuriayaFallFromFloorTrigger(ai); }
    static Trigger* hodir_biting_cold(ShadowAI* ai) { return new HodirBitingColdTrigger(ai); }
    static Trigger* hodir_near_snowpacked_icicle(ShadowAI* ai) { return new HodirNearSnowpackedIcicleTrigger(ai); }
    static Trigger* hodir_frost_resistance_trigger(ShadowAI* ai) { return new BossFrostResistanceTrigger(ai, "hodir"); }
    static Trigger* freya_near_nature_bomb(ShadowAI* ai) { return new FreyaNearNatureBombTrigger(ai); }
    static Trigger* freya_fire_resistance_trigger(ShadowAI* ai) { return new BossFireResistanceTrigger(ai, "freya"); }
    static Trigger* freya_nature_resistance_trigger(ShadowAI* ai) { return new BossNatureResistanceTrigger(ai, "freya"); }
    static Trigger* freya_mark_dps_target_trigger(ShadowAI* ai) { return new FreyaMarkDpsTargetTrigger(ai); }
    static Trigger* freya_move_to_healing_spore_trigger(ShadowAI* ai) { return new FreyaMoveToHealingSporeTrigger(ai); }
    static Trigger* thorim_frost_resistance_trigger(ShadowAI* ai) { return new BossFrostResistanceTrigger(ai, "thorim"); }
    static Trigger* thorim_nature_resistance_trigger(ShadowAI* ai) { return new BossNatureResistanceTrigger(ai, "thorim"); }
    static Trigger* thorim_unbalancing_strike_trigger(ShadowAI* ai) { return new ThorimUnbalancingStrikeTrigger(ai); }
    static Trigger* thorim_mark_dps_target_trigger(ShadowAI* ai) { return new ThorimMarkDpsTargetTrigger(ai); }
    static Trigger* thorim_arena_positioning_trigger(ShadowAI* ai) { return new ThorimArenaPositioningTrigger(ai); }
    static Trigger* thorim_gauntlet_positioning_trigger(ShadowAI* ai) { return new ThorimGauntletPositioningTrigger(ai); }
    static Trigger* thorim_fall_from_floor_trigger(ShadowAI* ai) { return new ThorimFallFromFloorTrigger(ai); }
    static Trigger* thorim_phase2_positioning_trigger(ShadowAI* ai) { return new ThorimPhase2PositioningTrigger(ai); }
    static Trigger* mimiron_fire_resistance_trigger(ShadowAI* ai) { return new BossFireResistanceTrigger(ai, "mimiron"); }
    static Trigger* mimiron_shock_blast_trigger(ShadowAI* ai) { return new MimironShockBlastTrigger(ai); }
    static Trigger* mimiron_phase_1_positioning_trigger(ShadowAI* ai) { return new MimironPhase1PositioningTrigger(ai); }
    static Trigger* mimiron_p3wx2_laser_barrage_trigger(ShadowAI* ai) { return new MimironP3Wx2LaserBarrageTrigger(ai); }
    static Trigger* mimiron_rapid_burst_trigger(ShadowAI* ai) { return new MimironRapidBurstTrigger(ai); }
    static Trigger* mimiron_aerial_command_unit_trigger(ShadowAI* ai) { return new MimironAerialCommandUnitTrigger(ai); }
    static Trigger* mimiron_rocket_strike_trigger(ShadowAI* ai) { return new MimironRocketStrikeTrigger(ai); }
    static Trigger* mimiron_phase_4_mark_dps_trigger(ShadowAI* ai) { return new MimironPhase4MarkDpsTrigger(ai); }
    static Trigger* mimiron_cheat_trigger(ShadowAI* ai) { return new MimironCheatTrigger(ai); }
    static Trigger* vezax_cheat_trigger(ShadowAI* ai) { return new VezaxCheatTrigger(ai); }
    static Trigger* vezax_shadow_crash_trigger(ShadowAI* ai) { return new VezaxShadowCrashTrigger(ai); }
    static Trigger* vezax_shadow_resistance_trigger(ShadowAI* ai) { return new BossShadowResistanceTrigger(ai, "general vezax"); }
    static Trigger* sara_shadow_resistance_trigger(ShadowAI* ai) { return new BossShadowResistanceTrigger(ai, "sara"); }
    static Trigger* yogg_saron_shadow_resistance_trigger(ShadowAI* ai) { return new BossShadowResistanceTrigger(ai, "yogg-saron"); }
    static Trigger* vezax_mark_of_the_faceless_trigger(ShadowAI* ai) { return new VezaxMarkOfTheFacelessTrigger(ai); }
    static Trigger* yogg_saron_ominous_cloud_cheat_trigger(ShadowAI* ai) { return new YoggSaronOminousCloudCheatTrigger(ai); }
    static Trigger* yogg_saron_guardian_positioning_trigger(ShadowAI* ai) { return new YoggSaronGuardianPositioningTrigger(ai); }
    static Trigger* yogg_saron_sanity_trigger(ShadowAI* ai) { return new YoggSaronSanityTrigger(ai); }
    static Trigger* yogg_saron_death_orb_trigger(ShadowAI* ai) { return new YoggSaronDeathOrbTrigger(ai); }
    static Trigger* yogg_saron_malady_of_the_mind_trigger(ShadowAI* ai) { return new YoggSaronMaladyOfTheMindTrigger(ai); }
    static Trigger* yogg_saron_mark_target_trigger(ShadowAI* ai) { return new YoggSaronMarkTargetTrigger(ai); }
    static Trigger* yogg_saron_brain_link_trigger(ShadowAI* ai) { return new YoggSaronBrainLinkTrigger(ai); }
    static Trigger* yogg_saron_move_to_enter_portal_trigger(ShadowAI* ai) { return new YoggSaronMoveToEnterPortalTrigger(ai); }
    static Trigger* yogg_saron_use_portal_trigger(ShadowAI* ai) { return new YoggSaronUsePortalTrigger(ai); }
    static Trigger* yogg_saron_fall_from_floor_trigger(ShadowAI* ai) { return new YoggSaronFallFromFloorTrigger(ai); }
    static Trigger* yogg_saron_boss_room_movement_cheat_trigger(ShadowAI* ai) { return new YoggSaronBossRoomMovementCheatTrigger(ai); }
    static Trigger* yogg_saron_illusion_room_trigger(ShadowAI* ai) { return new YoggSaronIllusionRoomTrigger(ai); }
    static Trigger* yogg_saron_move_to_exit_portal_trigger(ShadowAI* ai) { return new YoggSaronMoveToExitPortalTrigger(ai); }
    static Trigger* yogg_saron_lunatic_gaze_trigger(ShadowAI* ai) { return new YoggSaronLunaticGazeTrigger(ai); }
    static Trigger* yogg_saron_phase_3_positioning_trigger(ShadowAI* ai) { return new YoggSaronPhase3PositioningTrigger(ai); }
};

#endif
