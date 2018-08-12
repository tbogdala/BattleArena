// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BattleArenaTarget : TargetRules
{
	public BattleArenaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "BattleArena" } );
	}
}
