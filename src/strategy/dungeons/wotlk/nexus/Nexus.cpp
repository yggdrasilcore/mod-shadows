#include "Nexus.h"

#include "Shadows.h"
#include "AiObject.h"
#include "AiObjectContext.h"
#include "GenericSpellActions.h"
#include "ChooseTargetActions.h"
#include "MovementActions.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool FactionCommanderWhirlwindTrigger::IsActive()
{
    Unit* boss = nullptr;
    uint8 faction = bot->GetTeamId();

    switch (bot->GetMap()->GetDifficulty())
    {
        case DUNGEON_DIFFICULTY_NORMAL:
            if (faction == TEAM_ALLIANCE)
            {
                boss = AI_VALUE2(Unit*, "find target", "horde commander");
            }
            else //if (faction == TEAM_HORDE)
            {
                boss = AI_VALUE2(Unit*, "find target", "alliance commander");
            }
            break;
        case DUNGEON_DIFFICULTY_HEROIC:
            if (faction == TEAM_ALLIANCE)
            {
                boss = AI_VALUE2(Unit*, "find target", "commander kolurg");
            }
            else //if (faction == TEAM_HORDE)
            {
                boss = AI_VALUE2(Unit*, "find target", "commander stoutbeard");
            }
            break;
        default:
            break;
    }

    if (boss && boss->HasUnitState(UNIT_STATE_CASTING))
    {
        if (boss->FindCurrentSpellBySpellId(SPELL_WHIRLWIND))
        {
            return true;
        }
    }
    return false;
}

bool TelestraFirebombTrigger::IsActive()
{
    if (botAI->IsMelee(bot)) { return false; }

    Unit* boss = AI_VALUE2(Unit*, "find target", "grand magus telestra");
    // Avoid split phase with the fake Telestra units, only match the true boss id
    return boss && boss->GetEntry() == NPC_TELESTRA;
}

bool TelestraSplitPhaseTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "grand magus telestra");
    // Only match split phase with the fake Telestra units
    return boss && boss->GetEntry() != NPC_TELESTRA;
}

bool ChaoticRiftTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "anomalus");
    return boss && boss->HasAura(BUFF_RIFT_SHIELD);
}

bool OrmorokSpikesTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ormorok the tree-shaper");
    if (!boss || !botAI->IsTank(bot)) { return false; }

    GuidVector objects = AI_VALUE(GuidVector, "closest game objects");
    for (auto i = objects.begin(); i != objects.end(); ++i)
    {
        GameObject* go = botAI->GetGameObject(*i);
        if (go && go->GetEntry() == GO_CRYSTAL_SPIKE)
        {
            return true;
        }
    }
    return false;
}

bool OrmorokStackTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ormorok the tree-shaper");
    return (boss && !botAI->IsTank(bot));
}

bool IntenseColdTrigger::IsActive()
{
    // Adjust as needed - too much interrupting loses dps time,
    // but too many stacks is deadly. Assuming 3-5 is a good number to clear
    int stackThreshold = 5;
    Unit* boss = AI_VALUE2(Unit*, "find target", "keristrasza");
    return boss && botAI->GetAura("intense cold", bot, false, false, stackThreshold);
}

bool KeristraszaPositioningTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "keristrasza");
    // Include healers here for now, otherwise they stand in things
    return boss && !botAI->IsTank(bot) && !botAI->IsRangedDps(bot);
    // return boss && botAI->IsMelee(bot) && !botAI->IsTank(bot);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool MoveFromWhirlwindAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = nullptr;
    uint8 faction = bot->GetTeamId();
    float targetDist = 10.0f; // Whirlwind has a range of 8, adding a safety buffer

    switch (bot->GetMap()->GetDifficulty())
    {
        case DUNGEON_DIFFICULTY_NORMAL:
            if (faction == TEAM_ALLIANCE)
            {
                boss = AI_VALUE2(Unit*, "find target", "horde commander");
            }
            else // TEAM_HORDE
            {
                boss = AI_VALUE2(Unit*, "find target", "alliance commander");
            }
            break;
        case DUNGEON_DIFFICULTY_HEROIC:
            if (faction == TEAM_ALLIANCE)
            {
                boss = AI_VALUE2(Unit*, "find target", "commander kolurg");
            }
            else // TEAM_HORDE
            {
                boss = AI_VALUE2(Unit*, "find target", "commander stoutbeard");
            }
            break;
        default:
            break;
    }

    // Ensure boss is valid before accessing its methods
    if (!boss)
    {
        return false;
    }

    float bossDistance = bot->GetExactDist2d(boss->GetPosition());

    // Check if the bot is already at a safe distance
    if (bossDistance > targetDist)
    {
        return false;
    }

    // Move away from the boss to avoid Whirlwind
    return MoveAway(boss, targetDist - bossDistance);
}

