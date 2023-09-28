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

#ifndef TRINITY_SPELLDEFINES_H
#define TRINITY_SPELLDEFINES_H

#include "Define.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Position.h"
#include <vector>

class AuraEffect;
class Corpse;
class GameObject;
class Item;
class Player;
class Unit;
class WorldObject;

namespace WorldPackets
{
    namespace Spells
    {
        struct SpellTargetData;
    }
}

enum SpellInterruptFlags : uint32
{
    SPELL_INTERRUPT_FLAG_MOVEMENT     = 0x01, // 移动中断标志, 表示法术可以中断目标的移动  // why need this for instant?
    SPELL_INTERRUPT_FLAG_PUSH_BACK    = 0x02, // 推回中断标志, 表示法术可以将目标推回     // push back
    SPELL_INTERRUPT_FLAG_UNK3         = 0x04, // 未知标志3, 用于表示某种未知的中断信息    // any info?
    SPELL_INTERRUPT_FLAG_INTERRUPT    = 0x08, // 中断标志, 表示法术可以中断目标的施法     // interrupt
    SPELL_INTERRUPT_FLAG_ABORT_ON_DMG = 0x10  // 完全中断标志, 表示法术可以在受到直接伤害时完全中断  //_complete_ interrupt on direct damage
  //SPELL_INTERRUPT_UNK               = 0x20  // 未知标志, 564 个中的 727 个法术具有此标志, 这些法术通常以 “Glyph” 开头     // unk, 564 of 727 spells having this spell start with "Glyph"
};

// See SpellAuraInterruptFlags for other values definitions
enum SpellChannelInterruptFlags : uint32
{
    CHANNEL_INTERRUPT_FLAG_INTERRUPT = 0x0008,  // interrupt
    CHANNEL_FLAG_DELAY               = 0x4000
};

enum SpellAuraInterruptFlags : uint32
{
    AURA_INTERRUPT_FLAG_HITBYSPELL                  = 0x00000001,   // 0    被负面法术击中时移除?
    AURA_INTERRUPT_FLAG_TAKE_DAMAGE                 = 0x00000002,   // 1    任何伤害会移除
    AURA_INTERRUPT_FLAG_CAST                        = 0x00000004,   // 2    施放任何法术时移除
    AURA_INTERRUPT_FLAG_MOVE                        = 0x00000008,   // 3    任何移动会移除
    AURA_INTERRUPT_FLAG_TURNING                     = 0x00000010,   // 4    任何转向会移除
    AURA_INTERRUPT_FLAG_JUMP                        = 0x00000020,   // 5    进入战斗时移除
    AURA_INTERRUPT_FLAG_NOT_MOUNTED                 = 0x00000040,   // 6    下马时移除
    AURA_INTERRUPT_FLAG_NOT_ABOVEWATER              = 0x00000080,   // 7    进入水中时移除
    AURA_INTERRUPT_FLAG_NOT_UNDERWATER              = 0x00000100,   // 8    离开水中时移除
    AURA_INTERRUPT_FLAG_NOT_SHEATHED                = 0x00000200,   // 9    解鞘武器时移除
    AURA_INTERRUPT_FLAG_TALK                        = 0x00000400,   // 10   与 NPC 交互时移除?
    AURA_INTERRUPT_FLAG_LOOTING                     = 0x00000800,   // 11   对游戏对象进行采集/使用/开启操作或拾取时移除
    AURA_INTERRUPT_FLAG_MELEE_ATTACK                = 0x00001000,   // 12   攻击时移除
    AURA_INTERRUPT_FLAG_SPELL_ATTACK                = 0x00002000,   // 13   ???
    AURA_INTERRUPT_FLAG_UNK14                       = 0x00004000,   // 14
    AURA_INTERRUPT_FLAG_TRANSFORM                   = 0x00008000,   // 15   变身时移除?
    AURA_INTERRUPT_FLAG_UNK16                       = 0x00010000,   // 16
    AURA_INTERRUPT_FLAG_MOUNT                       = 0x00020000,   // 17   误导、宠物技能、游泳速度等移除
    AURA_INTERRUPT_FLAG_NOT_SEATED                  = 0x00040000,   // 18   站立起来时移除 (主要用于食物、饮料以及休息/假死类效果)
    AURA_INTERRUPT_FLAG_CHANGE_MAP                  = 0x00080000,   // 19   离开地图/被传送时移除
    AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION    = 0x00100000,   // 20   被使你无敌的光环移除, 或使其他人失去对你的选择
    AURA_INTERRUPT_FLAG_UNK21                       = 0x00200000,   // 21
    AURA_INTERRUPT_FLAG_TELEPORTED                  = 0x00400000,   // 22   传送时移除
    AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT            = 0x00800000,   // 23   进入 PvP 战斗时移除
    AURA_INTERRUPT_FLAG_DIRECT_DAMAGE               = 0x01000000,   // 24   任何直接伤害会移除
    AURA_INTERRUPT_FLAG_LANDING                     = 0x02000000,   // 25   着陆时移除
    AURA_INTERRUPT_FLAG_LEAVE_COMBAT                = 0x80000000,   // 31   离开战斗时移除

