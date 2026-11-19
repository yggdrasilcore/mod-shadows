/*
 * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _SHADOW_TARGETVALUE_H
#define _SHADOW_TARGETVALUE_H

#include "NamedObjectContext.h"
#include "TravelMgr.h"
#include "Value.h"

class ShadowAI;
class ThreatMgr;
class Unit;

class FindTargetStrategy
{
public:
    FindTargetStrategy(ShadowAI* botAI) : result(nullptr), botAI(botAI) {}

    Unit* GetResult();
    virtual void CheckAttacker(Unit* attacker, ThreatMgr* threatMgr) = 0;
    void GetPlayerCount(Unit* creature, uint32* tankCount, uint32* dpsCount);
    bool IsHighPriority(Unit* attacker);

protected:
    Unit* result;
    ShadowAI* botAI;
    std::map<Unit*, uint32> tankCountCache;
    std::map<Unit*, uint32> dpsCountCache;
    bool foundHighPriority = false;
};

class FindNonCcTargetStrategy : public FindTargetStrategy
{
public:
    FindNonCcTargetStrategy(ShadowAI* botAI) : FindTargetStrategy(botAI) {}

protected:
    virtual bool IsCcTarget(Unit* attacker);
};

class TargetValue : public UnitCalculatedValue
{
public:
    TargetValue(ShadowAI* botAI, std::string const name = "target", int checkInterval = 1)
        : UnitCalculatedValue(botAI, name, checkInterval)
    {
    }

protected:
    Unit* FindTarget(FindTargetStrategy* strategy);
};

class RpgTargetValue : public ManualSetValue<GuidPosition>
{
public:
    RpgTargetValue(ShadowAI* botAI, std::string const name = "rpg target")
        : ManualSetValue<GuidPosition>(botAI, GuidPosition(), name)
    {
    }
};

class TravelTargetValue : public ManualSetValue<TravelTarget*>
{
public:
    TravelTargetValue(ShadowAI* botAI, std::string const name = "travel target")
        : ManualSetValue<TravelTarget*>(botAI, new TravelTarget(botAI), name)
    {
    }

    virtual ~TravelTargetValue() { delete value; }
};

class LastLongMoveValue : public CalculatedValue<WorldPosition>
{
public:
    LastLongMoveValue(ShadowAI* botAI) : CalculatedValue<WorldPosition>(botAI, "last long move", 30 * 1000) {}

    WorldPosition Calculate() override;
};

class HomeBindValue : public CalculatedValue<WorldPosition>
{
public:
    HomeBindValue(ShadowAI* botAI) : CalculatedValue<WorldPosition>(botAI, "home bind", 30 * 1000) {}

    WorldPosition Calculate() override;
};

class IgnoreRpgTargetValue : public ManualSetValue<GuidSet&>
{
public:
    IgnoreRpgTargetValue(ShadowAI* botAI) : ManualSetValue<GuidSet&>(botAI, data, "ignore rpg targets") {}

private:
    GuidSet data;
};

class TalkTargetValue : public ManualSetValue<ObjectGuid>
{
public:
    TalkTargetValue(ShadowAI* botAI, std::string const name = "talk target")
        : ManualSetValue<ObjectGuid>(botAI, ObjectGuid::Empty, name)
    {
    }
};

class PullTargetValue : public ManualSetValue<ObjectGuid>
{
public:
    PullTargetValue(ShadowAI* botAI, std::string const name = "pull target")
        : ManualSetValue<ObjectGuid>(botAI, ObjectGuid::Empty, name)
    {
    }

    void Set(ObjectGuid val) override;
    void SetForced(ObjectGuid val);
};

class FindTargetValue : public UnitCalculatedValue, public Qualified
{
public:
    FindTargetValue(ShadowAI* ai) : UnitCalculatedValue(ai, "find target", /*2 * 1000*/ 1) {}

public:
    Unit* Calculate();
};

class FindBossTargetStrategy : public FindTargetStrategy
{
public:
    FindBossTargetStrategy(ShadowAI* ai) : FindTargetStrategy(ai) {}
    virtual void CheckAttacker(Unit* attacker, ThreatMgr* threatManager);
};

class BossTargetValue : public TargetValue, public Qualified
{
public:
    BossTargetValue(ShadowAI* ai) : TargetValue(ai, "boss target", 2 * 1000) {}

public:
    Unit* Calculate();
};
#endif
