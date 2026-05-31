# CLAUDE.md

UE 5.3 MOBA demo (RiftTrial), framework migrated from Aura GAS tutorial. Single runtime module `RiftTrial`, API macro `RIFTTRIAL_API`.

## Build

```bash
"<UE5_ROOT>/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" -projectfiles -project="<project_dir>/RiftTrial.uproject" -game -engine
"<UE5_ROOT>/Engine/Build/BatchFiles/Build.bat" RiftTrialEditor Win64 Development -Project="<project_dir>/RiftTrial.uproject"
```

Or Rider: open `RiftTrial.sln` (git-ignored, auto-gen), build `RiftTrialEditor`.

## Architecture

### Character Hierarchy
```
ACharacter → ARiftTrialCharacterBase (Abstract, GAS+Combat)
              ├─ ARiftTrialCharacter (player: camera, WASD move)
              │     → BP_Garen
              └─ ARiftTrialMinion (AI: capsule Visibility block, highlight)
                    → BP_Blue/Red Melee/Ranged Minions
AActor → ARiftTrialTower (GAS+Combat+SphereOverlap targeting)
           → BP_Tower_Blue / BP_Tower_Red
AActor → ARiftTrialMinionSpawner (Spline + recursive timer spawn)
           → BP_MinionSpawner_Blue / BP_MinionSpawner_Red
```

### GAS Ability Hierarchy
```
UGameplayAbility → URiftTrialGameplayAbility (StartupInputTag)
                 → URiftTrialDamageGameplayAbility (DamageEffectClass, DamageTypes, AttackMontages array, GetAttackTarget, GetNextAttackMontage)
                     ├─ UMeleeAttackAbility (ApplyDamageToTarget)
                     ├─ URangedAttackAbility (SpawnProjectile, CanActivateAbility)
                     └─ UTowerAttackAbility (GetAttackTarget → Tower::SelectTarget)
```

**Cooldown**: Uses standard GAS Cooldown GE + MMC_AttackCooldown (reads AttackSpeed, returns 1.0/AttackSpeed). GA BPs call CommitAbility. No manual tag management.

### Attribute System (MOBA-style, 18+2 attrs)

| Category | Attributes |
|----------|-----------|
| Vital | Health, MaxHealth, Mana, MaxMana |
| Offense | PhysicalAttack, MagicalAttack, AttackSpeed, CooldownReduction, CritChance |
| Defense | Armor, MagicResistance |
| Penetration | ArmorPenFlat/Percent, MagicPenFlat/Percent |
| Base | MoveSpeed |
| Utility | AttackRange, HealthRegen, ManaRegen, Tenacity |
| Meta | IncomingDamage, CritResult (transient) |

Defaults: `InitMoveSpeed(600.f)` in AttributeSet constructor. Attributes initialized via GE (DefaultAttributes on each BP).

### Damage Pipeline
1. GA builds Spec with SetByCaller magnitudes (Damage.Physical / Damage.Magical)
2. `ExecCalc_Damage`: sums SetByCaller → applies resistance + dual penetration → crit roll (1.5x)
3. Writes `bIsCriticalHit` to EffectContext (guarded by GetScriptStruct() type check — requires `FRiftTrialGameplayEffectContext`)
4. `PostGameplayEffectExecute`: final damage → Health, Death if Health <= 0, floating text

**Fatal crash lesson**: ExecCalc MUST NOT blind `static_cast` EffectContext. Use `GetScriptStruct() == FRiftTrialGameplayEffectContext::StaticStruct()` guard. `URiftTrialAbilitySystemGlobals` requires `DefaultGame.ini` config to work correctly on PIE startup.

### GameplayTags (Singleton, always via `Get()`)
- `Attributes.Vital.*`, `Offense.*`, `Defense.*`, `Penetration.*`, `Base.*`, `Utility.*`
- `InputTag.LMB/RMB/1-6/Q/W/E/R/A/S/D/F/Up/Down/Left/Right`
- `Damage.Physical`, `Damage.Magical` (DamageTypeToResistances map)
- `State.Dead`, `State.Aggroed`, `Type.Minion`, `Type.Hero`, `Type.Tower`
- `Ability.Attack` (all attack GAs), `Cooldown.Attack` (Cooldown GE grants this)
- `Event.Montage.SpawnProjectile` (ranged/tower AnimNotify)
- `Event.Montage.AttackHit` (melee AnimNotify)
- `Effects.HitReact` (deprecated, no longer used in code)

### Input System
- Enhanced Input: `URiftTrialInputConfig` (DataAsset) maps `UInputAction` → `FGameplayTag`
- `BindAbilityActions` (static) on `URiftTrialInputComponent`
- `ARiftTrialPlayerController`: cursor trace (IEnemyInterface highlight), RMB click-to-move (nav path + spline auto-run), WASD/arrow movement via MoveAction
- `IA_Move` (Axis2D) with arrow keys mapped in `IMC_Default`