    AURA_INTERRUPT_FLAG_NOT_VICTIM = (AURA_INTERRUPT_FLAG_HITBYSPELL | AURA_INTERRUPT_FLAG_TAKE_DAMAGE | AURA_INTERRUPT_FLAG_DIRECT_DAMAGE)
};

enum SpellModOp : uint8
{
    SPELLMOD_DAMAGE                     =  0,
    SPELLMOD_DURATION                   =  1,
    SPELLMOD_THREAT                     =  2,
    SPELLMOD_EFFECT1                    =  3,
    SPELLMOD_CHARGES                    =  4,
    SPELLMOD_RANGE                      =  5,
    SPELLMOD_RADIUS                     =  6,
    SPELLMOD_CRITICAL_CHANCE            =  7,
    SPELLMOD_ALL_EFFECTS                =  8,
    SPELLMOD_NOT_LOSE_CASTING_TIME      =  9,
    SPELLMOD_CASTING_TIME               = 10,
    SPELLMOD_COOLDOWN                   = 11,
    SPELLMOD_EFFECT2                    = 12,
    SPELLMOD_IGNORE_ARMOR               = 13,
    SPELLMOD_COST                       = 14,
    SPELLMOD_CRIT_DAMAGE_BONUS          = 15,
    SPELLMOD_RESIST_MISS_CHANCE         = 16,
    SPELLMOD_JUMP_TARGETS               = 17,
    SPELLMOD_CHANCE_OF_SUCCESS          = 18,
    SPELLMOD_ACTIVATION_TIME            = 19,
    SPELLMOD_DAMAGE_MULTIPLIER          = 20,
    SPELLMOD_GLOBAL_COOLDOWN            = 21,
    SPELLMOD_DOT                        = 22,
    SPELLMOD_EFFECT3                    = 23,
    SPELLMOD_BONUS_MULTIPLIER           = 24,
    // spellmod 25
    SPELLMOD_PROC_PER_MINUTE            = 26,
    SPELLMOD_VALUE_MULTIPLIER           = 27,
    SPELLMOD_RESIST_DISPEL_CHANCE       = 28,
    SPELLMOD_CRIT_DAMAGE_BONUS_2        = 29, //one not used spell
    SPELLMOD_SPELL_COST_REFUND_ON_FAIL  = 30,

    MAX_SPELLMOD
};

enum SpellValueMod : uint8
{
    SPELLVALUE_BASE_POINT0,
    SPELLVALUE_BASE_POINT1,
    SPELLVALUE_BASE_POINT2,
    SPELLVALUE_RADIUS_MOD,
    SPELLVALUE_MAX_TARGETS,
    SPELLVALUE_AURA_STACK,
    SPELLVALUE_CRIT_CHANCE
};

enum SpellFacingFlags
{
    SPELL_FACING_FLAG_INFRONT = 0x0001
};

