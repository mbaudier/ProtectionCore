//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_RecruitAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	override bool IsFeatureEnabled()
	{
		return true;
	}

	override bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		if(faction == controlledEntityFaction)
			return true; // non-modded logic
		
		if (!faction.IsMilitary())
			return false;
		
		if (controlledEntityFaction.IsFactionEnemy(faction))
			return false;
		
		FactionManager factionManager = GetGame().GetFactionManager();
		ARGEO_ProtectionFactionManagerComponent protectionFactionManagerComponent = ARGEO_ProtectionFactionManagerComponent.Cast(factionManager.FindComponent(ARGEO_ProtectionFactionManagerComponent));
		return protectionFactionManagerComponent && protectionFactionManagerComponent.CanAlliesCanBeRecruited();
	}

	override bool IsProtecting()
	{
		return false;
	}
	
}