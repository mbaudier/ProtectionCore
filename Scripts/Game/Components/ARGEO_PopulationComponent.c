[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton coordinating the civilian population.")]
class ARGEO_PopulationComponentClass : SCR_BaseGameModeComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Game component coordinating the automated population of buildings
//! and the safety status of populated territories.
class ARGEO_PopulationComponent : SCR_BaseGameModeComponent
{
	[Attribute("CIV", desc: "Default population faction.", category: "Population")]
	protected FactionKey m_sDefaultPopulationFaction;
	
	private Faction m_DefaultPopulationFaction;

	[Attribute(defvalue: "0", UIWidgets.Slider, desc: "Buildings occupation ratio, in percentage.", params: "0 100 1", category: "Population")]
	protected float m_fGlobalBuildingsOccupation;
	
	[Attribute(defvalue: "20", UIWidgets.Slider, desc: "Which percentage of the buildings must be destroyed to trigger UNLIVABLE status.", params: "0 100 1", category: "Populated Territory Safety Status")]
	protected float m_fProportionOfDestroyedBuildingUnlivable;
	
	[Attribute(defvalue: "10", UIWidgets.Slider, desc: "Ratio in percentage of the number of (all) war crimes to the original population triggering UNLIVABLE status.", params: "0 100 1", category: "Populated Territory Safety Status")]
	protected float m_fProportionOfWarCrimesUnlivable;
	
	[Attribute(defvalue: "20", UIWidgets.Slider, desc: "Probability for an houshold to flee by TENSE status, in percentage.", params: "0 100 1", category: "Displacement Probability")]
	protected float m_fProbabilityToFleeWhenTense;
	
	[Attribute(defvalue: "60", UIWidgets.Slider, desc: "Probability for an houshold to flee by DANGEROUS status, in percentage.", params: "0 100 1", category: "Displacement Probability")]
	protected float m_fProbabilityToFleeWhenDangerous;
	
	[Attribute(defvalue: "90", UIWidgets.Slider, desc: "Probability for an houshold to flee by UNLIVABLE status, in percentage.", params: "0 100 1", category: "Displacement Probability")]
	protected float m_fProbabilityToFleeWhenUnlivable;
	
	protected static ARGEO_PopulationComponent s_Instance;
	
	protected ref array<ref Faction> m_aPopulationFactions = new array<ref Faction>();
	
	protected ref array<ref ARGEO_PopulatedTerritory> m_aPopulatedTerritories = new array<ref ARGEO_PopulatedTerritory>;
	
	protected ref array<ARGEO_CivicCenterEntity> m_aCivicCenters = new array<ARGEO_CivicCenterEntity>;
	
	private bool m_bPopulationAppliedOnce = false;
	
	//
	// LIFECYLE
	//
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!s_Instance)
			s_Instance = this;

		// override configuration with mission header
		SCR_MissionHeader header = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (header)
		{
			if (header.m_fGlobalBuildingsOccupation != 0)
			{		
				m_fGlobalBuildingsOccupation = header.m_fGlobalBuildingsOccupation;
				Print("Global building occupation set from mission header with value '" + header.m_fGlobalBuildingsOccupation + "'");
			}
		}
		
		Print("Global building occupation: " + m_fGlobalBuildingsOccupation + "%");
	}

	//------------------------------------------------------------------------------------------------
	override void OnGameModeStart()
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return; // we won't be able to do anything
		
		for (int i = 0; i < factionManager.GetFactionsCount(); i++)
		{
			Faction faction = factionManager.GetFactionByIndex(i);	
			if (faction.GetFactionKey() == m_sDefaultPopulationFaction)
			{
				m_DefaultPopulationFaction = faction;
				m_aPopulationFactions.Insert(m_DefaultPopulationFaction);
			}
			else
			{
				SCR_Faction f = SCR_Faction.Cast(faction);
				if (f && f.GetFactionHomeTerritoryConfig() && f.GetFactionHomeTerritoryConfig().IsFactionPopulation())
				{
					m_aPopulationFactions.Insert(f);
				}
			}
		}
		
		if (!m_DefaultPopulationFaction)
			Print("Default population faction" + m_DefaultPopulationFaction + " was not found", LogLevel.ERROR);

		// apply population only after the backend delay also used by the ambient patrol system
		GetGame().GetCallqueue().CallLater(ApplyPopulation, SCR_GameModeCampaign.BACKEND_DELAY);
	}
	
	//
	// AUTOMATED POPULATION
	//
	
	//------------------------------------------------------------------------------------------------
	//! The main logic of populating territories based on the relative weights of the factions.
	void ApplyPopulation()
	{
		int totalSpawnPoints = 0;
		int totalWeight = 0;
		foreach (ARGEO_PopulatedTerritory populatedTerritory : m_aPopulatedTerritories)
		{
			totalSpawnPoints += populatedTerritory.GetSpawnPointsCount();
			totalWeight += populatedTerritory.GetTotalWeight();
		}
		
		int totalPopulatedSpawnPoints = totalSpawnPoints * (m_fGlobalBuildingsOccupation / 100);
		foreach (ARGEO_PopulatedTerritory populatedTerritory : m_aPopulatedTerritories)
		{
			int toPopulateSpawnPointsCount = (totalPopulatedSpawnPoints * populatedTerritory.GetTotalWeight()) / totalWeight;
			populatedTerritory.PopulateRandomSpawnPoints(toPopulateSpawnPointsCount);
		}
		m_bPopulationAppliedOnce = true;
		Print("Applied population");
	}
	
	//
	// REGISTER
	//
	
	//------------------------------------------------------------------------------------------------
	//! Register a building that can be populated.
	void RegisterBuildingHousehold(notnull ARGEO_BuildingHouseholdEntity buildingHousehold)
	{
		ARGEO_PopulatedTerritoryID populatedTerritoryID = buildingHousehold.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		populatedTerritory.AddBuildingHousehold(buildingHousehold);
		Print("Registered household in " + populatedTerritoryID + " with " + buildingHousehold.GetSpawnPointsCount() + " spawn points", LogLevel.DEBUG);
		
		if (m_bPopulationAppliedOnce)
			ApplyPopulation(); // update 
	}
	
	//------------------------------------------------------------------------------------------------
	//! Register a civic center to which the population can flee if needed.
	void RegisterCivicCenter(notnull ARGEO_CivicCenterEntity civicCenter)
	{
		m_aCivicCenters.Insert(civicCenter);
		Print("Registered civic center at position " + civicCenter.GetOrigin());
		
		// notify
		foreach (ARGEO_PopulatedTerritory populatedTerritory : m_aPopulatedTerritories)
		{
			populatedTerritory.GetEventHandlerManager().RaiseEvent(ARGEO_PopulatedTerritory.EVENT_CIVIC_CENTER_CREATED, 1, civicCenter);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Registers an ambient vehicle, so that its faction can be set properly.
	void RegisterAmbientVehicle(ARGEO_PopulatedTerritoryID populatedTerritoryID, notnull SCR_AmbientVehicleSpawnPointComponent vehicleSpawnPoint)
	{
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		Print("Vehicle spawnpoint found for territory " + populatedTerritoryID, LogLevel.SPAM);
		FactionAffiliationComponent vehicleFactionAffiliation = FactionAffiliationComponent.Cast(vehicleSpawnPoint.GetOwner().FindComponent(FactionAffiliationComponent));
		OptionallySetPopulationFactionByKey(vehicleFactionAffiliation, populatedTerritory.GetRandomFactionKey());
	}
	
	//
	// EVENTS
	//
	
	//------------------------------------------------------------------------------------------------
	//! Notifies that a civic center has been destroyed and is not available anymore.
	void NotifyCivicCenterDestroyed(notnull ARGEO_CivicCenterEntity civicCenter)
	{
		m_aCivicCenters.RemoveItem(civicCenter);
		Print("Unregistered civic center at position " + civicCenter.GetOrigin());
		
		// notify
		foreach (ARGEO_PopulatedTerritory populatedTerritory : m_aPopulatedTerritories)
		{
			populatedTerritory.GetEventHandlerManager().RaiseEvent(ARGEO_PopulatedTerritory.EVENT_CIVIC_CENTER_DESTROYED, 1, civicCenter);
		}
		
		ARGEO_PopulatedTerritoryID populatedTerritoryID = civicCenter.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		if (populatedTerritory.GetSafetyStatus() > ARGEO_EPopulationSafetyStatus.DANGEROUS)
			populatedTerritory.ChangeSafetyStatus(ARGEO_EPopulationSafetyStatus.DANGEROUS);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notifies the destruction of a building, possibly changing the safety status of its territory.
	void NotifyBuildingDestroyed(ARGEO_BuildingPopulationEntity building)
	{
		ARGEO_PopulatedTerritoryID populatedTerritoryID = building.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		populatedTerritory.IncreaseBuildingDestroyedCount();
		EvaluateSafetyStatus(populatedTerritory);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notifies the damage of a building, possibly changing the safety status of its territory.
	void NotifyBuildingDamaged(ARGEO_BuildingPopulationEntity building, BaseDamageContext damageContext)
	{
		ARGEO_PopulatedTerritoryID populatedTerritoryID = building.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		populatedTerritory.IncreaseBuildingDamageCount();
		EvaluateSafetyStatus(populatedTerritory);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notifies a new war crime, possibly changing the safety status of the related territory.
	//! \param populatedTerritoryID ID of the populated territory where the war crime happenned.
	//! \param warCrime The war crime.
	void NotifyWarCrime(ARGEO_PopulatedTerritoryID populatedTerritoryID, ARGEO_WarCrimeEntity warCrime)
	{
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		
		if (SCR_ECrimeNotification.TEAMKILL == warCrime.GetCrime())
			return; // team kills do not really affect the population
		
		populatedTerritory.IncreaseWarCrimeCount();
		EvaluateSafetyStatus(populatedTerritory);
	}
	
	//
	// SAFETY
	//
	
	//------------------------------------------------------------------------------------------------
	//! Main logic evaluating the safety status of a given populated territory, based on
	//! damages to buildings and committed war crimes. Currently, the status can only worsen.
	//! A worsening of the safety status will indirectly trigger more civilians to flee the territory.
	//! At this stage, this is mostly hardcoded based on the proportion of damaged buildings
	//! and on the proportion of war crimes relative to the population.
	//! It can be overridden as long as populatedTerritory.ChangeSafetyStatus() is called.
	void EvaluateSafetyStatus(ARGEO_PopulatedTerritory populatedTerritory)
	{
		ARGEO_EPopulationSafetyStatus currentStatus = populatedTerritory.GetSafetyStatus();
		ARGEO_EPopulationSafetyStatus newStatus = currentStatus;
		
		bool tense = populatedTerritory.GetBuildingDamageCount() > 0;
		if (newStatus > ARGEO_EPopulationSafetyStatus.TENSE && tense)
			newStatus = ARGEO_EPopulationSafetyStatus.TENSE;

		bool dangerous = populatedTerritory.GetBuildingDamageCount() / populatedTerritory.GetBuildingHouseholdsCount() > 1
		 || populatedTerritory.GetBuildingDestroyedCount() > 0
		 || populatedTerritory.GetWarCrimeCount() > 0;
		if (newStatus > ARGEO_EPopulationSafetyStatus.DANGEROUS && dangerous)
			newStatus = ARGEO_EPopulationSafetyStatus.DANGEROUS;
		
		bool unlivable = populatedTerritory.GetBuildingDestroyedCount() / populatedTerritory.GetBuildingHouseholdsCount() > (m_fProportionOfDestroyedBuildingUnlivable / 100)
		 || populatedTerritory.GetWarCrimeCount() / populatedTerritory.GetOriginalPopulation() > (m_fProportionOfWarCrimesUnlivable / 100);
		if (unlivable)
			newStatus = ARGEO_EPopulationSafetyStatus.UNLIVABLE;
		
		if (newStatus != currentStatus)
			populatedTerritory.ChangeSafetyStatus(newStatus);
	}

	//
	// DISPLACEMENT
	//
	
	//------------------------------------------------------------------------------------------------
	//! Whether a given household should flee its territory, randomly based on the territory
	//! safety status.
	bool ShouldFlee(ARGEO_BuildingHouseholdEntity household)
	{
		ARGEO_PopulatedTerritoryID populatedTerritoryID = household.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory || household.HasFled())
			return false; // should not happen
		
		int rand = Math.RandomInt(0, 100);
		ARGEO_EPopulationSafetyStatus status = populatedTerritory.GetSafetyStatus(); 
		switch(status)
		{
			case ARGEO_EPopulationSafetyStatus.PEACE: return false;
			case ARGEO_EPopulationSafetyStatus.SAFE: return false;
			case ARGEO_EPopulationSafetyStatus.TENSE: return rand < m_fProbabilityToFleeWhenTense;
			case ARGEO_EPopulationSafetyStatus.DANGEROUS: return rand < m_fProbabilityToFleeWhenDangerous;
			case ARGEO_EPopulationSafetyStatus.UNLIVABLE: return rand < m_fProbabilityToFleeWhenUnlivable;
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Finds the nearest civic center from a given position.
	// \param pos A position in the world.
	// \return The nearest civic center.
	ARGEO_CivicCenterEntity GetNearestCivicCenter(vector pos)
	{
		ARGEO_CivicCenterEntity best;
		float nearest;
		foreach (ARGEO_CivicCenterEntity civicCenter : m_aCivicCenters)
		{	
			float distanceSq = vector.DistanceSqXZ(pos, civicCenter.GetOrigin());
			if (!best)
			{
				best = civicCenter;
				nearest = distanceSq;
			}
			else
			{
				if(nearest > distanceSq)
				{
					best = civicCenter;
					nearest = distanceSq;
				}		
			}
		}
		return best;
	}

	//------------------------------------------------------------------------------------------------
	//! Is this non-combatant close enough from a civic center to be discharged?
	//! \param character The non-combatant character.
	//! \return The civic center where to discharge or null if none close enough.
	ARGEO_CivicCenterEntity CanNonCombatantBeDischarged(IEntity character)
	{
		ARGEO_CivicCenterEntity civicCenter = GetNearestCivicCenter(character.GetOrigin());
		if (civicCenter)
		{
			float distance = vector.DistanceXZ(character.GetOrigin(), civicCenter.GetOrigin());
			if(distance <= civicCenter.GetDischargeRadius())
				return civicCenter;
		}
		return null;
	}
			
	//------------------------------------------------------------------------------------------------
	//! Is this prisoner close enough from a civic center to be discharged?
	//! \param character The prisoner character.
	//! \return The civic center where to discharge or null if none close enough.
	ARGEO_CivicCenterEntity CanPrisonerBeDischarged(IEntity character)
	{
		// TODO Implement a system for prison, etc.
		ARGEO_CivicCenterEntity civicCenter = GetNearestCivicCenter(character.GetOrigin());
		if (civicCenter)
		{
			float distance = vector.DistanceXZ(character.GetOrigin(), civicCenter.GetOrigin());
			if(distance <= civicCenter.GetDischargeRadius())
				return civicCenter;
		}
		return null;
	}
			
	//
	// ACCESSORS
	//
	
	//------------------------------------------------------------------------------------------------
	//! Overall desentity of population, as a percentage of the maximum poissble.
	float GetGlobalBuildingsOccupation()
	{
		return m_fGlobalBuildingsOccupation;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get the populated territory object with this ID, possibly lazy-initializing it.
	ARGEO_PopulatedTerritory GetPopulatedTerritory(ARGEO_PopulatedTerritoryID populatedTerritoryID)
	{
		if (!populatedTerritoryID)
			return null;
		
		foreach (ARGEO_PopulatedTerritory t : m_aPopulatedTerritories)
			if (t.GetPopulatedTerritoryID() == populatedTerritoryID)
				return t;
		
		// create
		ARGEO_PopulatedTerritory populatedTerritory = null;
		foreach (Faction faction : m_aPopulationFactions)
		{
			SCR_Faction f = SCR_Faction.Cast(faction);
			if (f)
			{
				SCR_FactionHomeTerritoryConfig homeTerritoryConfig = f.GetFactionHomeTerritoryConfig();
				// we use the same IDs and weights as the vanilla home territories
				if (homeTerritoryConfig && homeTerritoryConfig.IsFactionPopulation())
				{
					for (int j = 0; j < homeTerritoryConfig.GetHomeTerritoriesCount(); j++)
					{
						SCR_FactionHomeTerritory homeTerritory = homeTerritoryConfig.GetHomeTerritory(j);
						// we assume homeTerritory will never be null
						if (populatedTerritoryID == homeTerritory.m_sID)
						{
							if (!populatedTerritory)
							{
								populatedTerritory = new ARGEO_PopulatedTerritory(populatedTerritoryID);
								m_aPopulatedTerritories.Insert(populatedTerritory);
							}
							populatedTerritory.SetFactionWeight(faction.GetFactionKey(), homeTerritory.m_iWeight);
						}
					}
				}
			}
		}
		
		if (!populatedTerritory)
			Print("No populated territory with ID " + populatedTerritoryID, LogLevel.ERROR);
		
		return populatedTerritory;
	}
	
	//
	// UTILITIES
	//
	
	//------------------------------------------------------------------------------------------------
	//! Safely set a population faction by key. Will do nothing if it is not possible for some reason.
	void OptionallySetPopulationFactionByKey(FactionAffiliationComponent factionAffiliation, FactionKey factionKey)
	{
		if (!factionKey || !factionAffiliation)
			return;
		if (factionAffiliation.GetAffiliatedFactionKey())
			return; // was explicitly set
		
		Faction factionToSet = null;
		Faction currentFaction = null;
		foreach (Faction faction : m_aPopulationFactions)
		{
			if (faction.GetFactionKey() == factionAffiliation.GetAffiliatedFactionKey())
			{
				currentFaction = faction;
			}
			
			if (faction.GetFactionKey() == factionKey)
			{
				factionToSet = faction;
			}
		}
		
		if (currentFaction)
			return; // a registered population faction was explicitly set
		
		if (factionToSet)
			factionAffiliation.SetAffiliatedFaction(factionToSet);
		else if (factionAffiliation.GetAffiliatedFactionKey()) // typically a CIV ambient vehicle
			factionAffiliation.SetAffiliatedFaction(m_DefaultPopulationFaction);
		else
			Print("Faction " + factionKey + " is not a population faction", LogLevel.WARNING);
	}

	//------------------------------------------------------------------------------------------------
	//! The singleton instance.
	static ARGEO_PopulationComponent GetInstance()
	{
		return s_Instance;
	}
}
