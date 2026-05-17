// Copyright Yerik Guo

using UnrealBuildTool;
using System.Collections.Generic;

public class RiftTrialEditorTarget : TargetRules
{
	public RiftTrialEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("RiftTrial");
	}
}
