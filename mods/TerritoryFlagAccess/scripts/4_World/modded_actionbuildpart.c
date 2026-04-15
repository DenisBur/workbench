modded class ActionBuildPart
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast(action_data.m_Target.GetObject());
		PlayerBase pl = PlayerBase.Cast(action_data.m_Player);
		if (base_building && pl && pl.GetIdentity())
		{
			if (!TFA_Rules.MayBuildAt(pl, base_building.GetPosition()))
			{
				pl.MessageStatus("На территории чужого флага нельзя строить.");
				return;
			}
		}

		super.OnFinishProgressServer(action_data);
	}
}
