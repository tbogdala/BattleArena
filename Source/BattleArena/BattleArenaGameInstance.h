// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleArenaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API UBattleArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category="Game Options")
	bool bSpawnBots;

	
};
