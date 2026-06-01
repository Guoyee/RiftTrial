<p align="center">
  <img src="https://img.shields.io/badge/UE-5.3-0C1622?logo=unrealengine" alt="UE 5.3">
  <img src="https://img.shields.io/badge/C%2B%2B-20-00599C?logo=c%2B%2B" alt="C++20">
  <img src="https://img.shields.io/badge/GAS-Framework-8B5CF6" alt="GAS">
  <img src="https://img.shields.io/badge/MOBA-Demo-F59E0B" alt="MOBA">
</p>

<h1 align="center">⚔️ RiftTrial</h1>
<h3 align="center">UE5 MOBA 战斗框架 · 基于 GAS 从零搭建</h3>

<p align="center">
  <b>~100 个 C++ 文件 · 80+ 个 Blueprint · 18+2 MOBA 属性 · Behavior Tree AI · 英雄 QWER 完整技能组</b>
</p>

---

## 📖 项目概述

RiftTrial 是一个基于 **Unreal Engine 5.3 + Gameplay Ability System（GAS）** 从零搭建的 MOBA 战斗框架 Demo，实现了 **角色 → 技能 → 伤害 → AI → UI** 完整闭环。GAS 是 Fortnite、Paragon 等 AAA 游戏使用的同款技能框架。

**当前完成度：**

- ✅ 自定义 **18+2 MOBA 属性集**（Vital / Offense / Defense / Penetration / Utility / Meta）
- ✅ **多层技能继承体系**——从通用冷却管理到英雄专属 QWER 技能
- ✅ **类 LoL 伤害管线**——物理 / 魔法 / 真实伤害，含抗性、双重穿透、暴击判定
- ✅ **Behavior Tree AI**——小兵沿兵线行军、索敌、战斗，含仇恨锁定
- ✅ **防御塔自动索敌**——SphereOverlap 检测 + `Aggroed > Minion > Hero` 优先级
- ✅ **盖伦完整技能组**——Q（沉默+加速）、W（减伤+韧性）、E（旋转 AoE）、R（斩杀大招）
- ✅ **WidgetController UI 架构**——GAS 属性绑定，解耦 UI 与数据层
- ✅ **Niagara 弹射物系统**——跟踪弹射物 + 飞行/命中特效分离

---

## 🏗 架构设计

### 角色层级

```
ACharacter
 └─ ARiftTrialCharacterBase（抽象基类 · GAS + CombatInterface + TeamID）
     ├─ ARiftTrialCharacter（玩家：相机、WASD+RMB 双输入模式）
     │   └─ BP_Garen
     └─ ARiftTrialMinion（AI 驱动：Behavior Tree、Spline 行军、仇恨系统）
         ├─ BP_BlueMeleeMinion   BP_RedMeleeMinion
         └─ BP_BlueRangedMinion  BP_RedRangedMinion

AActor
 └─ ARiftTrialTower（GAS + SphereOverlap 检测 + 优先级选目标）
     ├─ BP_Tower_Blue
     └─ BP_Tower_Red
```

### 技能继承体系

```
UGameplayAbility
 └─ URiftTrialGameplayAbility          ← StartupInputTag 输入绑定
     ├─ URiftTrialDamageGameplayAbility ← DamageEffectClass、AttackMontages、GetAttackTarget
     │   ├─ UMeleeAttackAbility         ← ApplyDamageToTarget（AnimNotify 驱动命中帧）
     │   ├─ URangedAttackAbility        ← SpawnProjectile（跟踪 + 距离命中判定）
     │   └─ UTowerAttackAbility         ← SelectTarget（Overlap 优先级选敌）
     └─ UHeroGameplayAbility            ← 冷却覆盖、自身 Buff 系统、鼠标指向目标
         ├─ UHeroMeleeAttackAbility     ← 连击蒙太奇轮播
         ├─ UGarenQAbility              ← 自身 Buff：移速加成 + 下一次普攻沉默
         ├─ UGarenWAbility              ← 自身 Buff：减伤 + 韧性
         ├─ UGarenEAbility              ← Timer 驱动的旋转 AoE（可中途取消）
         └─ UGarenRAbility              ← 指向性真实伤害斩杀
```

### 伤害管线

```
┌─────────────────────────────────────────────────────────────┐
│  1. GA 构建 FGameplayEffectSpec                              │
│     └─ SetByCaller: Damage.Physical / Damage.Magical        │
│                      / Damage.True                          │
├─────────────────────────────────────────────────────────────┤
│  2. ExecCalc_Damage 捕获属性：                               │
│     ┌─ Target: Armor, MagicResistance, DamageReduction      │
│     └─ Source: ArmorPenFlat/Percent, MagicPenFlat/Percent,  │
│                CritChance                                   │
│                                                             │
│     每种伤害类型分别计算：                                      │
│       有效抗性 = 抗性 × (1 - 百分比穿透/100) - 固定穿透          │
│       伤害系数 = 100 / (100 + 有效抗性)    [有效抗性 ≥ 0]       │
│       伤害 += 原始值 × 伤害系数                                │
│                                                             │
│     真实伤害：跳过所有抗性 → 直接累加                            │
│                                                             │
│     求和后：× (1 - 目标减伤系数)，暴击则 × 1.5                  │
├─────────────────────────────────────────────────────────────┤
│  3. 输出 → IncomingDamage（Meta 属性，不参与网络复制）          │
├─────────────────────────────────────────────────────────────┤
│  4. PostGameplayEffectExecute:                              │
│     Health -= IncomingDamage → 死亡判定 → 浮动伤害文字        │
└─────────────────────────────────────────────────────────────┘
```

