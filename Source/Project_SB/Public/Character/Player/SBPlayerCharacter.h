// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SBCharacterBase.h"
#include "Player/SBPlayerState.h"
#include "SBPlayerCharacter.generated.h"

/**
 * 
 */

// Trying to forward declare playercontroller
class ASBPlayerController;

UCLASS()
class PROJECT_SB_API ASBPlayerCharacter : public ASBCharacterBase 
{
	GENERATED_BODY()
	
public:
	ASBPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

	UFUNCTION(BlueprintCallable, Category = "SB|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "SB|Camera")
	FVector GetStartingCameraBoomLocation();

protected:
	ASBPlayerController* OurPlayerController;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SB|Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SB|Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SB|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "SB|Camera")
	FVector StartingCameraBoomArmLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SB|Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SB|Camera")
	class UCameraComponent* FollowCamera;

	UFUNCTION(BlueprintImplementableEvent, Category = "SB|Events")
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);
	
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	friend UCharacterAttributeSetBase;

	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void LookUp(float Value);
	void LookUpRate(float Value);
	void Turn(float Value);
	void TurnRate(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);

	void RotateToFaceCursor();



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SB|Camera");
	float DeadzoneRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SB|Camera");
	float CameraMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SB|Camera");
	float MaxCameraDistance;
	FVector LastCameraLocation;

	void UpdateCameraPosition(float DeltaTime);


	virtual void OnRep_PlayerState() override;

	void InitializeStartingValues(ASBPlayerState* PS);

	void BindASCInput();
};
