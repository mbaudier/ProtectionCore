//------------------------------------------------------------------------------------------------
//! Delegates killing XP evaluation to the war crimes game component.
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

//------------------------------------------------------------------------------------------------
//! Extends and refines the XP rewards related to war crimes or commanding. 
modded enum SCR_EXPRewards
{
	NON_COMBATANT_KILL, //!< A non-combatant was killed.
	HORS_DE_COMBAT_KILL, //!< An hors de combat enemy was killed.
	PERFIDY, //!< Player was disguised as a protected faction.
	
	PROTECT_NON_COMBATANT, //!< A non-combatant was protected in a commanded group.
	ABANDON_NON_COMBATANT, //!< A non-combatant was abandoned after having been protected.
	DISCHARGE_NON_COMBATANT, //!< A non-combatant was properly discharged.
	CAPTURE_PRISONER, //!< An enemy was captured in a commanded group.
	ABANDON_PRISONER, //!< A prisoner was abandoned after having been captured.
	DISCHARGE_PRISONER, //!< A prisoner was properly discharged.
}
