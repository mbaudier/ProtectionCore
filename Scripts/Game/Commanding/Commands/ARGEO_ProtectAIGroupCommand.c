//------------------------------------------------------------------------------------------------
//! Command for protecting a non-combatant AI or group of AIs.
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_ProtectAIGroupCommand : ARGEO_BaseAddAIGroupCommand
{
	//------------------------------------------------------------------------------------------------
	override bool IsFeatureEnabled()
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		ARGEO_ProtectionFactionManagerComponent protectionFactionManagerComponent = ARGEO_ProtectionFactionManagerComponent.Cast(factionManager.FindComponent(ARGEO_ProtectionFactionManagerComponent));
		return protectionFactionManagerComponent && protectionFactionManagerComponent.CanNonCombattantsBeProtected();
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		if (!controlledEntityFaction.IsMilitary())
			return false;
		
		if (faction.IsMilitary())
			return false;
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownForCharacter(notnull SCR_ChimeraCharacter targetCharacter)
	{
		ARGEO_WarCrimesComponent warCrimesComp = ARGEO_WarCrimesComponent.GetInstance();
		if (warCrimesComp)
			return warCrimesComp.IsNonCombatant(targetCharacter);
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PostRecruitment(int playerID, int count)
	{
		SCR_XPHandlerComponent xpComp = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));
		if (xpComp)
			xpComp.AwardXP(playerID, SCR_EXPRewards.PROTECT_NON_COMBATANT);
	}
}