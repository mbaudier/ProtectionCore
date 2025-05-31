[BaseContainerProps()]
modded class SCR_DataCollectorShootingModule
{

	override void OnAIKilled(IEntity AIEntity, IEntity killerEntity, notnull Instigator instigator, notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnAIKilled(AIEntity, killerEntity, instigator, instigatorContextData);

		SCR_PlayerData killerData = NULL;
		if (instigator.GetInstigatorType() == InstigatorType.INSTIGATOR_PLAYER)
		{	
			if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER | SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER))
			{		
				int killerId = instigator.GetInstigatorPlayerID();	
				killerData = GetGame().GetDataCollector().GetPlayerData(killerId);
			}
		}
		
		IEntity victimEntity = AIEntity;
		evaluateLegalCrime(instigatorContextData, victimEntity, true, killerData);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator instigator, notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, instigator, instigatorContextData);
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
		
		IEntity victimEntity = playerEntity;
		
		SCR_ECharacterControlType victimControlType = instigatorContextData.GetVictimCharacterControlType();
		bool isVictimAI = (victimControlType == SCR_ECharacterControlType.POSSESSED_AI);

		evaluateLegalCrime(instigatorContextData, victimEntity, isVictimAI, killerData);
	}
	
	protected bool evaluateLegalCrime(notnull SCR_InstigatorContextData instigatorContextData, IEntity victimEntity, bool isVictimAI, SCR_PlayerData killerData)
	{
		ARGEO_WarCrimesComponent warCrimesComponent = ARGEO_WarCrimesComponent.GetInstance();
		
		bool isLegalKill = true;
		
		if(warCrimesComponent.IsProtected(victimEntity))
		{	
			isLegalKill = false;
			if(killerData)
				if(isVictimAI)
					killerData.AddStat(SCR_EDataStats.PROTECTED_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.PROTECTED_KILLS);
		}
		else if(warCrimesComponent.IsHorsDecombat(victimEntity) && warCrimesComponent.IsKillingHorsDeCombatWarCrime())
		{	
			isLegalKill = false;
			if(killerData)
				if(isVictimAI)
					killerData.AddStat(SCR_EDataStats.HDC_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.HDC_KILLS);
		}

		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER))
		{
			if (instigatorContextData.IsEnemyKillPunished(SCR_EDisguisedKillingPunishment.WARCRIME))
			{ 
				isLegalKill = false;
				// TODO check faction in order to add proper stat
			}			
		}
		else // friendly fire
		{
			if (instigatorContextData.DoesPlayerKillCountAsTeamKill(true, true))
			{
				isLegalKill = false;
				if(killerData)
					if(isVictimAI)
						killerData.AddStat(SCR_EDataStats.ALLIED_AI_KILLS);
					else
						killerData.AddStat(SCR_EDataStats.ALLIED_KILLS);
			}
			else
			{
				return true; // ignore friendly kill
			}
		}
		
		if(isLegalKill)
		{	
			if(killerData)
			{
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.LEGAL_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.LEGAL_KILLS);
				
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.KILLS);
			}
		}
		else
		{
			if(killerData)
			{
				if(warCrimesComponent.TreatAllWarCrimesAsFriendlyKills())
				{
					if (isVictimAI)
						killerData.AddStat(SCR_EDataStats.FRIENDLY_AI_KILLS);
					else
						killerData.AddStat(SCR_EDataStats.FRIENDLY_KILLS);
				}
			}
		}
		
		return isLegalKill;		
	}
};
