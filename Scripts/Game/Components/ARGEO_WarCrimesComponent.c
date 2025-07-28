[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton centralizing war crimes management.")]
class ARGEO_WarCrimesComponentClass : SCR_BaseGameModeComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Game component centralizing the evaluation of the legality of kills and their impact
//! on various statistics or scoring mechanisms.
class ARGEO_WarCrimesComponent : SCR_BaseGameModeComponent
{
	[Attribute("0", desc: "Killing an unarmed or wounded enemy is a war crime (ihl-databases.icrc.org - Rule 47).", category: "War Crimes")]
	protected bool m_bKillingHorsDeCombatIsWarCrime;

	[Attribute("0", desc: "Killing while disguised is war crime (ihl-databases.icrc.org - Rule 62).", category: "War Crimes")]
	protected bool m_bKillingWhileDisguisedIsWarCrime;

	[Attribute("0", desc: "Perfidy, that is killing while disguised as protected, is war crime (ihl-databases.icrc.org - Rule 65).", category: "War Crimes")]
	protected bool m_bPerfidyIsWarCrime;

	[Attribute("{5A45CA8948A1D825}Prefabs/Systems/WarCrimes/WarCrime_Base.et", desc: "The prefab that will be spawned when a war cime is committed, to be used in triggers or to investigate atrocities a posteriori.", category: "Integration")]
	protected ResourceName m_sWarCrimePrefab;

	[Attribute("1", desc: "All war crimes will also be considered as vanilla friendly kills in statistics, for compatibility with mods expecting that.", category: "Compatibility")]
	protected bool m_bTreatAllWarCrimesAsFriendlyKills;

	[Attribute("1", desc: "Career/kick statistics use vanilla logic.", category: "Compatibility")]
	protected bool m_bVanillaLogicForStatistics;

	[Attribute("1", desc: "XP rewards use vanilla logic.", category: "Compatibility")]
	protected bool m_bVanillaLogicForXP;

	[Attribute("1", desc: "Scoring use vanilla logic.", category: "Compatibility")]
	protected bool m_bVanillaLogicForScoring;

	[Attribute("1", desc: "Force playable factions to be friendly to non-military factions (ihl-databases.icrc.org - Rule 1).", category: "Consider Arma Reforger EULAs Before Changing This")]
	protected bool m_bPlayableFactionsFriendlyToNonMilitary;

	static const string EVENT_WAR_CRIME_CREATED = "OnWarCrimeCreated";

	protected static ARGEO_WarCrimesComponent s_Instance;
	
	protected ref array<EventHandlerManagerComponent> m_aEventBus = new array<EventHandlerManagerComponent>();
	
	//
	// LIFECYCLE
	//
	
	//------------------------------------------------------------------------------------------------
	override void OnGameModeStart()
	{
		if (!s_Instance)
			s_Instance = this;

		if (m_bPlayableFactionsFriendlyToNonMilitary)
		{
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			SCR_SortedArray<SCR_Faction> factions = new SCR_SortedArray<SCR_Faction>();
			factionManager.GetSortedFactionsList(factions);

			array<SCR_Faction> nonMilitary = new array<SCR_Faction>();
			array<SCR_Faction> playable = new array<SCR_Faction>();
			for (int i = 0; i < factions.Count(); i++)
			{		
				SCR_Faction f = factions.Get(i);
				if (!f.IsMilitary())
				{			
					nonMilitary.Insert(f);
				}
				if (f.IsPlayable())
				{			
					playable.Insert(f);
				}
			}
			
			foreach (SCR_Faction nm : nonMilitary)
			{
				foreach (SCR_Faction p : playable)
				{
					factionManager.SetFactionsFriendly(nm, p, -1, false);
				}
			}
			factionManager.RequestUpdateAllTargetsFactions();
		}
		
		// event bus
		array<Managed> arr = {};
		GetGame().GetGameMode().FindComponents(EventHandlerManagerComponent, arr);
		foreach (Managed m : arr)
		{
			m_aEventBus.Insert(EventHandlerManagerComponent.Cast(m));
		}
	}
	
	//
	// NOTIFICATIONS
	//
	
	//------------------------------------------------------------------------------------------------
	//! Spawn a war crime entity in the world, which can then be used by triggers or investigators.
	//! \param crime The type of war crime.
	//! \param instigatorContextData Information about the killer and the victim.
	void RegisterWarCrime(SCR_ECrimeNotification crime, notnull SCR_InstigatorContextData instigatorContextData)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = instigatorContextData.GetVictimEntity().GetOrigin();

		ARGEO_WarCrimeEntity warCrimeEntity = ARGEO_WarCrimeEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sWarCrimePrefab), null, params));
		if (!warCrimeEntity)
			return;
		
		warCrimeEntity.SetCrime(crime);

		// criminal
		FactionAffiliationComponent criminalFactionComp = FactionAffiliationComponent.Cast(instigatorContextData.GetKillerEntity().FindComponent(FactionAffiliationComponent));
		if (criminalFactionComp)
			warCrimeEntity.SetCriminalFactionKey(criminalFactionComp.GetAffiliatedFactionKey());		
		CharacterIdentityComponent criminalCharacterIdentity = CharacterIdentityComponent.Cast(instigatorContextData.GetKillerEntity().FindComponent(CharacterIdentityComponent));
		if (criminalCharacterIdentity)
			warCrimeEntity.SetCriminalIdentity(criminalCharacterIdentity.GetIdentity().GetSurname(), criminalCharacterIdentity.GetIdentity().GetName());
		
		// victim
		// we gather more identity information, as it will be happen only once
		FactionAffiliationComponent victimFactionComp = FactionAffiliationComponent.Cast(instigatorContextData.GetVictimEntity().FindComponent(FactionAffiliationComponent));
		if (victimFactionComp)
			warCrimeEntity.SetVictimFactionKey(victimFactionComp.GetAffiliatedFactionKey());
		CharacterIdentityComponent victimCharacterIdentity = CharacterIdentityComponent.Cast(instigatorContextData.GetVictimEntity().FindComponent(CharacterIdentityComponent));
		if (victimCharacterIdentity)
		{
			string surname = victimCharacterIdentity.GetIdentity().GetSurname();
			string givenName = victimCharacterIdentity.GetIdentity().GetName();
			SCR_ExtendedCharacterIdentityComponent victimExtendedCharacterIdentity = SCR_ExtendedCharacterIdentityComponent.Cast(instigatorContextData.GetVictimEntity().FindComponent(SCR_ExtendedCharacterIdentityComponent));
			if (victimExtendedCharacterIdentity)
				warCrimeEntity.SetVictimIdentity(surname, givenName, victimExtendedCharacterIdentity.GetGender(), victimExtendedCharacterIdentity.GetExtendedIdentity(), victimExtendedCharacterIdentity.GetIdentityBio());
			else
				warCrimeEntity.SetVictimIdentity(surname, givenName, SCR_EIdentityGender.NEUTRAL, null, null);
		}
		
		// cannot be modified from now on
		warCrimeEntity.SetImmutable();
		
		foreach (EventHandlerManagerComponent eventHandlerManager : m_aEventBus)
		{
			eventHandlerManager.RaiseEvent(EVENT_WAR_CRIME_CREATED, 1, warCrimeEntity);
		}
	}

	//
	// KILL LEGALITY
	//
	
	//------------------------------------------------------------------------------------------------
	//! Evaluates a kill legality and possibly impacts player statistics.
	//! If a war crime has been identified, it will also spawn a war crime entity.
	bool EvaluateKillLegalityStats(notnull SCR_InstigatorContextData instigatorContextData, SCR_PlayerData killerData)
	{
		return EvaluateKillLegality(instigatorContextData, true, killerData, null, null);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Evaluates a kill legality and possibly impacts XP scoring.
	bool EvaluateKillLegalityXP(notnull SCR_InstigatorContextData instigatorContextData, SCR_XPHandlerComponent xpHandlerComp)
	{
		return EvaluateKillLegality(instigatorContextData, false, null, xpHandlerComp, null);
	}

	//------------------------------------------------------------------------------------------------
	//! Evaluates a kill legality and possibly impacts base scoring.
	bool EvaluateKillLegalityScoring(notnull SCR_InstigatorContextData instigatorContextData, SCR_BaseScoringSystemComponent scoringSystemComp)
	{
		return EvaluateKillLegality(instigatorContextData, false, null, null, scoringSystemComp);
	}

	//------------------------------------------------------------------------------------------------
	//! Centralizes the complex logic of evaluating a kill legality
	//! across statistics, notifications and XP rewards.
	//! \param instigatorContextData Information about the killer and the victim.
	//! \param register Whether war crime entities should be spawned. To use only once per war crime.
	//! \param killerData Player statistics of the killer, can be null.
	//! \param xpHandlerComp XP handler component, can be null.
	//! \param scoringSystemComp Base scoring system component, can be null.
	//! \return true if the kill was legal, false if it was a war crime. 
	protected bool EvaluateKillLegality(notnull SCR_InstigatorContextData instigatorContextData, bool register, SCR_PlayerData killerData, SCR_XPHandlerComponent xpHandlerComp, SCR_BaseScoringSystemComponent scoringSystemComp)
	{
		IEntity victimEntity = instigatorContextData.GetVictimEntity();
		IEntity killerEntity = instigatorContextData.GetKillerEntity();
		int killerId = instigatorContextData.GetKillerPlayerID();

		SCR_ECharacterControlType victimControlType = instigatorContextData.GetVictimCharacterControlType();
		bool isVictimAI = (victimControlType == SCR_ECharacterControlType.POSSESSED_AI) || (victimControlType == SCR_ECharacterControlType.AI);
				
		bool killedByEnemy = instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_PLAYER)
		 || instigatorContextData.HasAnyVictimKillerRelation(SCR_ECharacterDeathStatusRelations.KILLED_BY_ENEMY_AI);
		
		// we assume legal kill, then systematically check potential crimes
		bool isLegalKill = true;

		// check non-combatant killed		
		if (IsNonCombatant(victimEntity))
		{	
			isLegalKill = false;
			if (register)
				RegisterWarCrime(SCR_ECrimeNotification.NON_COMBATANT_KILL, instigatorContextData);
			
			if (killerData)
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.PROTECTED_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.PROTECTED_KILLS);
			
			if (xpHandlerComp)
				xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.NON_COMBATANT_KILL);
			
			if (scoringSystemComp && killerId != 0)
				scoringSystemComp.AddNonCombatantKill(killerId);
		}
		// check hors de combat killed
		else if(killedByEnemy
		 && IsHorsDeCombat(victimEntity)
		 && IsKillingHorsDeCombatWarCrime())
		{	
			isLegalKill = false;
			if (register)
				RegisterWarCrime(SCR_ECrimeNotification.HORS_DE_COMBAT_KILL, instigatorContextData);

			if (killerData)
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.HDC_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.HDC_KILLS);

			if (xpHandlerComp)
				xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.HORS_DE_COMBAT_KILL);

			if (scoringSystemComp && killerId != 0)
				scoringSystemComp.AddHorsDeCombatKill(killerId);
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
						if (IsKillingWhileDisguisedWarCrime())
						{
							isLegalKill = false;
							if (register)
								RegisterWarCrime(SCR_ECrimeNotification.KILLING_WHILE_DISGUISED, instigatorContextData);

							if (killerData)
								killerData.AddStat(SCR_EDataStats.DISGUISED_KILLER);

							if (xpHandlerComp)
								xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.KILLING_WHILE_DISGUISED);
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
						
						if (isPerfidy && IsPerfidyWarCrime())
						{
							isLegalKill = false;
							if (register)
								RegisterWarCrime(SCR_ECrimeNotification.PERFIDY, instigatorContextData);
							
							if (killerData)
								killerData.AddStat(SCR_EDataStats.PERFIDY_KILLER);
						
							if (xpHandlerComp)
								xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.PERFIDY);
						}
					}
					
				}
			}
		}
		
		// check friendly fire
		if (!killedByEnemy)
		{
			if (!IsNonCombatant(victimEntity) // already checked, do not count twice
				&& instigatorContextData.DoesPlayerKillCountAsTeamKill(true, true))
			{
				isLegalKill = false;
				if (register)
					RegisterWarCrime(SCR_ECrimeNotification.TEAMKILL, instigatorContextData);
				
				if (killerData)
					if (isVictimAI)
						killerData.AddStat(SCR_EDataStats.ALLIED_AI_KILLS);
					else
						killerData.AddStat(SCR_EDataStats.ALLIED_KILLS);
				
				if (xpHandlerComp)
					xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.FRIENDLY_KILL);

				if (scoringSystemComp && killerId != 0)
					scoringSystemComp.AddTeamKill(killerId);
			}
			else
			{
				// completely ignore friendly kill, no stats or XP will be gathered
				return true;
			}
		}
		
		// it was a legal kill, process positive stats, XP and scoring
		if (isLegalKill)
		{	
			if (killerData)
			{
				if (isVictimAI)
					killerData.AddStat(SCR_EDataStats.LEGAL_AI_KILLS);
				else
					killerData.AddStat(SCR_EDataStats.LEGAL_KILLS);
				
			}
			
			if (xpHandlerComp)
			{
				SCR_ChimeraCharacter instigatorChar = SCR_ChimeraCharacter.Cast(instigatorContextData.GetKillerEntity());
				if (instigatorChar && instigatorChar.IsInVehicle())
					xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.ENEMY_KILL_VEH);
				else 
					xpHandlerComp.AwardXP(killerId, SCR_EXPRewards.ENEMY_KILL);
			}
			
			if (scoringSystemComp && killerId != 0)
				scoringSystemComp.AddKill(killerId);
		}

		// compatibility with vanilla
		if (TreatAllWarCrimesAsFriendlyKills() && killerData)
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

	//------------------------------------------------------------------------------------------------
	//! Whether this entity (currently only characters) should be considered as a non-combatant,
	//! that is, an unarmed member of a non-military faction.
	bool IsNonCombatant(IEntity entity)
	{		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if(character)
		{
			SCR_Faction faction = SCR_Faction.Cast(character.GetFaction());
			if(faction && faction.IsMilitary())
				return false;
	
			return IsDisarmed(character);
		}
	
		// TODO deal with buildings
		return false; 
	}

	//------------------------------------------------------------------------------------------------
	//! Whether this character should be considered as "hors de combat",
	//! that is, an unarmed member of a military faction.
	bool IsHorsDeCombat(IEntity entity)
	{		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if(character)
		{
			SCR_Faction faction = SCR_Faction.Cast(character.GetFaction());
			if(faction && !faction.IsMilitary())
				return false;
			// TODO check health, unconsciousness
			return IsDisarmed(character);
		}
	
		return false; 
	}

	//------------------------------------------------------------------------------------------------
	//! Whether this character should be considered as disarmed. The default implementation
	//! looks at the actual weapons list, not at the perception.
	//! Can be overridden in order to refine the logic.
	protected bool IsDisarmed(SCR_ChimeraCharacter character)
	{
		CharacterWeaponManagerComponent weaponManager = CharacterWeaponManagerComponent.Cast(character.FindComponent(CharacterWeaponManagerComponent));
		if (!weaponManager)
			return true;

		array<IEntity> weapons = {};
		int weaponsCount = 	weaponManager.GetWeaponsList(weapons);
		bool disarmed = weaponsCount == 0;

		return disarmed;
	}
	
	//
	// ACCESSORS
	//
	bool IsKillingHorsDeCombatWarCrime()
	{
		return m_bKillingHorsDeCombatIsWarCrime;
	}
	
	bool IsKillingWhileDisguisedWarCrime()
	{
		return m_bKillingWhileDisguisedIsWarCrime;
	}
	
	bool IsPerfidyWarCrime()
	{
		return m_bPerfidyIsWarCrime;
	}
	
	bool TreatAllWarCrimesAsFriendlyKills()
	{
		return m_bTreatAllWarCrimesAsFriendlyKills;
	}
	
	bool UseVanillaLogicForStatistics()
	{
		return m_bVanillaLogicForStatistics;
	}
	
	bool UseVanillaLogicForXP()
	{
		return m_bVanillaLogicForXP;
	}
	
	bool UseVanillaLogicForScoring()
	{
		return m_bVanillaLogicForScoring;
	}
	
	//------------------------------------------------------------------------------------------------
	//! The singleton instance.
	static ARGEO_WarCrimesComponent GetInstance()
	{
		return s_Instance;
	}
}