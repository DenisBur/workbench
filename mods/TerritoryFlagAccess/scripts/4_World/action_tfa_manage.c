class ActionTFAManage: ActionSingleUseBase
{
	void ActionTFAManage()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "Территория: доступ";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.BASEBUILDING);
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		TerritoryFlag tf = TerritoryFlag.Cast(target.GetObject());
		if (!tf || !player)
			return false;
		return true;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		TerritoryFlag tf = TerritoryFlag.Cast(action_data.m_Target.GetObject());
		PlayerBase pl = PlayerBase.Cast(action_data.m_Player);
		if (tf && pl)
			tf.TFA_ServerOpenUiFor(pl);
	}
}
