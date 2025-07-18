modded class SCR_ScoringSystemComponent
{
	[Attribute("-2", UIWidgets.EditBox, "Non-combatant kill score multiplier", category: "Scoring: Multipliers")]
	protected int m_iNonCombatantKillScoreMultiplier;

	[Attribute("-1", UIWidgets.EditBox, "Hors de combat kill score multiplier", category: "Scoring: Multipliers")]
	protected int m_iHorsDeCombatKillScoreMultiplier;

	protected override int CalculateScore(SCR_ScoreInfo info)
	{
		int score = vanilla.CalculateScore(info);
		
		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForScoring())
		{
			return score;
		}

		score = score +
				info.m_iNonCombatantKills 	* m_iNonCombatantKillScoreMultiplier +
				info.m_iHorsDeCombatKills	* m_iHorsDeCombatKillScoreMultiplier;
		
		if (score < 0)
			return 0;
		
		return score;
	}

	protected override void OnPlayerKilled(notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilled(instigatorContextData);

		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForScoring())
		{
			vanilla.OnPlayerKilled(instigatorContextData);
			return;
		}
		
		// Add death no matter what
		AddDeath(instigatorContextData.GetVictimPlayerID());
		
		Instigator instigator = instigatorContextData.GetInstigator();
		
		//~ Killed by AI
		if (instigator.GetInstigatorType() != InstigatorType.INSTIGATOR_PLAYER)
			return;
		
		SCR_ECharacterControlType victimControlType = instigatorContextData.GetVictimCharacterControlType();
		SCR_ECharacterControlType killerControlType = instigatorContextData.GetKillerCharacterControlType();
		
		//~ Score for killing self, killing enemy player and killing friendly player
		//~ Player killed self
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.SUICIDE))
		{
			//~ Possessed AI do not count the suicide
			if (victimControlType == SCR_ECharacterControlType.POSSESSED_AI)
				return;
			
			AddSuicide(instigatorContextData.GetVictimPlayerID());
			return;
		}
		
		warCrimeComp.EvaluateKillLegalityScoring(instigatorContextData, this);
	}

	protected override void OnControllableDestroyed(notnull SCR_InstigatorContextData instigatorContextData)
	{
		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForScoring())
		{
			vanilla.OnControllableDestroyed(instigatorContextData);
			return;
		}
		
		Instigator instigator = instigatorContextData.GetInstigator();
		
		//~ Killed by AI
		if (instigator.GetInstigatorType() != InstigatorType.INSTIGATOR_PLAYER)
			return;
		
		// hors de combat not considered for AI since their kills are not considered either
		if(warCrimeComp.IsNonCombatant(instigatorContextData.GetVictimEntity()))
		{
			warCrimeComp.EvaluateKillLegalityScoring(instigatorContextData, this);
		}
		
	}
}