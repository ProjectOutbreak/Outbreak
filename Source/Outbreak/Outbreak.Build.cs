// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using System.Linq;
using UnrealBuildTool;

public class Outbreak : ModuleRules
{
	public Outbreak(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"MovieScene",
			"LevelSequence",
			"MovieSceneTracks",
			"UMG",
			"Slate",
			"SlateCore",
			"EnhancedInput",
			"NavigationSystem",
			"AIModule",
			"Sockets",
			"Networking",
			"PacketHandler",
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
			"OnlineSubsystemNull",
			"Niagara",
			"GameplayCameras"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "Paper2D", "AnimGraphRuntime" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		//Load AWS SDK 
		bUseRTTI = true;
		bEnableExceptions = true;
		if (Target.Type == TargetType.Server)
		{
			PublicAdditionalLibraries.Add("/FORCE:MULTIPLE");
		}
		bUseUnity = false;
		LoadAWSSDK(Target);
	}

	// get full path Source/Outbreak/ThirdParty 
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "ThirdParty/")); }
	}
	
	private void LoadAWSSDK(ReadOnlyTargetRules Target)
	{
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string AWSSDKPath = Path.Combine(ThirdPartyPath, "AwsSDK"); 
        
			string SDKLibPath = Path.Combine(AWSSDKPath, "lib");
			string SDKIncludePath = Path.Combine(AWSSDKPath, "include");

			PublicIncludePaths.Add(SDKIncludePath);

			string[] LibFiles = Directory.GetFiles(SDKLibPath, "*.lib", SearchOption.AllDirectories);
          
			foreach (string LibFile in LibFiles)
			{
				string FileName = Path.GetFileName(LibFile).ToLower();
				if (FileName.Contains("test") || FileName.Contains("mock") || FileName.Contains("gtest"))
				{
					continue; 
				}
				PublicAdditionalLibraries.Add(LibFile);
			}

			PublicAdditionalLibraries.Add("winhttp.lib");
			PublicAdditionalLibraries.Add("bcrypt.lib");
			PublicAdditionalLibraries.Add("userenv.lib");     
			PublicAdditionalLibraries.Add("version.lib");     
			PublicAdditionalLibraries.Add("shlwapi.lib");     
			PublicAdditionalLibraries.Add("crypt32.lib");     
			PublicAdditionalLibraries.Add("secur32.lib");     
			PublicAdditionalLibraries.Add("ncrypt.lib");      
		}
	}}