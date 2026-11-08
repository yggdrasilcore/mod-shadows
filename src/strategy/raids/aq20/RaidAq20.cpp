#include "RaidAq20.h"

#include "SpellAuras.h"
#include "SharedDefines.h"
#include "Shadows.h"
#include "Strategy.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

uint32 const OSSIRIAN_BUFF = 25176;
uint32 const OSSIRIAN_DEBUFFS[] = {25177, 25178, 25180, 25181, 25183};
uint32 const OSSIRIAN_CRYSTAL_GO_ENTRY = 180619;

bool RaidAq20Utils::IsOssirianBuffActive(Unit* ossirian)
{
    return ossirian && ossirian->HasAura(OSSIRIAN_BUFF);
}

int32 RaidAq20Utils::GetOssirianDebuffTimeRemaining(Unit* ossirian)
{
    int32 retVal = 0xffffff;
    if (ossirian)
    {
        for (uint32 debuff : OSSIRIAN_DEBUFFS)
        {
            if (AuraApplication* auraApplication = ossirian->GetAuraApplication(debuff))
            {
                if (Aura* aura = auraApplication->GetBase())
                {
                    int32 duration = aura->GetDuration();
                    if (retVal > duration)
                        retVal = duration;
                }
            }
        }
    }
    return retVal;
}

GameObject* RaidAq20Utils::GetNearestCrystal(Unit* ossirian)
{
    return ossirian ? ossirian->FindNearestGameObject(OSSIRIAN_CRYSTAL_GO_ENTRY, 200.0f) : nullptr;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool Aq20MoveToCrystalTrigger::IsActive()
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "ossirian the unscarred"))
    {
        if (boss->IsInCombat())
        {
            // if buff is active move to crystal
            if (RaidAq20Utils::IsOssirianBuffActive(boss))
                return true;

            // if buff is not active a debuff will be, buff becomes active once debuff expires
            // so move to crystal when debuff almost done, or based debuff time left and
            // distance bot is from crystal (ie: start moving early enough to make it)
            int32 debuffTimeRemaining = RaidAq20Utils::GetOssirianDebuffTimeRemaining(boss);
            if (debuffTimeRemaining < 5000)
                return true;
            if (debuffTimeRemaining < 30000)
            {
                if (GameObject* crystal = RaidAq20Utils::GetNearestCrystal(boss))
                {
                    float botDist = bot->GetDistance(crystal);
                    float timeToReach = botDist / bot->GetSpeed(MOVE_RUN);
                    // bot should ideally activate crystal a ~5 seconds early (due to time it takes for crystal
                    // to activate) so aim to get there in time to do so
                    return debuffTimeRemaining - 5000 < timeToReach * 1000.0f;
                }
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool Aq20UseCrystalAction::Execute(Event event [[maybe_unused]])
{
    if (Unit* boss = AI_VALUE2(Unit*, "find target", "ossirian the unscarred"))
    {
        if (GameObject* crystal = RaidAq20Utils::GetNearestCrystal(boss))
        {
            float botDist = bot->GetDistance(crystal);
            if (botDist > INTERACTION_DISTANCE)
                return MoveTo(bot->GetMapId(),
                    crystal->GetPositionX() + frand(-3.5f, 3.5f),
                    crystal->GetPositionY() + frand(-3.5f, 3.5f),
                    crystal->GetPositionZ());

            // if we're already in range just wait here until it's time to activate crystal
            SetNextMovementDelay(500);

            // don't activate crystal if boss too far or its already been activated
            if (boss->GetDistance(crystal) > 25.0f ||
                crystal->HasGameObjectFlag(GO_FLAG_IN_USE))
                return false;

            // don't activate crystal if boss doesn't have buff yet AND isn't going to have it soon
            // (though ideally bot should activate it ~5 seconds early due to time it takes for
            // crystal to activate and remove buff)
            if (!RaidAq20Utils::IsOssirianBuffActive(boss) &&
                RaidAq20Utils::GetOssirianDebuffTimeRemaining(boss) > 5000)
                return false;

            // this makes crystal do its animation (then disappear after)
            WorldPacket data1(CMSG_GAMEOBJ_USE);
            data1 << crystal->GetGUID();
            bot->GetSession()->HandleGameObjectUseOpcode(data1);

            // this makes crystal actually remove the buff and put on debuff (took a while to figure that out)
            WorldPacket data2(CMSG_GAMEOBJ_USE);
            data2 << crystal->GetGUID();
            bot->GetSession()->HandleGameobjectReportUse(data2);

            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidAq20Strategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode("aq20 move to crystal",
            NextAction::array(0, new NextAction("aq20 use crystal", ACTION_RAID), nullptr)));

}
