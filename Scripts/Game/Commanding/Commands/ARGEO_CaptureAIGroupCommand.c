[BaseContainerProps()]
class ARGEO_CaptureAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	override bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		if (faction.IsMilitary() && controlledEntityFaction.IsFactionEnemy(faction))
			return true;
		else
			return false;
	}
	
	override void PostRecruitment(int count)
	{
		if (count == 1)
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Enemy captured", "Captured", 3.0);	
		else	
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Group of enemies captured", "Captured", 3.0);	
	}
}