bool FirebombSpreadAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "grand magus telestra");
    float radius = 5.0f;
    float targetDist = radius + 1.0f;
    if (!boss) { return false; }

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        Unit* unit = botAI->GetUnit(member);
        if (!unit || bot->GetGUID() == member) { continue; }

        if (bot->GetExactDist2d(unit) < targetDist)
        {
            return MoveAway(unit, targetDist);
        }
    }
    return false;
}

bool TelestraSplitTargetAction::isUseful() { return !botAI->IsHeal(bot); }
bool TelestraSplitTargetAction::Execute(Event event [[maybe_unused]])
{
    GuidVector attackers = AI_VALUE(GuidVector, "attackers");
    Unit* splitTargets[3] = {nullptr, nullptr, nullptr};

    for (auto& attacker : attackers)
    {
        Unit* unit = botAI->GetUnit(attacker);
        if (!unit) { continue; }

        switch (unit->GetEntry())
        {
            // Focus arcane clone first
            case NPC_ARCANE_MAGUS:
                splitTargets[0] = unit;
                break;
            // Then the frost clone
            case NPC_FROST_MAGUS:
                splitTargets[1] = unit;
                break;
            // Fire clone last
            case NPC_FIRE_MAGUS:
                splitTargets[2] = unit;
                break;
        }
    }

    for (Unit* target : splitTargets)
    {
        // Attack the first valid split target in the priority list
        if (target)
        {
            if (AI_VALUE(Unit*, "current target") != target)
            {
                return Attack(target);
            }
            // Don't continue loop here, the target exists so we don't
            // want to move down the prio list. We just don't need to send attack
            // command again, just return false and exit the loop that way
            return false;
        }
    }

    return false;
}

bool ChaoticRiftTargetAction::isUseful() { return !botAI->IsHeal(bot); }
bool ChaoticRiftTargetAction::Execute(Event event [[maybe_unused]])
{
    Unit* chaoticRift = nullptr;

    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetName() == "Chaotic Rift")
        {
            chaoticRift = unit;
            break;
        }
    }
    if (!chaoticRift || AI_VALUE(Unit*, "current target") == chaoticRift)
    {
        return false;
    }
    return Attack(chaoticRift);
}

bool DodgeSpikesAction::isUseful()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ormorok the tree-shaper");
    if (!boss) { return false; }

    return bot->GetExactDist2d(boss) > 0.5f;
}
bool DodgeSpikesAction::Execute(Event event [[maybe_unused]])
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ormorok the tree-shaper");
    if (!boss) { return false; }

    return Move(bot->GetAngle(boss), bot->GetExactDist2d(boss) - 0.3f);
}

