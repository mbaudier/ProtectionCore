[BaseContainerProps()]
modded class SCR_DataCollectorCrimesModule
{
	[Attribute("2", UIWidgets.Slider, desc: "Negative proportionality points for killing a non-combatant human-controlled soldier")]
	protected float MODIFIER_PROPORTIONALITY_PROTECTED_KILLS;
	
	[Attribute("1", UIWidgets.Slider, desc: "Negative proportionality points for killing a non-combatant AI-controlled soldier")]
	protected float MODIFIER_PROPORTIONALITY_PROTECTED_AI_KILLS;

	[Attribute("2", UIWidgets.Slider, desc: "Negative proportionality points for killing an hors de combat human-controlled soldier")]
	protected float MODIFIER_PROPORTIONALITY_HDC_KILLS;
	
	[Attribute("1", UIWidgets.Slider, desc: "Negative proportionality points for killing an hors de combat AI-controlled soldier")]
	protected float MODIFIER_PROPORTIONALITY_HDC_AI_KILLS;

	[Attribute("1", UIWidgets.Slider, desc: "Negative proportionality points for killing while disguised as an enemy")]
	protected float MODIFIER_PROPORTIONALITY_DISGUISED_KILLS;

	[Attribute("2", UIWidgets.Slider, desc: "Negative proportionality points for perfidy (killing while disguised as a non-combatant)")]
	protected float MODIFIER_PROPORTIONALITY_PERFIDY_KILLS;

	// Fix typo in vanilla description
	[Attribute("1.5", UIWidgets.Slider, desc: "Positive proportionality points for killing an enemy AI-controlled soldier")]
	protected float MODIFIER_PROPORTIONALITY_AI_KILLS;
	
	//------------------------------------------------------------------------------------------------
	//! First decrease the score up until now by doing Time * DecreasePerMinute
	//! Second add to the score given the current crimes: PointsOfCrime * CRIME_ACCELERATION_FACTOR
	override void EvaluatePlayerCrimes(int playerId, bool evaluatePunishment = true)
	{
		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForStatistics())
		{
			vanilla.EvaluatePlayerCrimes(playerId, evaluatePunishment);
			return;
		}
		
		SCR_PlayerData playerData = GetGame().GetDataCollector().GetPlayerData(playerId, false);
		if (!playerData)
			return;

		//Calculate decreasing of criminal score based on time lapsed
		float decreasingOfScore = (System.GetTickCount() - playerData.GetLatestCriminalScoreUpdateTick()) * 0.001 * SCR_PlayerDataConfigs.GetInstance().GetScoreDecreasePerSecond();

		///////////////////////////////////////////
		// Analsing warcrime_harming_friendlies: //
		///////////////////////////////////////////

		array<SCR_ECrimeNotification> crimeNotifications = {};
		float warCrimesScore = EvaluateWarCrimePoints(playerData, crimeNotifications) * playerData.GetStat(SCR_EDataStats.CRIME_ACCELERATION);
		
		///////////////////////////////
		// Done analysing war crimes //
		///////////////////////////////

		//Process temporal stats
		ProcessTemporalStats(playerData);

		//Update score
		UpdateCriminalScore(playerData, warCrimesScore, decreasingOfScore);
		
		float currentScore = playerData.GetCriminalScore();
		
		if (warCrimesScore > 0)
			Print("Player with id " + playerId + " got " + warCrimesScore + " more criminal points. Current criminal score is " + currentScore, LogLevel.DEBUG);
		else
			return;

		//Kick or ban players
		if (evaluatePunishment && currentScore >= SCR_PlayerDataConfigs.GetInstance().GetScoreThreshold())
		{
			//Logic for kicking player
			KickPlayer(playerId, playerData, SCR_PlayerManagerKickReason.FRIENDLY_FIRE);
			return;
		}

		//Send educational hints to clients
		SendWarCrimesHints(playerId, crimeNotifications, currentScore);
	}
	
	protected float EvaluateWarCrimePoints(notnull SCR_PlayerData playerData, out array<SCR_ECrimeNotification> crimeNotifications)
	{
		array<float> accumulatedActions = playerData.GetAccumulatedActions();
		
		//Kills
		float harmingAlliesPoints = accumulatedActions[SCR_EDataStats.ALLIED_KILLS] * MODIFIER_PROPORTIONALITY_FRIENDLY_KILLS
		 + accumulatedActions[SCR_EDataStats.ALLIED_AI_KILLS] * MODIFIER_PROPORTIONALITY_FRIENDLY_AI_KILLS;		
		
		float harmingProtectedPoints = accumulatedActions[SCR_EDataStats.PROTECTED_KILLS] * MODIFIER_PROPORTIONALITY_PROTECTED_KILLS
		 + accumulatedActions[SCR_EDataStats.PROTECTED_AI_KILLS] * MODIFIER_PROPORTIONALITY_PROTECTED_AI_KILLS;
		
		float harmingHdcPoints = accumulatedActions[SCR_EDataStats.HDC_KILLS] * MODIFIER_PROPORTIONALITY_HDC_KILLS
		 + accumulatedActions[SCR_EDataStats.HDC_AI_KILLS] * MODIFIER_PROPORTIONALITY_HDC_AI_KILLS;

		//Disguised
		float disguisedPoints = accumulatedActions[SCR_EDataStats.DISGUISED_KILLER] * MODIFIER_PROPORTIONALITY_DISGUISED_KILLS;
		float perfidyPoints = accumulatedActions[SCR_EDataStats.PERFIDY_KILLER] * MODIFIER_PROPORTIONALITY_PERFIDY_KILLS;

		float warCrimesPoints = harmingAlliesPoints + harmingProtectedPoints + harmingHdcPoints + disguisedPoints + perfidyPoints;
				
		//Proportionality		
		float proportionalityPoints = 0;
		if (m_bWarCrimesProportionalityPrincipleEnabled)
			proportionalityPoints = accumulatedActions[SCR_EDataStats.LEGAL_KILLS] * MODIFIER_PROPORTIONALITY_KILLS + accumulatedActions[SCR_EDataStats.LEGAL_AI_KILLS] * MODIFIER_PROPORTIONALITY_AI_KILLS;

		if (warCrimesPoints <= proportionalityPoints)
			return 0;//Actions were proportional

		if (harmingAlliesPoints > 0)
		{
			playerData.AddStat(SCR_EDataStats.WARCRIME_HARMING_FRIENDLIES,
			 harmingAlliesPoints - (proportionalityPoints * harmingAlliesPoints/warCrimesPoints), false);
			crimeNotifications.Insert(SCR_ECrimeNotification.TEAMKILL);
		}
		
		if (harmingProtectedPoints > 0)
		{
			playerData.AddStat(SCR_EDataStats.WARCRIME_HARMING_PROTECTED,
			 harmingProtectedPoints - (proportionalityPoints * harmingProtectedPoints/warCrimesPoints), false);
			crimeNotifications.Insert(SCR_ECrimeNotification.NON_COMBATANT_KILL);
		}
		
		if (harmingHdcPoints > 0)
		{
			playerData.AddStat(SCR_EDataStats.WARCRIME_HARMING_HDC,
			 harmingHdcPoints - (proportionalityPoints * harmingHdcPoints/warCrimesPoints), false);
			crimeNotifications.Insert(SCR_ECrimeNotification.HORS_DE_COMBAT_KILL);
		}

		if (disguisedPoints > 0)
		{
			playerData.AddStat(SCR_EDataStats.WARCRIME_DISGUISED,
			 disguisedPoints - (proportionalityPoints * disguisedPoints/warCrimesPoints), false);
			crimeNotifications.Insert(SCR_ECrimeNotification.KILLING_WHILE_DISGUISED);
		}

		if (perfidyPoints > 0)
		{
			playerData.AddStat(SCR_EDataStats.WARCRIME_PERFIDY,
			 perfidyPoints - (proportionalityPoints * perfidyPoints/warCrimesPoints), false);
			crimeNotifications.Insert(SCR_ECrimeNotification.PERFIDY);
		}

		//All war crimes
		playerData.AddStat(SCR_EDataStats.WARCRIMES, warCrimesPoints - proportionalityPoints, false);

		//Single friendly kill or multiple friendly kills?
		float PointsOfCrime;

		if (accumulatedActions[SCR_EDataStats.ALLIED_KILLS]
		 + accumulatedActions[SCR_EDataStats.ALLIED_AI_KILLS]
		 + accumulatedActions[SCR_EDataStats.PROTECTED_KILLS]
		 + accumulatedActions[SCR_EDataStats.PROTECTED_AI_KILLS]
		 + accumulatedActions[SCR_EDataStats.HDC_KILLS]
		 + accumulatedActions[SCR_EDataStats.HDC_AI_KILLS]
		 == 1)
			PointsOfCrime = SCR_PlayerDataConfigs.GetInstance().GetCrimePointsFriendlyKill();
		else
			PointsOfCrime = SCR_PlayerDataConfigs.GetInstance().GetCrimePointsFriendlyMultiKill();

		return PointsOfCrime * (warCrimesPoints - proportionalityPoints);
	}
	
	void SendWarCrimesHints(int playerId, array<SCR_ECrimeNotification> crimeNotifications, float criminalScore)
	{
		if (playerId <= 0)
			return;

		if (criminalScore > 0)
		{
			IEntity playerController = GetGame().GetPlayerManager().GetPlayerController(playerId);
			if (!playerController)
				return;

			ARGEO_WarCrimesHintComponent hintsComp = ARGEO_WarCrimesHintComponent.Cast(playerController.FindComponent(ARGEO_WarCrimesHintComponent));
			if (!hintsComp)
				return;

			hintsComp.NotifyClientCriminalScoreIncreased(crimeNotifications, criminalScore);
		}
	}
}
