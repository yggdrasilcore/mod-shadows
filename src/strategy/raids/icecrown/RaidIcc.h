#ifndef _SHADOW_RAIDICC_H
#define _SHADOW_RAIDICC_H

#include "ShadowAI.h"
#include "Shadows.h"
#include "Trigger.h"
#include "Multiplier.h"
#include "AiObjectContext.h"
#include "Strategy.h"
#include "Action.h"
#include "MovementActions.h"
#include "AttackAction.h"
#include "LastMovementValue.h"
#include "ObjectGuid.h"
#include "ShadowAIConfig.h"
#include "ScriptedCreature.h"
#include "SharedDefines.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Vehicle.h"
#include "NamedObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum CreatureIdsICC
{

    // Lord Marrowgar
    NPC_SPIKE1                          = 36619,
    NPC_SPIKE2                          = 38711,
    NPC_SPIKE3                          = 38712,

    // Lady Deathwhisper
    NPC_SHADE                           = 38222,

    // Gunship Battle
    NPC_KOR_KRON_BATTLE_MAGE            = 37117,
    NPC_KOR_KRON_AXETHROWER             = 36968,
    NPC_KOR_KRON_ROCKETEER              = 36982,
    NPC_SKYBREAKER_SORCERER             = 37116,
    NPC_SKYBREAKER_RIFLEMAN             = 36969,
    NPC_SKYBREAKER_MORTAR_SOLDIER       = 36978,
    NPC_IGB_HIGH_OVERLORD_SAURFANG      = 36939,
    NPC_IGB_MURADIN_BRONZEBEARD         = 36948,
    NPC_CANNONA                         = 36838,
    NPC_CANNONH                         = 36839,
    NPC_MURADIN_BRONZEBEARD             = 36948,
    NPC_HIGH_OVERLORD_SAURFANG          = 36939,

    // Deathbringer Saurfang
    NPC_BLOOD_BEAST1                    = 38508,
    NPC_BLOOD_BEAST2                    = 38596,
    NPC_BLOOD_BEAST3                    = 38597,
    NPC_BLOOD_BEAST4                    = 38598,

    // Rotface
    NPC_PUDDLE                          = 37013,
    NPC_BIG_OOZE                        = 36899,

    // Putricide
    NPC_MALLEABLE_OOZE_STALKER          = 38556,
    NPC_GROWING_OOZE_PUDDLE             = 37690,
    NPC_CHOKING_GAS_BOMB                = 38159,

    // Blood Prince Council
    NPC_DARK_NUCLEUS                    = 38369,
    NPC_PRINCE_KELESETH                 = 37972,
    NPC_PRINCE_TALDARAM                 = 37973,
    NPC_PRINCE_VALANAR                  = 37970,
    NPC_KINETIC_BOMB1                   = 38454,
    NPC_KINETIC_BOMB2                   = 38775,
    NPC_KINETIC_BOMB3                   = 38776,
    NPC_KINETIC_BOMB4                   = 38777,
    NPC_BALL_OF_FLAME                   = 38332,
    NPC_BALL_OF_INFERNO_FLAME           = 38451,

    // Blood Queen Lana'thel
    NPC_SWARMING_SHADOWS                = 38163,

    // Sister Svalna
    NPC_SPEAR                           = 38248,
    ITEM_SPEAR                          = 50307,

    // Valithria Dreamwalker
    NPC_VALITHRIA_DREAMWALKER           = 36789,
    NPC_DREAM_PORTAL                    = 37945,
    NPC_NIGHTMARE_PORTAL                = 38430,
    NPC_DREAM_CLOUD                     = 37985,
    NPC_NIGHTMARE_CLOUD                 = 38421,
    NPC_RISEN_ARCHMAGE                  = 37868,
    NPC_BLAZING_SKELETON                = 36791,
    NPC_SUPPRESSER                      = 37863,
    NPC_BLISTERING_ZOMBIE               = 37934,
    NPC_GLUTTONOUS_ABOMINATION          = 37886,
    NPC_ROT_WORM                        = 37907,
    NPC_COLUMN_OF_FROST                 = 37918,
    NPC_MANA_VOID                       = 38068,
    NPC_DREAM_PORTAL_PRE_EFFECT         = 38186,
    NPC_NIGHTMARE_PORTAL_PRE_EFFECT     = 38429,

    // Sindragosa
    NPC_SINDRAGOSA                      = 36853,
    NPC_TOMB1                           = 36980,
    NPC_TOMB2                           = 38320,
    NPC_TOMB3                           = 38321,
    NPC_TOMB4                           = 38322,

    // Lich King
    NPC_THE_LICH_KING                   = 36597,
    NPC_TERENAS_MENETHIL                = 36823,
    NPC_TERENAS_MENETHIL_HC             = 39217,
    NPC_SPIRIT_BOMB                     = 39189,
    NPC_WICKED_SPIRIT1                  = 39190,
    NPC_WICKED_SPIRIT2                  = 39287,
    NPC_WICKED_SPIRIT3                  = 39288,
    NPC_WICKED_SPIRIT4                  = 39289,
    NPC_SHADOW_TRAP                     = 39137,
    NPC_SHAMBLING_HORROR1               = 37698,
    NPC_SHAMBLING_HORROR2               = 39299,
    NPC_SHAMBLING_HORROR3               = 39300,
    NPC_SHAMBLING_HORROR4               = 39301,
    NPC_ICE_SPHERE1                     = 36633,
    NPC_ICE_SPHERE2                     = 39305,
    NPC_ICE_SPHERE3                     = 39306,
    NPC_ICE_SPHERE4                     = 39307,
    NPC_RAGING_SPIRIT1                  = 36701,
    NPC_RAGING_SPIRIT2                  = 39302,
    NPC_RAGING_SPIRIT3                  = 39303,
    NPC_RAGING_SPIRIT4                  = 39304,
    NPC_DRUDGE_GHOUL1                   = 37695,
    NPC_DRUDGE_GHOUL2                   = 39309,
    NPC_DRUDGE_GHOUL3                   = 39310,
    NPC_DRUDGE_GHOUL4                   = 39311,
    NPC_VALKYR_SHADOWGUARD1             = 36609,
    NPC_VALKYR_SHADOWGUARD2             = 39120,
    NPC_VALKYR_SHADOWGUARD3             = 39121,
    NPC_VALKYR_SHADOWGUARD4             = 39122,
    NPC_VILE_SPIRIT1                    = 37799,
    NPC_VILE_SPIRIT2                    = 39284,
    NPC_VILE_SPIRIT3                    = 39285,
    NPC_VILE_SPIRIT4                    = 39286,

};

enum SpellIdsICC
{
    // ICC cheat spells
    SPELL_EMPOWERED_BLOOD               = 70227, //70304 -->50%, 70227 /*100% more dmg, 100% more att speed*/
    SPELL_EXPERIENCED                   = 71188, //dmg 30% 20% att speed
    SPELL_NO_THREAT                     = 70115, //reduce threat
    SPELL_SPITEFULL_FURY                = 36886, //500% more threat
    SPELL_NITRO_BOOSTS                  = 54861, //Speed
    SPELL_PAIN_SUPPRESION               = 69910, //40% dmg reduction
    SPELL_AGEIS_OF_DALARAN              = 71638, //268 all ress
    SPELL_CYCLONE                       = 33786,
    SPELL_HAMMER_OF_JUSTICE             = 10308, //stun

    // Lady Deathwhisper
    SPELL_DARK_RECKONING                = 69483,

    // Gunship Battle
    SPELL_DEATH_PLAGUE                  = 72865,
    SPELL_BELOW_ZERO                    = 69705,

    // Festergut
    SPELL_GAS_SPORE                     = 69279,

    // Rotface
    SPELL_SLIME_SPRAY                   = 69508,
    SPELL_OOZE_FLOOD                    = 71215,
    SPELL_UNSTABLE_OOZE_EXPLOSION       = 69839,
    SPELL_OOZE_FLOOD_VISUAL             = 69785,

    // Putricide
    SPELL_MALLEABLE_GOO                 = 70852,
    SPELL_GROW_AURA                     = 70347,

    // Blood Prince Council
    SPELL_EMPOWERED_SHOCK_VORTEX1       = 72039,
    SPELL_EMPOWERED_SHOCK_VORTEX2       = 73037,
    SPELL_EMPOWERED_SHOCK_VORTEX3       = 73038,
    SPELL_EMPOWERED_SHOCK_VORTEX4       = 73039,

    // Blood Queen Lana'thel
    SPELL_PACT_OF_THE_DARKFALLEN        = 71340,

    // Sister Svalna
    SPELL_AETHER_SHIELD                 = 71463,

    // Valithria Dreamwalker
    SPELL_DREAM_STATE                   = 70766,
    SPELL_EMERALD_VIGOR                 = 70873,

    // Sindragosa
    SPELL_FROST_BEACON                  = 70126,
    SPELL_ICE_TOMB                      = 70157,
    SPELL_FROST_BOMB_VISUAL             = 70022,
    SPELL_BLISTERING_COLD1              = 70123,
    SPELL_BLISTERING_COLD2              = 71047,
    SPELL_BLISTERING_COLD3              = 71048,
    SPELL_BLISTERING_COLD4              = 71049,

