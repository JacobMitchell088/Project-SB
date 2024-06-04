// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SB_AICharacter.h"

ASB_AICharacter::ASB_AICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = APlayerController::StaticClass();
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AI_AttributeSetBase = CreateDefaultSubobject<UAI_AttributeSetBase>(TEXT("AI_AttributeSetBase"));

	//AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//AI_AttributeSetBase = CreateDefaultSubobject<UAI_AttributeSetBase>(TEXT("AI_AttributeSetBase"));

	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ASB_AICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASB_AICharacter::ApplyDamageAI(float DamageAmount)
{

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
