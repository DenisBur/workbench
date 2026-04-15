modded class KitBase
{
	override bool CanBePlaced(Man player, vector position)
	{
		if (!super.CanBePlaced(player, position))
			return false;

		PlayerBase pb = PlayerBase.Cast(player);
		if (!pb || !pb.GetIdentity())
			return true;

		return TFA_Rules.MayBuildAt(pb, position);
	}

	override string CanBePlacedFailMessage(Man player, vector position)
	{
		if (!super.CanBePlaced(player, position))
			return super.CanBePlacedFailMessage(player, position);

		PlayerBase pb = PlayerBase.Cast(player);
		if (pb && pb.GetIdentity() && !TFA_Rules.MayBuildAt(pb, position))
			return "На территории чужого флага (300 м) нельзя размещать постройки.";

		return super.CanBePlacedFailMessage(player, position);
	}
}
