// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

#include "BattleArenaPlayerState.h"
#include "Net/UnrealNetwork.h"



void ABattleArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleArenaPlayerState, Wins);
	DOREPLIFETIME(ABattleArenaPlayerState, Losses);
}
