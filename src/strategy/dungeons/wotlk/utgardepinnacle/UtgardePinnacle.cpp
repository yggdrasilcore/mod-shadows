#include "UtgardePinnacle.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"
#include "Action.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool SkadiFreezingCloudTrigger::IsActive()
{
    Unit* bossMount = AI_VALUE2(Unit*, "find target", "grauf");
    if (!bossMount) { return false; }

    // Need to check two conditions here - the persistent ground effect doesn't
    // seem to be detectable until 3-5 secs in, despite it dealing damage.
    // The initial breath triggers straight away but once it's over, the bots will run back on
    // to the freezing cloud and take damage.
    // Therefore check both conditions and trigger on either.

    // Check this one first, if true then we don't need to iterate over any objects
    if (bossMount->HasAura(SPELL_FREEZING_CLOUD_BREATH))
    {
        return true;
    }

    // Otherwise, check for persistent ground objects emitting the freezing cloud
    GuidVector objects = AI_VALUE(GuidVector, "nearest hostile npcs");
    for (auto i = objects.begin(); i != objects.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_BREATH_TRIGGER)
        {
            Unit::AuraApplicationMap const& Auras = unit->GetAppliedAuras();
            for (Unit::AuraApplicationMap::const_iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
            {
                Aura* aura = itr->second->GetBase();
                if (aura && aura->GetId() == SPELL_FREEZING_CLOUD)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool SkadiWhirlwindTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "skadi the ruthless");
    return boss && boss->HasAura(SPELL_SKADI_WHIRLWIND);
}

bool YmironBaneTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "king ymiron");
    if (!boss) { return false; }

    return boss->FindCurrentSpellBySpellId(SPELL_BANE) || boss->HasAura(SPELL_BANE);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool AvoidFreezingCloudAction::Execute(Event event [[maybe_unused]])
{
    Unit* closestTrigger = nullptr;
    GuidVector objects = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (auto i = objects.begin(); i != objects.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_BREATH_TRIGGER)
        {
            if (!closestTrigger || bot->GetExactDist2d(unit) < bot->GetExactDist2d(closestTrigger))
                closestTrigger = unit;
        }
    }

    if (!closestTrigger)
        return false;

    float distance = bot->GetExactDist2d(closestTrigger->GetPosition());
    float radius = 3.0f;
    float distanceExtra = 3.0f;

    if (distance < radius + distanceExtra - 1.0f)
        return MoveAway(closestTrigger, radius + distanceExtra - distance);

    return false;
}

bool AvoidSkadiWhirlwindAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "skadi the ruthless");
    if (!boss)
        return false;

    float distance = bot->GetExactDist2d(boss->GetPosition());
    float radius = 5.0f;
    float distanceExtra = 2.0f;

    if (distance < radius + distanceExtra)
    {
        if (botAI->IsTank(bot))
            return MoveAway(boss, (2.0f * radius) + distanceExtra - distance);
        return MoveAway(boss, radius + distanceExtra - distance);
    }

    return false;
}

