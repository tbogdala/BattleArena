// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BattleArenaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API ABattleArenaPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 Wins;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	int32 Losses;

};
