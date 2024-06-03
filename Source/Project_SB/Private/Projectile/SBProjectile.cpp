// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/SBProjectile.h"
#include "Components/SphereComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASBProjectile::ASBProjectile()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Enable replication
    bReplicates = true;

    // Create the sphere collision component
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollision->InitSphereRadius(15.0f);
    SphereCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    //SphereCollision->SetIsReplicated(true);
    RootComponent = SphereCollision;

    //// Create the static mesh component and attach it to the sphere collision component
    //ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    //ProjectileMesh->SetupAttachment(SphereCollision); // Attach to SphereCollision
    //ProjectileMesh->SetCollisionProfileName(TEXT("NoCollision"));
    ////ProjectileMesh->SetIsReplicated(true);

    //// Create the projectile movement component and set its properties
    //ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    //ProjectileMovementComponent->UpdatedComponent = SphereCollision;
    //ProjectileMovementComponent->InitialSpeed = 3000.0f;
    //ProjectileMovementComponent->MaxSpeed = 3000.0f;
    //ProjectileMovementComponent->bRotationFollowsVelocity = true;
    //ProjectileMovementComponent->bShouldBounce = false;
    //ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ////ProjectileMovementComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void ASBProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}