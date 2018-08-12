// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#include "BattleCharacter.h"
#include "Weapon.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABattleCharacter::ABattleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// create the spring arm and camera to follow behind the player character
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ABattleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// the server will spawn the starting weapon which gets replicated to the client
	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(StartingWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			UE_LOG(LogTemp, Log, TEXT("Spawned starting weapon for player %s"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to spawn starting weapon for player %s"), *GetName());
		}
	}
}

// Called every frame
void ABattleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABattleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABattleCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABattleCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABattleCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ABattleCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABattleCharacter::Jump);
	PlayerInputComponent->BindAction("FirePrimary", IE_Pressed, this, &ABattleCharacter::FirePrimary);
	PlayerInputComponent->BindAction("FireAlternate", IE_Pressed, this, &ABattleCharacter::FireAlternate);
}

void ABattleCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ABattleCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ABattleCharacter::FirePrimary()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->FirePrimary();
	}
}

void ABattleCharacter::FireAlternate()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->FireAlternate();
	}
}

void ABattleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleCharacter, CurrentWeapon);
}