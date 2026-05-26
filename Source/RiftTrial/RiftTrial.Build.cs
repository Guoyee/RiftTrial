// Copyright Yerik Guo

using UnrealBuildTool;

public class RiftTrial : ModuleRules
{
	public RiftTrial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayTags",
			"GameplayTasks",
			"NavigationSystem",
			"AIModule",
			"Niagara"
		});
	}
}
