class CfgPatches
{
	class TerritoryFlagAccess_Script
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
	};
};

class CfgMods
{
	class TerritoryFlagAccess
	{
		dir = "TerritoryFlagAccess";
		name = "Territory Flag Access";
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"TerritoryFlagAccess/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"TerritoryFlagAccess/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"TerritoryFlagAccess/scripts/5_Mission"};
			};
		};
	};
};
