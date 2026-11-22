/*
 * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "ShadowAI.h"
#include "WipeAction.h"

bool WipeAction::Execute(Event event)
{
    Player* master = event.getOwner();
    if (!master || !botAI->GetMaster() || botAI->GetMaster()->GetGUID() != master->GetGUID())
        return false;

    bot->Kill(bot, bot);
    return true;
}