    // The Lich King
    SPELL_HARVEST_SOUL_VALKYR           = 68985,
    SPELL_QUAKE                         = 72262,
    SPELL_REMORSELESS_WINTER1           = 72259,
    SPELL_REMORSELESS_WINTER2           = 74273,
    SPELL_REMORSELESS_WINTER3           = 74274,
    SPELL_REMORSELESS_WINTER4           = 74275,
    SPELL_REMORSELESS_WINTER5           = 68981,
    SPELL_REMORSELESS_WINTER6           = 74270,
    SPELL_REMORSELESS_WINTER7           = 74271,
    SPELL_REMORSELESS_WINTER8           = 74272,
};

const uint32 DEFILE_AURAS[] = {72756, 74162, 74163, 74164};
const uint32 DEFILE_CAST_ID = 72762;
const uint32 DEFILE_NPC_ID = 38757;
const size_t DEFILE_AURA_COUNT = 4;

// All fanatics and adherents entry ids Lady Deathwhisper
static const std::array<uint32, 23> addEntriesLady    = {
                                                        37949, 38394, 38625, 38626, 38010, 38397, 39000, 39001,
                                                        38136, 38396, 38632, 38633, 37890, 38393, 38628, 38629,
                                                        38135, 38395, 38634, 38009, 38398, 38630, 38631};

const std::vector<uint32> spellEntriesFlood           = {
                                                        69782, 69783, 69796, 69797, 69798,
                                                        69799, 69801, 69802, 69795};

const std::vector<uint32> availableTargetsGS          = {
    NPC_KOR_KRON_AXETHROWER, NPC_KOR_KRON_ROCKETEER,        NPC_KOR_KRON_BATTLE_MAGE, NPC_IGB_HIGH_OVERLORD_SAURFANG,
    NPC_SKYBREAKER_RIFLEMAN, NPC_SKYBREAKER_MORTAR_SOLDIER, NPC_SKYBREAKER_SORCERER,  NPC_IGB_MURADIN_BRONZEBEARD};

static std::vector<ObjectGuid> sporeOrder;

//Lord Marrowgar
class IccLmTrigger : public Trigger
{
public:
    IccLmTrigger(ShadowAI* botAI) : Trigger(botAI, "icc lm") {}
    bool IsActive() override;
};

//Lady Deathwhisper
class IccDarkReckoningTrigger : public Trigger
{
public:
    IccDarkReckoningTrigger(ShadowAI* botAI) : Trigger(botAI, "icc dark reckoning") {}
    bool IsActive() override;
};

class IccLadyDeathwhisperTrigger : public Trigger
{
public:
    IccLadyDeathwhisperTrigger(ShadowAI* botAI) : Trigger(botAI, "icc lady deathwhisper") {}
    bool IsActive() override;
};

//Gunship Battle
class IccRottingFrostGiantTankPositionTrigger : public Trigger
{
public:
    IccRottingFrostGiantTankPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc rotting frost giant tank position") {}
    bool IsActive() override;
};

class IccInCannonTrigger : public Trigger
{
public:
    IccInCannonTrigger(ShadowAI* botAI) : Trigger(botAI, "icc in cannon") {}
    bool IsActive() override;
};

class IccGunshipCannonNearTrigger : public Trigger
{
public:
    IccGunshipCannonNearTrigger(ShadowAI* botAI) : Trigger(botAI, "icc in cannon") {}
    bool IsActive() override;
};

class IccGunshipTeleportAllyTrigger : public Trigger
{
public:
    IccGunshipTeleportAllyTrigger(ShadowAI* botAI) : Trigger(botAI, "icc gunship teleport ally") {}
    bool IsActive() override;
};

class IccGunshipTeleportHordeTrigger : public Trigger
{
public:
    IccGunshipTeleportHordeTrigger(ShadowAI* botAI) : Trigger(botAI, "icc gunship teleport horde") {}
    bool IsActive() override;
};

//DBS
class IccDbsTrigger : public Trigger
{
public:
    IccDbsTrigger(ShadowAI* botAI) : Trigger(botAI, "icc dbs") {}
    bool IsActive() override;
};

class IccDbsMainTankRuneOfBloodTrigger : public Trigger
{
public:
    IccDbsMainTankRuneOfBloodTrigger(ShadowAI* botAI) : Trigger(botAI, "icc dbs main tank rune of blood") {}
    bool IsActive() override;
};

//DOGS
class IccStinkyPreciousMainTankMortalWoundTrigger : public Trigger
{
public:
    IccStinkyPreciousMainTankMortalWoundTrigger(ShadowAI* botAI) : Trigger(botAI, "icc stinky precious main tank mortal wound") {}
    bool IsActive() override;
};

//FESTERGUT
class IccFestergutGroupPositionTrigger : public Trigger
{
public:
    IccFestergutGroupPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc festergut group position") {}
    bool IsActive() override;
};

class IccFestergutMainTankGastricBloatTrigger : public Trigger
{
public:
    IccFestergutMainTankGastricBloatTrigger(ShadowAI* botAI) : Trigger(botAI, "icc festergut main tank gastric bloat") {}
    bool IsActive() override;
};

class IccFestergutSporeTrigger : public Trigger
{
public:
    IccFestergutSporeTrigger(ShadowAI* botAI) : Trigger(botAI, "icc festergut spore") {}
    bool IsActive() override;
};

//ROTFACE
class IccRotfaceTankPositionTrigger : public Trigger
{
public:
    IccRotfaceTankPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc rotface tank position") {}
    bool IsActive() override;
};

class IccRotfaceGroupPositionTrigger : public Trigger
{
public:
    IccRotfaceGroupPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc rotface group position") {}
    bool IsActive() override;
};


class IccRotfaceMoveAwayFromExplosionTrigger : public Trigger
{
public:
    IccRotfaceMoveAwayFromExplosionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc rotface move away from explosion") {}
    bool IsActive() override;
};

//PP
class IccPutricideVolatileOozeTrigger : public Trigger
{
public:
    IccPutricideVolatileOozeTrigger(ShadowAI* botAI) : Trigger(botAI, "icc putricide volatile ooze") {}
    bool IsActive() override;
};

class IccPutricideGasCloudTrigger : public Trigger
{
public:
    IccPutricideGasCloudTrigger(ShadowAI* botAI) : Trigger(botAI, "icc putricide gas cloud") {}
    bool IsActive() override;
};


class IccPutricideGrowingOozePuddleTrigger : public Trigger
{
public:
    IccPutricideGrowingOozePuddleTrigger(ShadowAI* botAI) : Trigger(botAI, "icc putricide growing ooze puddle") {}
    bool IsActive() override;
};

class IccPutricideMainTankMutatedPlagueTrigger : public Trigger
{
public:
    IccPutricideMainTankMutatedPlagueTrigger(ShadowAI* botAI) : Trigger(botAI, "icc putricide main tank mutated plague") {}
    bool IsActive() override;
};

class IccPutricideMalleableGooTrigger : public Trigger
{
public:
    IccPutricideMalleableGooTrigger(ShadowAI* ai) : Trigger(ai, "icc putricide malleable goo") {}
    bool IsActive() override;
};

//BPC
class IccBpcKelesethTankTrigger : public Trigger
{
public:
    IccBpcKelesethTankTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bpc keleseth tank") {}
    bool IsActive() override;
};

class IccBpcMainTankTrigger : public Trigger
{
public:
    IccBpcMainTankTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bpc main tank") {}
    bool IsActive() override;
};

class IccBpcEmpoweredVortexTrigger : public Trigger
{
public:
    IccBpcEmpoweredVortexTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bpc empowered vortex") {}
    bool IsActive() override;
};

class IccBpcKineticBombTrigger : public Trigger
{
public:
    IccBpcKineticBombTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bpc kinetic bomb") {}
    bool IsActive() override;
};

class IccBpcBallOfFlameTrigger : public Trigger
{
public:
    IccBpcBallOfFlameTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bpc ball of flame") {}
    bool IsActive() override;
};

//Bql
class IccBqlGroupPositionTrigger : public Trigger
{
public:
    IccBqlGroupPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bql tank position") {}
    bool IsActive() override;
};

class IccBqlPactOfDarkfallenTrigger : public Trigger
{
public:
    IccBqlPactOfDarkfallenTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bql pact of darkfallen") {}
    bool IsActive() override;
};

class IccBqlVampiricBiteTrigger : public Trigger
{
public:
    IccBqlVampiricBiteTrigger(ShadowAI* botAI) : Trigger(botAI, "icc bql vampiric bite") {}
    bool IsActive() override;
};

// Sister Svalna
class IccValkyreSpearTrigger : public Trigger
{
public:
    IccValkyreSpearTrigger(ShadowAI* botAI) : Trigger(botAI, "icc valkyre spear") {}
    bool IsActive() override;
};

class IccSisterSvalnaTrigger : public Trigger
{
public:
    IccSisterSvalnaTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sister svalna") {}
    bool IsActive() override;
};


// Valithria Dreamwalker

class IccValithriaGroupTrigger : public Trigger
{
public:
    IccValithriaGroupTrigger(ShadowAI* botAI) : Trigger(botAI, "icc valithria group") {}
    bool IsActive() override;
};

class IccValithriaPortalTrigger : public Trigger
{
public:
    IccValithriaPortalTrigger(ShadowAI* botAI) : Trigger(botAI, "icc valithria portal") {}
    bool IsActive() override;
};

class IccValithriaHealTrigger : public Trigger
{
public:
    IccValithriaHealTrigger(ShadowAI* botAI) : Trigger(botAI, "icc valithria heal") {}
    bool IsActive() override;
};

