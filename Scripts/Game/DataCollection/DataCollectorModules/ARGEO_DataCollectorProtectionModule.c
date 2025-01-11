[BaseContainerProps()]
class ARGEO_DataCollectorProtectionModule : SCR_DataCollectorModule
{

	override void OnAIKilled(IEntity AIEntity, IEntity killerEntity, notnull Instigator instigator, notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnAIKilled(AIEntity, killerEntity, instigator, instigatorContextData);
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(AIEntity);
		CharacterWeaponManagerComponent weaponManager = CharacterWeaponManagerComponent.Cast(character.FindComponent(CharacterWeaponManagerComponent));
		WeaponSlotComponent weapon = weaponManager.GetCurrentSlot();
		bool unarmed = !weapon;
		if(unarmed)
		{
			Print("Killing an unarmed AI");
		}
		else
		{
			Print("Weapon slot type "+weapon.GetWeaponSlotType());
		}
		
		//This method adds a kill no matter the mean by which the AI was killed.
		//The name of the module is a little bit misleading		
		if (instigator.GetInstigatorType() != InstigatorType.INSTIGATOR_PLAYER)
			return;
		
		//~ Not a player kill so ignore (Like suicide)
		if (!instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER | SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER))
			return;
		
		int killerId = instigator.GetInstigatorPlayerID();

		SCR_PlayerData killerData = GetGame().GetDataCollector().GetPlayerData(killerId);
		if (!killerData)
			return;
		
		//~ Add an AI kill or friendly kill stats
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER))
		{
			killerData.AddStat(SCR_EDataStats.AI_KILLS);
			return;
		}
		else
		{
			//~ Friendly kills do not count for admins, GMs and possessed AI by GM
			SCR_ECharacterControlType killerControlType = instigatorContextData.GetKillerCharacterControlType();
			if (killerControlType == SCR_ECharacterControlType.UNLIMITED_EDITOR || killerControlType == SCR_ECharacterControlType.POSSESSED_AI)
				return;
			
			//~ Friendly kills only counted if friendly fire is punished else the player can rank up a massive crime score
			SCR_AdditionalGameModeSettingsComponent additionalGameModeSettings = SCR_AdditionalGameModeSettingsComponent.GetInstance();
			if (additionalGameModeSettings && !additionalGameModeSettings.IsTeamKillingPunished())
				return;
				
			killerData.AddStat(SCR_EDataStats.FRIENDLY_AI_KILLS);
			return;
		}	
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator instigator, notnull SCR_InstigatorContextData instigatorContextData)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, instigator, instigatorContextData);

		SCR_PlayerData playerData = GetGame().GetDataCollector().GetPlayerData(playerId);
		playerData.AddStat(SCR_EDataStats.DEATHS);
		
		//~ Also tracks Possessed AI as if player kill as the data is not seen by player
		if (instigator.GetInstigatorType() != InstigatorType.INSTIGATOR_PLAYER)
			return;
		
		//~ Not a player kill so ignore (Like suicide)
		if (!instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER | SCR_ECharacterDeathStatusRelations.KILLED_BY_FRIENDLY_PLAYER))
			return;
		
		int killerId = instigator.GetInstigatorPlayerID();
		
		SCR_PlayerData killerData = GetGame().GetDataCollector().GetPlayerData(killerId);
		if (!killerData)
			return;
		
		SCR_ECharacterControlType victimControlType = instigatorContextData.GetVictimCharacterControlType();
		
		//~ Possessed AI count towards AI kills
		if (victimControlType == SCR_ECharacterControlType.POSSESSED_AI)
		{
			//~ Add an AI kill or friendly kill stats
			if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER))
			{
				killerData.AddStat(SCR_EDataStats.AI_KILLS);
				return;
			}
			else
			{
				//~ Friendly kills do not count for admins, GMs and possessed AI by GM
				SCR_ECharacterControlType killerControlType = instigatorContextData.GetKillerCharacterControlType();
				if (killerControlType == SCR_ECharacterControlType.UNLIMITED_EDITOR || killerControlType == SCR_ECharacterControlType.POSSESSED_AI)
					return;
				
				//~ Friendly kills only counted if friendly fire is punished else the player can rank up a massive crime score
				SCR_AdditionalGameModeSettingsComponent additionalGameModeSettings = SCR_AdditionalGameModeSettingsComponent.GetInstance();
				if (additionalGameModeSettings && !additionalGameModeSettings.IsTeamKillingPunished())
					return;
				
				killerData.AddStat(SCR_EDataStats.FRIENDLY_AI_KILLS);
				return;
			}
		}
		
		//~ Add kill or friendly kill stats
		if (instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER))
		{
			killerData.AddStat(SCR_EDataStats.KILLS);
			return;
		}
		else
		{
			//~ Friendly kills do not count for admins, GMs and possessed AI by GM
			SCR_ECharacterControlType killerControlType = instigatorContextData.GetKillerCharacterControlType();
			if (killerControlType == SCR_ECharacterControlType.UNLIMITED_EDITOR || killerControlType == SCR_ECharacterControlType.POSSESSED_AI)
				return;
			
			//~ Friendly kills only counted if friendly fire is punished else the player can rank up a massive crime score
			SCR_AdditionalGameModeSettingsComponent additionalGameModeSettings = SCR_AdditionalGameModeSettingsComponent.GetInstance();
			if (additionalGameModeSettings && !additionalGameModeSettings.IsTeamKillingPunished())
				return;
			
			killerData.AddStat(SCR_EDataStats.FRIENDLY_KILLS);
			return;
		}
			
	}

	//------------------------------------------------------------------------------------------------
	override void Update(float timeTick)
	{
		//If there's no data collector, do nothing
		if (!GetGame().GetDataCollector())
			return;

		m_fTimeSinceUpdate += timeTick;

		if (m_fTimeSinceUpdate < m_fUpdatePeriod)
			return;

		SCR_PlayerData playerData;
		int playerId;

		m_fTimeSinceUpdate = 0;
	}

#ifdef ENABLE_DIAGX
	//------------------------------------------------------------------------------------------------
	override void CreateVisualization()
	{
		super.CreateVisualization();
		if (!m_StatsVisualization)
			return;

		CreateEntry("Deaths: ", 0, SCR_EShootingModuleStats.DEATHS);
		CreateEntry("Player Kills: ", 0, SCR_EShootingModuleStats.PLAYERKILLS);
		CreateEntry("AI Kills: ", 0, SCR_EShootingModuleStats.AIKILLS);
		CreateEntry("Friendly Player Kills: ", 0, SCR_EShootingModuleStats.FRIENDLYPLAYERKILLS);
		CreateEntry("Friendly AI Kills: ", 0, SCR_EShootingModuleStats.FRIENDLYAIKILLS);
		CreateEntry("Bullets Shot: ", 0, SCR_EShootingModuleStats.BULLETSSHOT);
		CreateEntry("Grenades Thrown: ", 0, SCR_EShootingModuleStats.GRENADESTHROWN);
	}
#endif
};

#ifdef ENABLE_DIAGX
enum SCR_EShootingModuleStats
{
	DEATHS,
	PLAYERKILLS,
	AIKILLS,
	FRIENDLYPLAYERKILLS,
	FRIENDLYAIKILLS,
	BULLETSSHOT,
	GRENADESTHROWN
};
#endif
