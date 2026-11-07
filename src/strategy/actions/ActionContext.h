/*
 * Copyright (C) 2016+ YggdrasilCore <yggdrasilcore.github.io>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _SHADOW_ACTIONCONTEXT_H
#define _SHADOW_ACTIONCONTEXT_H

#include "AddLootAction.h"
#include "AttackAction.h"
#include "ShareQuestAction.h"
#include "BattleGroundTactics.h"
#include "AutoMaintenanceOnLevelupAction.h"
#include "BattleGroundJoinAction.h"
#include "BattleGroundTactics.h"
#include "BuyAction.h"
#include "CastCustomSpellAction.h"
#include "ChangeStrategyAction.h"
#include "ChangeTalentsAction.h"
#include "CheckMailAction.h"
#include "CheckValuesAction.h"
#include "ChooseRpgTargetAction.h"
#include "ChooseTargetActions.h"
#include "ChooseTravelTargetAction.h"
#include "CombatActions.h"
#include "DelayAction.h"
#include "DestroyItemAction.h"
#include "EmoteAction.h"
#include "FollowActions.h"
#include "GenericActions.h"
#include "GenericSpellActions.h"
#include "GiveItemAction.h"
#include "GreetAction.h"
#include "GuildAcceptAction.h"
#include "GuildCreateActions.h"
#include "GuildManagementActions.h"
#include "ImbueAction.h"
#include "InviteToGroupAction.h"
#include "LeaveGroupAction.h"
#include "LootAction.h"
#include "LootRollAction.h"
#include "MoveToRpgTargetAction.h"
#include "MoveToTravelTargetAction.h"
#include "MovementActions.h"
#include "NonCombatActions.h"
#include "OutfitAction.h"
#include "PositionAction.h"
#include "DropQuestAction.h"
#include "RaidNaxx.h"
#include "RandomBotUpdateAction.h"
#include "ReachTargetActions.h"
#include "ReleaseSpiritAction.h"
#include "RemoveAuraAction.h"
#include "ResetInstancesAction.h"
#include "RevealGatheringItemAction.h"
#include "RpgAction.h"
#include "RpgSubActions.h"
#include "RtiAction.h"
#include "SayAction.h"
#include "StayActions.h"
#include "SuggestWhatToDoAction.h"
#include "TravelAction.h"
#include "VehicleActions.h"
#include "WorldBuffAction.h"
#include "XpGainAction.h"
#include "NewRpgAction.h"
#include "CancelChannelAction.h"

class ShadowAI;

class ActionContext : public NamedObjectContext<Action>
{
public:
    ActionContext()
    {
        creators["mark rti"] = &ActionContext::mark_rti;
        creators["set return position"] = &ActionContext::set_return_position;
        creators["rpg"] = &ActionContext::rpg;
        creators["crpg"] = &ActionContext::crpg;
        creators["choose rpg target"] = &ActionContext::choose_rpg_target;
        creators["move to rpg target"] = &ActionContext::move_to_rpg_target;
        creators["travel"] = &ActionContext::travel;
        creators["choose travel target"] = &ActionContext::choose_travel_target;
        creators["move to travel target"] = &ActionContext::move_to_travel_target;
        creators["move out of collision"] = &ActionContext::move_out_of_collision;
        creators["move random"] = &ActionContext::move_random;
        creators["attack"] = &ActionContext::melee;
        creators["melee"] = &ActionContext::melee;
        creators["switch to melee"] = &ActionContext::switch_to_melee;
        creators["switch to ranged"] = &ActionContext::switch_to_ranged;
        creators["reach spell"] = &ActionContext::ReachSpell;
        creators["reach melee"] = &ActionContext::ReachMelee;
        creators["reach party member to heal"] = &ActionContext::reach_party_member_to_heal;
        creators["reach party member to resurrect"] = &ActionContext::reach_party_member_to_resurrect;
        creators["flee"] = &ActionContext::flee;
        creators["flee with pet"] = &ActionContext::flee_with_pet;
        creators["avoid aoe"] = &ActionContext::avoid_aoe;
        creators["combat formation move"] = &ActionContext::combat_formation_move;
        creators["tank face"] = &ActionContext::tank_face;
        creators["rear flank"] = &ActionContext::rear_flank;
        creators["disperse set"] = &ActionContext::disperse_set;
        creators["gift of the naaru"] = &ActionContext::gift_of_the_naaru;
        creators["shoot"] = &ActionContext::shoot;
        creators["lifeblood"] = &ActionContext::lifeblood;
        creators["arcane torrent"] = &ActionContext::arcane_torrent;
        creators["end pull"] = &ActionContext::end_pull;
        creators["healthstone"] = &ActionContext::healthstone;
        creators["healing potion"] = &ActionContext::healing_potion;
        creators["mana potion"] = &ActionContext::mana_potion;
        creators["food"] = &ActionContext::food;
        creators["drink"] = &ActionContext::drink;
        creators["tank assist"] = &ActionContext::tank_assist;
        creators["dps assist"] = &ActionContext::dps_assist;
        creators["dps aoe"] = &ActionContext::dps_aoe;
        creators["attack rti target"] = &ActionContext::attack_rti_target;
        creators["loot"] = &ActionContext::loot;
        creators["add loot"] = &ActionContext::add_loot;
        creators["add gathering loot"] = &ActionContext::add_gathering_loot;
        creators["add all loot"] = &ActionContext::add_all_loot;
        creators["release loot"] = &ActionContext::release_loot;
        creators["shoot"] = &ActionContext::shoot;
        creators["follow"] = &ActionContext::follow;
        creators["move from group"] = &ActionContext::move_from_group;
        creators["flee to master"] = &ActionContext::flee_to_master;
        creators["runaway"] = &ActionContext::runaway;
        creators["stay"] = &ActionContext::stay;
        creators["sit"] = &ActionContext::sit;
        creators["attack anything"] = &ActionContext::attack_anything;
        creators["attack least hp target"] = &ActionContext::attack_least_hp_target;
        creators["attack enemy player"] = &ActionContext::attack_enemy_player;
        creators["emote"] = &ActionContext::emote;
        creators["talk"] = &ActionContext::talk;
        creators["suggest what to do"] = &ActionContext::suggest_what_to_do;
        creators["suggest trade"] = &ActionContext::suggest_trade;
        creators["suggest dungeon"] = &ActionContext::suggest_dungeon;
        creators["return"] = &ActionContext::_return;
        creators["move to loot"] = &ActionContext::move_to_loot;
        creators["open loot"] = &ActionContext::open_loot;
        creators["guard"] = &ActionContext::guard;
        creators["return to stay position"] = &ActionContext::return_to_stay_position;
        creators["move out of enemy contact"] = &ActionContext::move_out_of_enemy_contact;
        creators["set facing"] = &ActionContext::set_facing;
        creators["set behind"] = &ActionContext::set_behind;
        creators["attack duel opponent"] = &ActionContext::attack_duel_opponent;
        creators["drop target"] = &ActionContext::drop_target;
        creators["check mail"] = &ActionContext::check_mail;
        creators["say"] = &ActionContext::say;
        creators["reveal gathering item"] = &ActionContext::reveal_gathering_item;
        creators["outfit"] = &ActionContext::outfit;
        creators["random bot update"] = &ActionContext::random_bot_update;
        creators["delay"] = &ActionContext::delay;
        creators["greet"] = &ActionContext::greet;
        creators["check values"] = &ActionContext::check_values;
        creators["ra"] = &ActionContext::ra;
        creators["apply poison"] = &ActionContext::apply_poison;
        creators["apply stone"] = &ActionContext::apply_stone;
        creators["apply oil"] = &ActionContext::apply_oil;
        creators["try emergency"] = &ActionContext::try_emergency;
        creators["give food"] = &ActionContext::give_food;
        creators["give water"] = &ActionContext::give_water;
        creators["mount"] = &ActionContext::mount;
        creators["war stomp"] = &ActionContext::war_stomp;
        creators["blood fury"] = &ActionContext::blood_fury;
        creators["berserking"] = &ActionContext::berserking;
        creators["use trinket"] = &ActionContext::use_trinket;
        creators["auto talents"] = &ActionContext::auto_talents;
        creators["auto share quest"] = &ActionContext::auto_share_quest;
        creators["auto maintenance on levelup"] = &ActionContext::auto_maintenance_on_levelup;
        creators["xp gain"] = &ActionContext::xp_gain;
        creators["invite nearby"] = &ActionContext::invite_nearby;
        creators["invite guild"] = &ActionContext::invite_guild;
        creators["leave far away"] = &ActionContext::leave_far_away;
        creators["move to dark portal"] = &ActionContext::move_to_dark_portal;
        creators["move from dark portal"] = &ActionContext::move_from_dark_portal;
        creators["use dark portal azeroth"] = &ActionContext::use_dark_portal_azeroth;
        creators["world buff"] = &ActionContext::world_buff;
        creators["hearthstone"] = &ActionContext::hearthstone;
        creators["cast random spell"] = &ActionContext::cast_random_spell;
        creators["free bg join"] = &ActionContext::free_bg_join;
        creators["use random recipe"] = &ActionContext::use_random_recipe;
        creators["use random quest item"] = &ActionContext::use_random_quest_item;
        creators["craft random item"] = &ActionContext::craft_random_item;
        creators["smart destroy item"] = &ActionContext::smart_destroy_item;
        creators["disenchant random item"] = &ActionContext::disenchant_random_item;
        creators["enchant random item"] = &ActionContext::enchant_random_item;
        creators["reset instances"] = &ActionContext::reset_instances;
        creators["buy petition"] = &ActionContext::buy_petition;
        creators["offer petition"] = &ActionContext::offer_petition;
        creators["offer petition nearby"] = &ActionContext::offer_petition_nearby;
        creators["turn in petition"] = &ActionContext::turn_in_petition;
        creators["buy tabard"] = &ActionContext::buy_tabard;
        creators["guild manage nearby"] = &ActionContext::guild_manage_nearby;
        creators["clean quest log"] = &ActionContext::clean_quest_log;
        creators["roll"] = &ActionContext::roll_action;
        creators["cancel channel"] = &ActionContext::cancel_channel;

        // BG Tactics
        creators["bg tactics"] = &ActionContext::bg_tactics;
        creators["bg move to start"] = &ActionContext::bg_move_to_start;
        creators["bg reset objective force"] = &ActionContext::bg_reset_objective_force;
        creators["bg move to objective"] = &ActionContext::bg_move_to_objective;
        creators["bg select objective"] = &ActionContext::bg_select_objective;
        creators["bg check objective"] = &ActionContext::bg_check_objective;
        creators["bg attack fc"] = &ActionContext::bg_attack_fc;
        creators["bg protect fc"] = &ActionContext::bg_protect_fc;
        creators["bg use buff"] = &ActionContext::bg_use_buff;
        creators["attack enemy flag carrier"] = &ActionContext::attack_enemy_fc;
        creators["bg check flag"] = &ActionContext::bg_check_flag;

        // Vehicles
        creators["enter vehicle"] = &ActionContext::enter_vehicle;
        creators["leave vehicle"] = &ActionContext::leave_vehicle;
        creators["hurl boulder"] = &ActionContext::hurl_boulder;
        creators["ram"] = &ActionContext::ram;
        creators["steam rush"] = &ActionContext::steam_rush;
        creators["steam blast"] = &ActionContext::steam_blast;
        creators["napalm"] = &ActionContext::napalm;
        creators["fire cannon"] = &ActionContext::fire_cannon;
        creators["incendiary rocket"] = &ActionContext::incendiary_rocket;
        creators["rocket blast"] = &ActionContext::rocket_blast;
        creators["blade salvo"] = &ActionContext::blade_salvo;
        creators["glaive throw"] = &ActionContext::glaive_throw;

        //Rpg
        creators["rpg stay"] = &ActionContext::rpg_stay;
        creators["rpg work"] = &ActionContext::rpg_work;
        creators["rpg emote"] = &ActionContext::rpg_emote;
        creators["rpg cancel"] = &ActionContext::rpg_cancel;
        creators["rpg taxi"] = &ActionContext::rpg_taxi;
        creators["rpg discover"] = &ActionContext::rpg_discover;
        creators["rpg start quest"] = &ActionContext::rpg_start_quest;
        creators["rpg end quest"] = &ActionContext::rpg_end_quest;
        creators["rpg buy"] = &ActionContext::rpg_buy;
        creators["rpg sell"] = &ActionContext::rpg_sell;
        creators["rpg repair"] = &ActionContext::rpg_repair;
        creators["rpg train"] = &ActionContext::rpg_train;
        creators["rpg heal"] = &ActionContext::rpg_heal;
        creators["rpg home bind"] = &ActionContext::rpg_home_bind;
        creators["rpg queue bg"] = &ActionContext::rpg_queue_bg;
        creators["rpg buy petition"] = &ActionContext::rpg_buy_petition;
        creators["rpg use"] = &ActionContext::rpg_use;
        creators["rpg spell"] = &ActionContext::rpg_spell;
        creators["rpg craft"] = &ActionContext::rpg_craft;
        creators["rpg trade useful"] = &ActionContext::rpg_trade_useful;
        creators["rpg duel"] = &ActionContext::rpg_duel;
        creators["rpg mount anim"] = &ActionContext::rpg_mount_anim;

        creators["toggle pet spell"] = &ActionContext::toggle_pet_spell;
        creators["pet attack"] = &ActionContext::pet_attack;
        creators["set pet stance"] = &ActionContext::set_pet_stance;

        creators["new rpg status update"] = &ActionContext::new_rpg_status_update;
        creators["new rpg go grind"] = &ActionContext::new_rpg_go_grind;
        creators["new rpg go camp"] = &ActionContext::new_rpg_go_camp;
        creators["new rpg wander random"] = &ActionContext::new_rpg_wander_random;
        creators["new rpg wander npc"] = &ActionContext::new_rpg_wander_npc;
        creators["new rpg do quest"] = &ActionContext::new_rpg_do_quest;
        creators["new rpg travel flight"] = &ActionContext::new_rpg_travel_flight;
    }

private:
    static Action* give_water(ShadowAI* botAI) { return new GiveWaterAction(botAI); }
    static Action* give_food(ShadowAI* botAI) { return new GiveFoodAction(botAI); }
    static Action* ra(ShadowAI* botAI) { return new RemoveAuraAction(botAI); }
    static Action* mark_rti(ShadowAI* botAI) { return new MarkRtiAction(botAI); }
    static Action* set_return_position(ShadowAI* botAI) { return new SetReturnPositionAction(botAI); }
    static Action* rpg(ShadowAI* botAI) { return new RpgAction(botAI); }
    static Action* crpg(ShadowAI* botAI) { return new CRpgAction(botAI); }
    static Action* choose_rpg_target(ShadowAI* botAI) { return new ChooseRpgTargetAction(botAI); }
    static Action* move_to_rpg_target(ShadowAI* botAI) { return new MoveToRpgTargetAction(botAI); }
    static Action* travel(ShadowAI* botAI) { return new TravelAction(botAI); }
    static Action* choose_travel_target(ShadowAI* botAI) { return new ChooseTravelTargetAction(botAI); }
    static Action* move_to_travel_target(ShadowAI* botAI) { return new MoveToTravelTargetAction(botAI); }
    static Action* move_out_of_collision(ShadowAI* botAI) { return new MoveOutOfCollisionAction(botAI); }
    static Action* move_random(ShadowAI* botAI) { return new MoveRandomAction(botAI); }
    static Action* check_values(ShadowAI* botAI) { return new CheckValuesAction(botAI); }
    static Action* greet(ShadowAI* botAI) { return new GreetAction(botAI); }
    static Action* check_mail(ShadowAI* botAI) { return new CheckMailAction(botAI); }
    static Action* drop_target(ShadowAI* botAI) { return new DropTargetAction(botAI); }
    static Action* attack_duel_opponent(ShadowAI* botAI) { return new AttackDuelOpponentAction(botAI); }
    static Action* guard(ShadowAI* botAI) { return new GuardAction(botAI); }
    static Action* return_to_stay_position(ShadowAI* botAI) { return new ReturnToStayPositionAction(botAI); }
    static Action* open_loot(ShadowAI* botAI) { return new OpenLootAction(botAI); }
    static Action* move_to_loot(ShadowAI* botAI) { return new MoveToLootAction(botAI); }
    static Action* _return(ShadowAI* botAI) { return new ReturnAction(botAI); }
    static Action* shoot(ShadowAI* botAI) { return new CastShootAction(botAI); }
    static Action* melee(ShadowAI* botAI) { return new MeleeAction(botAI); }
    static Action* switch_to_melee(ShadowAI* botAI) { return new SwitchToMeleeAction(botAI); }
    static Action* switch_to_ranged(ShadowAI* botAI) { return new SwitchToRangedAction(botAI); }
    static Action* ReachSpell(ShadowAI* botAI) { return new ReachSpellAction(botAI); }
    static Action* ReachMelee(ShadowAI* botAI) { return new ReachMeleeAction(botAI); }
    static Action* reach_party_member_to_heal(ShadowAI* botAI) { return new ReachPartyMemberToHealAction(botAI); }
    static Action* reach_party_member_to_resurrect(ShadowAI* botAI) { return new ReachPartyMemberToResurrectAction(botAI); }
    static Action* flee(ShadowAI* botAI) { return new FleeAction(botAI); }
    static Action* flee_with_pet(ShadowAI* botAI) { return new FleeWithPetAction(botAI); }
    static Action* avoid_aoe(ShadowAI* botAI) { return new AvoidAoeAction(botAI); }
    static Action* combat_formation_move(ShadowAI* botAI) { return new CombatFormationMoveAction(botAI); }
    static Action* tank_face(ShadowAI* botAI) { return new TankFaceAction(botAI); }
    static Action* rear_flank(ShadowAI* botAI) { return new RearFlankAction(botAI); }
    static Action* disperse_set(ShadowAI* botAI) { return new DisperseSetAction(botAI); }
    static Action* gift_of_the_naaru(ShadowAI* botAI) { return new CastGiftOfTheNaaruAction(botAI); }
    static Action* lifeblood(ShadowAI* botAI) { return new CastLifeBloodAction(botAI); }
    static Action* arcane_torrent(ShadowAI* botAI) { return new CastArcaneTorrentAction(botAI); }
    static Action* mana_tap(ShadowAI* botAI) { return new CastManaTapAction(botAI); }
    static Action* end_pull(ShadowAI* botAI) { return new ChangeCombatStrategyAction(botAI, "-pull"); }
    static Action* cancel_channel(ShadowAI* botAI) { return new CancelChannelAction(botAI); }

    static Action* emote(ShadowAI* botAI) { return new EmoteAction(botAI); }
    static Action* talk(ShadowAI* botAI) { return new TalkAction(botAI); }
    static Action* suggest_what_to_do(ShadowAI* botAI) { return new SuggestWhatToDoAction(botAI); }
    static Action* suggest_trade(ShadowAI* botAI) { return new SuggestTradeAction(botAI); }
    static Action* suggest_dungeon(ShadowAI* botAI) { return new SuggestDungeonAction(botAI); }
    static Action* attack_anything(ShadowAI* botAI) { return new AttackAnythingAction(botAI); }
    static Action* attack_least_hp_target(ShadowAI* botAI) { return new AttackLeastHpTargetAction(botAI); }
    static Action* attack_enemy_player(ShadowAI* botAI) { return new AttackEnemyPlayerAction(botAI); }
    static Action* stay(ShadowAI* botAI) { return new StayAction(botAI); }
    static Action* sit(ShadowAI* botAI) { return new SitAction(botAI); }
    static Action* runaway(ShadowAI* botAI) { return new RunAwayAction(botAI); }
    static Action* follow(ShadowAI* botAI) { return new FollowAction(botAI); }
    static Action* move_from_group(ShadowAI* botAI) { return new MoveFromGroupAction(botAI); }
    static Action* flee_to_master(ShadowAI* botAI) { return new FleeToMasterAction(botAI); }
    static Action* add_gathering_loot(ShadowAI* botAI) { return new AddGatheringLootAction(botAI); }
    static Action* add_loot(ShadowAI* botAI) { return new AddLootAction(botAI); }
    static Action* add_all_loot(ShadowAI* botAI) { return new AddAllLootAction(botAI); }
    static Action* loot(ShadowAI* botAI) { return new LootAction(botAI); }
    static Action* release_loot(ShadowAI* botAI) { return new ReleaseLootAction(botAI); }
    static Action* dps_assist(ShadowAI* botAI) { return new DpsAssistAction(botAI); }
    static Action* dps_aoe(ShadowAI* botAI) { return new DpsAoeAction(botAI); }
    static Action* attack_rti_target(ShadowAI* botAI) { return new AttackRtiTargetAction(botAI); }
    static Action* tank_assist(ShadowAI* botAI) { return new TankAssistAction(botAI); }
    static Action* drink(ShadowAI* botAI) { return new DrinkAction(botAI); }
    static Action* food(ShadowAI* botAI) { return new EatAction(botAI); }
    static Action* mana_potion(ShadowAI* botAI) { return new UseManaPotion(botAI); }
    static Action* healing_potion(ShadowAI* botAI) { return new UseHealingPotion(botAI); }
    static Action* healthstone(ShadowAI* botAI) { return new UseItemAction(botAI, "healthstone"); }
    static Action* move_out_of_enemy_contact(ShadowAI* botAI) { return new MoveOutOfEnemyContactAction(botAI); }
    static Action* set_facing(ShadowAI* botAI) { return new SetFacingTargetAction(botAI); }
    static Action* set_behind(ShadowAI* botAI) { return new SetBehindTargetAction(botAI); }
    static Action* say(ShadowAI* botAI) { return new SayAction(botAI); }
    static Action* reveal_gathering_item(ShadowAI* botAI) { return new RevealGatheringItemAction(botAI); }
    static Action* outfit(ShadowAI* botAI) { return new OutfitAction(botAI); }
    static Action* random_bot_update(ShadowAI* botAI) { return new RandomBotUpdateAction(botAI); }
    static Action* delay(ShadowAI* botAI) { return new DelayAction(botAI); }

    static Action* apply_poison(ShadowAI* botAI) { return new ImbueWithPoisonAction(botAI); }
    static Action* apply_oil(ShadowAI* botAI) { return new ImbueWithOilAction(botAI); }
    static Action* apply_stone(ShadowAI* botAI) { return new ImbueWithStoneAction(botAI); }
    static Action* try_emergency(ShadowAI* botAI) { return new TryEmergencyAction(botAI); }
    static Action* mount(ShadowAI* botAI) { return new CastSpellAction(botAI, "mount"); }
    static Action* war_stomp(ShadowAI* botAI) { return new CastWarStompAction(botAI); }
    static Action* blood_fury(ShadowAI* botAI) { return new CastBloodFuryAction(botAI); }
    static Action* berserking(ShadowAI* botAI) { return new CastBerserkingAction(botAI); }
    static Action* use_trinket(ShadowAI* botAI) { return new UseTrinketAction(botAI); }
    static Action* auto_talents(ShadowAI* botAI) { return new AutoSetTalentsAction(botAI); }
    static Action* auto_share_quest(ShadowAI* ai) { return new AutoShareQuestAction(ai); }
    static Action* auto_maintenance_on_levelup(ShadowAI* botAI) { return new AutoMaintenanceOnLevelupAction(botAI); }
    static Action* xp_gain(ShadowAI* botAI) { return new XpGainAction(botAI); }
    static Action* invite_nearby(ShadowAI* botAI) { return new InviteNearbyToGroupAction(botAI); }
    static Action* invite_guild(ShadowAI* botAI) { return new InviteGuildToGroupAction(botAI); }
    static Action* leave_far_away(ShadowAI* botAI) { return new LeaveFarAwayAction(botAI); }
    static Action* move_to_dark_portal(ShadowAI* botAI) { return new MoveToDarkPortalAction(botAI); }
    static Action* use_dark_portal_azeroth(ShadowAI* botAI) { return new DarkPortalAzerothAction(botAI); }
    static Action* move_from_dark_portal(ShadowAI* botAI) { return new MoveFromDarkPortalAction(botAI); }
    static Action* world_buff(ShadowAI* botAI) { return new WorldBuffAction(botAI); }
    static Action* hearthstone(ShadowAI* botAI) { return new UseHearthStone(botAI); }
    static Action* cast_random_spell(ShadowAI* botAI) { return new CastRandomSpellAction(botAI); }
    static Action* free_bg_join(ShadowAI* botAI) { return new FreeBGJoinAction(botAI); }

    static Action* use_random_recipe(ShadowAI* botAI) { return new UseRandomRecipe(botAI); }
    static Action* use_random_quest_item(ShadowAI* botAI) { return new UseRandomQuestItem(botAI); }
    static Action* craft_random_item(ShadowAI* botAI) { return new CraftRandomItemAction(botAI); }
    static Action* smart_destroy_item(ShadowAI* botAI) { return new SmartDestroyItemAction(botAI); }
    static Action* disenchant_random_item(ShadowAI* botAI) { return new DisEnchantRandomItemAction(botAI); }
    static Action* enchant_random_item(ShadowAI* botAI) { return new EnchantRandomItemAction(botAI); }
    static Action* reset_instances(ShadowAI* botAI) { return new ResetInstancesAction(botAI); }
    static Action* buy_petition(ShadowAI* botAI) { return new BuyPetitionAction(botAI); }
    static Action* offer_petition(ShadowAI* botAI) { return new PetitionOfferAction(botAI); }
    static Action* offer_petition_nearby(ShadowAI* botAI) { return new PetitionOfferNearbyAction(botAI); }
    static Action* turn_in_petition(ShadowAI* botAI) { return new PetitionTurnInAction(botAI); }
    static Action* buy_tabard(ShadowAI* botAI) { return new BuyTabardAction(botAI); }
    static Action* guild_manage_nearby(ShadowAI* botAI) { return new GuildManageNearbyAction(botAI); }
    static Action* clean_quest_log(ShadowAI* botAI) { return new CleanQuestLogAction(botAI); }
    static Action* roll_action(ShadowAI* botAI) { return new RollAction(botAI); }

    // BG Tactics
    static Action* bg_tactics(ShadowAI* botAI) { return new BGTactics(botAI); }
    static Action* bg_move_to_start(ShadowAI* botAI) { return new BGTactics(botAI, "move to start"); }
    static Action* bg_reset_objective_force(ShadowAI* botAI) { return new BGTactics(botAI, "reset objective force"); }
    static Action* bg_move_to_objective(ShadowAI* botAI) { return new BGTactics(botAI, "move to objective"); }
    static Action* bg_select_objective(ShadowAI* botAI) { return new BGTactics(botAI, "select objective"); }
    static Action* bg_check_objective(ShadowAI* botAI) { return new BGTactics(botAI, "check objective"); }
    static Action* bg_attack_fc(ShadowAI* botAI) { return new BGTactics(botAI, "attack fc"); }
    static Action* bg_protect_fc(ShadowAI* botAI) { return new BGTactics(botAI, "protect fc"); }
    static Action* attack_enemy_fc(ShadowAI* botAI) { return new AttackEnemyFlagCarrierAction(botAI); }
    static Action* bg_use_buff(ShadowAI* botAI) { return new BGTactics(botAI, "use buff"); }
    static Action* bg_check_flag(ShadowAI* botAI) { return new BGTactics(botAI, "check flag"); }

    // Vehicles
    static Action* enter_vehicle(ShadowAI* botAI) { return new EnterVehicleAction(botAI); }
    static Action* leave_vehicle(ShadowAI* botAI) { return new LeaveVehicleAction(botAI); }
    static Action* hurl_boulder(ShadowAI* botAI) { return new CastHurlBoulderAction(botAI); }
    static Action* ram(ShadowAI* botAI) { return new CastRamAction(botAI); }
    static Action* steam_blast(ShadowAI* botAI) { return new CastSteamBlastAction(botAI); }
    static Action* steam_rush(ShadowAI* botAI) { return new CastSteamRushAction(botAI); }
    static Action* napalm(ShadowAI* botAI) { return new CastNapalmAction(botAI); }
    static Action* fire_cannon(ShadowAI* botAI) { return new CastFireCannonAction(botAI); }
    static Action* incendiary_rocket(ShadowAI* botAI) { return new CastIncendiaryRocketAction(botAI); }
    static Action* rocket_blast(ShadowAI* botAI) { return new CastRocketBlastAction(botAI); }
    static Action* glaive_throw(ShadowAI* botAI) { return new CastGlaiveThrowAction(botAI); }
    static Action* blade_salvo(ShadowAI* botAI) { return new CastBladeSalvoAction(botAI); }

    // Rpg
    static Action* rpg_stay(ShadowAI* botAI) { return new RpgStayAction(botAI); }
    static Action* rpg_work(ShadowAI* botAI) { return new RpgWorkAction(botAI); }
    static Action* rpg_emote(ShadowAI* botAI) { return new RpgEmoteAction(botAI); }
    static Action* rpg_cancel(ShadowAI* botAI) { return new RpgCancelAction(botAI); }
    static Action* rpg_taxi(ShadowAI* botAI) { return new RpgTaxiAction(botAI); }
    static Action* rpg_discover(ShadowAI* botAI) { return new RpgDiscoverAction(botAI); }
    static Action* rpg_start_quest(ShadowAI* botAI) { return new RpgStartQuestAction(botAI); }
    static Action* rpg_end_quest(ShadowAI* botAI) { return new RpgEndQuestAction(botAI); }
    static Action* rpg_buy(ShadowAI* botAI) { return new RpgBuyAction(botAI); }
    static Action* rpg_sell(ShadowAI* botAI) { return new RpgSellAction(botAI); }
    static Action* rpg_repair(ShadowAI* botAI) { return new RpgRepairAction(botAI); }
    static Action* rpg_train(ShadowAI* botAI) { return new RpgTrainAction(botAI); }
    static Action* rpg_heal(ShadowAI* botAI) { return new RpgHealAction(botAI); }
    static Action* rpg_home_bind(ShadowAI* botAI) { return new RpgHomeBindAction(botAI); }
    static Action* rpg_queue_bg(ShadowAI* botAI) { return new RpgQueueBgAction(botAI); }
    static Action* rpg_buy_petition(ShadowAI* botAI) { return new RpgBuyPetitionAction(botAI); }
    static Action* rpg_use(ShadowAI* botAI) { return new RpgUseAction(botAI); }
    static Action* rpg_spell(ShadowAI* botAI) { return new RpgSpellAction(botAI); }
    static Action* rpg_craft(ShadowAI* botAI) { return new RpgCraftAction(botAI); }
    static Action* rpg_trade_useful(ShadowAI* botAI) { return new RpgTradeUsefulAction(botAI); }
    static Action* rpg_duel(ShadowAI* botAI) { return new RpgDuelAction(botAI); }
    static Action* rpg_mount_anim(ShadowAI* botAI) { return new RpgMountAnimAction(botAI); }

    static Action* toggle_pet_spell(ShadowAI* ai) { return new TogglePetSpellAutoCastAction(ai); }
    static Action* pet_attack(ShadowAI* ai) { return new PetAttackAction(ai); }
    static Action* set_pet_stance(ShadowAI* ai) { return new SetPetStanceAction(ai); }

    static Action* new_rpg_status_update(ShadowAI* ai) { return new NewRpgStatusUpdateAction(ai); }
    static Action* new_rpg_go_grind(ShadowAI* ai) { return new NewRpgGoGrindAction(ai); }
    static Action* new_rpg_go_camp(ShadowAI* ai) { return new NewRpgGoCampAction(ai); }
    static Action* new_rpg_wander_random(ShadowAI* ai) { return new NewRpgWanderRandomAction(ai); }
    static Action* new_rpg_wander_npc(ShadowAI* ai) { return new NewRpgWanderNpcAction(ai); }
    static Action* new_rpg_do_quest(ShadowAI* ai) { return new NewRpgDoQuestAction(ai); }
    static Action* new_rpg_travel_flight(ShadowAI* ai) { return new NewRpgTravelFlightAction(ai); }
};

#endif
