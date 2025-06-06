//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_RecruitAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	override bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		if (faction.IsMilitary() && controlledEntityFaction.IsFactionEnemy(faction))
			return false;
		else
			return faction.IsMilitary();
	}

	override bool IsProtecting()
	{
		return false;
	}
	
}