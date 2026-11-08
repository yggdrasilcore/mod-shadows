/*
 * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "AreaTriggerAction.h"

#include <ctime>
#include <unordered_map>

#include "Event.h"
#include "LastMovementValue.h"
#include "Shadows.h"
#include "Transport.h"

static std::unordered_map<ObjectGuid, time_t> remoteTriggerSince;
static time_t remoteTriggerLastMessage = 0;
static const time_t REMOTE_TRIGGER_MESSAGE_DELAY = 5;
static const time_t REMOTE_TRIGGER_MESSAGE_COOLDOWN = 60;

bool ReachAreaTriggerAction::Execute(Event event)
{
    if (botAI->IsRealPlayer())  // Do not trigger own area trigger.
        return false;

    if (botAI->IsMovementSuppressed())
        return false;

    uint32 triggerId;
    WorldPacket p(event.getPacket());
    p.rpos(0);
    p >> triggerId;

    AreaTrigger const* at = sObjectMgr->GetAreaTrigger(triggerId);
    if (!at)
        return false;

    if (!sObjectMgr->GetAreaTriggerTeleport(triggerId))
    {
        WorldPacket p1(CMSG_AREATRIGGER);
        p1 << triggerId;
        p1.rpos(0);
        bot->GetSession()->HandleAreaTriggerOpcode(p1);

        return true;
    }

    if (bot->GetMapId() != at->map)
    {
        ObjectGuid guid = bot->GetGUID();
        time_t now = time(nullptr);
        auto it = remoteTriggerSince.find(guid);
        if (it == remoteTriggerSince.end() || now - it->second > 60)
            remoteTriggerSince[guid] = now;
        else if (now - it->second >= REMOTE_TRIGGER_MESSAGE_DELAY &&
                 now - remoteTriggerLastMessage >= REMOTE_TRIGGER_MESSAGE_COOLDOWN)
        {
            botAI->TellError("I won't follow: too far away");
            remoteTriggerLastMessage = now;
        }
        return true;
    }

    bot->GetMotionMaster()->MovePoint(at->map, at->x, at->y, at->z);

    float distance = bot->GetDistance(at->x, at->y, at->z);
    float delay = 1000.0f * distance / bot->GetSpeed(MOVE_RUN) + sShadowAIConfig->reactDelay;
    botAI->TellError("Wait for me");
    botAI->SetNextCheckDelay(delay);
    context->GetValue<LastMovement&>("last area trigger")->Get().lastAreaTrigger = triggerId;

    return true;
}

bool AreaTriggerAction::Execute(Event event [[maybe_unused]])
{
    LastMovement& movement = context->GetValue<LastMovement&>("last area trigger")->Get();

    uint32 triggerId = movement.lastAreaTrigger;
    movement.lastAreaTrigger = 0;

    if (!sObjectMgr->GetAreaTrigger(triggerId))
        return false;

    if (!sObjectMgr->GetAreaTriggerTeleport(triggerId))
        return true;

    WorldPacket p(CMSG_AREATRIGGER);
    p << triggerId;
    p.rpos(0);
    bot->GetSession()->HandleAreaTriggerOpcode(p);

    botAI->TellMaster("Hello");
    return true;
}
