modded class SCR_XPHandlerComponent
{
	//------------------------------------------------------------------------------------------------
	override void OnControllableDestroyed(notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnControllableDestroyed(instigatorContextData);

		// Handle XP for kills of players
		if (instigatorContextData.GetInstigator().GetInstigatorType() != InstigatorType.INSTIGATOR_PLAYER)
			return;
		
		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForXP())
		{	
			vanilla.OnControllableDestroyed(instigatorContextData);
			return;
		}
		
		warCrimeComp.EvaluateKillLegalityXP(instigatorContextData, this);
	}

}

modded enum SCR_EXPRewards
{
	NON_COMBATANT_KILL,
	HORS_DE_COMBAT_KILL,
	PERFIDY,
	
	PROTECT_NON_COMBATANT,
	ABANDON_NON_COMBATANT,
	DISCHARGE_NON_COMBATANT,
	CAPTURE_PRISONER,
	ABANDON_PRISONER,
	DISCHARGE_PRISONER,
}
