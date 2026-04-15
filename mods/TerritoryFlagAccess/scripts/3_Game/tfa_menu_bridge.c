//! ID меню (совпадает с case в modded MissionBase).
const int MENU_TERRITORY_FLAG_ACCESS = 984100;

//! Мост World → UI: модуль World не может ссылаться на классы Mission.
class TFA_MenuBridge
{
	protected static string s_InitialPayload;
	protected static string s_RefreshPayload;

	//! Открыть меню или обновить уже открытое (после действий на сервере).
	static void DeliverUiPayload(string payload)
	{
		if (GetGame().IsDedicatedServer())
			return;

		UIManager ui = GetGame().GetUIManager();
		if (!ui)
			return;

		if (ui.IsMenuOpen(MENU_TERRITORY_FLAG_ACCESS))
		{
			s_RefreshPayload = payload;
			return;
		}

		s_InitialPayload = payload;
		ui.EnterScriptedMenu(MENU_TERRITORY_FLAG_ACCESS, ui.GetMenu());
	}

	static string ConsumeInitialPayload()
	{
		string t = s_InitialPayload;
		s_InitialPayload = "";
		return t;
	}

	static string ConsumeRefreshPayload()
	{
		string t = s_RefreshPayload;
		s_RefreshPayload = "";
		return t;
	}
}