class IccValithriaDreamCloudTrigger : public Trigger
{
public:
    IccValithriaDreamCloudTrigger(ShadowAI* botAI) : Trigger(botAI, "icc valithria dream cloud") {}
    bool IsActive() override;
};


//SINDRAGOSA
class IccSindragosaGroupPositionTrigger : public Trigger
{
public:
    IccSindragosaGroupPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa group position") {}
    bool IsActive() override;
};

class IccSindragosaFrostBeaconTrigger : public Trigger
{
public:
    IccSindragosaFrostBeaconTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa frost beacon") {}
    bool IsActive() override;
};

class IccSindragosaBlisteringColdTrigger : public Trigger
{
public:
    IccSindragosaBlisteringColdTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa blistering cold") {}
    bool IsActive() override;
};

class IccSindragosaUnchainedMagicTrigger : public Trigger
{
public:
    IccSindragosaUnchainedMagicTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa unchained magic") {}
    bool IsActive() override;
};

class IccSindragosaChilledToTheBoneTrigger : public Trigger
{
public:
    IccSindragosaChilledToTheBoneTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa chilled to the bone") {}
    bool IsActive() override;
};

class IccSindragosaMysticBuffetTrigger : public Trigger
{
public:
    IccSindragosaMysticBuffetTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa mystic buffet") {}
    bool IsActive() override;
};

class IccSindragosaMainTankMysticBuffetTrigger : public Trigger
{
public:
    IccSindragosaMainTankMysticBuffetTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa main tank mystic buffet") {}
    bool IsActive() override;
};

class IccSindragosaTankSwapPositionTrigger : public Trigger
{
public:
    IccSindragosaTankSwapPositionTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa tank swap position") {}
    bool IsActive() override;
};

class IccSindragosaFrostBombTrigger : public Trigger
{
public:
    IccSindragosaFrostBombTrigger(ShadowAI* botAI) : Trigger(botAI, "icc sindragosa frost bomb") {}
    bool IsActive() override;
};


//LICH KING
class IccLichKingShadowTrapTrigger : public Trigger
{
public:
    IccLichKingShadowTrapTrigger(ShadowAI* botAI) : Trigger(botAI, "icc lich king shadow trap") {}
    bool IsActive() override;
};

class IccLichKingNecroticPlagueTrigger : public Trigger
{
public:
    IccLichKingNecroticPlagueTrigger(ShadowAI* botAI) : Trigger(botAI, "icc lich king necrotic plague") {}
    bool IsActive() override;
};

class IccLichKingWinterTrigger : public Trigger
{
public:
    IccLichKingWinterTrigger(ShadowAI* botAI) : Trigger(botAI, "icc lich king winter") {}
    bool IsActive() override;
};

