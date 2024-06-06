// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/PlayerAIController.h"

APlayerAIController::APlayerAIController()
{
	bWantsPlayerState = false;

	
}


void APlayerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASB_AICharacter* AIPawn = Cast<ASB_AICharacter>(InPawn)) {
		//AIPawn->GetAbilitySystemComponent()->InitAbilityActorInfo(InPawn, InPawn); // Originally, AIPawn->GetAbilitySystemComponent()->InitAbilityActorInfo(this, InPawn);
																				  // The problem with the idea of assigning the Avatar ownership by giving it to the AI controller is that when we invoke gameplay effects we see that the controller
																				  // Is being passed in as an instigator, however since this only exists on the server we cannot access it's own pawn.
																				  // Will now call this from inside of InitializeAttributes() to ensure ASC is properly setup.
		UE_LOG(LogTemp, Warning, TEXT("InitAbilityActorInfo Success"), *FString(__FUNCTION__), *GetName());
		
		AIPawn->InitializeAttributes();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("OnPossess In PlayerAIController Failed to InitializeAbilityActorInfo @Kimic"), *FString(__FUNCTION__), *GetName());
	}







	//// Tree activation
	//if (ASB_AICharacter* const SB_AICharacter = Cast<ASB_AICharacter>(InPawn)) { // Throw in for behavior tree activation when an AI posses this pawn

	//	if (UBehaviorTree* const Tree = SB_AICharacter->GetBehaviorTree()) {
	//		UBlackboardComponent* b;
	//		UseBlackboard(Tree->BlackboardAsset, b);
	//		Blackboard = b;
	//		RunBehaviorTree(Tree);
	//	}
	//	else {
	//		UE_LOG(LogTemp, Error, TEXT("Failure to see behavior tree"), *FString(__FUNCTION__), *GetName());
	//	}

	//}
}

void APlayerAIController::BeginPlay()
{
	Super::BeginPlay();

}