enum TriggerCastFlags : uint32
{
    TRIGGERED_NONE                                  = 0x00000000,   //! 未触发
    TRIGGERED_IGNORE_GCD                            = 0x00000001,   //! 忽略全局冷却时间 (GCD)
    TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD          = 0x00000002,   //! 忽略法术 (Spell) 和类别 (Category) 冷却时间
    TRIGGERED_IGNORE_POWER_AND_REAGENT_COST         = 0x00000004,   //! 忽略能量和材料消耗
    TRIGGERED_IGNORE_CAST_ITEM                      = 0x00000008,   //! 不会消耗施法物品或更新相关的成就条件
    TRIGGERED_IGNORE_AURA_SCALING                   = 0x00000010,   //! 忽略光环的缩放
    TRIGGERED_IGNORE_CAST_IN_PROGRESS               = 0x00000020,   //! 不会检查当前是否正在施放法术
    TRIGGERED_IGNORE_COMBO_POINTS                   = 0x00000040,   //! 忽略连击点要求
    TRIGGERED_CAST_DIRECTLY                         = 0x00000080,   //! 在 Spell::prepare 中, 将直接施放法术, 不会设置已执行法术的容器
    TRIGGERED_IGNORE_AURA_INTERRUPT_FLAGS           = 0x00000100,   //! 忽略可中断光环的中断标志
    TRIGGERED_IGNORE_SET_FACING                     = 0x00000200,   //! 不会调整面向目标 (如果有的话)
    TRIGGERED_IGNORE_SHAPESHIFT                     = 0x00000400,   //! 忽略变形检查
    TRIGGERED_IGNORE_CASTER_AURASTATE               = 0x00000800,   //! 忽略施法者光环状态, 包括战斗要求和死亡状态
    TRIGGERED_DISALLOW_PROC_EVENTS                  = 0x00001000,   //! 禁止触发法术的事件 (默认)
    TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE   = 0x00002000,   //! 忽略施法者处于骑乘或在载具上的限制
    // reuse                                        = 0x00004000,
    // reuse                                        = 0x00008000,
    TRIGGERED_IGNORE_CASTER_AURAS                   = 0x00010000,   //! 忽略施法者的光环限制或要求
    TRIGGERED_DONT_RESET_PERIODIC_TIMER             = 0x00020000,   //! 允许周期性光环定时器继续计时 (而不是重置)
    TRIGGERED_DONT_REPORT_CAST_ERROR                = 0x00040000,   //! 在 CheckCast 函数中返回 SPELL_FAILED_DONT_REPORT
    TRIGGERED_FULL_MASK                             = 0x0007FFFF,   //! 执行 CastSpell 时 triggered 参数设置为 true 时使用的

    // 调试标志 (用于 .cast triggered 命令)
    TRIGGERED_IGNORE_EQUIPPED_ITEM_REQUIREMENT      = 0x00080000,   //! 忽略装备物品要求
    TRIGGERED_FULL_DEBUG_MASK                       = 0xFFFFFFFF
};

enum SpellCastTargetFlags : uint32
{
    TARGET_FLAG_NONE            = 0x00000000,
    TARGET_FLAG_UNUSED_1        = 0x00000001,               // 未使用
    TARGET_FLAG_UNIT            = 0x00000002,               // 单位目标, 通常使用目标的 GUID (pguid)
    TARGET_FLAG_UNIT_RAID       = 0x00000004,               // 不发送, 用于验证目标是否为团队成员
    TARGET_FLAG_UNIT_PARTY      = 0x00000008,               // 不发送, 用于验证目标是否为队伍成员
    TARGET_FLAG_ITEM            = 0x00000010,               // 物品目标, 通常使用目标的 GUID (pguid)
    TARGET_FLAG_SOURCE_LOCATION = 0x00000020,               // 源位置目标, 通常使用目标的 GUID 和 3 个浮点数坐标
    TARGET_FLAG_DEST_LOCATION   = 0x00000040,               // 目标位置目标, 通常使用目标的 GUID 和 3 个浮点数坐标
    TARGET_FLAG_UNIT_ENEMY      = 0x00000080,               // 不发送, 用于验证目标是否为敌人
    TARGET_FLAG_UNIT_ALLY       = 0x00000100,               // 不发送, 用于验证目标是否为盟友
    TARGET_FLAG_CORPSE_ENEMY    = 0x00000200,               // 尸体目标, 通常使用目标的 GUID (pguid)
    TARGET_FLAG_UNIT_DEAD       = 0x00000400,               // 不发送, 用于验证目标是否为已死亡生物
    TARGET_FLAG_GAMEOBJECT      = 0x00000800,               // 游戏对象目标, 通常使用目标的 GUID (pguid), 与 TARGET_GAMEOBJECT_TARGET 一起使用
    TARGET_FLAG_TRADE_ITEM      = 0x00001000,               // 交易物品目标, 通常使用目标的 GUID (pguid)
    TARGET_FLAG_STRING          = 0x00002000,               // 字符串目标, 通常使用字符串
    TARGET_FLAG_GAMEOBJECT_ITEM = 0x00004000,               // 不发送, 与 TARGET_GAMEOBJECT_ITEM_TARGET 一起使用
    TARGET_FLAG_CORPSE_ALLY     = 0x00008000,               // 尸体盟友目标, 通常使用目标的 GUID (pguid)
    TARGET_FLAG_UNIT_MINIPET    = 0x00010000,               // 小宠物目标, 通常使用目标的 GUID (pguid), 用于验证目标是否为非战斗宠物
    TARGET_FLAG_GLYPH_SLOT      = 0x00020000,               // 用于雕文 (glyph) 法术
    TARGET_FLAG_DEST_TARGET     = 0x00040000,               // 有时与 DEST_TARGET 法术一起出现 (对于给定法术可能会出现或不出现)
    TARGET_FLAG_UNUSED20        = 0x00080000,               // uint32 计数器, 循环 { vec3 - 屏幕位置 (?), guid }, 目前尚未使用
    TARGET_FLAG_UNIT_PASSENGER  = 0x00100000,               // 猜测, 用于验证目标是否为载具乘客