### 属性系统

| 分类 | 属性 | 网络复制 |
|------|------|:--------:|
| **Vital** | Health, MaxHealth, Mana, MaxMana | ✅ |
| **Offense** | PhysicalAttack, MagicalAttack, AttackSpeed, CooldownReduction, CritChance | ✅ |
| **Defense** | Armor, MagicResistance, DamageReduction | ✅ |
| **Penetration** | ArmorPenFlat, ArmorPenPercent, MagicPenFlat, MagicPenPercent | ✅ |
| **Base** | MoveSpeed | ✅ |
| **Utility** | AttackRange, HealthRegen, ManaRegen, Tenacity | ✅ |
| **Meta** | IncomingDamage | ❌ 仅本地 |

> 属性默认值在 `URiftTrialAttributeSet` 构造函数中设定（如 `InitMoveSpeed(600.f)`），具体数值通过 Blueprint 中的 DefaultAttributes GE 覆写。

### AI 行为树

```
BT_Minion（Selector 根节点）
 ├─ AttackTarget（阻塞条件：TargetToFollow 已设置）
 │   ├─ MoveToActor（AcceptRadius: 20）
 │   └─ TryActivateAbility（按冷却自动释放）
 └─ FollowSplinePath（兜底行为）
     └─ MoveToLocation 沿 Spline 移动（速度从 GAS MoveSpeed 读取）

BTService:
 • FindNearestEnemy — TeamID 筛选，锁定目标直到死亡或超出 AggroRange，跳过 State.Dead
 • UpdateBlackboard — 距离、攻击范围、连击索引
```

### 输入系统

```
Enhanced Input
  ├─ UInputAction → URiftTrialInputConfig（DataAsset）→ FGameplayTag
  ├─ URiftTrialInputComponent::BindAbilityActions（静态方法）
  └─ ARiftTrialPlayerController:
       ├─ WASD / 方向键 → MoveAction（Axis2D）
       ├─ RMB 短按 → 攻击指令（CursorTrace + 高亮）
       ├─ RMB 长按 → Spline 自动寻路
       └─ 技能按键 → AbilityInputTagPressed/Held/Released → ASC::AbilitySpecInputPressed
```

---

## 🧠 技术要点

### 可组合 Buff 系统（`FSelfBuffDef`）

英雄技能不硬编码 Buff 逻辑，通过数据驱动的结构体实现通用 Buff 框架：

```cpp
USTRUCT(BlueprintType)
struct FSelfBuffDef
{
    TSubclassOf<UGameplayEffect> EffectClass;  // 要应用的 GE
    float Duration  = 0.f;   // >0 时通过 SetByCaller(Data.BuffDuration) 传入
    float Magnitude = 0.f;   // SetByCaller(Data.BuffMagnitude)
    FGameplayTag GrantedTag; // 可选：动态授予的 Tag
};
```

策划在 Blueprint 中通过 `SelfBuffs` 数组自由组合 Buff 效果——Q 的加速、W 的减伤，都走同一套逻辑，调参不需要修改 C++ 代码。

### 类型安全的 EffectContext

UE 的 `FGameplayEffectContext` 基类不携带自定义数据。ExecCalc 需要把暴击标记写回 Context：

```cpp
// ❌ 若 Context 是基类型则崩溃
auto* Ctx = static_cast<FMyContext*>(Context);

// ✅ 运行时类型检查
if (Context->GetScriptStruct() == FMyContext::StaticStruct())
{
    auto* Ctx = static_cast<FMyContext*>(Context);
    Ctx->SetIsCriticalHit(bCritical);
}
```

同时需要在 `DefaultGame.ini` 中配置 `AbilitySystemGlobalsClassName`，否则 PIE 启动时类型会回退为基类。

### ASC 职责边界

**ASC 只负责按键分发**（`AbilitySpecInputPressed` → `TryActivateAbility`），技能业务逻辑（再次按键取消、形态切换等）必须通过重写 `UGameplayAbility::InputPressed()` 在 Ability 子类中实现。原因是 `AbilitySpec.Ability` 指向 CDO 而非实例，在 ASC 中操作会导致全局状态污染。

### 双穿透伤害公式

```cpp
// 有效抗性 = 抗性 × (1 - 百分比穿透) - 固定穿透
const float EffectiveResistance = Resistance * (1.f - PercentPen / 100.f) - FlatPen;

// 伤害系数（LoL 公式）
if (EffectiveResistance >= 0.f)
    Multiplier = 100.f / (100.f + EffectiveResistance);
else
    Multiplier = 2.f - 100.f / (100.f - EffectiveResistance);  // 负抗性上限
```

