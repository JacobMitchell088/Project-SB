// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SB_AICharacter.h"

ASB_AICharacter::ASB_AICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = APlayerController::StaticClass();
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AI_AttributeSetBase = CreateDefaultSubobject<UAI_AttributeSetBase>(TEXT("AI_AttributeSetBase"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;

	//InitializeAttributes();
	//AI_AttributeSetBase->SetAI_Health(GetAIMaxHealth());

}

void ASB_AICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent) {
		AIHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AI_AttributeSetBase->GetAI_HealthAttribute()).AddUObject(this, &ASB_AICharacter::AIHealthChanged);
		AIMaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AI_AttributeSetBase->GetAI_HealthAttribute()).AddUObject(this, &ASB_AICharacter::AIMaxHealthChanged);
		

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASB_AICharacter::AIStunTagChanged);
	}
}

void ASB_AICharacter::ApplyDamageAI(float DamageAmount)
{

}

UBehaviorTree* ASB_AICharacter::GetBehaviorTree() const
{
	return Tree;
}


void ASB_AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASB_AICharacter::Die()
{
    Destroy();
}

UAbilitySystemComponent* ASB_AICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAI_AttributeSetBase* ASB_AICharacter::GetAttributeSetBase() const
{
	return AI_AttributeSetBase;
}

bool ASB_AICharacter::IsAliveAI() const
{
	return GetAIHealth() > 0.0f;
}

float ASB_AICharacter::GetAIHealth() const
{
	return AI_AttributeSetBase->GetAI_Health();
}

float ASB_AICharacter::GetAIMaxHealth() const
{
	return AI_AttributeSetBase->GetAI_MaxHealth();
}

void ASB_AICharacter::AIHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("AI Health Changed"));
}

void ASB_AICharacter::AIMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("AI Max Health Changed"));
}

void ASB_AICharacter::AIStunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) {
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);

	}
}
