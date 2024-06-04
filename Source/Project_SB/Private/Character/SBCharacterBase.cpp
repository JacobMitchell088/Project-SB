// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SBCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/AttributeSets/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASBCharacterBase::ASBCharacterBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));
}

UAbilitySystemComponent* ASBCharacterBase::GetAbilitySystemComponent() const {

	return AbilitySystemComponent.Get();
}

bool ASBCharacterBase::IsAlive() const {

	return GetHealth() > 0.0f;
}

int32 ASBCharacterBase::GetAbilityLevel(SBAbilityID AbilityID) const {

	return 1; // Hard coding every ability to be 1 for now. Can add upgrades later
}

void ASBCharacterBase::RemoveCharacterAbilities() {
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->CharacterAbilitiesGiven) {
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities()) {
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass())) {
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++) {
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = false;
	bAbilitiesInitialized = false;
}

void ASBCharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized) {
		OnHealthChanged(DeltaValue, EventTags);
	}
}

float ASBCharacterBase::GetCharacterLevel() const {

	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetLevel();
	}
	return 0.0f;
}

float ASBCharacterBase::GetHealth() const {

	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetHealth();
	}
	return 0.0f;
}

float ASBCharacterBase::GetMana() const {

	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMana();
	}
	return 0.0f;
}

float ASBCharacterBase::GetMaxHealth() const {

	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMaxHealth();
	}
	return 0.0f;
}

float ASBCharacterBase::GetMaxMana() const {

	if (AttributeSetBase.IsValid()) {
		return AttributeSetBase->GetMaxMana();
	}
	return 0.0f;
}

void ASBCharacterBase::Die() {
	UE_LOG(LogTemp, Warning, TEXT("%s() DIE()"), *FString(__FUNCTION__), *GetName());

	RemoveCharacterAbilities();
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0); // Can re-enable/disable if you dont want corpses flying everywhere
	// EnableRagdoll(); // Remove later for death montage

	OnCharacterDied.Broadcast(this);

	if (AbilitySystemComponent.IsValid()) {
		AbilitySystemComponent->CancelAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsEremoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	if (DeathMontage) {
		PlayAnimMontage(DeathMontage);
	}
	else {
		FinishDying();
	}
}

void ASBCharacterBase::FinishDying() {
	Destroy();
}

// Called when the game starts or when spawned
void ASBCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASBCharacterBase::EnableRagdoll()
{
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
}

void ASBCharacterBase::AddCharacterAbilities() {
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven) { // Previously: !AbilitySystemComponent->CharacterAbilitiesGiven monitor for bugs
		return;
	}

	for (TSubclassOf<UCharacterGameplayAbility>& StartupAbility : CharacterAbilities) {
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
	bAbilitiesInitialized = true;

}

void ASBCharacterBase::InitializeAttributes() {

	if (!AbilitySystemComponent.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("%s() Invalid ASC In Initialize Attributes."), *FString(__FUNCTION__), *GetName());
		return;
	}

	if (!DefaultAttributes) {
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes For %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);


	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid()) {
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ASBCharacterBase::AddStartupEffects() {

	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven) { // Previously: !AbilitySystemComponent->CharacterAbilitiesGiven monitor for bugs
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects) {
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid()) {
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}


void ASBCharacterBase::SetHealth(float Health) {
	if (!AbilitySystemComponent.IsValid()) {
		AttributeSetBase->SetHealth(Health);
	}
}

void ASBCharacterBase::SetMana(float Mana) {
	if (!AbilitySystemComponent.IsValid()) {
		AttributeSetBase->SetHealth(Mana);
	}
}

//// Called every frame
//void ASBCharacterBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//// Called to bind functionality to input
//void ASBCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