### AI / Behavior Tree
- `ARiftTrialAIController`: OnPossess → RunBehaviorTree
- `BB_Minion`: keys: TargetToFollow(Object), DistanceToTarget(Float), SplinePathActor(Object), SplineDistance(Float), AttackRange(Float), AggroRange(Float), ComboIndex(Int)
- `BT_Minion`: Selector → AttackTarget (Blocking: TargetToFollow Is Set) / FollowSplinePath (fallback)
- `FindNearestEnemy` service: TeamID-based, locks current target until dead or out of AggroRange, skips State.Dead
- `FollowSplinePath`: MoveToLocation along spline, MoveSpeed from GAS, smooth rejoin on return
- `AttackTarget`: MoveToActor(20.f accept radius), activates GA on cooldown

### Projectile
- `ARangedProjectile`: Sphere + ProjectileMovement, tick-driven homing, distance-based hit
- Niagara: FlightEffect (BeginPlay attach) + ImpactEffect (Multicast on hit)
- No mesh — sphere collision + Niagara visuals

### Death & Respawn
- `Die()`: State.Dead tag → StopTree(EBTStopMode::Safe) → StopMovement → DisableMovement → NoCollision → MulticastPlayDeathMontage → Hero: PlayerState respawn / Minion: SetLifeSpan
- `MulticastPlayDeathMontage`: Montage_Play + bEnableAutoBlendOut=false + Montage_Pause on timer
- `ARiftTrialPlayerState::StartRespawnTimer`: 10s → RespawnPawn (restore health, collision, anim, teleport to PlayerStart)

### Team/Allegiance
- `int32 TeamID` on `ARiftTrialCharacterBase` (0=neutral, 1=blue, 2=red)
- Identity tags via GE (Duration=Infinite): GE_Identity_Minion/Hero/Tower apply Type.* tags
- Highlight: CUSTOM_DEPTH_RED(250) for enemies, CUSTOM_DEPTH_BLUE(251) for allies

### Tower
- `ARiftTrialTower`: USphereComponent overlap targeting, SelectTarget priority: Aggroed > Minion > Hero, skips dead/same-team/towers
- `GA_TowerAttack`: BP event graph, Timer every 0.3s → TryActivateAbilitiesByTag(Ability.Attack.Tower) → CommitAbility → SelectTarget → SpawnProjectile → EndAbility

- **缩进格式**: `.h` 和 `.cpp` 统一使用 **4 空格** 缩进，禁止 Tab。

### Known Constraints / Patterns

- **MoveToActor vs Dist**: MoveToActor uses edge distance, FVector::Dist uses center distance (~70 unit gap). Use small fixed AcceptRadius (20.f) for MoveToActor, rely on Dist check for attack trigger.
- **PreAttributeChange clamp order**: MaxHealth modifier must come before Health modifier in GE, or Health gets clamped to 0.
- **ExecCalc EffectContext**: Always type-check via GetScriptStruct() before static_cast. Crash occurred when context was base type.
- **Edit/Write approval**: User prefers to approve all edits. Previously configured ask permission for Edit/Write tools.
- **No proactive commits**: Only commit when user explicitly asks.
- **UPROPERTY Categories**: Use `"RiftTrial|Xxx"` format for ALL UPROPERTY categories. Examples: `"RiftTrial|Ability"`, `"RiftTrial|Cooldown"`, `"RiftTrial|Buff"`, `"RiftTrial|E"`, `"RiftTrial|Q"`, `"RiftTrial|R"`. Never use `"Garen|X"` or other un-prefixed forms.
- **ASC 职责边界**: ASC 只负责按键分发（`AbilitySpecInputPressed` → `TryActivateAbility`），不得包含任何技能业务逻辑（如"再次按键取消"、"按键切换形态"等）。业务逻辑通过重写 `UGameplayAbility::InputPressed()` 在 Ability 子类中实现。原因是 `AbilitySpec.Ability` 指向 CDO 而非实例，在 ASC 中操作会导致状态污染。
- **注释保留**: 添加新代码时，不得删除或覆盖文件中已有的、与本次修改无关的注释。只修改与改动相关的行。
- **CDO 安全**: `UGameplayAbility` 的 CDO 是全局共享模板，禁止在 ASC 中通过 `Cast<XXXAbility>(AbilitySpec.Ability)` 读取/写入实例数据（如 `ActivateTime`）。实例状态应通过 `InstancedPerActor` 的 Ability 实例管理。

### Key Config Files
- `Config/DefaultEngine.ini`: AssetManagerClassName, Engine.GameMapsSettings
- `Config/DefaultGame.ini`: AbilitySystemGlobalsClassName, ProjectID, GameMaps
- `Config/DefaultInput.ini`: standard input

### Pending / Next
- Tower aggro system (State.Aggroed from ally-hero protection)
- Tower variable attack range (currently fixed DetectionSphere)
- Tower attack count heat-up (damage stacks per consecutive hit)
- Minimap
- Hero abilities beyond basic attack (Q/W/E/R skills)
- UI overlay (health bars, gold, score)
