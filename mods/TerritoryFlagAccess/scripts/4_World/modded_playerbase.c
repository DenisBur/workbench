modded class PlayerBase
{
	override void OnConnect()
	{
		super.OnConnect();
		if (GetGame().IsServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TFA_DelayedTerritoryCheck, 3000, false, this);
	}

	override void OnReconnect()
	{
		super.OnReconnect();
		if (GetGame().IsServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TFA_DelayedTerritoryCheck, 3000, false, this);
	}

	void TFA_DelayedTerritoryCheck()
	{
		TFA_Rules.TeleportOutIfNeeded(this);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == TFA_Constants.RPC_TFA_UI_PAYLOAD)
		{
			Param1<string> p = new Param1<string>("");
			if (!ctx.Read(p))
				return;
			PlayerBase lp = PlayerBase.Cast(GetGame().GetPlayer());
			if (lp != this)
				return;
			if (!GetGame().IsDedicatedServer())
				TFA_MenuBridge.DeliverUiPayload(p.param1);
		}
	}
}
