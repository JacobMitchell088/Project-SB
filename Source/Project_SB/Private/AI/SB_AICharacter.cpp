// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SB_AICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

ASB_AICharacter::ASB_AICharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AIControllerClass = APlayerController::StaticClass();
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); 

	AI_AttributeSetBase = CreateDefaultSubobject<UAI_AttributeSetBase>(TEXT("AI_AttributeSetBase"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
	bIsStunned = false;


	if (AbilitySystemComponent) {
		AbilitySystemComponent->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ASB_AICharacter::OnGameplayEffectApplied); // Bind event listener for tags
		AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &ASB_AICharacter::OnGameplayEffectRemoved);
	}
}

void ASB_AICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent) {
		UE_LOG(LogTemp, Warning, TEXT("%s() ASC Was VALID at begin play. Went on and bound delegate handles."), *FString(__FUNCTION__), *GetName());
		AIHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AI_AttributeSetBase->GetAI_HealthAttribute()).AddUObject(this, &ASB_AICharacter::AIHealthChanged);
		AIMaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AI_AttributeSetBase->GetAI_HealthAttribute()).AddUObject(this, &ASB_AICharacter::AIMaxHealthChanged);
		

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASB_AICharacter::AIStunTagChanged);
		//InitializeAttributes();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s() ASC Was INVALID at beginplay. Delegatehandles are now invalid."), *FString(__FUNCTION__), *GetName());
	}
}

void ASB_AICharacter::ApplyDamageAI(float DamageAmount)
{

}

//UBehaviorTree* ASB_AICharacter::GetBehaviorTree() const
//{
//	return Tree;
//}

void ASB_AICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);


	// Moved to AI controller possesed
	// ASB_AICharacter* AIChar = Cast<ASB_AICharacter>(NewController->GetPawn());
	//if (AIChar) {
	//	InitializeAttributes();
	//}
	//else {
	//	UE_LOG(LogTemp, Warning, TEXT("SB_AICharacter 'PossesedBy' Function failed to initialize attributes - Kimic"));
	//}
	// InitializeAttributes();
}


void ASB_AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASB_AICharacter::Die()
{
	Super::Die();
}

UAbilitySystemComponent* ASB_AICharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASB_AICharacter::InitializeAttributes() // Morphed into initialize everything for our AI
{
	UE_LOG(LogTemp, Warning, TEXT("Attemping to Initialize Attributes For AI"));



	ASBCharacterBase::AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(GetAbilitySystemComponent()); // Prob don't need to cast if it's going to be same type
	// AttributeSetBase = Cast<UAttributeSet>(GetAttributeSetBase()); Our attribute of this character is not inherited or of type UCharacterAttributeSet, which is what this wants. Luckily we already have our own getters/setters
	if (!ASBCharacterBase::AbilitySystemComponent.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("%s() Base Character weak pointer ASC is INVALID."), *FString(__FUNCTION__), *GetName());
	}
	if (!AttributeSetBase.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("%s() Base Character weak pointer ATSB is INVALID."), *FString(__FUNCTION__), *GetName());
	}




	ASB_AICharacter::AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	ASBCharacterBase::AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

	

	ASB_AICharacter::AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Super::InitializeAttributes();
	AI_AttributeSetBase->SetAI_Health(GetAIMaxHealth()); 

	AddStartupEffects();
	AddCharacterAbilities();
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

void ASB_AICharacter::OnGameplayEffectApplied(UAbilitySystemComponent* SourceASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle ActiveHandle) // Event listener for when we receive tags such as stun
{
	if (Spec.Def->InheritableGameplayEffectTags.CombinedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")))) {
		bIsStunned = true;
		MovementUpdate();
	}
}

void ASB_AICharacter::OnGameplayEffectRemoved(const FActiveGameplayEffect& EffectRemoved)
{
	const FGameplayEffectSpec& Spec = EffectRemoved.Spec;

	if (Spec.Def->InheritableGameplayEffectTags.CombinedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")))) {
		bIsStunned = false;
		MovementUpdate();
	}
}

void ASB_AICharacter::MovementUpdate() // Called when we are either stunned or unstunned
{
	AAIController* AIController = Cast<AAIController>(GetController());

	if (bIsStunned) { // When getting stunned
		if (AIController) {
			AIController->StopMovement();

			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp) {
				BlackboardComp->SetValueAsBool("IsStunned", true);
			}
		}
	}
	else { // When getting unstunned
		if (AIController) {
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp) {
				BlackboardComp->SetValueAsBool("IsStunned", false);
			}
		}
	}
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
