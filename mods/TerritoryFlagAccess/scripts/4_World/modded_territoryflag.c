modded class TerritoryFlag
{
	protected string m_TFA_OwnerUID;
	protected ref map<string, int> m_TFA_Members;

	int TFA_GetRefresherRemaining()
	{
		return m_RefresherTimeRemaining;
	}

	string TFA_GetOwnerUID()
	{
		return m_TFA_OwnerUID;
	}

	int TFA_GetMemberRole(string uid)
	{
		if (!m_TFA_Members || uid == "")
			return -1;
		if (!m_TFA_Members.Contains(uid))
			return -1;
		return m_TFA_Members.Get(uid);
	}

	override void EEInit()
	{
		super.EEInit();
		if (!m_TFA_Members)
			m_TFA_Members = new map<string, int>;
		if (GetGame().IsServer())
			TFA_Registry.Register(this);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame().IsServer())
			TFA_Registry.Unregister(this);
		super.EEDelete(parent);
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		if (!GetGame().IsServer())
			return;
		if (slot_name != "Material_FPole_Flag")
			return;
		if (!Flag_Base.Cast(item))
			return;
		if (m_TFA_OwnerUID != "")
			return;

		PlayerBase p = PlayerBase.Cast(item.GetHierarchyRootPlayer());
		if (!p || !p.GetIdentity())
			p = TFA_Rules.FindNearestPlayer(GetPosition(), 5.0);
		if (p && p.GetIdentity())
			m_TFA_OwnerUID = p.GetIdentity().GetId();
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_TFA_OwnerUID);
		ctx.Write(TFA_Roster.SerializeMembersOnly(m_TFA_Members, m_TFA_OwnerUID));
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!m_TFA_Members)
			m_TFA_Members = new map<string, int>;

		string o = "";
		string blob = "";
		if (ctx.Read(o) && ctx.Read(blob))
		{
			m_TFA_OwnerUID = o;
			TFA_Roster.DeserializeMembers(blob, m_TFA_Members);
		}

		return true;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTFAManage);
	}

	void TFA_ServerOpenUiFor(PlayerBase pl)
	{
		if (!GetGame().IsServer() || !pl || !pl.GetIdentity())
			return;
		if (!TFA_Game.IsNearObject(pl, this, 6.0))
			return;

		string payload = TFA_MakeUiPayload(pl);
		pl.RPCSingleParam(TFA_Constants.RPC_TFA_UI_PAYLOAD, new Param1<string>(payload), true, pl.GetIdentity());
	}

	protected string TFA_MakeUiPayload(PlayerBase pl)
	{
		int lo;
		int hi;
		GetNetworkID(lo, hi);
		string my = pl.GetIdentity().GetId();
		int can = 0;
		if (my == m_TFA_OwnerUID)
			can = 1;
		int law = 0;
		if (TFA_Rules.IsLawActive(this))
			law = 1;

		string blob = TFA_Roster.SerializeMembersOnly(m_TFA_Members, m_TFA_OwnerUID);
		return lo.ToString() + "|" + hi.ToString() + "|" + m_TFA_OwnerUID + "|" + blob + "|" + my + "|" + can.ToString() + "|" + law.ToString();
	}

	protected void TFA_RefreshUi(PlayerBase pl)
	{
		if (pl)
			TFA_ServerOpenUiFor(pl);
	}

	protected bool TFA_IsManager(PlayerBase pl)
	{
		if (!pl || !pl.GetIdentity())
			return false;
		return pl.GetIdentity().GetId() == m_TFA_OwnerUID;
	}

	protected void TFA_ServerAddNearestResident(PlayerBase mgr)
	{
		if (!TFA_IsManager(mgr))
			return;
		if (!TFA_Game.IsNearObject(mgr, this, 6.0))
			return;

		PlayerBase best;
		float bestD = TFA_Constants.ADD_MEMBER_MAX_DIST * TFA_Constants.ADD_MEMBER_MAX_DIST;
		array<Man> arr = new array<Man>;
		GetGame().GetPlayers(arr);
		for (int i = 0; i < arr.Count(); i++)
		{
			PlayerBase p = PlayerBase.Cast(arr[i]);
			if (!p || !p.GetIdentity() || p == mgr)
				continue;
			string uid = p.GetIdentity().GetId();
			if (uid == m_TFA_OwnerUID)
				continue;
			if (m_TFA_Members && m_TFA_Members.Contains(uid))
				continue;
			float d2 = vector.DistanceSq(p.GetPosition(), GetPosition());
			if (d2 <= bestD)
			{
				bestD = d2;
				best = p;
			}
		}

		if (best && best.GetIdentity())
		{
			m_TFA_Members.Set(best.GetIdentity().GetId(), ETFA_Role.Resident);
			SetSynchDirty();
		}
	}

	protected void TFA_ServerRemoveMember(PlayerBase mgr, string uid)
	{
		if (!TFA_IsManager(mgr) || uid == "")
			return;
		if (!TFA_Game.IsNearObject(mgr, this, 6.0))
			return;
		if (uid == m_TFA_OwnerUID)
			return;
		if (m_TFA_Members && m_TFA_Members.Contains(uid))
		{
			m_TFA_Members.Remove(uid);
			SetSynchDirty();
		}
	}

	protected void TFA_ServerSetRole(PlayerBase mgr, string uid, int role)
	{
		if (!TFA_IsManager(mgr) || uid == "")
			return;
		if (!TFA_Game.IsNearObject(mgr, this, 6.0))
			return;
		if (uid == m_TFA_OwnerUID)
			return;
		if (!m_TFA_Members || !m_TFA_Members.Contains(uid))
			return;
		if (role != ETFA_Role.Resident && role != ETFA_Role.Builder)
			return;
		m_TFA_Members.Set(uid, role);
		SetSynchDirty();
	}

	protected void TFA_ServerTransferToNearest(PlayerBase mgr)
	{
		if (!TFA_IsManager(mgr) || !TFA_Game.IsNearObject(mgr, this, 6.0))
			return;

		PlayerBase best;
		float bestD = 1.0e12;
		array<Man> arr = new array<Man>;
		GetGame().GetPlayers(arr);

		for (int i = 0; i < arr.Count(); i++)
		{
			PlayerBase p = PlayerBase.Cast(arr[i]);
			if (!p || !p.GetIdentity() || p == mgr)
				continue;
			string uid = p.GetIdentity().GetId();
			if (uid == m_TFA_OwnerUID)
				continue;
			if (!TFA_Game.IsNearObject(p, this, TFA_Constants.ADD_MEMBER_MAX_DIST))
				continue;
			float d2 = vector.DistanceSq(p.GetPosition(), GetPosition());
			if (d2 < bestD)
			{
				bestD = d2;
				best = p;
			}
		}

		if (best && best.GetIdentity())
			TFA_ServerTransferOwner(mgr, best.GetIdentity().GetId());
	}

	protected void TFA_ServerTransferOwner(PlayerBase mgr, string newUid)
	{
		if (!TFA_IsManager(mgr) || newUid == "" || newUid == m_TFA_OwnerUID)
			return;
		if (!TFA_Game.IsNearObject(mgr, this, 6.0))
			return;

		PlayerBase target;
		array<Man> arr = new array<Man>;
		GetGame().GetPlayers(arr);
		for (int i = 0; i < arr.Count(); i++)
		{
			PlayerBase p = PlayerBase.Cast(arr[i]);
			if (p && p.GetIdentity() && p.GetIdentity().GetId() == newUid)
			{
				target = p;
				break;
			}
		}
		if (!target || !TFA_Game.IsNearObject(target, this, TFA_Constants.ADD_MEMBER_MAX_DIST))
			return;

		string old = m_TFA_OwnerUID;
		if (old != "" && m_TFA_Members && !m_TFA_Members.Contains(old))
			m_TFA_Members.Set(old, ETFA_Role.Builder);
		if (m_TFA_Members && m_TFA_Members.Contains(newUid))
			m_TFA_Members.Remove(newUid);
		m_TFA_OwnerUID = newUid;
		SetSynchDirty();
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (!GetGame().IsServer())
			return;

		PlayerBase pl = TFA_Game.PlayerFromIdentity(sender);
		if (!pl)
			return;

		switch (rpc_type)
		{
			case TFA_Constants.RPC_TFA_ADD_MEMBER:
			{
				TFA_ServerAddNearestResident(pl);
				TFA_RefreshUi(pl);
				break;
			}
			case TFA_Constants.RPC_TFA_REMOVE_MEMBER:
			{
				Param1<string> p1 = new Param1<string>("");
				if (ctx.Read(p1))
					TFA_ServerRemoveMember(pl, p1.param1);
				TFA_RefreshUi(pl);
				break;
			}
			case TFA_Constants.RPC_TFA_SET_ROLE:
			{
				Param2<string, int> p2 = new Param2<string, int>("", 0);
				if (ctx.Read(p2))
					TFA_ServerSetRole(pl, p2.param1, p2.param2);
				TFA_RefreshUi(pl);
				break;
			}
			case TFA_Constants.RPC_TFA_TRANSFER_OWNER:
			{
				Param1<string> pt = new Param1<string>("");
				if (ctx.Read(pt))
					TFA_ServerTransferOwner(pl, pt.param1);
				TFA_RefreshUi(pl);
				break;
			}
			case TFA_Constants.RPC_TFA_TRANSFER_NEAREST:
			{
				TFA_ServerTransferToNearest(pl);
				TFA_RefreshUi(pl);
				break;
			}
		}
	}
}
