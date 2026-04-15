//! Правила: кто может строить / снимать постройки в зоне.
class TFA_Rules
{
	static PlayerBase FindNearestPlayer(vector pos, float radius)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		PlayerBase best;
		float r2 = radius * radius;
		float bestD = r2 + 1;

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase p = PlayerBase.Cast(players[i]);
			if (!p || !p.GetIdentity())
				continue;
			float d = vector.DistanceSq(p.GetPosition(), pos);
			if (d < bestD && d <= r2)
			{
				bestD = d;
				best = p;
			}
		}
		return best;
	}

	static bool IsLawActive(TerritoryFlag flag)
	{
		if (!flag)
			return false;
		if (!flag.FindAttachmentBySlotName("Material_FPole_Flag"))
			return false;
		return flag.TFA_GetRefresherRemaining() > 0;
	}

	static bool IsOnRoster(string uid, TerritoryFlag flag)
	{
		if (!flag || uid == "")
			return false;
		if (uid == flag.TFA_GetOwnerUID())
			return true;
		return flag.TFA_GetMemberRole(uid) >= 0;
	}

	//! Житель или строитель (или владелец) может оставаться в зоне при входе.
	static bool MayStayInZone(PlayerBase player, TerritoryFlag flag)
	{
		if (!player || !player.GetIdentity() || !flag)
			return true;
		string uid = player.GetIdentity().GetId();
		return IsOnRoster(uid, flag);
	}

	//! Строительство (новый холограм / части): владелец или Builder.
	static bool MayBuildAt(PlayerBase player, vector worldPos)
	{
		if (!player || !player.GetIdentity())
			return true;

		float d2;
		TerritoryFlag f = TFA_Registry.FindNearestFlag(worldPos, TFA_Constants.ZONE_RADIUS, d2);
		if (!f)
			return true;

		if (!IsLawActive(f))
			return true;

		string uid = player.GetIdentity().GetId();
		if (uid == f.TFA_GetOwnerUID())
			return true;
		if (f.TFA_GetMemberRole(uid) == ETFA_Role.Builder)
			return true;

		return false;
	}

	//! Разбор киркой/инструментом: при активном флаге — только свои Builder/владелец.
	static bool MayDismantleWithTool(PlayerBase player, vector partWorldPos)
	{
		if (!player || !player.GetIdentity())
			return true;

		float d2;
		TerritoryFlag f = TFA_Registry.FindNearestFlag(partWorldPos, TFA_Constants.ZONE_RADIUS, d2);
		if (!f)
			return true;
		if (!IsLawActive(f))
			return true;

		string uid = player.GetIdentity().GetId();
		if (uid == f.TFA_GetOwnerUID())
			return true;
		if (f.TFA_GetMemberRole(uid) == ETFA_Role.Builder)
			return true;

		return false;
	}

	static void TeleportOutIfNeeded(PlayerBase player)
	{
		if (!GetGame().IsServer() || !player || !player.GetIdentity())
			return;

		vector pos = player.GetPosition();
		float d2;
		TerritoryFlag f = TFA_Registry.FindNearestFlag(pos, TFA_Constants.ZONE_RADIUS, d2);
		if (!f)
			return;
		if (!IsLawActive(f))
			return;

		string uid = player.GetIdentity().GetId();
		if (MayStayInZone(player, f))
			return;

		vector fp = f.GetPosition();
		vector delta = pos - fp;
		delta[1] = 0;
		if (delta.Length() < 0.5)
			delta = "1 0 0";
		delta.Normalize();

		vector newPos = fp + delta * TFA_Constants.TELEPORT_EXTRA;
		newPos[1] = GetGame().SurfaceY(newPos[0], newPos[2]);
		player.SetPosition(newPos);
	}
}
