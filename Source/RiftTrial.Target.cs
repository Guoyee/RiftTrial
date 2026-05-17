// Copyright Yerik Guo

using UnrealBuildTool;
using System.Collections.Generic;

public class RiftTrialTarget : TargetRules
{
	public RiftTrialTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("RiftTrial");
	}
}
