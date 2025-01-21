[BaseContainerProps()]
class ARGEO_DataCollectorProtectionCrimesModule : SCR_DataCollectorCrimesModule
{
	override protected float EvaluateHarmingFriendlies(notnull SCR_PlayerData playerData)
	{
		// Instead of overriding EvaluatePlayerCrimes, we use the call to EvaluateHarmingFriendlies in orde rto extend war crimes
		//super.EvaluateHarmingFriendlies(playerData);
		
		array<float> accumulatedActions = playerData.GetAccumulatedActions();
		
		float harmingUnarmedPoints = accumulatedActions[SCR_EDataStats.UNARMED_KILLS] * MODIFIER_PROPORTIONALITY_FRIENDLY_KILLS + accumulatedActions[SCR_EDataStats.UNARMED_AI_KILLS] * MODIFIER_PROPORTIONALITY_FRIENDLY_AI_KILLS;
		float proportionalityPoints = 0;

		if (m_bWarCrimesProportionalityPrincipleEnabled)
			proportionalityPoints = accumulatedActions[SCR_EDataStats.KILLS] * MODIFIER_PROPORTIONALITY_KILLS + accumulatedActions[SCR_EDataStats.AI_KILLS] * MODIFIER_PROPORTIONALITY_AI_KILLS;

		if (harmingUnarmedPoints <= proportionalityPoints)
			return 0;

		playerData.AddStat(SCR_EDataStats.WARCRIME_HARMING_UNARMED, harmingUnarmedPoints - proportionalityPoints, false);
		playerData.AddStat(SCR_EDataStats.WARCRIMES, harmingUnarmedPoints - proportionalityPoints, false);

		//Single friendly kill or multiple friendly kills?
		float PointsOfCrime;

		// TODO configure properly
		if (accumulatedActions[SCR_EDataStats.UNARMED_KILLS] + accumulatedActions[SCR_EDataStats.UNARMED_AI_KILLS] == 1)
			PointsOfCrime = SCR_PlayerDataConfigs.GetInstance().GetCrimePointsFriendlyKill();
		else
			PointsOfCrime = SCR_PlayerDataConfigs.GetInstance().GetCrimePointsFriendlyMultiKill();

		return PointsOfCrime * (harmingUnarmedPoints - proportionalityPoints);
	}
}
