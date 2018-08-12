// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#include "Weapon.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AWeapon::FirePrimary()
{
	UE_LOG(LogTemp, Log, TEXT("Fire primary"));

	// if this is being called on the client, make a RPS call out to the server for authority implementation
	if (Role < ROLE_Authority)
	{
		Server_FirePrimary();
	}


}

void AWeapon::Server_FirePrimary_Implementation()
{
	FirePrimary();
}

bool AWeapon::Server_FirePrimary_Validate()
{
	return true;
}

void AWeapon::FireAlternate()
{
	UE_LOG(LogTemp, Log, TEXT("Fire alternate"));

	// if this is being called on the client, make a RPS call out to the server for authority implementation
	if (Role < ROLE_Authority)
	{
		Server_FireAlternate();
	}


}

void AWeapon::Server_FireAlternate_Implementation()
{
	FireAlternate();
}

bool AWeapon::Server_FireAlternate_Validate()
{
	return true;
}
