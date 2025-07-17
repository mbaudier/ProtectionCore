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
		EvaluateLegalCrime(instigatorContextData, victimEntity, killerEntity, true, killerData);
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

		EvaluateLegalCrime(instigatorContextData, victimEntity, killerEntity, isVictimAI, killerData);
	}
	
	protected bool EvaluateLegalCrime(notnull SCR_InstigatorContextData instigatorContextData, IEntity victimEntity, IEntity killerEntity, bool isVictimAI, SCR_PlayerData killerData)
	{
		// Note: killerData != null is the marker for player-specific logic
		
		ARGEO_WarCrimesComponent warCrimesComponent = ARGEO_WarCrimesComponent.GetInstance();
		if (!warCrimesComponent)
			return true; // TODO call vanilla logic?
		
		bool killedByEnemy = instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER)
		 || instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_AI);
		
		// we assume legal kill, then systematically check potential crimes
		bool isLegalKill = true;

		// check non-combatant killed		
		if (warCrimesComponent.IsProtected(victimEntity))
		{	
			isLegalKill = false;
			warCrimesComponent.RegisterWarCrime(SCR_ECrimeNotification.NON_COMBATANT_KILL, instigatorContextData);
			
			if (killerData)
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.PROTECTED_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.PROTECTED_KILLS);
		}
		// check hors de combat killed
		else if(killedByEnemy
		 && warCrimesComponent.IsHorsDecombat(victimEntity)
		 && warCrimesComponent.IsKillingHorsDeCombatWarCrime())
		{	
			isLegalKill = false;
			if (killerData)
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.HDC_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.HDC_KILLS);
		}

		// check disguised or perfidy
		if (killedByEnemy)
		{
			SCR_PerceivedFactionManagerComponent perceivedFactionManager = SCR_PerceivedFactionManagerComponent.GetInstance();
			SCR_CharacterFactionAffiliationComponent killerFactionAffiliation = SCR_CharacterFactionAffiliationComponent.Cast(killerEntity.FindComponent(SCR_CharacterFactionAffiliationComponent));
			if (perceivedFactionManager 
			 && perceivedFactionManager.GetCharacterPerceivedFactionOutfitType() != SCR_EPerceivedFactionOutfitType.DISABLED
			 && killerFactionAffiliation)
			{
				if (killerFactionAffiliation.HasPerceivedFaction()
				 && SCR_Enum.HasPartialFlag(perceivedFactionManager.GetPunishmentKillingWhileDisguisedFlags(), SCR_EDisguisedKillingPunishment.WARCRIME))
				{
					if (instigatorContextData.GetKillerDisguiseType() == SCR_ECharacterDisguiseType.HOSTILE_FACTION)
					{
						if (warCrimesComponent.IsKillingWhileDisguisedWarCrime())
						{
							isLegalKill = false;
							if (killerData)
								killerData.AddStat(SCR_EDataStats.DISGUISED_KILLER);
						}
					}
					else
					{
						bool isPerfidy = false;
						SCR_Faction killerPerceivedFaction = SCR_Faction.Cast(killerFactionAffiliation.GetPerceivedFaction());
						if (killerPerceivedFaction)
						{
							if (!killerPerceivedFaction.IsMilitary())
								isPerfidy = true;
						}
						else
						{
							isPerfidy = instigatorContextData.GetKillerDisguiseType() != SCR_ECharacterDisguiseType.DEFAULT_FACTION;
						}
						
						if (isPerfidy && warCrimesComponent.IsPerfidyWarCrime()
						)
						{
							isLegalKill = false;
							if (killerData)
								killerData.AddStat(SCR_EDataStats.PERFIDY_KILLER);
						}
					}
					
				}
			}
		
//			if (instigatorContextData.IsEnemyKillPunished(SCR_EDisguisedKillingPunishment.WARCRIME))
//			{ 
//				isLegalKill = false;
//			}			
		}
		
		// check friendly fire
		if (!killedByEnemy)
		{
			if (!warCrimesComponent.IsProtected(victimEntity) // already checked, do not count twice
				&& instigatorContextData.DoesPlayerKillCountAsTeamKill(true, true))
			{
				isLegalKill = false;
				if (killerData)
					if (isVictimAI)
						killerData.AddStat(SCR_EDataStats.ALLIED_AI_KILLS);
					else
						killerData.AddStat(SCR_EDataStats.ALLIED_KILLS);
			}
			else
			{
				 // completely ignore friendly kill, no stats will be gathered
				return true;
			}
		}
		
		if (isLegalKill)
		{	
			if (killerData)
			{
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.LEGAL_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.LEGAL_KILLS);
				
			}
		}

		// compatibility with vanilla
		if (warCrimesComponent.TreatAllWarCrimesAsFriendlyKills() && killerData)
		{
			if (isLegalKill)
			{	
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.KILLS);
			}
			else
			{
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.FRIENDLY_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.FRIENDLY_KILLS);
			}
		}
		
		return isLegalKill;		
	}
};
