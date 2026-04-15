class TerritoryFlagAccessMenu extends UIScriptedMenu
{
	protected MultilineTextWidget m_Help;
	protected TextListboxWidget m_List;
	protected ButtonWidget m_BtnClose;
	protected ButtonWidget m_BtnAdd;
	protected ButtonWidget m_BtnRemove;
	protected ButtonWidget m_BtnResident;
	protected ButtonWidget m_BtnBuilder;
	protected ButtonWidget m_BtnTransfer;

	protected int m_NetLo;
	protected int m_NetHi;
	protected string m_OwnerUid;
	protected string m_MembersBlob;
	protected string m_MyUid;
	protected int m_CanManage;
	protected int m_Law;

	void TerritoryFlagAccessMenu()
	{
		if (GetGame().GetMission())
		{
			GetGame().GetMission().AddActiveInputExcludes({"menu"});
			GetGame().GetMission().GetHud().ShowHudUI(false);
			GetGame().GetMission().GetHud().ShowQuickbarUI(false);
		}
	}

	void ~TerritoryFlagAccessMenu()
	{
		if (GetGame() && GetGame().GetMission())
		{
			GetGame().GetMission().RemoveActiveInputExcludes({"menu"}, true);
			GetGame().GetMission().GetHud().ShowHudUI(true);
			GetGame().GetMission().GetHud().ShowQuickbarUI(true);
		}
	}

	override int GetID()
	{
		return MENU_TERRITORY_FLAG_ACCESS;
	}

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("TerritoryFlagAccess/gui/layouts/tfa_flag_menu.layout");
		m_Help = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("tfa_help"));
		m_List = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("tfa_list"));
		m_BtnClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("tfa_btn_close"));
		m_BtnAdd = ButtonWidget.Cast(layoutRoot.FindAnyWidget("tfa_btn_add"));
		m_BtnRemove = ButtonWidget.Cast(layoutRoot.FindAnyWidget("tfa_btn_remove"));
		m_BtnResident = ButtonWidget.Cast(layoutRoot.FindAnyWidget("tfa_btn_resident"));
		m_BtnBuilder = ButtonWidget.Cast(layoutRoot.FindAnyWidget("tfa_btn_builder"));
		m_BtnTransfer = ButtonWidget.Cast(layoutRoot.FindAnyWidget("tfa_btn_transfer"));

		ParsePayload(TFA_MenuBridge.ConsumeInitialPayload());
		ApplyUiState();
		return layoutRoot;
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		string r = TFA_MenuBridge.ConsumeRefreshPayload();
		if (r != "")
		{
			ParsePayload(r);
			ApplyUiState();
		}
	}

	protected void ParsePayload(string raw)
	{
		m_NetLo = 0;
		m_NetHi = 0;
		m_OwnerUid = "";
		m_MembersBlob = "";
		m_MyUid = "";
		m_CanManage = 0;
		m_Law = 0;

		if (!raw || raw == "")
			return;

		TStringArray p = new TStringArray;
		raw.Split("|", p);
		if (p.Count() < 7)
			return;

		m_NetLo = p[0].ToInt();
		m_NetHi = p[1].ToInt();
		m_OwnerUid = p[2];
		m_MembersBlob = p[3];
		m_MyUid = p[4];
		m_CanManage = p[5].ToInt();
		m_Law = p[6].ToInt();
	}

	protected void ApplyUiState()
	{
		string help = "Владелец — тот, кто первым установил ткань флага в флагшток.\n";
		help = help + "Житель: может находиться в зоне при входе в игру.\n";
		help = help + "Строитель: житель + может строить и разбирать постройки киркой/инструментом.\n";
		help = help + "Чужаки при поднятом флаге с активным временем не могут строить и разбирать инструментом (рейд — взрывчатка/оружие).\n";
		if (m_Law == 1)
			help = help + "Сейчас правила флага активны.\n";
		else
			help = help + "Правила флага не активны (нет ткани или истекло время).\n";
		if (m_CanManage != 1)
			help = help + "Только владелец может менять список.";

		if (m_Help)
		{
			m_Help.SetText(help);
			m_Help.Update();
		}

		if (m_List)
		{
			m_List.ClearItems();
			int row = m_List.AddItem(m_OwnerUid, NULL, 0);
			m_List.SetItem(row, "владелец", NULL, 1);

			map<string, int> mem = new map<string, int>;
			TFA_Roster.DeserializeMembers(m_MembersBlob, mem);
			for (int i = 0; i < mem.Count(); i++)
			{
				string uid = mem.GetKey(i);
				int role = mem.GetElement(i);
				string rlab = "житель";
				if (role == 1)
					rlab = "строитель";
				int r = m_List.AddItem(uid, NULL, 0);
				m_List.SetItem(r, rlab, NULL, 1);
			}
		}

		bool mgr = m_CanManage == 1;
		if (m_BtnAdd)
			m_BtnAdd.Show(mgr);
		if (m_BtnRemove)
			m_BtnRemove.Show(mgr);
		if (m_BtnResident)
			m_BtnResident.Show(mgr);
		if (m_BtnBuilder)
			m_BtnBuilder.Show(mgr);
		if (m_BtnTransfer)
			m_BtnTransfer.Show(mgr);
	}

	protected TerritoryFlag TFA_GetFlag()
	{
		Object o = GetGame().GetObjectByNetworkId(m_NetLo, m_NetHi);
		return TerritoryFlag.Cast(o);
	}

	protected void TFA_SendRpc(int rpcType, Param param)
	{
		TerritoryFlag tf = TFA_GetFlag();
		if (!tf || !param)
			return;
		tf.RPCSingleParam(rpcType, param, true, NULL);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (w == m_BtnClose)
		{
			Close();
			return true;
		}

		if (m_CanManage != 1)
			return false;

		if (w == m_BtnAdd)
		{
			TFA_SendRpc(TFA_Constants.RPC_TFA_ADD_MEMBER, new Param1<bool>(true));
			return true;
		}

		if (w == m_BtnRemove)
		{
			if (!m_List)
				return true;
			int sel = m_List.GetSelectedRow();
			if (sel < 1)
				return true;
			string uid;
			m_List.GetItemText(sel, 0, uid);
			if (uid == "" || uid == m_OwnerUid)
				return true;
			TFA_SendRpc(TFA_Constants.RPC_TFA_REMOVE_MEMBER, new Param1<string>(uid));
			return true;
		}

		if (w == m_BtnResident || w == m_BtnBuilder)
		{
			if (!m_List)
				return true;
			int sel = m_List.GetSelectedRow();
			if (sel < 1)
				return true;
			string uid;
			m_List.GetItemText(sel, 0, uid);
			if (uid == "" || uid == m_OwnerUid)
				return true;
			int role = 0;
			if (w == m_BtnBuilder)
				role = 1;
			TFA_SendRpc(TFA_Constants.RPC_TFA_SET_ROLE, new Param2<string, int>(uid, role));
			return true;
		}

		if (w == m_BtnTransfer)
		{
			TFA_SendRpc(TFA_Constants.RPC_TFA_TRANSFER_NEAREST, new Param1<bool>(true));
			return true;
		}

		return false;
	}
}
