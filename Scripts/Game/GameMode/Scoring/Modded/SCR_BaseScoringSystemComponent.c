modded class SCR_BaseScoringSystemComponent
{
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	private void RpcDo_AddNonCombatantKill(int playerId, int factionIdx, int count)
	{
		SCR_ScoreInfo playerScore = m_mPlayerScores[playerId];
		playerScore.m_iNonCombatantKills += count;
		OnPlayerScoreChanged(playerId, playerScore);

		Faction faction = GetFactionByIndex(factionIdx);
		if (faction)
		{
			SCR_ScoreInfo factionScore = m_mFactionScores[faction];
			factionScore.m_iNonCombatantKills += count;
			OnFactionScoreChanged(faction, factionScore);
		}
	}

	void AddNonCombatantKill(int playerId, int count = 1)
	{
		// Server only
		if (!m_pGameMode.IsMaster())
			return;

		int factionIdx = GetPlayerFactionIndex(playerId);
		RpcDo_AddNonCombatantKill(playerId, factionIdx, count);
		Rpc(RpcDo_AddTeamKill, playerId, factionIdx, count);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	private void RpcDo_AddHorsDeCombatKill(int playerId, int factionIdx, int count)
	{
		SCR_ScoreInfo playerScore = m_mPlayerScores[playerId];
		playerScore.m_iHorsDeCombatKills += count;
		OnPlayerScoreChanged(playerId, playerScore);

		Faction faction = GetFactionByIndex(factionIdx);
		if (faction)
		{
			SCR_ScoreInfo factionScore = m_mFactionScores[faction];
			factionScore.m_iHorsDeCombatKills += count;
			OnFactionScoreChanged(faction, factionScore);
		}
	}

	void AddHorsDeCombatKill(int playerId, int count = 1)
	{
		// Server only
		if (!m_pGameMode.IsMaster())
			return;

		int factionIdx = GetPlayerFactionIndex(playerId);
		RpcDo_AddHorsDeCombatKill(playerId, factionIdx, count);
		Rpc(RpcDo_AddTeamKill, playerId, factionIdx, count);
	}

}