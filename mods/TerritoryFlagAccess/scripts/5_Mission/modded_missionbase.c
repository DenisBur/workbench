modded class MissionBase
{
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		if (id == MENU_TERRITORY_FLAG_ACCESS)
			return new TerritoryFlagAccessMenu;
		return super.CreateScriptedMenu(id);
	}
}
