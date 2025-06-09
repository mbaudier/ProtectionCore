//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_ProtectAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	override bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		if (!controlledEntityFaction.IsMilitary())
			return false;
		
		if (faction.IsMilitary())
			return false;
		
		return true;
	}

	override bool CanBeShownForCharacter(notnull SCR_ChimeraCharacter targetCharacter)
	{
		CharacterPerceivableComponent perceivableComp = CharacterPerceivableComponent.Cast(targetCharacter.FindComponent(CharacterPerceivableComponent));
		if (perceivableComp)
		{
			return perceivableComp.IsDisarmed();
		}
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