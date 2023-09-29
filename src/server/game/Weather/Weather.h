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

/// \addtogroup world
/// @{
/// \file

#ifndef __WEATHER_H
#define __WEATHER_H

#include "Common.h"
#include "SharedDefines.h"
#include "Timer.h"

class Player;

#define WEATHER_SEASONS 4
struct WeatherSeasonChances
{
    uint32 rainChance;
    uint32 snowChance;
    uint32 stormChance;
};

struct WeatherData
{
    WeatherSeasonChances data[WEATHER_SEASONS];
    uint32 ScriptId;
};

enum WeatherState : uint32  //天气状态
{
    WEATHER_STATE_FINE              = 0,    // 晴天
    WEATHER_STATE_FOG               = 1,    // 雾
    WEATHER_STATE_DRIZZLE           = 2,    // 细雨
    WEATHER_STATE_LIGHT_RAIN        = 3,    // 小雨
    WEATHER_STATE_MEDIUM_RAIN       = 4,    // 中雨
    WEATHER_STATE_HEAVY_RAIN        = 5,    // 大雨
    WEATHER_STATE_LIGHT_SNOW        = 6,    // 小雪
    WEATHER_STATE_MEDIUM_SNOW       = 7,    // 中雪
    WEATHER_STATE_HEAVY_SNOW        = 8,    // 大雪
    WEATHER_STATE_LIGHT_SANDSTORM   = 22,   // 小沙尘暴
    WEATHER_STATE_MEDIUM_SANDSTORM  = 41,   // 中沙尘暴
    WEATHER_STATE_HEAVY_SANDSTORM   = 42,   // 大沙尘暴
    WEATHER_STATE_THUNDERS          = 86,   // 雷暴
    WEATHER_STATE_BLACKRAIN         = 90,   // 黑雨
    WEATHER_STATE_BLACKSNOW         = 106   // 黑雪
};

/// Weather for one zone
class TC_GAME_API Weather
{
    public:

        Weather(uint32 zoneId, WeatherData const* weatherChances);
        ~Weather() { };

        bool Update(uint32 diff);
        bool ReGenerate();
        bool UpdateWeather();

        void SendWeatherUpdateToPlayer(Player* player);
        static void SendFineWeatherUpdateToPlayer(Player* player);
        void SetWeather(WeatherType type, float intensity);

        /// For which zone is this weather?
        uint32 GetZone() const { return m_zone; };
        uint32 GetScriptId() const { return m_weatherChances->ScriptId; }

    private:

        WeatherState GetWeatherState() const;
        uint32 m_zone;
        WeatherType m_type;
        float m_intensity;
        IntervalTimer m_timer;
        WeatherData const* m_weatherChances;
};
#endif
