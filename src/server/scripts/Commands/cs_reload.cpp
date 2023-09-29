/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: reload_commandscript
%Complete: 100
Comment: All reload related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "AuctionHouseMgr.h"
#include "BattlegroundMgr.h"
#include "Chat.h"
#include "CreatureTextMgr.h"
#include "DatabaseEnv.h"
#include "DisableMgr.h"
#include "ItemEnchantmentMgr.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Log.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "SkillDiscovery.h"
#include "SkillExtraItems.h"
#include "SmartAI.h"
#include "SpellMgr.h"
#include "StringConvert.h"
#include "TicketMgr.h"
#include "WaypointManager.h"
#include "World.h"

#if TRINITY_COMPILER == TRINITY_COMPILER_GNU
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

class reload_commandscript : public CommandScript
{
public:
    reload_commandscript() : CommandScript("reload_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> reloadAllCommandTable =
        {
            { "achievement",                   rbac::RBAC_PERM_COMMAND_RELOAD_ALL_ACHIEVEMENT,                  true,  &HandleReloadAllAchievementCommand,              "" },
            { "area",                          rbac::RBAC_PERM_COMMAND_RELOAD_ALL_AREA,                         true,  &HandleReloadAllAreaCommand,                     "" },
            { "gossips",                       rbac::RBAC_PERM_COMMAND_RELOAD_ALL_GOSSIP,                       true,  &HandleReloadAllGossipsCommand,                  "" },
            { "item",                          rbac::RBAC_PERM_COMMAND_RELOAD_ALL_ITEM,                         true,  &HandleReloadAllItemCommand,                     "" },
            { "locales",                       rbac::RBAC_PERM_COMMAND_RELOAD_ALL_LOCALES,                      true,  &HandleReloadAllLocalesCommand,                  "" },
            { "loot",                          rbac::RBAC_PERM_COMMAND_RELOAD_ALL_LOOT,                         true,  &HandleReloadAllLootCommand,                     "" },
            { "npc",                           rbac::RBAC_PERM_COMMAND_RELOAD_ALL_NPC,                          true,  &HandleReloadAllNpcCommand,                      "" },
            { "quest",                         rbac::RBAC_PERM_COMMAND_RELOAD_ALL_QUEST,                        true,  &HandleReloadAllQuestCommand,                    "" },
            { "scripts",                       rbac::RBAC_PERM_COMMAND_RELOAD_ALL_SCRIPTS,                      true,  &HandleReloadAllScriptsCommand,                  "" },
            { "spell",                         rbac::RBAC_PERM_COMMAND_RELOAD_ALL_SPELL,                        true,  &HandleReloadAllSpellCommand,                    "" },
            { "",                              rbac::RBAC_PERM_COMMAND_RELOAD_ALL,                              true,  &HandleReloadAllCommand,                         "" },
        };
        static std::vector<ChatCommand> reloadCommandTable =
        {
            { "auctions",                      rbac::RBAC_PERM_COMMAND_RELOAD_AUCTIONS,                         true,  &HandleReloadAuctionsCommand,                   "" },
            { "access_requirement",            rbac::RBAC_PERM_COMMAND_RELOAD_ACCESS_REQUIREMENT,               true,  &HandleReloadAccessRequirementCommand,          "" },
            { "achievement_criteria_data",     rbac::RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_CRITERIA_DATA,        true,  &HandleReloadAchievementCriteriaDataCommand,    "" },
            { "achievement_reward",            rbac::RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_REWARD,               true,  &HandleReloadAchievementRewardCommand,          "" },
            { "all",                           rbac::RBAC_PERM_COMMAND_RELOAD_ALL,                              true,  nullptr,                                           "", reloadAllCommandTable },
            { "areatrigger_involvedrelation",  rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_INVOLVEDRELATION,     true,  &HandleReloadQuestAreaTriggersCommand,          "" },
            { "areatrigger_tavern",            rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TAVERN,               true,  &HandleReloadAreaTriggerTavernCommand,          "" },
            { "areatrigger_teleport",          rbac::RBAC_PERM_COMMAND_RELOAD_AREATRIGGER_TELEPORT,             true,  &HandleReloadAreaTriggerTeleportCommand,        "" },
            { "autobroadcast",                 rbac::RBAC_PERM_COMMAND_RELOAD_AUTOBROADCAST,                    true,  &HandleReloadAutobroadcastCommand,              "" },
            { "battleground_template",         rbac::RBAC_PERM_COMMAND_RELOAD_BATTLEGROUND_TEMPLATE,            true,  &HandleReloadBattlegroundTemplate,              "" },
            { "broadcast_text",                rbac::RBAC_PERM_COMMAND_RELOAD_BROADCAST_TEXT,                   true,  &HandleReloadBroadcastTextCommand,              "" },
            { "conditions",                    rbac::RBAC_PERM_COMMAND_RELOAD_CONDITIONS,                       true,  &HandleReloadConditions,                        "" },
            { "config",                        rbac::RBAC_PERM_COMMAND_RELOAD_CONFIG,                           true,  &HandleReloadConfigCommand,                     "" },
            { "creature_text",                 rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_TEXT,                    true,  &HandleReloadCreatureText,                      "" },
            { "creature_questender",           rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_QUESTENDER,              true,  &HandleReloadCreatureQuestEnderCommand,         "" },
            { "creature_linked_respawn",       rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_LINKED_RESPAWN,          true,  &HandleReloadLinkedRespawnCommand,              "" },
            { "creature_loot_template",        rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_LOOT_TEMPLATE,           true,  &HandleReloadLootTemplatesCreatureCommand,      "" },
            { "creature_movement_override",    rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_MOVEMENT_OVERRIDE,       true,  &HandleReloadCreatureMovementOverrideCommand,   "" },
            { "creature_onkill_reputation",    rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_ONKILL_REPUTATION,       true,  &HandleReloadOnKillReputationCommand,           "" },
            { "creature_queststarter",         rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_QUESTSTARTER,            true,  &HandleReloadCreatureQuestStarterCommand,       "" },
            { "creature_summon_groups",        rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_SUMMON_GROUPS,           true,  &HandleReloadCreatureSummonGroupsCommand,       "" },
            { "creature_template",             rbac::RBAC_PERM_COMMAND_RELOAD_CREATURE_TEMPLATE,                true,  &HandleReloadCreatureTemplateCommand,           "" },
            { "disables",                      rbac::RBAC_PERM_COMMAND_RELOAD_DISABLES,                         true,  &HandleReloadDisablesCommand,                   "" },
            { "disenchant_loot_template",      rbac::RBAC_PERM_COMMAND_RELOAD_DISENCHANT_LOOT_TEMPLATE,         true,  &HandleReloadLootTemplatesDisenchantCommand,    "" },
            { "event_scripts",                 rbac::RBAC_PERM_COMMAND_RELOAD_EVENT_SCRIPTS,                    true,  &HandleReloadEventScriptsCommand,               "" },
            { "fishing_loot_template",         rbac::RBAC_PERM_COMMAND_RELOAD_FISHING_LOOT_TEMPLATE,            true,  &HandleReloadLootTemplatesFishingCommand,       "" },
            { "graveyard_zone",                rbac::RBAC_PERM_COMMAND_RELOAD_GRAVEYARD_ZONE,                   true,  &HandleReloadGameGraveyardZoneCommand,          "" },
            { "game_tele",                     rbac::RBAC_PERM_COMMAND_RELOAD_GAME_TELE,                        true,  &HandleReloadGameTeleCommand,                   "" },
            { "gameobject_questender",         rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUESTENDER,            true,  &HandleReloadGOQuestEnderCommand,               "" },
            { "gameobject_loot_template",      rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUEST_LOOT_TEMPLATE,   true,  &HandleReloadLootTemplatesGameobjectCommand,    "" },
            { "gameobject_queststarter",       rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_QUESTSTARTER,          true,  &HandleReloadGOQuestStarterCommand,             "" },
            { "gm_tickets",                    rbac::RBAC_PERM_COMMAND_RELOAD_GM_TICKETS,                       true,  &HandleReloadGMTicketsCommand,                  "" },
            { "gossip_menu",                   rbac::RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU,                      true,  &HandleReloadGossipMenuCommand,                 "" },
            { "gossip_menu_option",            rbac::RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU_OPTION,               true,  &HandleReloadGossipMenuOptionCommand,           "" },
            { "item_enchantment_template",     rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_ENCHANTMENT_TEMPLATE,        true,  &HandleReloadItemEnchantementsCommand,          "" },
            { "item_loot_template",            rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_LOOT_TEMPLATE,               true,  &HandleReloadLootTemplatesItemCommand,          "" },
            { "item_set_names",                rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_SET_NAMES,                   true,  &HandleReloadItemSetNamesCommand,               "" },
            { "lfg_dungeon_rewards",           rbac::RBAC_PERM_COMMAND_RELOAD_LFG_DUNGEON_REWARDS,              true,  &HandleReloadLfgRewardsCommand,                 "" },
            { "achievement_reward_locale",     rbac::RBAC_PERM_COMMAND_RELOAD_ACHIEVEMENT_REWARD_LOCALE,        true,  &HandleReloadLocalesAchievementRewardCommand,   "" },
            { "creature_template_locale",      rbac::RBAC_PERM_COMMAND_RELOAD_CRETURE_TEMPLATE_LOCALE,          true,  &HandleReloadLocalesCreatureCommand,            "" },
            { "creature_text_locale",          rbac::RBAC_PERM_COMMAND_RELOAD_CRETURE_TEXT_LOCALE,              true,  &HandleReloadLocalesCreatureTextCommand,        "" },
            { "gameobject_template_locale",    rbac::RBAC_PERM_COMMAND_RELOAD_GAMEOBJECT_TEMPLATE_LOCALE,       true,  &HandleReloadLocalesGameobjectCommand,          "" },
            { "gossip_menu_option_locale",     rbac::RBAC_PERM_COMMAND_RELOAD_GOSSIP_MENU_OPTION_LOCALE,        true,  &HandleReloadLocalesGossipMenuOptionCommand,    "" },
            { "item_template_locale",          rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_TEMPLATE_LOCALE,             true,  &HandleReloadLocalesItemCommand,                "" },
            { "item_set_name_locale",          rbac::RBAC_PERM_COMMAND_RELOAD_ITEM_SET_NAME_LOCALE,             true,  &HandleReloadLocalesItemSetNameCommand,         "" },
            { "npc_text_locale",               rbac::RBAC_PERM_COMMAND_RELOAD_NPC_TEXT_LOCALE,                  true,  &HandleReloadLocalesNpcTextCommand,             "" },
            { "page_text_locale",              rbac::RBAC_PERM_COMMAND_RELOAD_PAGE_TEXT_LOCALE,                 true,  &HandleReloadLocalesPageTextCommand,            "" },
            { "points_of_interest_locale",     rbac::RBAC_PERM_COMMAND_RELOAD_POINTS_OF_INTEREST_LOCALE,        true,  &HandleReloadLocalesPointsOfInterestCommand,    "" },
            { "quest_template_locale",         rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_TEMPLATE_LOCALE,            true,  &HandleReloadLocalesQuestCommand,               "" },
            { "mail_level_reward",             rbac::RBAC_PERM_COMMAND_RELOAD_MAIL_LEVEL_REWARD,                true,  &HandleReloadMailLevelRewardCommand,            "" },
            { "mail_loot_template",            rbac::RBAC_PERM_COMMAND_RELOAD_MAIL_LOOT_TEMPLATE,               true,  &HandleReloadLootTemplatesMailCommand,          "" },
            { "milling_loot_template",         rbac::RBAC_PERM_COMMAND_RELOAD_MILLING_LOOT_TEMPLATE,            true,  &HandleReloadLootTemplatesMillingCommand,       "" },
            { "npc_spellclick_spells",         rbac::RBAC_PERM_COMMAND_RELOAD_NPC_SPELLCLICK_SPELLS,            true,  &HandleReloadSpellClickSpellsCommand,           "" },
            { "npc_vendor",                    rbac::RBAC_PERM_COMMAND_RELOAD_NPC_VENDOR,                       true,  &HandleReloadNpcVendorCommand,                  "" },
            { "page_text",                     rbac::RBAC_PERM_COMMAND_RELOAD_PAGE_TEXT,                        true,  &HandleReloadPageTextsCommand,                  "" },
            { "pickpocketing_loot_template",   rbac::RBAC_PERM_COMMAND_RELOAD_PICKPOCKETING_LOOT_TEMPLATE,      true,  &HandleReloadLootTemplatesPickpocketingCommand, "" },
            { "points_of_interest",            rbac::RBAC_PERM_COMMAND_RELOAD_POINTS_OF_INTEREST,               true,  &HandleReloadPointsOfInterestCommand,           "" },
            { "prospecting_loot_template",     rbac::RBAC_PERM_COMMAND_RELOAD_PROSPECTING_LOOT_TEMPLATE,        true,  &HandleReloadLootTemplatesProspectingCommand,   "" },
            { "quest_greeting",                rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_GREETING,                   true,  &HandleReloadQuestGreetingCommand,              "" },
            { "quest_greeting_locale",         rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_GREETING_LOCALE,            true,  &HandleReloadLocalesQuestGreetingCommand,       "" },
            { "quest_poi",                     rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_POI,                        true,  &HandleReloadQuestPOICommand,                   "" },
            { "quest_template",                rbac::RBAC_PERM_COMMAND_RELOAD_QUEST_TEMPLATE,                   true,  &HandleReloadQuestTemplateCommand,              "" },
            { "rbac",                          rbac::RBAC_PERM_COMMAND_RELOAD_RBAC,                             true,  &HandleReloadRBACCommand,                       "" },
            { "reference_loot_template",       rbac::RBAC_PERM_COMMAND_RELOAD_REFERENCE_LOOT_TEMPLATE,          true,  &HandleReloadLootTemplatesReferenceCommand,     "" },
            { "reserved_name",                 rbac::RBAC_PERM_COMMAND_RELOAD_RESERVED_NAME,                    true,  &HandleReloadReservedNameCommand,               "" },
            { "reputation_reward_rate",        rbac::RBAC_PERM_COMMAND_RELOAD_REPUTATION_REWARD_RATE,           true,  &HandleReloadReputationRewardRateCommand,       "" },
            { "reputation_spillover_template", rbac::RBAC_PERM_COMMAND_RELOAD_SPILLOVER_TEMPLATE,               true,  &HandleReloadReputationRewardRateCommand,       "" },
            { "skill_discovery_template",      rbac::RBAC_PERM_COMMAND_RELOAD_SKILL_DISCOVERY_TEMPLATE,         true,  &HandleReloadSkillDiscoveryTemplateCommand,     "" },
            { "skill_extra_item_template",     rbac::RBAC_PERM_COMMAND_RELOAD_SKILL_EXTRA_ITEM_TEMPLATE,        true,  &HandleReloadSkillExtraItemTemplateCommand,     "" },
            { "skill_fishing_base_level",      rbac::RBAC_PERM_COMMAND_RELOAD_SKILL_FISHING_BASE_LEVEL,         true,  &HandleReloadSkillFishingBaseLevelCommand,      "" },
            { "skinning_loot_template",        rbac::RBAC_PERM_COMMAND_RELOAD_SKINNING_LOOT_TEMPLATE,           true,  &HandleReloadLootTemplatesSkinningCommand,      "" },
            { "smart_scripts",                 rbac::RBAC_PERM_COMMAND_RELOAD_SMART_SCRIPTS,                    true,  &HandleReloadSmartScripts,                      "" },
            { "spell_required",                rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_REQUIRED,                   true,  &HandleReloadSpellRequiredCommand,              "" },
            { "spell_area",                    rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_AREA,                       true,  &HandleReloadSpellAreaCommand,                  "" },
            { "spell_bonus_data",              rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_BONUS_DATA,                 true,  &HandleReloadSpellBonusesCommand,               "" },
            { "spell_group",                   rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_GROUP,                      true,  &HandleReloadSpellGroupsCommand,                "" },
            { "spell_learn_spell",             rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_LEARN_SPELL,                true,  &HandleReloadSpellLearnSpellCommand,            "" },
            { "spell_loot_template",           rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_LOOT_TEMPLATE,              true,  &HandleReloadLootTemplatesSpellCommand,         "" },
            { "spell_linked_spell",            rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_LINKED_SPELL,               true,  &HandleReloadSpellLinkedSpellCommand,           "" },
            { "spell_pet_auras",               rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_PET_AURAS,                  true,  &HandleReloadSpellPetAurasCommand,              "" },
            { "spell_proc",                    rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_PROC,                       true,  &HandleReloadSpellProcsCommand,                 "" },
            { "spell_scripts",                 rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_SCRIPTS,                    true,  &HandleReloadSpellScriptsCommand,               "" },
            { "spell_target_position",         rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_TARGET_POSITION,            true,  &HandleReloadSpellTargetPositionCommand,        "" },
            { "spell_threats",                 rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_THREATS,                    true,  &HandleReloadSpellThreatsCommand,               "" },
            { "spell_group_stack_rules",       rbac::RBAC_PERM_COMMAND_RELOAD_SPELL_GROUP_STACK_RULES,          true,  &HandleReloadSpellGroupStackRulesCommand,       "" },
            { "trainer",                       rbac::RBAC_PERM_COMMAND_RELOAD_TRAINER,                          true,  &HandleReloadTrainerCommand,                    "" },
            { "trinity_string",                rbac::RBAC_PERM_COMMAND_RELOAD_TRINITY_STRING,                   true,  &HandleReloadTrinityStringCommand,              "" },
            { "waypoint_scripts",              rbac::RBAC_PERM_COMMAND_RELOAD_WAYPOINT_SCRIPTS,                 true,  &HandleReloadWpScriptsCommand,                  "" },
            { "waypoint_data",                 rbac::RBAC_PERM_COMMAND_RELOAD_WAYPOINT_DATA,                    true,  &HandleReloadWpCommand,                         "" },
            { "vehicle_template",              rbac::RBAC_PERM_COMMAND_RELOAD_VEHICLE_TEMPLATE,                 true,  &HandleReloadVehicleTemplateCommand,            "" },
            { "vehicle_accessory",             rbac::RBAC_PERM_COMMAND_RELOAD_VEHICLE_ACCESORY,                 true,  &HandleReloadVehicleAccessoryCommand,           "" },
            { "vehicle_template_accessory",    rbac::RBAC_PERM_COMMAND_RELOAD_VEHICLE_TEMPLATE_ACCESSORY,       true,  &HandleReloadVehicleTemplateAccessoryCommand,   "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "reload",                        rbac::RBAC_PERM_COMMAND_RELOAD,                                  true,  nullptr,                                           "", reloadCommandTable },
        };
        return commandTable;
    }

    //reload commands
    static bool HandleReloadGMTicketsCommand(ChatHandler* /*handler*/, char const* /*args*/)
    {
        sTicketMgr->LoadTickets();
        return true;
    }

    static bool HandleReloadAllCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadSkillFishingBaseLevelCommand(handler, "");

        HandleReloadAllAchievementCommand(handler, "");
        HandleReloadAllAreaCommand(handler, "");
        HandleReloadAllLootCommand(handler, "");
        HandleReloadAllNpcCommand(handler, "");
        HandleReloadAllQuestCommand(handler, "");
        HandleReloadAllSpellCommand(handler, "");
        HandleReloadAllItemCommand(handler, "");
        HandleReloadAllGossipsCommand(handler, "");
        HandleReloadAllLocalesCommand(handler, "");

        HandleReloadAccessRequirementCommand(handler, "");
        HandleReloadMailLevelRewardCommand(handler, "");
        HandleReloadReservedNameCommand(handler, "");
        HandleReloadTrinityStringCommand(handler, "");
        HandleReloadGameTeleCommand(handler, "");

        HandleReloadCreatureMovementOverrideCommand(handler, "");
        HandleReloadCreatureSummonGroupsCommand(handler);

        HandleReloadVehicleAccessoryCommand(handler, "");
        HandleReloadVehicleTemplateAccessoryCommand(handler, "");

        HandleReloadAutobroadcastCommand(handler, "");
        HandleReloadBattlegroundTemplate(handler, "");
        return true;
    }

    static bool HandleReloadAllAchievementCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadAchievementCriteriaDataCommand(handler, "");
        HandleReloadAchievementRewardCommand(handler, "");
        return true;
    }

    static bool HandleReloadAllAreaCommand(ChatHandler* handler, char const* /*args*/)
    {
        //HandleReloadQuestAreaTriggersCommand(handler, ""); -- reloaded in HandleReloadAllQuestCommand
        HandleReloadAreaTriggerTeleportCommand(handler, "");
        HandleReloadAreaTriggerTavernCommand(handler, "");
        HandleReloadGameGraveyardZoneCommand(handler, "");
        return true;
    }

    static bool HandleReloadAllLootCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表...");
        LoadLootTables();
        handler->SendGlobalGMSysMessage("数据库表 `*_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadAllNpcCommand(ChatHandler* handler, char const* args)
    {
        if (*args != 'a')                                          // will be reloaded from all_gossips
        HandleReloadTrainerCommand(handler, "a");
        HandleReloadNpcVendorCommand(handler, "a");
        HandleReloadPointsOfInterestCommand(handler, "a");
        HandleReloadSpellClickSpellsCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllQuestCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadQuestGreetingCommand(handler, "");
        HandleReloadQuestAreaTriggersCommand(handler, "a");
        HandleReloadQuestPOICommand(handler, "a");
        HandleReloadQuestTemplateCommand(handler, "a");

        TC_LOG_INFO("misc", "重新加载任务关联数据...");
        sObjectMgr->LoadQuestStartersAndEnders();
        handler->SendGlobalGMSysMessage("数据库表 `*_queststarter` and `*_questender` 已重新加载.");
        return true;
    }

    static bool HandleReloadAllScriptsCommand(ChatHandler* handler, char const* /*args*/)
    {
        if (sMapMgr->IsScriptScheduled())
        {
            handler->PSendSysMessage("当前正在使用数据库脚本, 请稍后尝试重新加载.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        TC_LOG_INFO("misc", "重新加载脚本...");
        HandleReloadEventScriptsCommand(handler, "a");
        HandleReloadSpellScriptsCommand(handler, "a");
        handler->SendGlobalGMSysMessage("数据库表 `*_scripts` 已重新加载.");
        HandleReloadWpScriptsCommand(handler, "a");
        HandleReloadWpCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllSpellCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadSkillDiscoveryTemplateCommand(handler, "a");
        HandleReloadSkillExtraItemTemplateCommand(handler, "a");
        HandleReloadSpellRequiredCommand(handler, "a");
        HandleReloadSpellAreaCommand(handler, "a");
        HandleReloadSpellGroupsCommand(handler, "a");
        HandleReloadSpellLearnSpellCommand(handler, "a");
        HandleReloadSpellLinkedSpellCommand(handler, "a");
        HandleReloadSpellProcsCommand(handler, "a");
        HandleReloadSpellBonusesCommand(handler, "a");
        HandleReloadSpellTargetPositionCommand(handler, "a");
        HandleReloadSpellThreatsCommand(handler, "a");
        HandleReloadSpellGroupStackRulesCommand(handler, "a");
        HandleReloadSpellPetAurasCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllGossipsCommand(ChatHandler* handler, char const* args)
    {
        HandleReloadGossipMenuCommand(handler, "a");
        HandleReloadGossipMenuOptionCommand(handler, "a");
        if (*args != 'a')                                          // already reload from all_scripts
        HandleReloadPointsOfInterestCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllItemCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadPageTextsCommand(handler, "a");
        HandleReloadItemEnchantementsCommand(handler, "a");
        return true;
    }

    static bool HandleReloadAllLocalesCommand(ChatHandler* handler, char const* /*args*/)
    {
        HandleReloadLocalesAchievementRewardCommand(handler, "a");
        HandleReloadLocalesCreatureCommand(handler, "a");
        HandleReloadLocalesCreatureTextCommand(handler, "a");
        HandleReloadLocalesGameobjectCommand(handler, "a");
        HandleReloadLocalesGossipMenuOptionCommand(handler, "a");
        HandleReloadLocalesItemCommand(handler, "a");
        HandleReloadLocalesNpcTextCommand(handler, "a");
        HandleReloadLocalesPageTextCommand(handler, "a");
        HandleReloadLocalesPointsOfInterestCommand(handler, "a");
        HandleReloadLocalesQuestCommand(handler, "a");
        HandleReloadLocalesQuestOfferRewardCommand(handler, "a");
        HandleReloadLocalesQuestRequestItemsCommand(handler, "a");
        HandleReloadLocalesQuestGreetingCommand(handler, "");
        return true;
    }

    static bool HandleReloadConfigCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载配置设置...");
        sWorld->LoadConfigSettings(true);
        sMapMgr->InitializeVisibilityDistanceInfo();
        handler->SendGlobalGMSysMessage("世界配置设置已重新加载.");
        return true;
    }

    static bool HandleReloadAccessRequirementCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载进入副本或区域条件...");
        sObjectMgr->LoadAccessRequirements();
        handler->SendGlobalGMSysMessage("数据库表 `access_requirement` 已重新加载.");
        return true;
    }

    static bool HandleReloadAchievementCriteriaDataCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载额外成就条件数据...");
        sAchievementMgr->LoadAchievementCriteriaData();
        handler->SendGlobalGMSysMessage("数据库表 `achievement_criteria_data` 已重新加载.");
        return true;
    }

    static bool HandleReloadAchievementRewardCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载成就奖励数据...");
        sAchievementMgr->LoadRewards();
        handler->SendGlobalGMSysMessage("数据库表 `achievement_reward` 已重新加载.");
        return true;
    }

    static bool HandleReloadAreaTriggerTavernCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载酒馆区域触发器...");
        sObjectMgr->LoadTavernAreaTriggers();
        handler->SendGlobalGMSysMessage("数据库表 `areatrigger_tavern` 已重新加载.");
        return true;
    }

    static bool HandleReloadAreaTriggerTeleportCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载区域触发器传送点定义...");
        sObjectMgr->LoadAreaTriggerTeleports();
        handler->SendGlobalGMSysMessage("数据库表 `areatrigger_teleport` 已重新加载.");
        return true;
    }

    static bool HandleReloadAutobroadcastCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载自动广播...");
        sWorld->LoadAutobroadcasts();
        handler->SendGlobalGMSysMessage("数据库表 `autobroadcast` 已重新加载.");
        return true;
    }

    static bool HandleReloadBattlegroundTemplate(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战场模板...");
        sBattlegroundMgr->LoadBattlegroundTemplates();
        handler->SendGlobalGMSysMessage("数据库表 `battleground_template` 已重新加载.");
        return true;
    }

    static bool HandleReloadBroadcastTextCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载广播文本...");
        sObjectMgr->LoadBroadcastTexts();
        sObjectMgr->LoadBroadcastTextLocales();
        handler->SendGlobalGMSysMessage("数据库表 `broadcast_text` 已重新加载.");
        return true;
    }

    static bool HandleReloadOnKillReputationCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载生物奖励声望定义...");
        sObjectMgr->LoadReputationOnKill();
        handler->SendGlobalGMSysMessage("数据库表 `creature_onkill_reputation` 已重新加载.");
        return true;
    }

    static bool HandleReloadCreatureSummonGroupsCommand(ChatHandler* handler)
    {
        TC_LOG_INFO("misc", "重新加载生物召唤组...");
        sObjectMgr->LoadTempSummons();
        handler->SendGlobalGMSysMessage("数据库表 `creature_summon_groups` 已重新加载.");
        return true;
    }

    static bool HandleReloadCreatureTemplateCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        for (std::string_view entryStr : Trinity::Tokenize(args, ' ', false))
        {
            uint32 entry = Trinity::StringTo<uint32>(entryStr).value_or(0);

            WorldDatabasePreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_CREATURE_TEMPLATE);
            stmt->setUInt32(0, entry);
            PreparedQueryResult result = WorldDatabase.Query(stmt);

            if (!result)
            {
                handler->PSendSysMessage(LANG_COMMAND_CREATURETEMPLATE_NOTFOUND, entry);
                continue;
            }

            CreatureTemplate const* cInfo = sObjectMgr->GetCreatureTemplate(entry);
            if (!cInfo)
            {
                handler->PSendSysMessage(LANG_COMMAND_CREATURESTORAGE_NOTFOUND, entry);
                continue;
            }

            TC_LOG_INFO("misc", "重新加载生物模板 entry {}", entry);

            Field* fields = result->Fetch();
            sObjectMgr->LoadCreatureTemplate(fields);
            sObjectMgr->CheckCreatureTemplate(cInfo);
        }

        sObjectMgr->InitializeQueriesData(QUERY_DATA_CREATURES);
        handler->SendGlobalGMSysMessage("生物模板已重新加载.");
        return true;
    }

    static bool HandleReloadCreatureQuestStarterCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "加载任务关联... (`creature_queststarter`)");
        sObjectMgr->LoadCreatureQuestStarters();
        handler->SendGlobalGMSysMessage("数据库表 `creature_queststarter` 已重新加载.");
        return true;
    }

    static bool HandleReloadLinkedRespawnCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "加载关联重生... (`creature_linked_respawn`)");
        sObjectMgr->LoadLinkedRespawn();
        handler->SendGlobalGMSysMessage("数据库表 `creature_linked_respawn` (与生物关联的重生) 已重新加载.");
        return true;
    }

    static bool HandleReloadCreatureQuestEnderCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "加载任务关系... (`creature_questender`)");
        sObjectMgr->LoadCreatureQuestEnders();
        handler->SendGlobalGMSysMessage("数据库表 `creature_questender` 已重新加载.");
        return true;
    }

    static bool HandleReloadGossipMenuCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `gossip_menu` 表!");
        sObjectMgr->LoadGossipMenu();
        handler->SendGlobalGMSysMessage("数据库表 `gossip_menu` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadGossipMenuOptionCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `gossip_menu_option` 表!");
        sObjectMgr->LoadGossipMenuItems();
        handler->SendGlobalGMSysMessage("数据库表 `gossip_menu_option` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadGOQuestStarterCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "加载任务关系... (`gameobject_queststarter`)");
        sObjectMgr->LoadGameobjectQuestStarters();
        handler->SendGlobalGMSysMessage("数据库表 `gameobject_queststarter` 已重新加载.");
        return true;
    }

    static bool HandleReloadGOQuestEnderCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "加载任务关系... (`gameobject_questender`)");
        sObjectMgr->LoadGameobjectQuestEnders();
        handler->SendGlobalGMSysMessage("数据库表 `gameobject_questender` 已重新加载.");
        return true;
    }

    static bool HandleReloadQuestAreaTriggersCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务区域触发器...");
        sObjectMgr->LoadQuestAreaTriggers();
        handler->SendGlobalGMSysMessage("数据库表 `areatrigger_involvedrelation` (任务区域触发器) 已重新加载.");
        return true;
    }

    static bool HandleReloadQuestGreetingCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务问候...");
        sObjectMgr->LoadQuestGreetings();
        handler->SendGlobalGMSysMessage("数据库表 `quest_greeting` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesQuestGreetingCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务问候本地化数据...");
        sObjectMgr->LoadQuestGreetingLocales();
        handler->SendGlobalGMSysMessage("数据库表 `quest_greeting_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadQuestTemplateCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务模板...");
        sObjectMgr->LoadQuests();
        sObjectMgr->InitializeQueriesData(QUERY_DATA_QUESTS);
        handler->SendGlobalGMSysMessage("数据库表 `quest_template` (任务定义) 已重新加载.");

        /// dependent also from `gameobject` but this table not reloaded anyway
        TC_LOG_INFO("misc", "重新加载用于任务的游戏对象...");
        sObjectMgr->LoadGameObjectForQuests();
        handler->SendGlobalGMSysMessage("任务的游戏对象数据已重新加载.");
        return true;
    }

    static bool HandleReloadLootTemplatesCreatureCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`creature_loot_template`)");
        LoadLootTemplates_Creature();
        LootTemplates_Creature.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `creature_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadCreatureMovementOverrideCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "Re-Loading Creature movement overrides...");
        sObjectMgr->LoadCreatureMovementOverrides();
        handler->SendGlobalGMSysMessage("数据库表 `creature_movement_override` 已重新加载.");
        return true;
    }

    static bool HandleReloadLootTemplatesDisenchantCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`disenchant_loot_template`)");
        LoadLootTemplates_Disenchant();
        LootTemplates_Disenchant.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `disenchant_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesFishingCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`fishing_loot_template`)");
        LoadLootTemplates_Fishing();
        LootTemplates_Fishing.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `fishing_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesGameobjectCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`gameobject_loot_template`)");
        LoadLootTemplates_Gameobject();
        LootTemplates_Gameobject.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `gameobject_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesItemCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`item_loot_template`)");
        LoadLootTemplates_Item();
        LootTemplates_Item.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `item_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesMillingCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`milling_loot_template`)");
        LoadLootTemplates_Milling();
        LootTemplates_Milling.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `milling_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesPickpocketingCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`pickpocketing_loot_template`)");
        LoadLootTemplates_Pickpocketing();
        LootTemplates_Pickpocketing.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `pickpocketing_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesProspectingCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`prospecting_loot_template`)");
        LoadLootTemplates_Prospecting();
        LootTemplates_Prospecting.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `prospecting_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesMailCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`mail_loot_template`)");
        LoadLootTemplates_Mail();
        LootTemplates_Mail.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `mail_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesReferenceCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`reference_loot_template`)");
        LoadLootTemplates_Reference();
        handler->SendGlobalGMSysMessage("数据库表 `reference_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesSkinningCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`skinning_loot_template`)");
        LoadLootTemplates_Skinning();
        LootTemplates_Skinning.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `skinning_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadLootTemplatesSpellCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载战利品表... (`spell_loot_template`)");
        LoadLootTemplates_Spell();
        LootTemplates_Spell.CheckLootRefs();
        handler->SendGlobalGMSysMessage("数据库表 `spell_loot_template` 已重新加载.");
        sConditionMgr->LoadConditions(true);
        return true;
    }

    static bool HandleReloadTrinityStringCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 trinity_string 表!");
        sObjectMgr->LoadTrinityStrings();
        handler->SendGlobalGMSysMessage("数据库表 `trinity_string` 已重新加载.");
        return true;
    }

    static bool HandleReloadTrainerCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `trainer` 表!");
        sObjectMgr->LoadTrainers();
        sObjectMgr->LoadCreatureDefaultTrainers();
        handler->SendGlobalGMSysMessage("数据库表 `trainer` 已重新加载.");
        handler->SendGlobalGMSysMessage("数据库表 `trainer_locale` 已重新加载.");
        handler->SendGlobalGMSysMessage("数据库表 `trainer_spell` 已重新加载.");
        handler->SendGlobalGMSysMessage("数据库表 `creature_default_trainer` 已重新加载.");
        return true;
    }

    static bool HandleReloadNpcVendorCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `npc_vendor` 表!");
        sObjectMgr->LoadVendors();
        handler->SendGlobalGMSysMessage("数据库表 `npc_vendor` 已重新加载.");
        return true;
    }

    static bool HandleReloadPointsOfInterestCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `points_of_interest` 表!");
        sObjectMgr->LoadPointsOfInterest();
        handler->SendGlobalGMSysMessage("数据库表 `points_of_interest` 已重新加载.");
        return true;
    }

    static bool HandleReloadQuestPOICommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务兴趣点...");
        sObjectMgr->LoadQuestPOI();
        sObjectMgr->InitializeQueriesData(QUERY_DATA_POIS);
        handler->SendGlobalGMSysMessage("数据库表 `quest_poi` and `quest_poi_points` 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellClickSpellsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `npc_spellclick_spells` 表!");
        sObjectMgr->LoadNPCSpellClickSpells();
        handler->SendGlobalGMSysMessage("数据库表 `npc_spellclick_spells` 已重新加载.");
        return true;
    }

    static bool HandleReloadReservedNameCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "加载保留名称... (`reserved_name`)");
        sObjectMgr->LoadReservedPlayersNames();
        handler->SendGlobalGMSysMessage("数据库表 `reserved_name` (玩家保留名) 已重新加载.");
        return true;
    }

    static bool HandleReloadReputationRewardRateCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `reputation_reward_rate` 重新加载!" );
        sObjectMgr->LoadReputationRewardRate();
        handler->SendGlobalSysMessage("数据库表 `reputation_reward_rate` 已重新加载.");
        return true;
    }

    static bool HandleReloadReputationSpilloverTemplateCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `reputation_spillover_template` 重新加载!" );
        sObjectMgr->LoadReputationSpilloverTemplate();
        handler->SendGlobalSysMessage("数据库表 `reputation_spillover_template` 已重新加载.");
        return true;
    }

    static bool HandleReloadSkillDiscoveryTemplateCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载专业技能发现表...");
        LoadSkillDiscoveryTable();
        handler->SendGlobalGMSysMessage("数据库表 `skill_discovery_template` (制作时发现的配方) 已重新加载.");
        return true;
    }

    static bool HandleReloadSkillPerfectItemTemplateCommand(ChatHandler* handler, char const* /*args*/)
    { // latched onto HandleReloadSkillExtraItemTemplateCommand as it's part of that table group (and i don't want to chance all the command IDs)
        TC_LOG_INFO("misc", "重新加载法术完美定义数据表...");
        LoadSkillPerfectItemTable();
        handler->SendGlobalGMSysMessage("数据库表 `skill_perfect_item_template` (制作时完美的物品机率) 已重新加载.");
        return true;
    }

    static bool HandleReloadSkillExtraItemTemplateCommand(ChatHandler* handler, char const* args)
    {
        TC_LOG_INFO("misc", "重新加载专业技能额外物品数据表...");
        LoadSkillExtraItemTable();
        handler->SendGlobalGMSysMessage("数据库表 `skill_extra_item_template` (制作时的额外物品创造) 已重新加载.");

        return HandleReloadSkillPerfectItemTemplateCommand(handler, args);
    }

    static bool HandleReloadSkillFishingBaseLevelCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载钓鱼的基本等级要求...");
        sObjectMgr->LoadFishingBaseSkillLevel();
        handler->SendGlobalGMSysMessage("数据库表 `skill_fishing_base_level` (区域/子区域的钓鱼基本等级) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellAreaCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术区域数据...");
        sSpellMgr->LoadSpellAreas();
        handler->SendGlobalGMSysMessage("数据库表 `spell_area` (法术依赖于区域/任务/光环状态) reloaded.");
        return true;
    }

    static bool HandleReloadSpellRequiredCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术需求数据...");
        sSpellMgr->LoadSpellRequired();
        handler->SendGlobalGMSysMessage("数据库表 `spell_required` 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellGroupsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术组...");
        sSpellMgr->LoadSpellGroups();
        handler->SendGlobalGMSysMessage("数据库表 `spell_group` (法术组) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellLearnSpellCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术学习法术...");
        sSpellMgr->LoadSpellLearnSpells();
        handler->SendGlobalGMSysMessage("数据库表 `spell_learn_spell` 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellLinkedSpellCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术链接法术...");
        sSpellMgr->LoadSpellLinked();
        handler->SendGlobalGMSysMessage("数据库表 `spell_linked_spell` 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellProcsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术触发条件和数据...");
        sSpellMgr->LoadSpellProcs();
        handler->SendGlobalGMSysMessage("数据库表 `spell_proc` (法术触发条件和数据) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellBonusesCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术加成数据...");
        sSpellMgr->LoadSpellBonuses();
        handler->SendGlobalGMSysMessage("数据库表 `spell_bonus_data` (法术伤害/治疗系数) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellTargetPositionCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术目标坐标...");
        sSpellMgr->LoadSpellTargetPositions();
        handler->SendGlobalGMSysMessage("数据库表 `spell_target_position` (法术目标的目的地坐标) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellThreatsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载仇恨法术定义...");
        sSpellMgr->LoadSpellThreats();
        handler->SendGlobalGMSysMessage("数据库表 `spell_threat` (法术仇恨定义) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellGroupStackRulesCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术组堆叠规则...");
        sSpellMgr->LoadSpellGroupStackRules();
        handler->SendGlobalGMSysMessage("数据库表 `spell_group_stack_rules` (法术堆叠规则定义) 已重新加载.");
        return true;
    }

    static bool HandleReloadSpellPetAurasCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载法术宠物光环...");
        sSpellMgr->LoadSpellPetAuras();
        handler->SendGlobalGMSysMessage("数据库表 `spell_pet_auras` 已重新加载.");
        return true;
    }

    static bool HandleReloadPageTextsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载页面文本...");
        sObjectMgr->LoadPageTexts();
        handler->SendGlobalGMSysMessage("数据库表 `page_text` 已重新加载.");
        return true;
    }

    static bool HandleReloadItemEnchantementsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载物品随机附魔表...");
        LoadRandomEnchantmentsTable();
        handler->SendGlobalGMSysMessage("数据库表 `item_enchantment_template` 已重新加载.");
        return true;
    }

    static bool HandleReloadItemSetNamesCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载物品套装名称...");
        sObjectMgr->LoadItemSetNames();
        handler->SendGlobalGMSysMessage("数据库表 `item_set_names` 已重新加载.");
        return true;
    }

    static bool HandleReloadEventScriptsCommand(ChatHandler* handler, char const* args)
    {
        if (sMapMgr->IsScriptScheduled())
        {
            handler->SendSysMessage("当前正在使用数据库脚本, 请稍后尝试重新加载.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*args != 'a')
            TC_LOG_INFO("misc", "重新加载 `event_scripts` 的脚本...");

        sObjectMgr->LoadEventScripts();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("数据库表 `event_scripts` 已重新加载.");

        return true;
    }

    static bool HandleReloadWpScriptsCommand(ChatHandler* handler, char const* args)
    {
        if (sMapMgr->IsScriptScheduled())
        {
            handler->SendSysMessage("当前正在使用数据库脚本, 请稍后尝试重新加载.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*args != 'a')
            TC_LOG_INFO("misc", "重新加载 `waypoint_scripts` 的脚本...");

        sObjectMgr->LoadWaypointScripts();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("数据库表 `waypoint_scripts` 已重新加载.");

        return true;
    }

    static bool HandleReloadWpCommand(ChatHandler* handler, char const* args)
    {
        if (*args != 'a')
            TC_LOG_INFO("misc", "重新加载 'waypoints_data' 的路径点数据");

        sWaypointMgr->Load();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("数据库表 'waypoint_data' 已重新加载.");

        return true;
    }

    static bool HandleReloadSpellScriptsCommand(ChatHandler* handler, char const* args)
    {
        if (sMapMgr->IsScriptScheduled())
        {
            handler->SendSysMessage("当前正在使用数据库脚本, 请稍后尝试重新加载.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (*args != 'a')
            TC_LOG_INFO("misc", "重新加载 `spell_scripts` 的脚本...");

        sObjectMgr->LoadSpellScripts();

        if (*args != 'a')
            handler->SendGlobalGMSysMessage("数据库表 `spell_scripts` 已重新加载.");

        return true;
    }

    static bool HandleReloadGameGraveyardZoneCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载墓地-区域链接...");

        sObjectMgr->LoadGraveyardZones();

        handler->SendGlobalGMSysMessage("数据库表 `game_graveyard_zone` 已重新加载.");

        return true;
    }

    static bool HandleReloadGameTeleCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载游戏传送坐标...");

        sObjectMgr->LoadGameTele();

        handler->SendGlobalGMSysMessage("数据库表 `game_tele` 已重新加载.");

        return true;
    }

    static bool HandleReloadDisablesCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载禁用表...");
        DisableMgr::LoadDisables();
        TC_LOG_INFO("misc", "检查任务禁用...");
        DisableMgr::CheckQuestDisables();
        handler->SendGlobalGMSysMessage("数据库表 `disables` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesAchievementRewardCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载成就奖励数据的地化数据...");
        sAchievementMgr->LoadRewardLocales();
        handler->SendGlobalGMSysMessage("数据库表 `achievement_reward_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLfgRewardsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载随机地下城的奖励数据...");
        sLFGMgr->LoadRewards();
        handler->SendGlobalGMSysMessage("数据库表 `lfg_dungeon_rewards` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesCreatureCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载生物模板本地化数据...");
        sObjectMgr->LoadCreatureLocales();
        handler->SendGlobalGMSysMessage("数据库表 `creature_template_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesCreatureTextCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载生物文本本地化数据...");
        sCreatureTextMgr->LoadCreatureTextLocales();
        handler->SendGlobalGMSysMessage("数据库表 `creature_text_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesGameobjectCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载游戏对象模板本地化数据...");
        sObjectMgr->LoadGameObjectLocales();
        handler->SendGlobalGMSysMessage("数据库表 `gameobject_template_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesGossipMenuOptionCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载对话菜单选项本地化数据...");
        sObjectMgr->LoadGossipMenuItemsLocales();
        handler->SendGlobalGMSysMessage("数据库表 `gossip_menu_option_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesItemCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载物品模板本地化数据...");
        sObjectMgr->LoadItemLocales();
        handler->SendGlobalGMSysMessage("数据库表 `item_template_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesItemSetNameCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载物品套装名称本地化数据...");
        sObjectMgr->LoadItemSetNameLocales();
        handler->SendGlobalGMSysMessage("数据库表 `item_set_name_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesNpcTextCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 NPC 文本本地化数据...");
        sObjectMgr->LoadNpcTextLocales();
        handler->SendGlobalGMSysMessage("数据库表 `npc_text_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesPageTextCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载页面文本本地化数据...");
        sObjectMgr->LoadPageTextLocales();
        handler->SendGlobalGMSysMessage("数据库表 `page_text_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesPointsOfInterestCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载兴趣点本地化数据...");
        sObjectMgr->LoadPointOfInterestLocales();
        handler->SendGlobalGMSysMessage("数据库表 `points_of_interest_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesQuestCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务模板本地化数据...");
        sObjectMgr->LoadQuestLocales();
        handler->SendGlobalGMSysMessage("数据库表 `quest_template_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesQuestOfferRewardCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务奖励本地化数据...");
        sObjectMgr->LoadQuestOfferRewardLocale();
        handler->SendGlobalGMSysMessage("数据库表 `quest_offer_reward_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadLocalesQuestRequestItemsCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载任务请求物品本地化数据...");
        sObjectMgr->LoadQuestRequestItemsLocale();
        handler->SendGlobalGMSysMessage("数据库表 `quest_request_item_locale` 已重新加载.");
        return true;
    }

    static bool HandleReloadMailLevelRewardCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载玩家等级相关的邮件奖励...");
        sObjectMgr->LoadMailLevelRewards();
        handler->SendGlobalGMSysMessage("数据库表 `mail_level_reward` 已重新加载.");
        return true;
    }

    static bool HandleReloadAuctionsCommand(ChatHandler* handler, char const* /*args*/)
    {
        ///- Reload dynamic data tables from the database
        TC_LOG_INFO("misc", "重新加载拍卖行...");
        sAuctionMgr->LoadAuctionItems();
        sAuctionMgr->LoadAuctions();
        handler->SendGlobalGMSysMessage("拍卖行已重新加载.");
        return true;
    }

    static bool HandleReloadConditions(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载条件...");
        sConditionMgr->LoadConditions(true);
        handler->SendGlobalGMSysMessage("条件已重新加载.");
        return true;
    }

    static bool HandleReloadCreatureText(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载生物文本...");
        sCreatureTextMgr->LoadCreatureTexts();
        handler->SendGlobalGMSysMessage("生物文本已重新加载.");
        return true;
    }

    static bool HandleReloadSmartScripts(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载智能脚本...");
        sSmartScriptMgr->LoadSmartAIFromDB();
        handler->SendGlobalGMSysMessage("智能脚本已重新加载.");
        return true;
    }

    static bool HandleReloadVehicleTemplateCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 vehicle_template 表...");
        sObjectMgr->LoadVehicleTemplate();
        handler->SendGlobalGMSysMessage("载具模板已重新加载.");
        return true;
    }

    static bool HandleReloadVehicleAccessoryCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `vehicle_accessory` 表...");
        sObjectMgr->LoadVehicleAccessories();
        handler->SendGlobalGMSysMessage("载具驾驶员已重新加载.");
        return true;
    }

    static bool HandleReloadVehicleTemplateAccessoryCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `vehicle_template_accessory` 表...");
        sObjectMgr->LoadVehicleTemplateAccessories();
        handler->SendGlobalGMSysMessage("载具模板驾驶员已重新加载.");
        return true;
    }

    static bool HandleReloadRBACCommand(ChatHandler* handler, char const* /*args*/)
    {
        TC_LOG_INFO("misc", "重新加载 `RBAC` 表...");
        sAccountMgr->LoadRBAC();
        sWorld->ReloadRBAC();
        handler->SendGlobalGMSysMessage("RBAC 数据已重新加载.");
        return true;
    }
};

void AddSC_reload_commandscript()
{
    new reload_commandscript();
}
