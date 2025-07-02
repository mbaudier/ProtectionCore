//------------------------------------------------------------------------------------------------
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_ProtectAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	override bool IsFeatureEnabled()
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		ARGEO_ProtectionFactionManagerComponent protectionFactionManagerComponent = ARGEO_ProtectionFactionManagerComponent.Cast(factionManager.FindComponent(ARGEO_ProtectionFactionManagerComponent));
		return protectionFactionManagerComponent && protectionFactionManagerComponent.CanNonCombattantsBeProtected();
	}

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
		return ARGEO_WarCrimesComponent.GetInstance().IsDisarmed(targetCharacter);
	}
	
	override void PostRecruitment(int count)
	{
		if (count == 1)
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Civilian protected", "Protected", 3.0);	
		else	
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Group of civilians protected", "Protected", 3.0);	
	}
}