    TARGET_FLAG_UNIT_MASK = TARGET_FLAG_UNIT | TARGET_FLAG_UNIT_RAID | TARGET_FLAG_UNIT_PARTY
        | TARGET_FLAG_UNIT_ENEMY | TARGET_FLAG_UNIT_ALLY | TARGET_FLAG_UNIT_DEAD | TARGET_FLAG_UNIT_MINIPET | TARGET_FLAG_UNIT_PASSENGER,
    TARGET_FLAG_GAMEOBJECT_MASK = TARGET_FLAG_GAMEOBJECT | TARGET_FLAG_GAMEOBJECT_ITEM,
    TARGET_FLAG_CORPSE_MASK = TARGET_FLAG_CORPSE_ALLY | TARGET_FLAG_CORPSE_ENEMY,
    TARGET_FLAG_ITEM_MASK = TARGET_FLAG_TRADE_ITEM | TARGET_FLAG_ITEM | TARGET_FLAG_GAMEOBJECT_ITEM
};

struct TC_GAME_API SpellDestination
{
    SpellDestination();
    SpellDestination(float x, float y, float z, float orientation = 0.0f, uint32 mapId = MAPID_INVALID);
    SpellDestination(Position const& pos);
    SpellDestination(WorldObject const& wObj);

    void Relocate(Position const& pos);
    void RelocateOffset(Position const& offset);

    WorldLocation _position;
    ObjectGuid _transportGUID;
    Position _transportOffset;
};

class TC_GAME_API SpellCastTargets
{
public:
    SpellCastTargets();
    ~SpellCastTargets();

    void Read(ByteBuffer& data, Unit* caster);
    void Write(WorldPackets::Spells::SpellTargetData& data);

    uint32 GetTargetMask() const { return m_targetMask; }
    void SetTargetMask(uint32 newMask) { m_targetMask = newMask; }

    void SetTargetFlag(SpellCastTargetFlags flag) { m_targetMask |= flag; }

    ObjectGuid GetUnitTargetGUID() const;
    Unit* GetUnitTarget() const;
    void SetUnitTarget(Unit* target);

    ObjectGuid GetGOTargetGUID() const;
    GameObject* GetGOTarget() const;
    void SetGOTarget(GameObject* target);

    ObjectGuid GetCorpseTargetGUID() const;
    Corpse* GetCorpseTarget() const;

    WorldObject* GetObjectTarget() const;
    ObjectGuid GetObjectTargetGUID() const;
    void RemoveObjectTarget();

    ObjectGuid GetItemTargetGUID() const { return m_itemTargetGUID; }
    Item* GetItemTarget() const { return m_itemTarget; }
    uint32 GetItemTargetEntry() const { return m_itemTargetEntry; }
    void SetItemTarget(Item* item);
    void SetTradeItemTarget(Player* caster);
    void UpdateTradeSlotItem();

    SpellDestination const* GetSrc() const;
    Position const* GetSrcPos() const;
    void SetSrc(float x, float y, float z);
    void SetSrc(Position const& pos);
    void SetSrc(WorldObject const& wObj);
    void ModSrc(Position const& pos);
    void RemoveSrc();

    SpellDestination const* GetDst() const;
    WorldLocation const* GetDstPos() const;
    void SetDst(float x, float y, float z, float orientation, uint32 mapId = MAPID_INVALID);
    void SetDst(Position const& pos);
    void SetDst(WorldObject const& wObj);
    void SetDst(SpellDestination const& spellDest);
    void SetDst(SpellCastTargets const& spellTargets);
    void ModDst(Position const& pos);
    void ModDst(SpellDestination const& spellDest);
    void RemoveDst();

    bool HasSrc() const;
    bool HasDst() const;
    bool HasTraj() const { return m_speed != 0; }

