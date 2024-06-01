// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SBPlayerController.h"
#include "Player/SBPlayerState.h"
#include "AbilitySystemComponent.h"

void ASBPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASBPlayerState* PS = GetPlayerState<ASBPlayerState>();

	if (PS) {
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void ASBPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetShowMouseCursor(true);
}


// TODO -- Add hud stuff