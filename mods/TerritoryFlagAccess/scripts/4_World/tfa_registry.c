//! Все активные флагштоки на сервере (для поиска зоны без полного сканирования карты).
class TFA_Registry
{
	protected static ref array<TerritoryFlag> s_Flags = new array<TerritoryFlag>;

	static void Register(TerritoryFlag f)
	{
		if (!f || s_Flags.Find(f) >= 0)
			return;
		s_Flags.Insert(f);
	}

	static void Unregister(TerritoryFlag f)
	{
		int idx = s_Flags.Find(f);
		if (idx >= 0)
			s_Flags.Remove(idx);
	}

	static TerritoryFlag FindNearestFlag(vector pos, float radius, out float distSq)
	{
		distSq = radius * radius;
		TerritoryFlag best;
		float bestD = distSq + 1;

		for (int i = 0; i < s_Flags.Count(); i++)
		{
			TerritoryFlag f = s_Flags[i];
			if (!f)
				continue;
			float d = vector.DistanceSq(f.GetPosition(), pos);
			if (d <= distSq && d < bestD)
			{
				bestD = d;
				best = f;
			}
		}

		distSq = bestD;
		return best;
	}
}
