﻿/*
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

#include "ChannelMgr.h"
#include "Channel.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "Log.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"

ChannelMgr::~ChannelMgr()
{
    for (auto itr = _channels.begin(); itr != _channels.end(); ++itr)
        delete itr->second;

    for (auto itr = _customChannels.begin(); itr != _customChannels.end(); ++itr)
        delete itr->second;
}

/*static*/ void ChannelMgr::LoadFromDB()
{
    if (!sWorld->getBoolConfig(CONFIG_PRESERVE_CUSTOM_CHANNELS))
    {
        TC_LOG_INFO("server.loading", ">> 加载了 0 个自定义聊天频道. 自定义频道保存已禁用.");
        return;
    }

    uint32 oldMSTime = getMSTime();
    if (uint32 days = sWorld->getIntConfig(CONFIG_PRESERVE_CUSTOM_CHANNEL_DURATION))
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_OLD_CHANNELS);
        stmt->setUInt32(0, days * DAY);
        CharacterDatabase.Execute(stmt);
    }

    QueryResult result = CharacterDatabase.Query("SELECT name, team, announce, ownership, password, bannedList FROM channels");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> 加载了 0 个自定义聊天频道. 数据库表 `channels` 为空.");
        return;
    }

    std::vector<std::pair<std::string, uint32>> toDelete;
    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        std::string dbName = fields[0].GetString();
        uint32 team = fields[1].GetUInt32();
        bool dbAnnounce = fields[2].GetBool();
        bool dbOwnership = fields[3].GetBool();
        std::string dbPass = fields[4].GetString();
        std::string dbBanned = fields[5].GetString();

        std::wstring channelName;
        if (!Utf8toWStr(dbName, channelName))
        {
            TC_LOG_ERROR("server.loading", "Failed to load custom chat channel '{}' from database - invalid utf8 sequence? Deleted.", dbName);
            toDelete.push_back({ dbName, team });
            continue;
        }

        ChannelMgr* mgr = forTeam(team);
        if (!mgr)
        {
            TC_LOG_ERROR("server.loading", "Failed to load custom chat channel '{}' from database - invalid team {}. Deleted.", dbName, team);
            toDelete.push_back({ dbName, team });
            continue;
        }

        Channel* channel = new Channel(dbName, team, dbBanned);
        channel->SetAnnounce(dbAnnounce);
        channel->SetOwnership(dbOwnership);
        channel->SetPassword(dbPass);
        mgr->_customChannels.emplace(channelName, channel);

        ++count;
    } while (result->NextRow());

    for (auto pair : toDelete)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHANNEL);
        stmt->setString(0, pair.first);
        stmt->setUInt32(1, pair.second);
        CharacterDatabase.Execute(stmt);
    }

    TC_LOG_INFO("server.loading", ">> 加载了 {} 个自定义聊天频道, 用时 {} 毫秒", count, GetMSTimeDiffToNow(oldMSTime));
}

