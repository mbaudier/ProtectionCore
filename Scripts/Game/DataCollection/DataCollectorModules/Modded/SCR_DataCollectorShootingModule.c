//------------------------------------------------------------------------------------------------
//! Delegates gathering of kills and war crimes statistics to the war crimes game component.
[BaseContainerProps()]
modded class SCR_DataCollectorShootingModule
{

	//------------------------------------------------------------------------------------------------
	override void OnAIKilled(IEntity AIEntity, IEntity killerEntity, notnull Instigator instigator, notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnAIKilled(AIEntity, killerEntity, instigator, instigatorContextData);

		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForStatistics())
		{
			vanilla.OnAIKilled(AIEntity, killerEntity, instigator, instigatorContextData);
			return;
		}

		SCR_PlayerData killerData = NULL;
		if (instigator.GetInstigatorType() == InstigatorType.INSTIGATOR_PLAYER)
		{	
			if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER | SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER))
			{		
				int killerId = instigator.GetInstigatorPlayerID();	
				killerData = GetGame().GetDataCollector().GetPlayerData(killerId);
			}
		}
		
		warCrimeComp.EvaluateKillLegalityStats(instigatorContextData, killerData);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator instigator, notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, instigator, instigatorContextData);

		ARGEO_WarCrimesComponent warCrimeComp = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimeComp || warCrimeComp.UseVanillaLogicForStatistics())
		{
			vanilla.OnPlayerKilled(playerId, playerEntity, killerEntity, instigator, instigatorContextData);
			return;
		}
		
		m_mTrackedPossibleShooters.Remove(playerId);

		SCR_PlayerData playerData = GetGame().GetDataCollector().GetPlayerData(playerId);
		playerData.AddStat(SCR_EDataStats.DEATHS);
		
		SCR_PlayerData killerData = NULL;
		if (instigator.GetInstigatorType() == InstigatorType.INSTIGATOR_PLAYER)
		{	
			if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER | SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER))
			{		
				int killerId = instigator.GetInstigatorPlayerID();	
				killerData = GetGame().GetDataCollector().GetPlayerData(killerId);
			}
		}
		
		warCrimeComp.EvaluateKillLegalityStats(instigatorContextData, killerData);
	}
	
};
