// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SBPlayerState.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"

ASBPlayerState::ASBPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 100.0f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

}

UAbilitySystemComponent* ASBPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSetBase* ASBPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool ASBPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void ASBPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	// TODO -- Implement hud later
}

float ASBPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float ASBPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float ASBPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float ASBPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

int32 ASBPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetLevel();
}

void ASBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent) {
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ASBPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ASBPlayerState::MaxHealthChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ASBPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ASBPlayerState::MaxManaChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &ASBPlayerState::CharacterLevelChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASBPlayerState::StunTagChanged);
	}
}

void ASBPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Health Changed"));
}

void ASBPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Health Changed"));
}

void ASBPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Mana Changed"));
}

void ASBPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Max Mana Changed"));
}

void ASBPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Character Level Changed"));
}

void ASBPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) {
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));
		
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);

	}
}
