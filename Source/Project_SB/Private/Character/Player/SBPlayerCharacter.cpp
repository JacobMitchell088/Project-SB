// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/SBPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "AI/PlayerAIController.h"
#include "Player/SBPlayerState.h"
#include "Character/Player/SBPlayerCharacter.h"
#include "Player/SBPlayerController.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ASBPlayerCharacter::ASBPlayerCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0, 0, 200));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->FieldOfView = 90.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	AIControllerClass = APlayerController::StaticClass();
	OurPlayerController = nullptr;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));


	DeadzoneRadius = 200.0f;
	CameraMovementSpeed = 300.0f;
	MaxCameraDistance = 1000.0f;
	LastCameraLocation = CameraBoom->GetComponentLocation();
}

void ASBPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASBPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASBPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASBPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASBPlayerCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("Turn", this, &ASBPlayerCharacter::Turn);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASBPlayerCharacter::TurnRate);

	BindASCInput();
}

// Server ASC Initilization
void ASBPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASBPlayerState* PS = GetPlayerState<ASBPlayerState>();

	if (PS) {
		InitializeStartingValues(PS);
	

		AddStartupEffects();
		AddCharacterAbilities();
	}
}

USpringArmComponent* ASBPlayerCharacter::GetCameraBoom()
{
	return CameraBoom;
}

UCameraComponent* ASBPlayerCharacter::GetFollowCamera()
{
	return FollowCamera;
}

float ASBPlayerCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector ASBPlayerCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomArmLocation;
}

void ASBPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomArmLocation = CameraBoom->GetRelativeLocation();

	OurPlayerController = Cast<ASBPlayerController>(GetController());
}

void ASBPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateToFaceCursor();
	// UpdateCameraPosition(DeltaTime);
}

void ASBPlayerCharacter::LookUp(float Value)
{
	//if (IsAlive()) {
	//	AddControllerPitchInput(Value);
	//}
}

void ASBPlayerCharacter::LookUpRate(float Value)
{
	//if (IsAlive()) {
	//	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->DeltaTimeSeconds);
	//}
}

void ASBPlayerCharacter::Turn(float Value)
{
	//if (IsAlive()) {
	//	AddControllerYawInput(Value);
	//}
}

void ASBPlayerCharacter::TurnRate(float Value)
{
	//if (IsAlive()) {
	//	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->DeltaTimeSeconds);
	//}
}

void ASBPlayerCharacter::MoveForward(float Value)
{
	if (IsAlive() && AbilitySystemComponent->GetGameplayTagCount(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))) == 0) {
		// AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value); // For Third Person idk what we're doing yet so I'll leave it
		AddMovementInput(FVector::ForwardVector, Value);
	}
}

void ASBPlayerCharacter::MoveRight(float Value)
{
	if (IsAlive() && AbilitySystemComponent->GetGameplayTagCount(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))) == 0) {
		// AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);  // See prev comment
		AddMovementInput(FVector::RightVector, Value);
	}
}

void ASBPlayerCharacter::RotateToFaceCursor()
{
	if (!IsAlive() || !OurPlayerController || AbilitySystemComponent->GetGameplayTagCount(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))) != 0) {
		return; 
	}

	FVector MouseLocation, MouseDirection;

	if (OurPlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection)) {
		FVector CharacterLocation = GetActorLocation();
		FVector WorldMousePosition = MouseLocation + MouseDirection * (FVector::DotProduct(CharacterLocation - MouseLocation, FVector::UpVector) / FVector::DotProduct(MouseDirection, FVector::UpVector));

		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, WorldMousePosition);
		SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f)); // Possibly try PlayerController->SetControlRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f)); To obtain a smoother turn rather than instant



	}

}

void ASBPlayerCharacter::UpdateCameraPosition(float DeltaTime) // Come back to it not working
{
	if (!OurPlayerController) {
		return;
	}

	// Get the mouse position
	float MouseX, MouseY;
	OurPlayerController->GetMousePosition(MouseX, MouseY);

	// Get the character's screen position
	FVector2D ScreenPosition;
	OurPlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenPosition);

	// Calculate the offset from the character to the mouse position
	FVector2D Offset = FVector2D(MouseX, MouseY) - ScreenPosition;

	// Current camera location
	FVector CurrentLocation = CameraBoom->GetComponentLocation();

	if (Offset.Size() > DeadzoneRadius) { // Outside deadzone
		// Normalize the offset to use as a direction
		FVector Direction = GetActorForwardVector(); // . getsafenormal()

		// Scale the direction by the offset size and a speed factor
		FVector NewLocation = CurrentLocation + Direction * Offset.Size() * CameraMovementSpeed * DeltaTime;

		// Calculate the distance from the character to the new camera location
		float DistanceFromCharacter = FVector::Dist(GetActorLocation(), NewLocation);

		// Clamp the camera distance to the maximum allowed distance
		if (DistanceFromCharacter > MaxCameraDistance) {
			NewLocation = GetActorLocation() + (NewLocation - GetActorLocation()).GetSafeNormal() * MaxCameraDistance;
			NewLocation.Z = CurrentLocation.Z;
		}

		// Set the new camera location
		// Vinterp(newlocation, oldlcoation)
		CameraBoom->SetWorldLocation(NewLocation);
	}
	else { // Inside deadzone, smoothly recenter
		FVector TargetLocation = GetActorLocation();
		TargetLocation.Z = CurrentLocation.Z; // Maintain the camera height

		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, CameraMovementSpeed);
		CameraBoom->SetWorldLocation(NewLocation);
	}
}


// Client ASC Initialization
void ASBPlayerCharacter::OnRep_PlayerState() 
{
	Super::OnRep_PlayerState();

	ASBPlayerState* PS = GetPlayerState<ASBPlayerState>();
	if (PS) {
		InitializeStartingValues(PS);
		BindASCInput();
	}
}

void ASBPlayerCharacter::InitializeStartingValues(ASBPlayerState* PS)
{
	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	AttributeSetBase = PS->GetAttributeSetBase();
	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	InitializeAttributes();
	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());
}

void ASBPlayerCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent)) {
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("SBAbilityID"), static_cast<int32>(SBAbilityID::Confirm), static_cast<int32>(SBAbilityID::Cancel)));

		ASCInputBound = true;
	}
}
