modded class ActionDismantlePart
{
	override protected bool DismantleCondition(PlayerBase player, ActionTarget target, ItemBase item, bool camera_check)
	{
		if (!super.DismantleCondition(player, target, item, camera_check))
			return false;

		Object targetObject = target.GetObject();
		EntityAI targetEntity;
		if (!Class.CastTo(targetEntity, targetObject))
			return false;

		BaseBuildingBase bb = BaseBuildingBase.Cast(targetEntity);
		if (bb && player && !TFA_Rules.MayDismantleWithTool(player, bb.GetPosition()))
			return false;

		return true;
	}
}
