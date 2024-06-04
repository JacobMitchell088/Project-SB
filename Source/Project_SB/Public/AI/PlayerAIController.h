// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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
};
