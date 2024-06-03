// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include <Project_SB/Project_SB.h>
#include "SBCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ASBCharacterBase*, Character);

UCLASS()
class PROJECT_SB_API ASBCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASBCharacterBase(const class FObjectInitializer& ObjectInitializer);

	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	//// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintAssignable, Category = "SB|Character")
	FCharacterDiedDelegate OnCharacterDied;

	UFUNCTION(BlueprintCallable, Category = "SB|Character")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "SB|Character")
	virtual int32 GetAbilityLevel(SBAbilityID AbilityID) const;

	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable, Category = "SB|Character")
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "SB|Character")
	virtual void FinishDying();



	// Getters for our attributes
	UFUNCTION(BlueprintCallable, Category = "SB|Character|Attributes")
	float GetCharacterLevel() const;
	UFUNCTION(BlueprintCallable, Category = "SB|Character|Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "SB|Character|Attributes")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "SB|Character|Attributes")
	float GetMana() const;
	UFUNCTION(BlueprintCallable, Category = "SB|Character|Attributes")
	float GetMaxMana() const;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TWeakObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag; // Apply to a character after it has died and ready to be removed from the game

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SB|Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SB|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SB|Abilities")
	TArray<TSubclassOf<class UCharacterGameplayAbility>> CharacterAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SB|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SB|Abilities") // Basically for effects we can used to indicate the spawning of a player
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UFUNCTION(BlueprintCallable, Category = "SB|Character")
	void EnableRagdoll();

	virtual void AddCharacterAbilities();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void SetHealth(float Health);
	
	virtual void SetMana(float Mana);

};
