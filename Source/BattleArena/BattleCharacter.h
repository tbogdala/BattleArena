// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleCharacter.generated.h"

// forward declares
class UCameraComponent;
class USpringArmComponent;
class AWeapon;

UCLASS()
class BATTLEARENA_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABattleCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FirePrimary();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireAlternate();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	TSubclassOf<AWeapon> StartingWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Player")
	FName WeaponAttachSocketName;

	UPROPERTY(Replicated)
	AWeapon* CurrentWeapon;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);

};