bool AvoidYmironBaneAction::Execute(Event event [[maybe_unused]])
{
    Creature* boss = bot->FindNearestCreature(26861, 100.0f);
    if (!boss)
        return false;

    Pet* pet = bot->GetPet();
    if (pet)
        pet->AttackStop();

    // Tank stops attacking but stands still
    if (boss->GetVictim() == bot)
        return false;

    // Warrior/Mage/Shaman must stay close to try to spellsteal/purge Bane
    if (!boss->HasAura(SPELL_BANE))
    {
        switch (bot->getClass())
        {
            case CLASS_WARRIOR:
            case CLASS_MAGE:
            case CLASS_SHAMAN:
                return false;
        }
    }
	
    // Warrior/Mage/Shaman try to reflect when Bane aura is active
    if (boss->HasAura(SPELL_BANE))
    {
        switch (bot->getClass())
        {
            case CLASS_WARRIOR:
                if (botAI->CanCastSpell(30356, boss, true))
                    return botAI->CastSpell(30356, boss);
                break;
            case CLASS_MAGE:
                if (botAI->CastSpell(30449, boss))
                    return true;
                break;
            case CLASS_SHAMAN:
                if (botAI->CastSpell(8012, boss))
                    return true;
                break;
        }
    }
    
    // Everyone else runs away (including warrior/mage/shaman if spell failed)
    float botX = bot->GetPositionX();
    float botY = bot->GetPositionY();
    float bossX = boss->GetPositionX();
    float bossY = boss->GetPositionY();
    
    float dirX = botX - bossX;
    float dirY = botY - bossY;
    float len = sqrt(dirX * dirX + dirY * dirY);
    
    if (len > 0)
    {
        dirX /= len;
        dirY /= len;
    }
    
    float moveDistance = 10.0f;
    float targetX = botX + dirX * moveDistance;
    float targetY = botY + dirY * moveDistance;
    float targetZ = 104.76f;
    
    // Healer: stay within 20 yards of tank
    if (botAI->IsHeal(bot))
    {
        Unit* tank = boss->GetVictim();
        if (tank)
        {
            float distToTank = sqrt(pow(targetX - tank->GetPositionX(), 2) + pow(targetY - tank->GetPositionY(), 2));
            if (distToTank > 20.0f)
            {
                // Clamp position to 20 yards from tank
                float tankDirX = targetX - tank->GetPositionX();
                float tankDirY = targetY - tank->GetPositionY();
                float tankLen = sqrt(tankDirX * tankDirX + tankDirY * tankDirY);
                if (tankLen > 0)
                {
                    tankDirX /= tankLen;
                    tankDirY /= tankLen;
                }
                targetX = tank->GetPositionX() + tankDirX * 20.0f;
                targetY = tank->GetPositionY() + tankDirY * 20.0f;
            }
        }
    }
    
    float minX = 351.0f;
    float maxX = 434.0f;
    float minY = -342.0f;
    float maxY = -306.0f;
    
    targetX = std::max(minX, std::min(maxX, targetX));
    targetY = std::max(minY, std::min(maxY, targetY));
    
    return MoveTo(bot->GetMapId(), targetX, targetY, targetZ);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float SkadiMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "skadi the ruthless");
    if (!boss) { return 1.0f; }

    Unit* bossMount = AI_VALUE2(Unit*, "find target", "grauf");

    if (!bossMount)
    // Actual bossfight (dismounted)
    {
        if (boss->HasAura(SPELL_SKADI_WHIRLWIND))
        {
            if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidSkadiWhirlwindAction*>(action))
            {
                return 0.0f;
            }
        }
    }
    else
    {
        // Bots tend to get stuck trying to attack the boss in the sky, not the adds on the ground
        if (dynamic_cast<AttackAction*>(action)
            && (action->GetTarget() == boss || action->GetTarget() == bossMount))
        {
            return 0.0f;
        }

        // TODO: BELOW IS EXPERIMENTAL
        // Meant to stop the jittery movement when dodging the breath.
        // Currently causes issues with making the bots unresponsive and often getting the healer killed.
        // Semi-glitchy movement is better than semi-afk bots, so this is commented out until it gets improved

        // bool cloudActive = false;
        // // Need to check two conditions here - the persistent ground effect doesn't
        // // seem to be detectable until 3-5 secs in, despite it dealing damage.
        // // The initial breath triggers straight away but once it's over, the bots will run back on
        // // to the frezzing cloud and take damage.
        // // Therefore check both conditions and trigger on either.

        // // Check this one early, if true then we don't need to iterate over any objects
        // if (bossMount->HasAura(SPELL_FREEZING_CLOUD_BREATH))
        // {
        //     cloudActive = true;
        // }

        // // Otherwise, check for persistent ground objects emitting the freezing cloud
        // GuidVector objects = AI_VALUE(GuidVector, "nearest hostile npcs");
        // for (auto i = objects.begin(); i != objects.end(); ++i)
        // {
        //     Unit* unit = botAI->GetUnit(*i);
        //     if (unit && unit->GetEntry() == NPC_BREATH_TRIGGER)
        //     {
        //         Unit::AuraApplicationMap const& Auras = unit->GetAppliedAuras();
        //         for (Unit::AuraApplicationMap::const_iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
        //         {
        //             Aura* aura = itr->second->GetBase();
        //             if (aura && aura->GetId() == SPELL_FREEZING_CLOUD)
        //             {
        //                 cloudActive = true;
        //                 break;
        //             }
        //         }
        //     }
        // }

        // if (cloudActive)
        // {
        //     if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<AvoidFreezingCloudAction*>(action))
        //     {
        //         return 0.0f;
        //     }
        // }
    }

    return 1.0f;
}

float YmironMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "king ymiron");
    if (!boss) { return 1.0f; }

    if (boss->FindCurrentSpellBySpellId(SPELL_BANE) || boss->HasAura(SPELL_BANE))
    {
        if (dynamic_cast<AttackAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonUPStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Svala Sorrowgrave

    // Gortok Palehoof

    // Skadi the Ruthless
    // TODO: Harpoons launchable via GameObject. For now players should do them
    triggers.push_back(new TriggerNode("freezing cloud",
        NextAction::array(0, new NextAction("avoid freezing cloud", ACTION_RAID + 5), nullptr)));
    triggers.push_back(new TriggerNode("skadi whirlwind",
        NextAction::array(0, new NextAction("avoid skadi whirlwind", ACTION_RAID + 4), nullptr)));
    
    // King Ymiron
    // May need to avoid orb.. unclear if the generic avoid AoE does this well
    triggers.push_back(new TriggerNode("ymiron bane",
        NextAction::array(0, new NextAction("avoid ymiron bane", ACTION_EMERGENCY + 6), nullptr)));
}

void WotlkDungeonUPStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new SkadiMultiplier(botAI));
    multipliers.push_back(new YmironMultiplier(botAI));
}