class IccLichKingAddsTrigger : public Trigger
{
public:
    IccLichKingAddsTrigger(ShadowAI* botAI) : Trigger(botAI, "icc lich king adds") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//Lady Deathwhisper
class IccLadyDeathwhisperMultiplier : public Multiplier
{
public:
    IccLadyDeathwhisperMultiplier(ShadowAI* ai) : Multiplier(ai, "icc lady deathwhisper") {}
    virtual float GetValue(Action* action);
};

//DBS
class IccAddsDbsMultiplier : public Multiplier
{
public:
    IccAddsDbsMultiplier(ShadowAI* ai) : Multiplier(ai, "icc adds dbs") {}
    virtual float GetValue(Action* action);
};

//DOGS

class IccDogsMultiplier : public Multiplier
{
public:
    IccDogsMultiplier(ShadowAI* ai) : Multiplier(ai, "icc dogs") {}
    virtual float GetValue(Action* action);
};

//FESTERGUT
class IccFestergutMultiplier : public Multiplier
{
public:
    IccFestergutMultiplier(ShadowAI* ai) : Multiplier(ai, "icc festergut") {}
    virtual float GetValue(Action* action);
};

//ROTFACE
class IccRotfaceMultiplier : public Multiplier
{
public:
    IccRotfaceMultiplier(ShadowAI* ai) : Multiplier(ai, "icc rotface") {}
    virtual float GetValue(Action* action);
};

/*class IccRotfaceGroupPositionMultiplier : public Multiplier
{
public:
    IccRotfaceGroupPositionMultiplier(ShadowAI* ai) : Multiplier(ai, "icc rotface group position") {}
    virtual float GetValue(Action* action);
};*/

//PP
class IccAddsPutricideMultiplier : public Multiplier
{
public:
    IccAddsPutricideMultiplier(ShadowAI* ai) : Multiplier(ai, "icc adds putricide") {}
    virtual float GetValue(Action* action);
};

//BPC
class IccBpcAssistMultiplier : public Multiplier
{
public:
    IccBpcAssistMultiplier(ShadowAI* botAI) : Multiplier(botAI, "icc bpc assist") {}
    virtual float GetValue(Action* action);
};

//BQL
class IccBqlMultiplier : public Multiplier
{
public:
    IccBqlMultiplier(ShadowAI* botAI) : Multiplier(botAI, "icc bql multiplier") {}
    virtual float GetValue(Action* action) override;
};

//VDW
class IccValithriaDreamCloudMultiplier : public Multiplier
{
public:
    IccValithriaDreamCloudMultiplier(ShadowAI* ai) : Multiplier(ai, "icc valithria dream cloud") {}
    virtual float GetValue(Action* action);
};

//SINDRAGOSA
class IccSindragosaMultiplier : public Multiplier
{
public:
    IccSindragosaMultiplier(ShadowAI* ai) : Multiplier(ai, "icc sindragosa") {}
    virtual float GetValue(Action* action);
};

//LK
class IccLichKingAddsMultiplier : public Multiplier
{
public:
    IccLichKingAddsMultiplier(ShadowAI* ai) : Multiplier(ai, "icc lich king adds") {}
    virtual float GetValue(Action* action);
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidIccStrategy : public Strategy
{
public:
    RaidIccStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "icc"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
    virtual void InitMultipliers(std::vector<Multiplier*> &multipliers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const Position ICC_LM_TANK_POSITION = Position(-391.0f, 2259.0f, 42.0f);
const Position ICC_DARK_RECKONING_SAFE_POSITION = Position(-523.33386f, 2211.2031f, 62.823116f);
const Position ICC_LDW_TANK_POSTION = Position(-570.1f, 2211.2456f, 49.476616f); //-590.0f
const Position ICC_ROTTING_FROST_GIANT_TANK_POSITION = Position(-328.5085f, 2225.5142f, 199.97298f);
const Position ICC_GUNSHIP_TELEPORT_ALLY = Position (-370.04645f, 1993.3536f, 466.65656f);
const Position ICC_GUNSHIP_TELEPORT_ALLY2 = Position (-392.66208f, 2064.893f, 466.5672f, 5.058196f);
const Position ICC_GUNSHIP_TELEPORT_HORDE = Position (-449.5343f, 2477.2024f, 470.17648f);
const Position ICC_GUNSHIP_TELEPORT_HORDE2 = Position (-429.81586f, 2400.6804f, 471.56537f);
const Position ICC_DBS_TANK_POSITION = Position(-494.26517f, 2211.549f, 541.11414f);
const Position ICC_FESTERGUT_TANK_POSITION = Position(4269.1772f, 3144.7673f, 360.38577f);
const Position ICC_FESTERGUT_RANGED_SPORE = Position(4261.143f, 3109.4146f, 360.38605f);
const Position ICC_FESTERGUT_MELEE_SPORE = Position(4269.1772f, 3144.7673f, 360.38577f);
const Position ICC_ROTFACE_TANK_POSITION = Position(4447.061f, 3150.9758f, 360.38568f);
const Position ICC_ROTFACE_BIG_OOZE_POSITION = Position(4432.687f, 3142.3035f, 360.38623f);
const Position ICC_ROTFACE_SAFE_POSITION = Position(4446.557f, 3065.6594f, 360.51843f);
const Position ICC_ROTFACE_CENTER_POSITION = Position(4446.0547f, 3144.8677f, 360.38593f); //actual center 4.74089 4445.6616f, 3137.1526f, 360.38608
const Position ICC_PUTRICIDE_TANK_POSITION = Position(4373.227f, 3222.058f, 389.4029f);
const Position ICC_PUTRICIDE_GREEN_POSITION = Position(4423.4126f, 3194.2715f, 389.37683f);
const Position ICC_PUTRICIDE_BAD_POSITION = Position(4356.1724f, 3261.5232f, 389.3985f);
//const Position ICC_PUTRICIDE_GAS3_POSITION = Position(4367.753f, 3177.5894f, 389.39575f);
//const Position ICC_PUTRICIDE_GAS4_POSITION = Position(4321.8486f, 3206.464f, 389.3982f);
const Position ICC_BPC_OT_POSITION = Position(4649.2236f, 2796.0972f, 361.1815f);
const Position ICC_BPC_MT_POSITION = Position(4648.5674f, 2744.847f, 361.18222f);
const Position ICC_BQL_CENTER_POSITION = Position(4595.0f, 2769.0f, 400.0f);
const Position ICC_BQL_LWALL1_POSITION = Position(4624.685f, 2789.4895f, 400.13834f);
const Position ICC_BQL_LWALL2_POSITION = Position(4600.749f, 2805.7568f, 400.1374f);
const Position ICC_BQL_LWALL3_POSITION = Position(4572.857f, 2797.3872f, 400.1374f);
const Position ICC_BQL_RWALL1_POSITION = Position(4625.724f, 2748.9917f, 400.13693f);
const Position ICC_BQL_RWALL2_POSITION = Position(4608.3774f, 2735.7466f, 400.13693f);
const Position ICC_BQL_RWALL3_POSITION = Position(4576.813f, 2739.6067f, 400.13693f);
const Position ICC_BQL_LRWALL4_POSITION = Position(4539.345f, 2769.3853f, 403.7267f);
const Position ICC_BQL_TANK_POSITION = Position(4629.746f, 2769.6396f, 401.7479f); //old just in front of stairs 4616.102f, 2768.9167f, 400.13797f
const Position ICC_VDW_HEAL_POSITION = Position(4203.752f, 2483.4343f, 364.87274f);
const Position ICC_VDW_GROUP1_POSITION = Position(4203.585f, 2464.422f, 364.86887f);
const Position ICC_VDW_GROUP2_POSITION = Position(4203.5806f, 2505.2383f, 364.87677f);
const Position ICC_VDW_PORTALSTART_POSITION = Position(4202.637f, 2488.171f, 375.00256f);
const Position ICC_SINDRAGOSA_TANK_POSITION = Position(4408.016f, 2508.0647f, 203.37955f);
const Position ICC_SINDRAGOSA_FLYING_POSITION = Position(4525.6f, 2485.15f, 245.082f);
const Position ICC_SINDRAGOSA_RANGED_POSITION = Position(4441.572f, 2484.482f, 203.37836f);
const Position ICC_SINDRAGOSA_MELEE_POSITION = Position(4423.4546f, 2491.7175f, 203.37686f);
const Position ICC_SINDRAGOSA_BLISTERING_COLD_POSITION = Position(4473.6616f, 2484.8489f, 203.38258f);
const Position ICC_SINDRAGOSA_THOMB1_POSITION = Position(4433.6484f, 2469.4133f, 203.3806f);
const Position ICC_SINDRAGOSA_THOMB2_POSITION = Position(4434.143f, 2486.201f, 203.37473f);
const Position ICC_SINDRAGOSA_THOMB3_POSITION = Position(4436.1147f, 2501.464f, 203.38266f);
const Position ICC_SINDRAGOSA_UNCHAINEDMAGIC1_POSITION = Position(4444.9707f, 2455.7322f, 203.38701f);
const Position ICC_SINDRAGOSA_UNCHAINEDMAGIC2_POSITION = Position(4461.3945f, 2463.5513f, 203.38727f);
const Position ICC_SINDRAGOSA_UNCHAINEDMAGIC3_POSITION = Position(4473.6616f, 2484.8489f, 203.38258f);
const Position ICC_SINDRAGOSA_UNCHAINEDMAGIC4_POSITION = Position(4459.9336f, 2507.409f, 203.38606f);
const Position ICC_SINDRAGOSA_UNCHAINEDMAGIC5_POSITION = Position(4442.3096f, 2512.4688f, 203.38647f);
const Position ICC_SINDRAGOSA_CENTER_POSITION = Position(4408.0464f, 2484.478f, 203.37529f);
const Position ICC_SINDRAGOSA_THOMBMB2_POSITION = Position(4436.895f, 2498.1401f, 203.38133f);
const Position ICC_SINDRAGOSA_FBOMB_POSITION = Position(4449.3647f, 2486.4524f, 203.379f);
const Position ICC_SINDRAGOSA_FBOMB10_POSITION = Position(4449.3647f, 2486.4524f, 203.379f);
const Position ICC_SINDRAGOSA_LOS2_POSITION = Position(4441.8286f, 2501.946f, 203.38435f);
const Position ICC_LICH_KING_ADDS_POSITION = Position(479.7332f, -2095.3894f, 840.857f);  // old 486.63647f, -2095.7915f, 840.857f
const Position ICC_LICH_KING_MELEE_POSITION = Position(503.5546f, -2106.8213f, 840.857f);
const Position ICC_LICH_KING_RANGED_POSITION = Position(501.3563f, -2085.1816f, 840.857f);
const Position ICC_LICH_KING_ASSISTHC_POSITION = Position(517.2145f, -2125.0674f, 840.857f);
const Position ICC_LK_FROST1_POSITION = Position(503.96548f, -2183.216f, 840.857f);
const Position ICC_LK_FROST2_POSITION = Position(563.07166f, -2125.7578f, 840.857f);
const Position ICC_LK_FROST3_POSITION = Position(503.40182f, -2067.3435f, 840.857f);
const Position ICC_LK_FROSTR1_POSITION = Position(481.168f, -2177.8723f, 840.857f);
const Position ICC_LK_FROSTR2_POSITION = Position(562.20807f, -2100.2393f, 840.857f);
const Position ICC_LK_FROSTR3_POSITION = Position(526.35297f, -2071.0317f, 840.857f);

//Lord Marrogwar
class IccLmTankPositionAction : public AttackAction
{
public:
    IccLmTankPositionAction(ShadowAI* botAI, std::string const name = "icc lm tank position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    bool MoveTowardPosition(const Position& position, float incrementSize);
};

class IccSpikeAction : public AttackAction
{
public:
    IccSpikeAction(ShadowAI* botAI) : AttackAction(botAI, "icc spike") {}
    bool Execute(Event event) override;

    bool HandleSpikeTargeting(Unit* boss);
    bool MoveTowardPosition(const Position& position, float incrementSize);
    void UpdateRaidTargetIcon(Unit* target);
};

//Lady Deathwhisper
class IccDarkReckoningAction : public MovementAction
{
public:
    IccDarkReckoningAction(ShadowAI* botAI, std::string const name = "icc dark reckoning")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

class IccRangedPositionLadyDeathwhisperAction : public AttackAction
{
public:
    IccRangedPositionLadyDeathwhisperAction(ShadowAI* botAI, std::string const name = "icc ranged position lady deathwhisper")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    bool MaintainRangedSpacing();
};

class IccAddsLadyDeathwhisperAction : public AttackAction
{
public:
    IccAddsLadyDeathwhisperAction(ShadowAI* botAI, std::string const name = "icc adds lady deathwhisper")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    bool IsTargetedByShade(uint32 shadeEntry);
    bool MoveTowardPosition(const Position& position, float incrementSize);
    bool HandleAddTargeting(Unit* boss);
    void UpdateRaidTargetIcon(Unit* target);

};

class IccShadeLadyDeathwhisperAction : public MovementAction
{
public:
    IccShadeLadyDeathwhisperAction(ShadowAI* botAI, std::string const name = "icc shade lady deathwhisper")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;
};

//Gunship Battle
class IccRottingFrostGiantTankPositionAction : public AttackAction
{
public:
    IccRottingFrostGiantTankPositionAction(ShadowAI* botAI, std::string const name = "icc rotting frost giant tank position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;
};

class IccCannonFireAction : public Action
{
public:
    IccCannonFireAction(ShadowAI* botAI, std::string const name = "icc cannon fire")
        : Action(botAI, name) {}
    bool Execute(Event event) override;

    Unit* FindValidCannonTarget();
    bool TryCastCannonSpell(uint32 spellId, Unit* target, Unit* vehicleBase);
};

class IccGunshipEnterCannonAction : public MovementAction
{
public:
    IccGunshipEnterCannonAction(ShadowAI* botAI, std::string const name = "icc gunship enter cannon")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;

    bool EnterVehicle(Unit* vehicleBase, bool moveIfFar);
    Unit* FindBestAvailableCannon();
    bool IsValidCannon(Unit* vehicle, const uint32 validEntries[]);
};

class IccGunshipTeleportAllyAction : public AttackAction
{
public:
    IccGunshipTeleportAllyAction(ShadowAI* botAI, std::string const name = "icc gunship teleport ally")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    void CleanupSkullIcon(uint8_t SKULL_ICON_INDEX);
    void UpdateBossSkullIcon(Unit* boss, uint8_t SKULL_ICON_INDEX);
};

class IccGunshipTeleportHordeAction : public AttackAction
{
public:
    IccGunshipTeleportHordeAction(ShadowAI* botAI, std::string const name = "icc gunship teleport horde")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    void CleanupSkullIcon(uint8_t SKULL_ICON_INDEX);
    void UpdateBossSkullIcon(Unit* boss, uint8_t SKULL_ICON_INDEX);
};

//DBS
class IccDbsTankPositionAction : public AttackAction
{
public:
    IccDbsTankPositionAction(ShadowAI* botAI, std::string const name = "icc dbs tank position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    bool CrowdControlBloodBeasts();
    bool EvadeBloodBeasts();
    bool PositionInRangedFormation();
};

class IccAddsDbsAction : public AttackAction
{
public:
    IccAddsDbsAction(ShadowAI* botAI, std::string const name = "icc adds dbs")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    Unit* FindPriorityTarget(Unit* boss);
    void UpdateSkullMarker(Unit* priorityTarget);
};

//FESTERGUT
class IccFestergutGroupPositionAction : public AttackAction
{
public:
    IccFestergutGroupPositionAction(ShadowAI* botAI, std::string const name = "icc festergut group position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    bool HasSporesInGroup();
    bool PositionNonTankMembers();
    int CalculatePositionIndex(Group* group);
};

class IccFestergutSporeAction : public AttackAction
{
public:
    IccFestergutSporeAction(ShadowAI* botAI, std::string const name = "icc festergut spore")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    Position CalculateSpreadPosition();
    struct SporeInfo
    {
        std::vector<Unit*> sporedPlayers;
        ObjectGuid lowestGuid;
        bool hasLowestGuid = false;
    };
    SporeInfo FindSporedPlayers();
    Position DetermineTargetPosition(bool hasSpore, const SporeInfo& sporeInfo, const Position& spreadRangedPos);
    bool CheckMainTankSpore();
};

//Rotface
class IccRotfaceTankPositionAction : public AttackAction
{
public:
    IccRotfaceTankPositionAction(ShadowAI* botAI, std::string const name = "icc rotface tank position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    void MarkBossWithSkull(Unit* boss);
    bool PositionMainTankAndMelee(Unit *boss);
    bool HandleAssistTankPositioning(Unit* boss);
    bool HandleBigOozePositioning(Unit* boss);
};

class IccRotfaceGroupPositionAction : public AttackAction
{
public:
    IccRotfaceGroupPositionAction(ShadowAI* botAI, std::string const name = "icc rotface group position")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    //bool MoveAwayFromBigOoze(Unit* bigOoze);
    bool HandlePuddleAvoidance(Unit* boss);
    bool MoveAwayFromPuddle(Unit* boss, Unit* puddle, float puddleDistance);
    bool HandleOozeTargeting();
    bool HandleOozeMemberPositioning();
    bool PositionRangedAndHealers(Unit* boss,Unit* smallOoze);
    bool FindAndMoveFromClosestMember(Unit* boss, Unit* smallOoze);
};

class IccRotfaceMoveAwayFromExplosionAction : public MovementAction
{
public:
    IccRotfaceMoveAwayFromExplosionAction(ShadowAI* botAI, std::string const name = "icc rotface move away from explosion")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;

    bool MoveToRandomSafeLocation();

};

//PP
class IccPutricideGrowingOozePuddleAction : public AttackAction
{
public:
    IccPutricideGrowingOozePuddleAction(ShadowAI* botAI, std::string const name = "icc putricide growing ooze puddle")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    Unit* FindClosestThreateningPuddle();
    Position CalculateSafeMovePosition(Unit* closestPuddle);
    bool IsPositionTooCloseToOtherPuddles(float x, float y, Unit* ignorePuddle);


};

class IccPutricideVolatileOozeAction : public AttackAction
{
public:
    IccPutricideVolatileOozeAction(ShadowAI* botAI, std::string const name = "icc putricide volatile ooze")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    void MarkOozeWithSkull(Unit* ooze);
    Unit* FindAuraTarget();
};

class IccPutricideGasCloudAction : public AttackAction
{
public:
    IccPutricideGasCloudAction(ShadowAI* botAI, std::string const name = "icc putricide gas cloud")
        : AttackAction(botAI, name) {}
    bool Execute(Event event) override;

    bool HandleGaseousBloatMovement(Unit* gasCloud);
    Position CalculateEmergencyPosition(const Position& botPos, float dx, float dy);
    bool FindSafeMovementPosition(const Position& botPos, const Position& cloudPos, float dx, float dy, int numAngles,
                                  Position& resultPos);
    bool HandleGroupAuraSituation(Unit* gasCloud);
    bool GroupHasGaseousBloat(Group* group);
};

class IccPutricideAvoidMalleableGooAction : public MovementAction
{
public:
    IccPutricideAvoidMalleableGooAction(ShadowAI* botAI, std::string const name = "icc putricide avoid malleable goo")
        : MovementAction(botAI, name) {}
    bool Execute(Event event) override;

    bool HandleTankPositioning(Unit* boss);
    bool HandleUnboundPlague(Unit* boss);
    bool HandleBossPositioning(Unit* boss);
    Position CalculateBossPosition(Unit* boss, float distance);
    bool HasObstacleBetween(const Position& from, const Position& to);
    bool IsOnPath(const Position& from, const Position& to, const Position& point, float threshold);
    Position CalculateArcPoint(const Position& current, const Position& target, const Position& center);
    Position CalculateIncrementalMove(const Position& current, const Position& target, float maxDistance);
};

//BPC
class IccBpcKelesethTankAction : public AttackAction
{
public:
    IccBpcKelesethTankAction(ShadowAI* botAI)
        : AttackAction(botAI, "icc bpc keleseth tank") {}
    bool Execute(Event event) override;
};

class IccBpcMainTankAction : public AttackAction
{
public:
    IccBpcMainTankAction(ShadowAI* botAI)
        : AttackAction(botAI, "icc bpc main tank") {}
    bool Execute(Event event) override;

    void MarkEmpoweredPrince();
};

class IccBpcEmpoweredVortexAction : public MovementAction
{
public:
    IccBpcEmpoweredVortexAction(ShadowAI* botAI)
        : MovementAction(botAI, "icc bpc empowered vortex") {}
    bool Execute(Event event) override;

    bool MaintainRangedSpacing();
    bool HandleEmpoweredVortexSpread();
};

class IccBpcKineticBombAction : public AttackAction
{
public:
    IccBpcKineticBombAction(ShadowAI* botAI)
        : AttackAction(botAI, "icc bpc kinetic bomb") {}
    bool Execute(Event event) override;

    Unit* FindOptimalKineticBomb();
    bool IsBombAlreadyHandled(Unit* bomb, Group* group);
};

class IccBpcBallOfFlameAction : public MovementAction
{
public:
    IccBpcBallOfFlameAction(ShadowAI* botAI)
        : MovementAction(botAI, "icc bpc ball of flame") {}
    bool Execute(Event event) override;
};

//Blood Queen Lana'thel
class IccBqlGroupPositionAction : public AttackAction
{
public:
    IccBqlGroupPositionAction(ShadowAI* botAI)
        : AttackAction(botAI, "icc group tank position") {}
    bool Execute(Event event) override;

    bool HandleTankPosition(Unit* boss, Aura* frenzyAura, Aura* shadowAura);
    bool HandleShadowsMovement();
    Position AdjustControlPoint(const Position& wall, const Position& center, float factor);
    Position CalculateBezierPoint(float t, const Position path[4]);
    bool HandleGroupPosition(Unit* boss, Aura* frenzyAura, Aura* shadowAura);

private:
    // Evaluate curves
    struct CurveInfo
    {
        Position moveTarget;
        int curveIdx = 0;
        bool foundSafe = false;
        float minDist = 0.0f;
        float score = 0.0f;
        Position closestPoint;
        float t_closest = 0.0f;
    };
};

class IccBqlPactOfDarkfallenAction : public MovementAction
{
public:
    IccBqlPactOfDarkfallenAction(ShadowAI* botAI)
        : MovementAction(botAI, "icc bql pact of darkfallen") {}
    bool Execute(Event event) override;

    void CalculateCenterPosition(Position& targetPos, const std::vector<Player*>& playersWithAura);
    bool MoveToTargetPosition(const Position& targetPos, int auraCount);
};

class IccBqlVampiricBiteAction : public AttackAction
{
public:
    IccBqlVampiricBiteAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc bql vampiric bite") {}
    bool Execute(Event event) override;

    Player* FindBestBiteTarget(Group* group);
    bool IsInvalidTarget(Player* player);
    bool MoveTowardsTarget(Player* target);
    bool CastVampiricBite(Player* target);
};

// Sister Svalna
class IccValkyreSpearAction : public AttackAction
{
public:
    IccValkyreSpearAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc valkyre spear") {}
    bool Execute(Event event) override;
};

class IccSisterSvalnaAction : public AttackAction
{
public:
    IccSisterSvalnaAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc sister svalna") {}
    bool Execute(Event event) override;
};

// Valithria Dreamwalker

class IccValithriaGroupAction : public AttackAction
{
public:
    IccValithriaGroupAction(ShadowAI* botAI)
        : AttackAction(botAI, "icc valithria group") {}
    bool Execute(Event event) override;

    bool MoveTowardsPosition(const Position& pos, float increment);
    bool Handle25ManGroupLogic();
    bool HandleMarkingLogic(bool inGroup1, bool inGroup2, const Position& group1Pos, const Position& group2Pos);
    bool Handle10ManGroupLogic();
};

class IccValithriaPortalAction : public MovementAction
{
public:
    IccValithriaPortalAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc valithria portal") {}
    bool Execute(Event event) override;
};

class IccValithriaHealAction : public AttackAction
{
public:
    IccValithriaHealAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc valithria heal") {}
    bool Execute(Event event) override;
};

class IccValithriaDreamCloudAction : public MovementAction
{
public:
    IccValithriaDreamCloudAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc valithria dream cloud") {}
    bool Execute(Event event) override;
};

//Sindragosa
class IccSindragosaGroupPositionAction : public AttackAction
{
public:
    IccSindragosaGroupPositionAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc sindragosa group position") {}
    bool Execute(Event event) override;

    bool HandleTankPositioning(Unit* boss);
    bool HandleNonTankPositioning();
    bool MoveIncrementallyToPosition(const Position& targetPos, float maxStep);
};

class IccSindragosaFrostBeaconAction : public MovementAction
{
public:
    IccSindragosaFrostBeaconAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc sindragosa frost beacon") {}
    bool Execute(Event event) override;

    void HandleSupportActions();
    bool HandleBeaconedPlayer(const Unit* boss);
    bool HandleNonBeaconedPlayer(const Unit* boss);
    bool MoveToPositionIfNeeded(const Position& position, float tolerance);
    bool MoveToPosition(const Position& position);
    bool IsBossFlying(const Unit* boss);

    private:
    static constexpr uint32 FROST_BEACON_AURA_ID = SPELL_FROST_BEACON;
    static constexpr uint32 HAND_OF_FREEDOM_SPELL_ID = 1044;
    static constexpr float POSITION_TOLERANCE = 1.0f;
    static constexpr float TOMB_POSITION_TOLERANCE = 0.5f;
    static constexpr float MIN_SAFE_DISTANCE = 13.0f;
    static constexpr float MOVE_TOLERANCE = 2.0f;
};

class IccSindragosaBlisteringColdAction : public MovementAction
{
public:
    IccSindragosaBlisteringColdAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc sindragosa blistering cold") {}
    bool Execute(Event event) override;
};

class IccSindragosaUnchainedMagicAction : public AttackAction
{
public:
    IccSindragosaUnchainedMagicAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc sindragosa unchained magic") {}
    bool Execute(Event event) override;
};

class IccSindragosaChilledToTheBoneAction : public AttackAction
{
public:
    IccSindragosaChilledToTheBoneAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc sindragosa chilled to the bone") {}
    bool Execute(Event event) override;
};

class IccSindragosaMysticBuffetAction : public MovementAction
{
public:
    IccSindragosaMysticBuffetAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc sindragosa mystic buffet") {}
    bool Execute(Event event) override;
};

class IccSindragosaFrostBombAction : public MovementAction
{
public:
    IccSindragosaFrostBombAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc sindragosa frost bomb") {}
    bool Execute(Event event) override;
};

class IccSindragosaTankSwapPositionAction : public AttackAction
{
    public:
        IccSindragosaTankSwapPositionAction(ShadowAI* botAI)
            : AttackAction(botAI, "sindragosa tank swap position") {}
        bool Execute(Event event) override;
};


//LK
class IccLichKingShadowTrapAction : public MovementAction
{
    public:
        IccLichKingShadowTrapAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc lich king shadow trap") {}
        bool Execute(Event event) override;
};

class IccLichKingNecroticPlagueAction : public MovementAction
{
    public:
        IccLichKingNecroticPlagueAction(ShadowAI* botAI)
            : MovementAction(botAI, "icc lich king necrotic plague") {}
        bool Execute(Event event) override;
};

class IccLichKingWinterAction : public AttackAction
{
    public:
        IccLichKingWinterAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc lich king winter") {}
        bool Execute(Event event) override;

        void HandlePositionCorrection();
        bool IsValidCollectibleAdd(Unit* unit);
        bool IsPositionSafeFromDefile(float x, float y, float z, float minSafeDistance);
        void HandleTankPositioning();
        void HandleMeleePositioning();
        void HandleRangedPositioning();
        void HandleMainTankAddManagement(Unit* boss, const Position* tankPos);
        void HandleAssistTankAddManagement(Unit* boss, const Position* tankPos);


    private:
        const Position* GetMainTankPosition();
        const Position* GetMainTankRangedPosition();
        bool TryMoveToPosition(float targetX, float targetY, float targetZ, bool isForced = true);
};

class IccLichKingAddsAction : public AttackAction
{
    public:
        IccLichKingAddsAction(ShadowAI* botAI)
            : AttackAction(botAI, "icc lich king adds") {}
        bool Execute(Event event) override;

        void HandleTeleportationFixes(Difficulty diff, Unit* terenasMenethilHC);
        bool HandleSpiritBombAvoidance(Difficulty diff, Unit* terenasMenethilHC);
        void HandleHeroicNonTankPositioning(Difficulty diff, Unit* terenasMenethilHC);
        void HandleSpiritMarkingAndTargeting(Difficulty diff, Unit* terenasMenethilHC);
        bool HandleQuakeMechanics(Unit* boss);
        void HandleShamblingHorrors(Unit* boss, bool hasPlague);
        bool HandleAssistTankAddManagement(Unit* boss, Difficulty diff);
        void HandleMeleePositioning(Unit* boss, bool hasPlague, Difficulty diff);
        void HandleMainTankTargeting(Unit* boss, Difficulty diff);
        void HandleNonTankHeroicPositioning(Unit* boss, Difficulty diff, bool hasPlague);
        void HandleRangedPositioning(Unit* boss, bool hasPlague, Difficulty diff);
        void HandleDefileMechanics(Unit* boss, Difficulty diff);
        void HandleValkyrMechanics(Difficulty diff);
        std::vector<size_t> CalculateBalancedGroupSizes(size_t totalAssist, size_t numValkyrs);
        size_t GetAssignedValkyrIndex(size_t assistIndex, const std::vector<size_t>& groupSizes);
        std::string GetRTIValueForValkyr(size_t valkyrIndex);
        void HandleValkyrMarking(const std::vector<Unit*>& grabbingValkyrs, Difficulty diff);
        void HandleValkyrAssignment(const std::vector<Unit*>& grabbingValkyrs);
        void ApplyCCToValkyr(Unit* valkyr);
        bool IsValkyr(Unit* unit);
        void HandleVileSpiritMechanics();
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidIccActionContext : public NamedObjectContext<Action>
{
public:
    RaidIccActionContext()
    {
        creators["icc lm tank position"] = &RaidIccActionContext::icc_lm_tank_position;
        creators["icc spike"] = &RaidIccActionContext::icc_spike;

        creators["icc dark reckoning"] = &RaidIccActionContext::icc_dark_reckoning;
        creators["icc ranged position lady deathwhisper"] = &RaidIccActionContext::icc_ranged_position_lady_deathwhisper;
        creators["icc adds lady deathwhisper"] = &RaidIccActionContext::icc_adds_lady_deathwhisper;
        creators["icc shade lady deathwhisper"] = &RaidIccActionContext::icc_shade_lady_deathwhisper;

        creators["icc rotting frost giant tank position"] = &RaidIccActionContext::icc_rotting_frost_giant_tank_position;
        creators["icc cannon fire"] = &RaidIccActionContext::icc_cannon_fire;
        creators["icc gunship enter cannon"] = &RaidIccActionContext::icc_gunship_enter_cannon;
        creators["icc gunship teleport ally"] = &RaidIccActionContext::icc_gunship_teleport_ally;
        creators["icc gunship teleport horde"] = &RaidIccActionContext::icc_gunship_teleport_horde;

        creators["icc dbs tank position"] = &RaidIccActionContext::icc_dbs_tank_position;
        creators["icc adds dbs"] = &RaidIccActionContext::icc_adds_dbs;

        creators["icc festergut group position"] = &RaidIccActionContext::icc_festergut_group_position;
        creators["icc festergut spore"] = &RaidIccActionContext::icc_festergut_spore;

        creators["icc rotface tank position"] = &RaidIccActionContext::icc_rotface_tank_position;
        creators["icc rotface group position"] = &RaidIccActionContext::icc_rotface_group_position;
        creators["icc rotface move away from explosion"] = &RaidIccActionContext::icc_rotface_move_away_from_explosion;

        creators["icc putricide volatile ooze"] = &RaidIccActionContext::icc_putricide_volatile_ooze;
        creators["icc putricide gas cloud"] = &RaidIccActionContext::icc_putricide_gas_cloud;
        creators["icc putricide growing ooze puddle"] = &RaidIccActionContext::icc_putricide_growing_ooze_puddle;
        creators["icc putricide avoid malleable goo"] = &RaidIccActionContext::icc_putricide_avoid_malleable_goo;

        creators["icc bpc keleseth tank"] = &RaidIccActionContext::icc_bpc_keleseth_tank;
        creators["icc bpc main tank"] = &RaidIccActionContext::icc_bpc_main_tank;
        creators["icc bpc empowered vortex"] = &RaidIccActionContext::icc_bpc_empowered_vortex;
        creators["icc bpc kinetic bomb"] = &RaidIccActionContext::icc_bpc_kinetic_bomb;
        creators["icc bpc ball of flame"] = &RaidIccActionContext::icc_bpc_ball_of_flame;

        creators["icc bql group position"] = &RaidIccActionContext::icc_bql_group_position;
        creators["icc bql pact of darkfallen"] = &RaidIccActionContext::icc_bql_pact_of_darkfallen;
        creators["icc bql vampiric bite"] = &RaidIccActionContext::icc_bql_vampiric_bite;

        creators["icc valkyre spear"] = &RaidIccActionContext::icc_valkyre_spear;
        creators["icc sister svalna"] = &RaidIccActionContext::icc_sister_svalna;

        creators["icc valithria group"] = &RaidIccActionContext::icc_valithria_group;
        creators["icc valithria portal"] = &RaidIccActionContext::icc_valithria_portal;
        creators["icc valithria heal"] = &RaidIccActionContext::icc_valithria_heal;
        creators["icc valithria dream cloud"] = &RaidIccActionContext::icc_valithria_dream_cloud;

        creators["icc sindragosa group position"] = &RaidIccActionContext::icc_sindragosa_group_position;
        creators["icc sindragosa frost beacon"] = &RaidIccActionContext::icc_sindragosa_frost_beacon;
        creators["icc sindragosa blistering cold"] = &RaidIccActionContext::icc_sindragosa_blistering_cold;
        creators["icc sindragosa unchained magic"] = &RaidIccActionContext::icc_sindragosa_unchained_magic;
        creators["icc sindragosa chilled to the bone"] = &RaidIccActionContext::icc_sindragosa_chilled_to_the_bone;
        creators["icc sindragosa mystic buffet"] = &RaidIccActionContext::icc_sindragosa_mystic_buffet;
        creators["icc sindragosa frost bomb"] = &RaidIccActionContext::icc_sindragosa_frost_bomb;
        creators["icc sindragosa tank swap position"] = &RaidIccActionContext::icc_sindragosa_tank_swap_position;

        creators["icc lich king shadow trap"] = &RaidIccActionContext::icc_lich_king_shadow_trap;
        creators["icc lich king necrotic plague"] = &RaidIccActionContext::icc_lich_king_necrotic_plague;
        creators["icc lich king winter"] = &RaidIccActionContext::icc_lich_king_winter;
        creators["icc lich king adds"] = &RaidIccActionContext::icc_lich_king_adds;
    }

private:
    static Action* icc_lm_tank_position(ShadowAI* ai) { return new IccLmTankPositionAction(ai); }
    static Action* icc_spike(ShadowAI* ai) { return new IccSpikeAction(ai); }

    static Action* icc_dark_reckoning(ShadowAI* ai) { return new IccDarkReckoningAction(ai); }
    static Action* icc_ranged_position_lady_deathwhisper(ShadowAI* ai) { return new IccRangedPositionLadyDeathwhisperAction(ai); }
    static Action* icc_adds_lady_deathwhisper(ShadowAI* ai) { return new IccAddsLadyDeathwhisperAction(ai); }
    static Action* icc_shade_lady_deathwhisper(ShadowAI* ai) { return new IccShadeLadyDeathwhisperAction(ai); }

    static Action* icc_rotting_frost_giant_tank_position(ShadowAI* ai) { return new IccRottingFrostGiantTankPositionAction(ai); }
    static Action* icc_cannon_fire(ShadowAI* ai) { return new IccCannonFireAction(ai); }
    static Action* icc_gunship_enter_cannon(ShadowAI* ai) { return new IccGunshipEnterCannonAction(ai); }
    static Action* icc_gunship_teleport_ally(ShadowAI* ai) { return new IccGunshipTeleportAllyAction(ai); }
    static Action* icc_gunship_teleport_horde(ShadowAI* ai) { return new IccGunshipTeleportHordeAction(ai); }

    static Action* icc_dbs_tank_position(ShadowAI* ai) { return new IccDbsTankPositionAction(ai); }
    static Action* icc_adds_dbs(ShadowAI* ai) { return new IccAddsDbsAction(ai); }

    static Action* icc_festergut_group_position(ShadowAI* ai) { return new IccFestergutGroupPositionAction(ai); }
    static Action* icc_festergut_spore(ShadowAI* ai) { return new IccFestergutSporeAction(ai); }

    static Action* icc_rotface_tank_position(ShadowAI* ai) { return new IccRotfaceTankPositionAction(ai); }
    static Action* icc_rotface_group_position(ShadowAI* ai) { return new IccRotfaceGroupPositionAction(ai); }
    static Action* icc_rotface_move_away_from_explosion(ShadowAI* ai) { return new IccRotfaceMoveAwayFromExplosionAction(ai); }

    static Action* icc_putricide_volatile_ooze(ShadowAI* ai) { return new IccPutricideVolatileOozeAction(ai); }
    static Action* icc_putricide_gas_cloud(ShadowAI* ai) { return new IccPutricideGasCloudAction(ai); }
    static Action* icc_putricide_growing_ooze_puddle(ShadowAI* ai) { return new IccPutricideGrowingOozePuddleAction(ai); }
    static Action* icc_putricide_avoid_malleable_goo(ShadowAI* ai) { return new IccPutricideAvoidMalleableGooAction(ai); }

    static Action* icc_bpc_keleseth_tank(ShadowAI* ai) { return new IccBpcKelesethTankAction(ai); }
    static Action* icc_bpc_main_tank(ShadowAI* ai) { return new IccBpcMainTankAction(ai); }
    static Action* icc_bpc_empowered_vortex(ShadowAI* ai) { return new IccBpcEmpoweredVortexAction(ai); }
    static Action* icc_bpc_kinetic_bomb(ShadowAI* ai) { return new IccBpcKineticBombAction(ai); }
    static Action* icc_bpc_ball_of_flame(ShadowAI* ai) { return new IccBpcBallOfFlameAction(ai); }

    static Action* icc_bql_group_position(ShadowAI* ai) { return new IccBqlGroupPositionAction(ai); }
    static Action* icc_bql_pact_of_darkfallen(ShadowAI* ai) { return new IccBqlPactOfDarkfallenAction(ai); }
    static Action* icc_bql_vampiric_bite(ShadowAI* ai) { return new IccBqlVampiricBiteAction(ai); }

    static Action* icc_valkyre_spear(ShadowAI* ai) { return new IccValkyreSpearAction(ai); }
    static Action* icc_sister_svalna(ShadowAI* ai) { return new IccSisterSvalnaAction(ai); }

    static Action* icc_valithria_group(ShadowAI* ai) { return new IccValithriaGroupAction(ai); }
    static Action* icc_valithria_portal(ShadowAI* ai) { return new IccValithriaPortalAction(ai); }
    static Action* icc_valithria_heal(ShadowAI* ai) { return new IccValithriaHealAction(ai); }
    static Action* icc_valithria_dream_cloud(ShadowAI* ai) { return new IccValithriaDreamCloudAction(ai); }

    static Action* icc_sindragosa_group_position(ShadowAI* ai) { return new IccSindragosaGroupPositionAction(ai); }
    static Action* icc_sindragosa_frost_beacon(ShadowAI* ai) { return new IccSindragosaFrostBeaconAction(ai); }
    static Action* icc_sindragosa_blistering_cold(ShadowAI* ai) { return new IccSindragosaBlisteringColdAction(ai); }
    static Action* icc_sindragosa_unchained_magic(ShadowAI* ai) { return new IccSindragosaUnchainedMagicAction(ai); }
    static Action* icc_sindragosa_chilled_to_the_bone(ShadowAI* ai) { return new IccSindragosaChilledToTheBoneAction(ai); }
    static Action* icc_sindragosa_mystic_buffet(ShadowAI* ai) { return new IccSindragosaMysticBuffetAction(ai); }
    static Action* icc_sindragosa_frost_bomb(ShadowAI* ai) { return new IccSindragosaFrostBombAction(ai); }
    static Action* icc_sindragosa_tank_swap_position(ShadowAI* ai) { return new IccSindragosaTankSwapPositionAction(ai); }

    static Action* icc_lich_king_shadow_trap(ShadowAI* ai) { return new IccLichKingShadowTrapAction(ai); }
    static Action* icc_lich_king_necrotic_plague(ShadowAI* ai) { return new IccLichKingNecroticPlagueAction(ai); }
    static Action* icc_lich_king_winter(ShadowAI* ai) { return new IccLichKingWinterAction(ai); }
    static Action* icc_lich_king_adds(ShadowAI* ai) { return new IccLichKingAddsAction(ai); }

};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidIccTriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidIccTriggerContext()
    {
        creators["icc lm"] = &RaidIccTriggerContext::icc_lm;

        creators["icc dark reckoning"] = &RaidIccTriggerContext::icc_dark_reckoning;
        creators["icc lady deathwhisper"] = &RaidIccTriggerContext::icc_lady_deathwhisper;

        creators["icc rotting frost giant tank position"] = &RaidIccTriggerContext::icc_rotting_frost_giant_tank_position;
        creators["icc in cannon"] = &RaidIccTriggerContext::icc_in_cannon;
        creators["icc gunship cannon near"] = &RaidIccTriggerContext::icc_gunship_cannon_near;
        creators["icc gunship teleport ally"] = &RaidIccTriggerContext::icc_gunship_teleport_ally;
        creators["icc gunship teleport horde"] = &RaidIccTriggerContext::icc_gunship_teleport_horde;

        creators["icc dbs"] = &RaidIccTriggerContext::icc_dbs;
        creators["icc dbs main tank rune of blood"] = &RaidIccTriggerContext::icc_dbs_main_tank_rune_of_blood;

        creators["icc stinky precious main tank mortal wound"] = &RaidIccTriggerContext::icc_stinky_precious_main_tank_mortal_wound;

        creators["icc festergut group position"] = &RaidIccTriggerContext::icc_festergut_group_position;
        creators["icc festergut main tank gastric bloat"] = &RaidIccTriggerContext::icc_festergut_main_tank_gastric_bloat;
        creators["icc festergut spore"] = &RaidIccTriggerContext::icc_festergut_spore;

        creators["icc rotface tank position"] = &RaidIccTriggerContext::icc_rotface_tank_position;
        creators["icc rotface group position"] = &RaidIccTriggerContext::icc_rotface_group_position;
        creators["icc rotface move away from explosion"] = &RaidIccTriggerContext::icc_rotface_move_away_from_explosion;

        creators["icc putricide volatile ooze"] = &RaidIccTriggerContext::icc_putricide_volatile_ooze;
        creators["icc putricide gas cloud"] = &RaidIccTriggerContext::icc_putricide_gas_cloud;
        creators["icc putricide growing ooze puddle"] = &RaidIccTriggerContext::icc_putricide_growing_ooze_puddle;
        creators["icc putricide main tank mutated plague"] = &RaidIccTriggerContext::icc_putricide_main_tank_mutated_plague;
        creators["icc putricide malleable goo"] = &RaidIccTriggerContext::icc_putricide_malleable_goo;

        creators["icc bpc keleseth tank"] = &RaidIccTriggerContext::icc_bpc_keleseth_tank;
        creators["icc bpc main tank"] = &RaidIccTriggerContext::icc_bpc_main_tank;
        creators["icc bpc empowered vortex"] = &RaidIccTriggerContext::icc_bpc_empowered_vortex;
        creators["icc bpc kinetic bomb"] = &RaidIccTriggerContext::icc_bpc_kinetic_bomb;
        creators["icc bpc ball of flame"] = &RaidIccTriggerContext::icc_bpc_ball_of_flame;

        creators["icc bql group position"] = &RaidIccTriggerContext::icc_bql_group_position;
        creators["icc bql pact of darkfallen"] = &RaidIccTriggerContext::icc_bql_pact_of_darkfallen;
        creators["icc bql vampiric bite"] = &RaidIccTriggerContext::icc_bql_vampiric_bite;

        creators["icc valkyre spear"] = &RaidIccTriggerContext::icc_valkyre_spear;
        creators["icc sister svalna"] = &RaidIccTriggerContext::icc_sister_svalna;

        creators["icc valithria group"] = &RaidIccTriggerContext::icc_valithria_group;
        creators["icc valithria portal"] = &RaidIccTriggerContext::icc_valithria_portal;
        creators["icc valithria heal"] = &RaidIccTriggerContext::icc_valithria_heal;
        creators["icc valithria dream cloud"] = &RaidIccTriggerContext::icc_valithria_dream_cloud;

        creators["icc sindragosa group position"] = &RaidIccTriggerContext::icc_sindragosa_group_position;
        creators["icc sindragosa frost beacon"] = &RaidIccTriggerContext::icc_sindragosa_frost_beacon;
        creators["icc sindragosa blistering cold"] = &RaidIccTriggerContext::icc_sindragosa_blistering_cold;
        creators["icc sindragosa unchained magic"] = &RaidIccTriggerContext::icc_sindragosa_unchained_magic;
        creators["icc sindragosa chilled to the bone"] = &RaidIccTriggerContext::icc_sindragosa_chilled_to_the_bone;
        creators["icc sindragosa mystic buffet"] = &RaidIccTriggerContext::icc_sindragosa_mystic_buffet;
        creators["icc sindragosa main tank mystic buffet"] = &RaidIccTriggerContext::icc_sindragosa_main_tank_mystic_buffet;
        creators["icc sindragosa frost bomb"] = &RaidIccTriggerContext::icc_sindragosa_frost_bomb;
        creators["icc sindragosa tank swap position"] = &RaidIccTriggerContext::icc_sindragosa_tank_swap_position;

        creators["icc lich king shadow trap"] = &RaidIccTriggerContext::icc_lich_king_shadow_trap;
        creators["icc lich king necrotic plague"] = &RaidIccTriggerContext::icc_lich_king_necrotic_plague;
        creators["icc lich king winter"] = &RaidIccTriggerContext::icc_lich_king_winter;
        creators["icc lich king adds"] = &RaidIccTriggerContext::icc_lich_king_adds;
    }

private:
    static Trigger* icc_lm(ShadowAI* ai) { return new IccLmTrigger(ai); }

    static Trigger* icc_dark_reckoning(ShadowAI* ai) { return new IccDarkReckoningTrigger(ai); }
    static Trigger* icc_lady_deathwhisper(ShadowAI* ai) { return new IccLadyDeathwhisperTrigger(ai); }

    static Trigger* icc_rotting_frost_giant_tank_position(ShadowAI* ai) { return new IccRottingFrostGiantTankPositionTrigger(ai); }
    static Trigger* icc_in_cannon(ShadowAI* ai) { return new IccInCannonTrigger(ai); }
    static Trigger* icc_gunship_cannon_near(ShadowAI* ai) { return new IccGunshipCannonNearTrigger(ai); }
    static Trigger* icc_gunship_teleport_ally(ShadowAI* ai) { return new IccGunshipTeleportAllyTrigger(ai); }
    static Trigger* icc_gunship_teleport_horde(ShadowAI* ai) { return new IccGunshipTeleportHordeTrigger(ai); }

    static Trigger* icc_dbs(ShadowAI* ai) { return new IccDbsTrigger(ai); }
    static Trigger* icc_dbs_main_tank_rune_of_blood(ShadowAI* ai) { return new IccDbsMainTankRuneOfBloodTrigger(ai); }

    static Trigger* icc_stinky_precious_main_tank_mortal_wound(ShadowAI* ai) { return new IccStinkyPreciousMainTankMortalWoundTrigger(ai); }

    static Trigger* icc_festergut_group_position(ShadowAI* ai) { return new IccFestergutGroupPositionTrigger(ai); }
    static Trigger* icc_festergut_main_tank_gastric_bloat(ShadowAI* ai) { return new IccFestergutMainTankGastricBloatTrigger(ai); }
    static Trigger* icc_festergut_spore(ShadowAI* ai) { return new IccFestergutSporeTrigger(ai); }

    static Trigger* icc_rotface_tank_position(ShadowAI* ai) { return new IccRotfaceTankPositionTrigger(ai); }
    static Trigger* icc_rotface_group_position(ShadowAI* ai) { return new IccRotfaceGroupPositionTrigger(ai); }
    static Trigger* icc_rotface_move_away_from_explosion(ShadowAI* ai) { return new IccRotfaceMoveAwayFromExplosionTrigger(ai); }

    static Trigger* icc_putricide_volatile_ooze(ShadowAI* ai) { return new IccPutricideVolatileOozeTrigger(ai); }
    static Trigger* icc_putricide_gas_cloud(ShadowAI* ai) { return new IccPutricideGasCloudTrigger(ai); }
    static Trigger* icc_putricide_growing_ooze_puddle(ShadowAI* ai) { return new IccPutricideGrowingOozePuddleTrigger(ai); }
    static Trigger* icc_putricide_main_tank_mutated_plague(ShadowAI* ai) { return new IccPutricideMainTankMutatedPlagueTrigger(ai); }
    static Trigger* icc_putricide_malleable_goo(ShadowAI* ai) { return new IccPutricideMalleableGooTrigger(ai); }

    static Trigger* icc_bpc_keleseth_tank(ShadowAI* ai) { return new IccBpcKelesethTankTrigger(ai); }
    static Trigger* icc_bpc_main_tank(ShadowAI* ai) { return new IccBpcMainTankTrigger(ai); }
    static Trigger* icc_bpc_empowered_vortex(ShadowAI* ai) { return new IccBpcEmpoweredVortexTrigger(ai); }
    static Trigger* icc_bpc_kinetic_bomb(ShadowAI* ai) { return new IccBpcKineticBombTrigger(ai); }
    static Trigger* icc_bpc_ball_of_flame(ShadowAI* ai) { return new IccBpcBallOfFlameTrigger(ai); }

    static Trigger* icc_bql_group_position(ShadowAI* ai) { return new IccBqlGroupPositionTrigger(ai); }
    static Trigger* icc_bql_pact_of_darkfallen(ShadowAI* ai) { return new IccBqlPactOfDarkfallenTrigger(ai); }
    static Trigger* icc_bql_vampiric_bite(ShadowAI* ai) { return new IccBqlVampiricBiteTrigger(ai); }

    static Trigger* icc_valkyre_spear(ShadowAI* ai) { return new IccValkyreSpearTrigger(ai); }
    static Trigger* icc_sister_svalna(ShadowAI* ai) { return new IccSisterSvalnaTrigger(ai); }

    static Trigger* icc_valithria_group(ShadowAI* ai) { return new IccValithriaGroupTrigger(ai); }
    static Trigger* icc_valithria_portal(ShadowAI* ai) { return new IccValithriaPortalTrigger(ai); }
    static Trigger* icc_valithria_heal(ShadowAI* ai) { return new IccValithriaHealTrigger(ai); }
    static Trigger* icc_valithria_dream_cloud(ShadowAI* ai) { return new IccValithriaDreamCloudTrigger(ai); }

    static Trigger* icc_sindragosa_group_position(ShadowAI* ai) { return new IccSindragosaGroupPositionTrigger(ai); }
    static Trigger* icc_sindragosa_frost_beacon(ShadowAI* ai) { return new IccSindragosaFrostBeaconTrigger(ai); }
    static Trigger* icc_sindragosa_blistering_cold(ShadowAI* ai) { return new IccSindragosaBlisteringColdTrigger(ai); }
    static Trigger* icc_sindragosa_unchained_magic(ShadowAI* ai) { return new IccSindragosaUnchainedMagicTrigger(ai); }
    static Trigger* icc_sindragosa_chilled_to_the_bone(ShadowAI* ai) { return new IccSindragosaChilledToTheBoneTrigger(ai); }
    static Trigger* icc_sindragosa_mystic_buffet(ShadowAI* ai) { return new IccSindragosaMysticBuffetTrigger(ai); }
    static Trigger* icc_sindragosa_main_tank_mystic_buffet(ShadowAI* ai) { return new IccSindragosaMainTankMysticBuffetTrigger(ai); }
    static Trigger* icc_sindragosa_frost_bomb(ShadowAI* ai) { return new IccSindragosaFrostBombTrigger(ai); }
    static Trigger* icc_sindragosa_tank_swap_position(ShadowAI* ai) { return new IccSindragosaTankSwapPositionTrigger(ai); }

    static Trigger* icc_lich_king_shadow_trap(ShadowAI* ai) { return new IccLichKingShadowTrapTrigger(ai); }
    static Trigger* icc_lich_king_necrotic_plague(ShadowAI* ai) { return new IccLichKingNecroticPlagueTrigger(ai); }
    static Trigger* icc_lich_king_winter(ShadowAI* ai) { return new IccLichKingWinterTrigger(ai); }
    static Trigger* icc_lich_king_adds(ShadowAI* ai) { return new IccLichKingAddsTrigger(ai); }

};

#endif
