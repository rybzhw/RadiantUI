

using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantUI_SDKTarget : TargetRules
{
	public RadiantUI_SDKTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange(
            new string[] {
                "RadiantUI_SDK",
            } );
	}
}
