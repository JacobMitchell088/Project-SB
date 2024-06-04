// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AttributeSets/AI_AttributeSetBase.h"
#include "Net/UnrealNetwork.h"

void UAI_AttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAI_AttributeSetBase, AI_Health, OldHealth);
}

void UAI_AttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAI_AttributeSetBase, AI_MaxHealth, OldMaxHealth);
}

void UAI_AttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAI_AttributeSetBase, AI_Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAI_AttributeSetBase, AI_MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAI_AttributeSetBase::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp) {
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f)
			? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			: NewMaxValue;
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UAI_AttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetAI_MaxHealthAttribute()) {
		AdjustAttributeForMaxChange(AI_Health, AI_MaxHealth, NewValue, GetAI_HealthAttribute());
	}
}

void UAI_AttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	//Super::PostGameplayEffectExecute(Data);

	//FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	//UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

	//const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	//float DeltaValue{ 0.f };

	//if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive) {
	//	DeltaValue = Data.EvaluatedData.Magnitude;
	//}

	//ASBCharacterBase* TargetCharacter{ nullptr };

	//if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid()) {
	//	AActor* TargetActor{ nullptr };
	//	TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	//	TargetCharacter = Cast<ASBCharacterBase>(TargetActor);
	//}


	//if (Data.EvaluatedData.Attribute == GetHealthAttribute()) {
	//	SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

	//	if (TargetCharacter) {
	//		TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
	//	}
	//}

}