bool IntenseColdJumpAction::Execute(Event event [[maybe_unused]])
{
    // This needs improving but maybe it should be done in the shadow core.
    // Jump doesn't seem to support zero offset (eg. jump on the spot) so need to add a tiny delta.
    // This does a tiny bunnyhop that takes a couple of ms, it doesn't do a natural jump.
    // Adding extra Z offset causes floating, and appears to scale the jump speed based on Z difference.
    // Probably best to revisit once bot movement is improved
    return JumpTo(bot->GetMap()->GetId(), bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ() + 0.01f);
    // bot->GetMotionMaster()->MoveFall();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

float FactionCommanderMultiplier::GetValue(Action* action)
{
    Unit* boss = nullptr;
    uint8 faction = bot->GetTeamId();

    switch (bot->GetMap()->GetDifficulty())
    {
        case DUNGEON_DIFFICULTY_NORMAL:
            if (faction == TEAM_ALLIANCE)
            {
                boss = AI_VALUE2(Unit*, "find target", "horde commander");
            }
            else //if (faction == TEAM_HORDE)
            {
                boss = AI_VALUE2(Unit*, "find target", "alliance commander");
            }
            break;
        case DUNGEON_DIFFICULTY_HEROIC:
            if (faction == TEAM_ALLIANCE)
            {
                boss = AI_VALUE2(Unit*, "find target", "commander kolurg");
            }
            else //if (faction == TEAM_HORDE)
            {
                boss = AI_VALUE2(Unit*, "find target", "commander stoutbeard");
            }
            break;
        default:
            break;
    }
    if (boss && boss->HasUnitState(UNIT_STATE_CASTING) &&
        boss->FindCurrentSpellBySpellId(SPELL_WHIRLWIND))
    {
        // Prevent movement actions other than flee during a whirlwind, to prevent running back in early.
        if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<MoveFromWhirlwindAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float TelestraMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "grand magus telestra");
    if (boss && boss->GetEntry() != NPC_TELESTRA)
    {
        // boss is split into clones, do not auto acquire target
        if (dynamic_cast<DpsAssistAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float AnomalusMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "anomalus");
    if (boss && boss->HasAura(BUFF_RIFT_SHIELD))
    {
        if (dynamic_cast<DpsAssistAction*>(action))
        {
            return 0.0f;
        }
    }
    return 1.0f;
}

float OrmorokMultiplier::GetValue(Action* action)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "ormorok the tree-shaper");
    if (!boss) { return 1.0f; }

    // These are used for auto ranged repositioning, need to suppress so ranged dps don't ping-pong
    if (dynamic_cast<FleeAction*>(action))
    {
        return 0.0f;
    }
    // This boss is annoying and shuffles around a lot. Don't let tank move once fight has started.
    // Extra checks are to allow the tank to close distance and engage the boss initially
    if (dynamic_cast<MovementAction*>(action) && !dynamic_cast<DodgeSpikesAction*>(action)
        && botAI->IsTank(bot) && bot->IsWithinMeleeRange(boss)
        && AI_VALUE2(bool, "facing", "current target"))
        {
            return 0.0f;
        }
    return 1.0f;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void WotlkDungeonNexStrategy::InitTriggers(std::vector<TriggerNode*> &triggers)
{
    // Horde Commander (Alliance N)/Commander Kolurg (Alliance H)
    // or
    // Alliance Commander (Horde N)/Commander Stoutbeard (Horde H)
    triggers.push_back(new TriggerNode("faction commander whirlwind",
        NextAction::array(0, new NextAction("move from whirlwind", ACTION_MOVE + 5), nullptr)));
    // TODO: Handle fear? (tremor totems, fear ward etc.)

    // Grand Magus Telestra
    triggers.push_back(new TriggerNode("telestra firebomb",
        NextAction::array(0, new NextAction("firebomb spread", ACTION_MOVE + 5), nullptr)));
    triggers.push_back(new TriggerNode("telestra split phase",
        NextAction::array(0, new NextAction("telestra split target", ACTION_RAID + 1), nullptr)));
    // TODO: Add priority interrupt on the frost split's Blizzard casts

    // Anomalus
    triggers.push_back(new TriggerNode("chaotic rift",
        NextAction::array(0, new NextAction("chaotic rift target", ACTION_RAID + 1), nullptr)));

    // Ormorok the Tree-Shaper
    // Tank trigger to stack inside boss. Can also add return action to prevent boss repositioning
    // if it becomes too much of a problem. He usually dies before he's up against a wall though
    triggers.push_back(new TriggerNode("ormorok spikes",
        NextAction::array(0, new NextAction("dodge spikes", ACTION_MOVE + 5), nullptr)));
    // Non-tank trigger to stack. Avoiding the spikes at range is.. harder than it seems.
    // TODO: This turns hunters into melee marshmallows, have not come up with a better solution yet
    triggers.push_back(new TriggerNode("ormorok stack",
        NextAction::array(0, new NextAction("dodge spikes", ACTION_MOVE + 5), nullptr)));
    // TODO: Add handling for spell reflect... best to spam low level/weak spells but don't want
    // to hardcode spells per class, might be difficult to dynamically generate this.
    // Will revisit if I find my altbots killing themselves in heroic, just heal through it for now

    // Keristrasza
    triggers.push_back(new TriggerNode("intense cold",
        NextAction::array(0, new NextAction("intense cold jump", ACTION_MOVE + 5), nullptr)));
    // Flank dragon positioning
    triggers.push_back(new TriggerNode("keristrasza positioning",
        NextAction::array(0, new NextAction("rear flank", ACTION_MOVE + 4), nullptr)));
    // TODO: Add frost resist aura for paladins?
}

void WotlkDungeonNexStrategy::InitMultipliers(std::vector<Multiplier*> &multipliers)
{
    multipliers.push_back(new FactionCommanderMultiplier(botAI));
    multipliers.push_back(new TelestraMultiplier(botAI));
    multipliers.push_back(new AnomalusMultiplier(botAI));
    multipliers.push_back(new OrmorokMultiplier(botAI));
}
