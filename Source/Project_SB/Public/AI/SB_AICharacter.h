// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SBCharacterBase.h"
#include "AttributeSets/AI_AttributeSetBase.h"
#include "GameplayEffectTypes.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "SB_AICharacter.generated.h"



UCLASS()
class PROJECT_SB_API ASB_AICharacter : public ASBCharacterBase
{
	GENERATED_BODY()


public:
	ASB_AICharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "SB|AI")
	void ApplyDamageAI(float DamageAmount);

	UBehaviorTree* GetBehaviorTree() const;

	virtual void PossessedBy(AController* NewController) override; // Will call InitializeAttributes()

	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void Tick(float DeltaTime) override;

	virtual void Die() override;

	friend UAI_AttributeSetBase; // Allow Attributes to use HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SB|AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;

	virtual void InitializeAttributes() override;


	// PS Transferred For Net stability


	class UAI_AttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "SB|AI")
	bool IsAliveAI() const;

	UFUNCTION(BlueprintCallable, Category = "SB|AI|Attributes")
	float GetAIHealth() const;

	UFUNCTION(BlueprintCallable, Category = "SB|AI|Attributes")
	float GetAIMaxHealth() const;

	//UFUNCTION(BlueprintCallable, Category = "SB|SBPlayerState|Attributes")
	//float GetMana() const;

	//UFUNCTION(BlueprintCallable, Category = "SB|SBPlayerState|Attributes")
	//float GetMaxMana() const;

	//UFUNCTION(BlueprintCallable, Category = "SB|SBPlayerState|Attributes")
	//int32 GetCharacterLevel() const;

protected:
	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UAI_AttributeSetBase* AI_AttributeSetBase;

	
	FGameplayTag DeadTag;


	FDelegateHandle AIHealthChangedDelegateHandle;
	FDelegateHandle AIMaxHealthChangedDelegateHandle;
	//FDelegateHandle ManaChangedDelegateHandle;
	//FDelegateHandle MaxManaChangedDelegateHandle;
	//FDelegateHandle CharacterLevelChangedDelegateHandle;

	//virtual void BeginPlay() override;

	virtual void AIHealthChanged(const FOnAttributeChangeData& Data);
	virtual void AIMaxHealthChanged(const FOnAttributeChangeData& Data);
	//virtual void ManaChanged(const FOnAttributeChangeData& Data);
	//virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	//virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);


	virtual void AIStunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

};
