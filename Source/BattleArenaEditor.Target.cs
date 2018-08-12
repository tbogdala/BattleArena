// Copyright 2018 by Timothy Bogdala. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BattleArenaEditorTarget : TargetRules
{
	public BattleArenaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "BattleArena" } );
	}
}