---

## 📁 目录结构

```
Source/RiftTrial/
├── Public/
│   ├── AbilitySystem/
│   │   ├── Abilities/          # GA 继承体系（10 个类）
│   │   ├── AbilityTasks/       # TargetDataUnderMouse
│   │   ├── Data/               # CharacterClassInfo, AttributeInfo
│   │   ├── ExecCalc/           # ExecCalc_Damage
│   │   ├── ModMagClac/         # 6 个 MMC 类
│   │   ├── RiftTrialAbilitySystemComponent.h
│   │   ├── RiftTrialAbilitySystemGlobals.h
│   │   ├── RiftTrialAbilitySystemLibrary.h
│   │   └── RiftTrialAttributeSet.h
│   ├── AI/                     # AIController + 4 个 BT 节点
│   ├── Actor/                  # RangedProjectile, RiftTrialTower
│   ├── Character/              # CharacterBase, Character, Minion, AnimInstance
│   ├── Game/                   # GameMode, MinionSpawner（波次生成器）
│   ├── Input/                  # InputComponent, InputConfig（DataAsset）
│   ├── Interaction/            # CombatInterface, EnemyInterface
│   ├── Player/                 # PlayerController, PlayerState
│   ├── UI/
│   │   ├── HUD/                # RiftTrialHUD
│   │   ├── WidgetController/   # Overlay, AttributeMenu WidgetController
│   │   └── Widgets/            # HealthBar, DamageText, UserWidget 基类
│   ├── RiftTrialAbilityTypes.h # 自定义 EffectContext
│   └── RiftTrialGameplayTags.h # Singleton 标签管理器
└── Private/                    # 与 Public 一一对应

Content/BluePrints/
├── AI/               # BB_Minion, BT_Minion, AIController, Spawner(蓝/红)
├── AbilitySystem/
│   ├── DefaultAttributes/  # 英雄/小兵/塔 初始属性 GE + 身份标签 GE
│   ├── GameplayAbilities/  # 小兵近战/远程 GA，塔攻击 GA，盖伦 5 个 GA
│   ├── GE_Damage/          # 伤害 GE + CurveTable
│   ├── GE_Buff/            # 减伤/移速/沉默/通用冷却
│   ├── GameplayCue/        # 盖伦 E 旋转 / Q 强化 / W 护盾 Niagara 特效
│   └── Projectile/         # 小兵弹射物(蓝/红) + 塔弹射物
├── Character/              # BP_Garen, 4 种小兵, 小兵动画蓝图
├── Input/                  # 18 个 InputAction + IMC + InputConfig
├── UI/HealthBar/           # Hero/Minion/Tower 三种血条 Widget
└── AnimNotifies/           # AN_MontageEvent（命中/发射弹射物）
```

---

## 🚀 构建与运行

```bash
# 生成项目文件
<UE5_ROOT>/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe \
  -projectfiles -project="<project_dir>/RiftTrial.uproject" -game -engine

# 编译 Editor 目标
<UE5_ROOT>/Engine/Build/BatchFiles/Build.bat \
  RiftTrialEditor Win64 Development -Project="<project_dir>/RiftTrial.uproject"
```

或在 Rider / VS 中打开 `RiftTrial.sln`（git-ignored，自动生成），编译 `RiftTrialEditor`。

**环境要求：** Unreal Engine 5.3、Visual Studio 2022 / Rider、Windows 10+

---

## 🎮 操作说明

| 按键 | 功能 |
|------|------|
| `W A S D` | 直接移动 |
| `RMB` 短按 | 攻击鼠标指向目标 |
| `RMB` 长按 | Spline 自动寻路 |
| `LMB` | 普攻（需先有攻击目标） |
| `Q` | 致命打击（加速 + 沉默普攻） |
| `W` | 勇气（减伤 + 韧性） |
| `E` | 审判（旋转 AoE，再按取消） |
| `R` | 德玛西亚正义（指向性斩杀） |
| `1` – `6` | 物品栏（预留） |
| `A` / `F` | 召唤师技能（预留） |

---

## ⚙️ 关键配置

| 文件 | 用途 |
|------|------|
| `Config/DefaultGame.ini` | `AbilitySystemGlobalsClassName`、GameMaps、ProjectID |
| `Config/DefaultEngine.ini` | `AssetManagerClassName`、GameMapsSettings |
| `Config/DefaultInput.ini` | 标准输入绑定 |

---

## 🔭 后续计划

- [ ] 防御塔仇恨系统（友方英雄被攻击时转移仇恨）
- [ ] 防御塔连续攻击升温机制
- [ ] 小地图 Widget
- [ ] 更多英雄技能（QWER 框架已就绪）
- [ ] 多人联机（DS 网络同步、RPC、预测回滚）
- [ ] 金币 / 计分板 UI 叠层
