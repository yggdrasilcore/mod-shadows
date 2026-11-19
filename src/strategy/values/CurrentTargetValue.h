/*
 * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _SHADOW_CURRENTTARGETVALUE_H
#define _SHADOW_CURRENTTARGETVALUE_H

#include "Value.h"

class ShadowAI;
class Unit;

class CurrentTargetValue : public UnitManualSetValue
{
public:
    CurrentTargetValue(ShadowAI* botAI, std::string const name = "current target")
        : UnitManualSetValue(botAI, nullptr, name)
    {
    }

    Unit* Get() override;
    void Set(Unit* unit) override;
    void SetForced(Unit* unit);

private:
    ObjectGuid selection;
};

#endif
