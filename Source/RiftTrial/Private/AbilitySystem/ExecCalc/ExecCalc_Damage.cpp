// Copyright Yerik Guo

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "RiftTrialAbilityTypes.h"
#include "RiftTrialGameplayTags.h"
#include "AbilitySystem/RiftTrialAttributeSet.h"

struct FDamageCaptureDefs
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationFlat);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationPercent);
    DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetrationFlat);
    DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetrationPercent);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(DamageReduction);

    FDamageCaptureDefs()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, Armor, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, MagicResistance, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, ArmorPenetrationFlat, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, ArmorPenetrationPercent, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, MagicPenetrationFlat, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, MagicPenetrationPercent, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, CritChance, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(URiftTrialAttributeSet, DamageReduction, Target, false);
    }
};

struct FPenetrationDefs
{
    FGameplayEffectAttributeCaptureDefinition FlatPenDef;
    FGameplayEffectAttributeCaptureDefinition PercentPenDef;
};

struct FResistanceCaptureDef
{
    FGameplayEffectAttributeCaptureDefinition ResistanceDef;
    FPenetrationDefs PenetrationDefs;
};

static const FDamageCaptureDefs& CaptureDefs()
{
    static FDamageCaptureDefs Defs;
    return Defs;
}

static void BuildResistanceMap(TMap<FGameplayTag, FResistanceCaptureDef>& OutMap)
{
    if (!OutMap.IsEmpty()) return;

    const FRiftTrialGameplayTags& Tags = FRiftTrialGameplayTags::Get();
    const FDamageCaptureDefs& Defs = CaptureDefs();

    // Physical → Armor
    {
        FResistanceCaptureDef RDef;
        RDef.ResistanceDef = Defs.ArmorDef;
        RDef.PenetrationDefs.FlatPenDef = Defs.ArmorPenetrationFlatDef;
        RDef.PenetrationDefs.PercentPenDef = Defs.ArmorPenetrationPercentDef;
        OutMap.Add(Tags.Damage_Physical, RDef);
    }

    // Magical → MagicResistance
    {
        FResistanceCaptureDef RDef;
        RDef.ResistanceDef = Defs.MagicResistanceDef;
        RDef.PenetrationDefs.FlatPenDef = Defs.MagicPenetrationFlatDef;
        RDef.PenetrationDefs.PercentPenDef = Defs.MagicPenetrationPercentDef;
        OutMap.Add(Tags.Damage_Magical, RDef);
    }
}

UExecCalc_Damage::UExecCalc_Damage()
{
    const FDamageCaptureDefs& Defs = CaptureDefs();
    RelevantAttributesToCapture.Add(Defs.ArmorDef);
    RelevantAttributesToCapture.Add(Defs.MagicResistanceDef);
    RelevantAttributesToCapture.Add(Defs.ArmorPenetrationFlatDef);
    RelevantAttributesToCapture.Add(Defs.ArmorPenetrationPercentDef);
    RelevantAttributesToCapture.Add(Defs.MagicPenetrationFlatDef);
    RelevantAttributesToCapture.Add(Defs.MagicPenetrationPercentDef);
    RelevantAttributesToCapture.Add(Defs.CritChanceDef);
    RelevantAttributesToCapture.Add(Defs.DamageReductionDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    static TMap<FGameplayTag, FResistanceCaptureDef> ResistanceMap;
    BuildResistanceMap(ResistanceMap);

    float Damage = 0.f;

    // ===== 真实伤害：跳过所有抵抗和减伤 =====
    const float TrueDamageValue = Spec.GetSetByCallerMagnitude(
        FRiftTrialGameplayTags::Get().Damage_True, false, 0.f);
    if (TrueDamageValue > 0.f)
    {
        Damage += TrueDamageValue;
    }

    // Per damage-type (Physical / Magical): apply resistance and penetration
    for (const auto& [DamageTypeTag, ResDef] : ResistanceMap)
    {
        const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);
        if (DamageTypeValue <= 0.f) continue;

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResDef.ResistanceDef, EvaluateParameters, Resistance);
        Resistance = FMath::Max(Resistance, 0.f);

        float FlatPen = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResDef.PenetrationDefs.FlatPenDef, EvaluateParameters, FlatPen);
        FlatPen = FMath::Max(FlatPen, 0.f);

        float PercentPen = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResDef.PenetrationDefs.PercentPenDef, EvaluateParameters, PercentPen);
        PercentPen = FMath::Clamp(PercentPen, 0.f, 100.f);

        // Effective resistance after penetration
        const float EffectiveResistance = Resistance * (1.f - PercentPen / 100.f) - FlatPen;

        // Damage multiplier: 100 / (100 + Resistance)
        float Multiplier;
        if (EffectiveResistance >= 0.f)
        {
            Multiplier = 100.f / (100.f + EffectiveResistance);
        }
        else
        {
            Multiplier = 2.f - 100.f / (100.f - EffectiveResistance);
        }

        Damage += DamageTypeValue * Multiplier;
    }

    // ===== 目标减伤系数（仅对非真伤生效） =====
    {
        float TargetDamageReduction = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
            CaptureDefs().DamageReductionDef, EvaluateParameters, TargetDamageReduction);
        TargetDamageReduction = FMath::Clamp(TargetDamageReduction, 0.f, 1.f);
        Damage *= (1.f - TargetDamageReduction);
    }

    // Critical hit
    float SourceCritChance = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDefs().CritChanceDef, EvaluateParameters, SourceCritChance);
    SourceCritChance = FMath::Max(SourceCritChance, 0.f);

    const bool bCriticalHit = FMath::RandRange(0, 99) < SourceCritChance;
    Damage = bCriticalHit ? Damage * 1.5f : Damage;

    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    if (FGameplayEffectContext* Context = EffectContextHandle.Get())
    {
        if (Context->GetScriptStruct() == FRiftTrialGameplayEffectContext::StaticStruct())
        {
            FRiftTrialGameplayEffectContext* RiftContext = static_cast<FRiftTrialGameplayEffectContext*>(Context);
            RiftContext->SetIsBlockedHit(false);
            RiftContext->SetIsCriticalHit(bCriticalHit);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ExecCalc_Damage: Context type mismatch, expected %s got %s"),
                *FRiftTrialGameplayEffectContext::StaticStruct()->GetName(),
                *Context->GetScriptStruct()->GetName());
        }
    }



    Damage = FMath::Max(Damage, 0.f);

    const FGameplayModifierEvaluatedData EvaluationData(
        URiftTrialAttributeSet::GetIncomingDamageAttribute(),
        EGameplayModOp::Additive,
        Damage);
    OutExecutionOutput.AddOutputModifier(EvaluationData);
}
