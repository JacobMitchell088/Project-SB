// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PlayerAIController.h"

APlayerAIController::APlayerAIController()
{
	bWantsPlayerState = false;

	
}

void APlayerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void APlayerAIController::BeginPlay()
{
	Super::BeginPlay();

}
