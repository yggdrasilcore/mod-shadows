#include "RaidBwl.h"

#include "SharedDefines.h"
#include "Shadows.h"
#include "Strategy.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool BwlSuppressionDeviceTrigger::IsActive()
{
    GuidVector gos = AI_VALUE(GuidVector, "nearest game objects");
    for (GuidVector::iterator i = gos.begin(); i != gos.end(); i++)
    {
        GameObject* go = botAI->GetGameObject(*i);
        if (!go)
        {
            continue;
        }
        if (go->GetEntry() != 179784 || go->GetDistance(bot) >= 15.0f || go->GetGoState() != GO_STATE_READY)
        {
            continue;
        }
        return true;
    }
    return false;
}

bool BwlAfflictionBronzeTrigger::IsActive() { return bot->HasAura(23170); }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

bool BwlOnyxiaScaleCloakAuraCheckAction::Execute(Event event [[maybe_unused]])
{
    bot->AddAura(22683, bot);
    return true;
}

bool BwlOnyxiaScaleCloakAuraCheckAction::isUseful() { return !bot->HasAura(22683); }

bool BwlTurnOffSuppressionDeviceAction::Execute(Event event [[maybe_unused]])
{
    GuidVector gos = AI_VALUE(GuidVector, "nearest game objects");
    for (GuidVector::iterator i = gos.begin(); i != gos.end(); i++)
    {
        GameObject* go = botAI->GetGameObject(*i);
        if (!go)
        {
            continue;
        }
        if (go->GetEntry() != 179784 || go->GetDistance(bot) >= 15.0f || go->GetGoState() != GO_STATE_READY)
        {
            continue;
        }
        go->SetGoState(GO_STATE_ACTIVE);
    }
    return true;
}

bool BwlUseHourglassSandAction::Execute(Event event [[maybe_unused]]) { return botAI->CastSpell(23645, bot); }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

void RaidBwlStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("often",
        NextAction::array(0, new NextAction("bwl check onyxia scale cloak", ACTION_RAID), NULL)));

    triggers.push_back(new TriggerNode("bwl suppression device",
                        NextAction::array(0, new NextAction("bwl turn off suppression device", ACTION_RAID), NULL)));

    triggers.push_back(new TriggerNode("bwl affliction bronze",
        NextAction::array(0, new NextAction("bwl use hourglass sand", ACTION_RAID), NULL)));
}
