//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ARGEO_ProtectAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	override bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		if (faction.IsMilitary())
			return false;
		else
			return true;
	}
	
	override void PostRecruitment(int count)
	{
		if (count == 1)
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Civilian protected", "Protected", 3.0);	
		else	
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Group of civilians protected", "Protected", 3.0);	
	}
}