    float GetElevation() const { return m_elevation; }
    void SetElevation(float elevation) { m_elevation = elevation; }
    float GetSpeed() const { return m_speed; }
    void SetSpeed(float speed) { m_speed = speed; }

    float GetDist2d() const { return m_src._position.GetExactDist2d(&m_dst._position); }
    float GetSpeedXY() const { return m_speed * std::cos(m_elevation); }
    float GetSpeedZ() const { return m_speed * std::sin(m_elevation); }

    void Update(WorldObject* caster);

private:
    uint32 m_targetMask;

    // objects (can be used at spell creating and after Update at casting)
    WorldObject* m_objectTarget;
    Item* m_itemTarget;

    // object GUID/etc, can be used always
    ObjectGuid m_objectTargetGUID;
    ObjectGuid m_itemTargetGUID;
    uint32 m_itemTargetEntry;

    SpellDestination m_src;
    SpellDestination m_dst;

    float m_elevation, m_speed;
    std::string m_strTarget;
};

struct TC_GAME_API CastSpellTargetArg
{
    CastSpellTargetArg() { Targets.emplace(); }
    CastSpellTargetArg(std::nullptr_t) { Targets.emplace(); }
    CastSpellTargetArg(WorldObject* target);
    CastSpellTargetArg(Item* itemTarget)
    {
        Targets.emplace();
        Targets->SetItemTarget(itemTarget);
    }
    CastSpellTargetArg(Position const& dest)
    {
        Targets.emplace();
        Targets->SetDst(dest);
    }
    CastSpellTargetArg(SpellCastTargets&& targets)
    {
        Targets.emplace(std::move(targets));
    }

    Optional<SpellCastTargets> Targets; // empty optional used to signal error state
};

struct TC_GAME_API CastSpellExtraArgs
{
    CastSpellExtraArgs() {}
    CastSpellExtraArgs(bool triggered) : TriggerFlags(triggered ? TRIGGERED_FULL_MASK : TRIGGERED_NONE) {}
    CastSpellExtraArgs(TriggerCastFlags trigger) : TriggerFlags(trigger) {}
    CastSpellExtraArgs(Item* item) : TriggerFlags(TRIGGERED_FULL_MASK), CastItem(item) {}
    CastSpellExtraArgs(AuraEffect const* eff) : TriggerFlags(TRIGGERED_FULL_MASK), TriggeringAura(eff) {}
    CastSpellExtraArgs(ObjectGuid const& origCaster) : TriggerFlags(TRIGGERED_FULL_MASK), OriginalCaster(origCaster) {}
    CastSpellExtraArgs(AuraEffect const* eff, ObjectGuid const& origCaster) : TriggerFlags(TRIGGERED_FULL_MASK), TriggeringAura(eff), OriginalCaster(origCaster) {}
    CastSpellExtraArgs(SpellValueMod mod, int32 val) { SpellValueOverrides.AddMod(mod, val); }

    CastSpellExtraArgs& SetTriggerFlags(TriggerCastFlags flag) { TriggerFlags = flag; return *this; }
    CastSpellExtraArgs& SetCastItem(Item* item) { CastItem = item; return *this; }
    CastSpellExtraArgs& SetTriggeringAura(AuraEffect const* triggeringAura) { TriggeringAura = triggeringAura; return *this; }
    CastSpellExtraArgs& SetOriginalCaster(ObjectGuid const& guid) { OriginalCaster = guid; return *this; }
    CastSpellExtraArgs& AddSpellMod(SpellValueMod mod, int32 val) { SpellValueOverrides.AddMod(mod, val); return *this; }
    CastSpellExtraArgs& AddSpellBP0(int32 val) { return AddSpellMod(SPELLVALUE_BASE_POINT0, val); } // because i don't want to type SPELLVALUE_BASE_POINT0 300 times

    TriggerCastFlags TriggerFlags = TRIGGERED_NONE;
    Item* CastItem = nullptr;
    AuraEffect const* TriggeringAura = nullptr;
    ObjectGuid OriginalCaster = ObjectGuid::Empty;
    struct
    {
        friend struct CastSpellExtraArgs;
        friend class WorldObject;

        private:
            void AddMod(SpellValueMod mod, int32 val) { data.push_back({ mod, val }); }

            auto begin() const { return data.cbegin(); }
            auto end() const { return data.cend(); }

            std::vector<std::pair<SpellValueMod, int32>> data;
    } SpellValueOverrides;
};

#endif
