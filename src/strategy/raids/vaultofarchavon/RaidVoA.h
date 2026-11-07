#ifndef _SHADOW_RAIDVOA_H
#define _SHADOW_RAIDVOA_H

#include "EventMap.h"
#include "GenericTriggers.h"
#include "ShadowAIConfig.h"
#include "Trigger.h"
#include "Action.h"
#include "MovementActions.h"
#include "BossAuraActions.h"
#include "BossAuraTriggers.h"
#include "ShadowAI.h"
#include "Event.h"
#include "Strategy.h"
#include "string"
#include "vector"
#include "NamedObjectContext.h"
#include "AiObjectContext.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

enum VoAIDs
{
    // Emalon the Storm Watcher
    AURA_OVERCHARGE = 64217,
    BOSS_EMALON = 33993,
    NPC_TEMPEST_MINION = 33998,
    SPELL_LIGHTNING_NOVA_10_MAN = 64216,
    SPELL_LIGHTNING_NOVA_25_MAN = 65279,
};

//
// Emalon the Storm Watcher
//
class EmalonMarkBossTrigger : public Trigger
{
public:
    EmalonMarkBossTrigger(ShadowAI* ai) : Trigger(ai, "emalon mark boss trigger") {}
    bool IsActive() override;
};

class EmalonLightingNovaTrigger : public Trigger
{
public:
    EmalonLightingNovaTrigger(ShadowAI* ai) : Trigger(ai, "emalon lighting nova trigger") {}
    bool IsActive() override;
};

class EmalonOverchargeTrigger : public Trigger
{
public:
    EmalonOverchargeTrigger(ShadowAI* ai) : Trigger(ai, "emalon overcharge trigger") {}
    bool IsActive() override;
};

class EmalonFallFromFloorTrigger : public Trigger
{
public:
    EmalonFallFromFloorTrigger(ShadowAI* ai) : Trigger(ai, "emalon fall from floor trigger") {}
    bool IsActive() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

//
//  Emalon the Storm Watcher
//

class EmalonMarkBossAction : public MovementAction
{
public:
    EmalonMarkBossAction(ShadowAI* botAI) : MovementAction(botAI, "emalon mark boss action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class EmalonLightingNovaAction : public MovementAction
{
public:
    EmalonLightingNovaAction(ShadowAI* botAI) : MovementAction(botAI, "emalon lighting nova action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class EmalonOverchargeAction : public Action
{
public:
    EmalonOverchargeAction(ShadowAI* botAI) : Action(botAI, "emalon overcharge action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

class EmalonFallFromFloorAction : public Action
{
public:
    EmalonFallFromFloorAction(ShadowAI* botAI) : Action(botAI, "emalon fall from floor action") {}
    bool Execute(Event event) override;
    bool isUseful() override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class RaidVoAStrategy : public Strategy
{
public:
    RaidVoAStrategy(ShadowAI* ai) : Strategy(ai) {}
    virtual std::string const getName() override { return "voa"; }
    virtual void InitTriggers(std::vector<TriggerNode*>& triggers) override;
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// /*
//  * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
//  and/or modify it under version 2 of the License, or (at your option), any later version.
//  */



class RaidVoAActionContext : public NamedObjectContext<Action>
{
public:
    RaidVoAActionContext()
    {
        creators["emalon mark boss action"] = &RaidVoAActionContext::emalon_mark_boss_action;
        creators["emalon lighting nova action"] = &RaidVoAActionContext::emalon_lighting_nova_action;
        creators["emalon overcharge action"] = &RaidVoAActionContext::emalon_overcharge_action;
        creators["emalon fall from floor action"] = &RaidVoAActionContext::emalon_fall_from_floor_action;
        creators["emalon nature resistance action"] = &RaidVoAActionContext::emalon_nature_resistance_action;
        creators["koralon fire resistance action"] = &RaidVoAActionContext::koralon_fire_resistance_action;
    }

private:
    static Action* emalon_mark_boss_action(ShadowAI* ai) { return new EmalonMarkBossAction(ai); }
    static Action* emalon_lighting_nova_action(ShadowAI* ai) { return new EmalonLightingNovaAction(ai); }
    static Action* emalon_overcharge_action(ShadowAI* ai) { return new EmalonOverchargeAction(ai); }
    static Action* emalon_fall_from_floor_action(ShadowAI* ai) { return new EmalonFallFromFloorAction(ai); }
    static Action* emalon_nature_resistance_action(ShadowAI* ai) { return new BossNatureResistanceAction(ai, "emalon the storm watcher"); }
    static Action* koralon_fire_resistance_action(ShadowAI* ai) { return new BossFireResistanceAction(ai, "koralon the flame watcher"); }
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

// /*
//  * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
//  and/or modify it under version 2 of the License, or (at your option), any later version.
//  */



class RaidVoATriggerContext : public NamedObjectContext<Trigger>
{
public:
    RaidVoATriggerContext()
    {
        creators["emalon mark boss trigger"] = &RaidVoATriggerContext::emalon_mark_boss_trigger;
        creators["emalon lighting nova trigger"] = &RaidVoATriggerContext::emalon_lighting_nova_trigger;
        creators["emalon overcharge trigger"] = &RaidVoATriggerContext::emalon_overcharge_trigger;
        creators["emalon fall from floor trigger"] = &RaidVoATriggerContext::emalon_fall_from_floor_trigger;
        creators["emalon nature resistance trigger"] = &RaidVoATriggerContext::emalon_nature_resistance_trigger;
        creators["koralon fire resistance trigger"] = &RaidVoATriggerContext::koralon_fire_resistance_trigger;
    }

private:
    static Trigger* emalon_mark_boss_trigger(ShadowAI* ai) { return new EmalonMarkBossTrigger(ai); }
    static Trigger* emalon_lighting_nova_trigger(ShadowAI* ai) { return new EmalonLightingNovaTrigger(ai); }
    static Trigger* emalon_overcharge_trigger(ShadowAI* ai) { return new EmalonOverchargeTrigger(ai); }
    static Trigger* emalon_fall_from_floor_trigger(ShadowAI* ai) { return new EmalonFallFromFloorTrigger(ai); }
    static Trigger* emalon_nature_resistance_trigger(ShadowAI* ai) { return new BossNatureResistanceTrigger(ai, "emalon the storm watcher"); }
    static Trigger* koralon_fire_resistance_trigger(ShadowAI* ai) { return new BossFireResistanceTrigger(ai, "koralon the flame watcher"); }
};

#endif