/*static*/ ChannelMgr* ChannelMgr::forTeam(uint32 team)
{
    static ChannelMgr allianceChannelMgr(ALLIANCE);
    static ChannelMgr hordeChannelMgr(HORDE);

    if (sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
        return &allianceChannelMgr;        // cross-faction

    if (team == ALLIANCE)
        return &allianceChannelMgr;

    if (team == HORDE)
        return &hordeChannelMgr;

    return nullptr;
}

Channel* ChannelMgr::GetChannelForPlayerByNamePart(std::string const& namePart, Player* playerSearcher)
{
    std::wstring channelNamePart;
    if (!Utf8toWStr(namePart, channelNamePart))
        return nullptr;

    wstrToLower(channelNamePart);
    for (Channel* channel : playerSearcher->GetJoinedChannels())
    {
        std::string chanName = channel->GetName(playerSearcher->GetSession()->GetSessionDbcLocale());

        std::wstring channelNameW;
        if (!Utf8toWStr(chanName, channelNameW))
            continue;

        wstrToLower(channelNameW);
        if (!channelNameW.compare(0, channelNamePart.size(), channelNamePart))
            return channel;
    }

    return nullptr;
}

void ChannelMgr::SaveToDB()
{
    for (auto pair : _customChannels)
        pair.second->UpdateChannelInDB();
}

Channel* ChannelMgr::GetSystemChannel(uint32 channelId, AreaTableEntry const* zoneEntry)
{
    ChatChannelsEntry const* channelEntry = sChatChannelsStore.AssertEntry(channelId);
    uint32 zoneId = zoneEntry ? zoneEntry->ID : 0;
    if (channelEntry->Flags & (CHANNEL_DBC_FLAG_GLOBAL | CHANNEL_DBC_FLAG_CITY_ONLY))
        zoneId = 0;

    std::pair<uint32, uint32> key = std::make_pair(channelId, zoneId);

    auto itr = _channels.find(key);
    if (itr != _channels.end())
        return itr->second;

    Channel* newChannel = new Channel(channelId, _team, zoneEntry);
    _channels[key] = newChannel;
    return newChannel;
}

Channel* ChannelMgr::CreateCustomChannel(std::string const& name)
{
    std::wstring channelName;
    if (!Utf8toWStr(name, channelName))
        return nullptr;

    wstrToLower(channelName);

    Channel*& c = _customChannels[channelName];
    if (c)
        return nullptr;

    Channel* newChannel = new Channel(name, _team);
    newChannel->SetDirty();

    c = newChannel;
    return newChannel;
}

Channel* ChannelMgr::GetCustomChannel(std::string const& name) const
{
    std::wstring channelName;
    if (!Utf8toWStr(name, channelName))
        return nullptr;

    wstrToLower(channelName);
    auto itr = _customChannels.find(channelName);
    if (itr != _customChannels.end())
        return itr->second;

    return nullptr;
}

Channel* ChannelMgr::GetChannel(uint32 channelId, std::string const& name, Player* player, bool pkt /*= true*/, AreaTableEntry const* zoneEntry /*= nullptr*/) const
{
    Channel* ret = nullptr;
    bool send = false;

    if (channelId) // builtin
    {
        ChatChannelsEntry const* channelEntry = sChatChannelsStore.AssertEntry(channelId);
        uint32 zoneId = zoneEntry ? zoneEntry->ID : 0;
        if (channelEntry->Flags & (CHANNEL_DBC_FLAG_GLOBAL | CHANNEL_DBC_FLAG_CITY_ONLY))
            zoneId = 0;

        std::pair<uint32, uint32> key = std::make_pair(channelId, zoneId);

        auto itr = _channels.find(key);
        if (itr != _channels.end())
            ret = itr->second;
        else
            send = true;
    }
    else // custom
    {
        std::wstring channelName;
        if (!Utf8toWStr(name, channelName))
            return nullptr;

        wstrToLower(channelName);
        auto itr = _customChannels.find(channelName);
        if (itr != _customChannels.end())
            ret = itr->second;
        else
            send = true;
    }

    if (send && pkt)
    {
        std::string channelName = name;
        Channel::GetChannelName(channelName, channelId, player->GetSession()->GetSessionDbcLocale(), zoneEntry);

        WorldPacket data;
        ChannelMgr::MakeNotOnPacket(&data, channelName);
        player->SendDirectMessage(&data);
    }

    return ret;
}

void ChannelMgr::LeftChannel(uint32 channelId, AreaTableEntry const* zoneEntry)
{
    ChatChannelsEntry const* channelEntry = sChatChannelsStore.AssertEntry(channelId);
    uint32 zoneId = zoneEntry ? zoneEntry->ID : 0;
    if (channelEntry->Flags & (CHANNEL_DBC_FLAG_GLOBAL | CHANNEL_DBC_FLAG_CITY_ONLY))
        zoneId = 0;

    std::pair<uint32, uint32> key = std::make_pair(channelId, zoneId);

    auto itr = _channels.find(key);
    if (itr == _channels.end())
        return;

    Channel* channel = itr->second;
    if (!channel->GetNumPlayers())
    {
        _channels.erase(itr);
        delete channel;
    }
}

void ChannelMgr::MakeNotOnPacket(WorldPacket* data, std::string const& name)
{
    data->Initialize(SMSG_CHANNEL_NOTIFY, 1 + name.size());
    (*data) << uint8(CHAT_NOT_MEMBER_NOTICE) << name;
}
