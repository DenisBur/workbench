class TFA_Game
{
	static PlayerBase PlayerFromIdentity(PlayerIdentity id)
	{
		if (!id)
			return NULL;

		string sid = id.GetId();
		array<Man> arr = new array<Man>;
		GetGame().GetPlayers(arr);

		for (int i = 0; i < arr.Count(); i++)
		{
			PlayerBase pb = PlayerBase.Cast(arr[i]);
			if (pb && pb.GetIdentity() && pb.GetIdentity().GetId() == sid)
				return pb;
		}
		return NULL;
	}

	static bool IsNearObject(PlayerBase p, Object o, float dist)
	{
		if (!p || !o)
			return false;
		return vector.Distance(p.GetPosition(), o.GetPosition()) <= dist;
	}
}
