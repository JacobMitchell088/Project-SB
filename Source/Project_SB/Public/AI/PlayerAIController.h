// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SB_AICharacter.h"
#include "PlayerAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SB_API APlayerAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	APlayerAIController();


protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;

	// virtual void OnRep_Controller() override; // Possibly need to make this to call InitializeAttributes such that clients would receive up to date details on enemies 

};
