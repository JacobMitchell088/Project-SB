// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Project_SB/Project_SB.h"
#include "CharacterGameplayAbility.generated.h"



/**
 * 
 */
UCLASS()
class PROJECT_SB_API UCharacterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCharacterGameplayAbility();



	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	SBAbilityID AbilityInputID = SBAbilityID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	SBAbilityID AbilityID = SBAbilityID::None;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;


	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	
};
