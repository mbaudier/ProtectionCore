[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton coordinating the civilian population.")]
class ARGEO_PopulationComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_PopulationComponent : SCR_BaseGameModeComponent
{
	[Attribute("CIV", desc: "Default population faction.", category: "Population")]
	protected FactionKey m_sDefaultFaction;
	
	private Faction m_DefaultPopulationFaction;

	[Attribute(defvalue: "0", UIWidgets.Slider, desc: "Buildings occupation ratio, in percentage.", params: "0 100 1", category: "Population")]
	protected float m_fGlobalBuildingsOccupation;

	protected static ARGEO_PopulationComponent s_Instance;
	
	protected ref array<ref Faction> m_aPopulationFactions = new array<ref Faction>();
	
	protected ref array<ref ARGEO_PopulatedTerritory> m_aPopulatedTerritories = new array<ref ARGEO_PopulatedTerritory>;
	protected ref array<ARGEO_CivicCenterEntity> m_aCivicCenters = new array<ARGEO_CivicCenterEntity>;
	
	private bool m_bPopulationAppliedOnce = false;
	
	//
	// LIFECYLE
	//
	override void OnPostInit(IEntity owner)
	{
		if (!s_Instance)
			s_Instance = this;

		SCR_MissionHeader header = SCR_MissionHeader.Cast(GetGame().GetMissionHeader());
		if (header)
		{
			if (header.m_fGlobalBuildingsOccupation != 0)
				m_fGlobalBuildingsOccupation = header.m_fGlobalBuildingsOccupation;
		}
		Print("Global building occupation: " + m_fGlobalBuildingsOccupation + "%");
	}

	override void OnGameModeStart()
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
			return; // we won't be able to do anything
		
		for (int i = 0; i < factionManager.GetFactionsCount(); i++)
		{
			Faction faction = factionManager.GetFactionByIndex(i);	
			if (faction.GetFactionKey() == m_sDefaultFaction)
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

		// apply population after the backend delay also used by the ambient patrol system
		//GetGame().GetCallqueue().CallLater(ApplyPopulation, 10000);
		GetGame().GetCallqueue().CallLater(ApplyPopulation, SCR_GameModeCampaign.BACKEND_DELAY);
	}
	
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
	void RegisterBuildingHousehold(notnull ARGEO_BuildingHouseholdEntity buildingHousehold)
	{
		string populatedTerritoryID = buildingHousehold.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		populatedTerritory.AddBuildingHousehold(buildingHousehold);
		Print("Registered household in " + populatedTerritoryID + " with " + buildingHousehold.GetSpawnPointsCount() + " spawn points", LogLevel.DEBUG);
		
		if (m_bPopulationAppliedOnce)
			ApplyPopulation(); // update 
	}
	
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
	
	void UnregisterCivicCenter(notnull ARGEO_CivicCenterEntity civicCenter)
	{
		m_aCivicCenters.RemoveItem(civicCenter);
		Print("Unregistered civic center at position " + civicCenter.GetOrigin());
		
		// notify
		foreach (ARGEO_PopulatedTerritory populatedTerritory : m_aPopulatedTerritories)
		{
			populatedTerritory.GetEventHandlerManager().RaiseEvent(ARGEO_PopulatedTerritory.EVENT_CIVIC_CENTER_DESTROYED, 1, civicCenter);
		}
	}
	
	void RegisterAmbientVehicle(string populatedTerritoryID, notnull SCR_AmbientVehicleSpawnPointComponent vehicleSpawnPoint)
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
	void NotifyBuildingDestroyed(ARGEO_BuildingPopulationEntity building)
	{
		string populatedTerritoryID = building.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		populatedTerritory.IncreaseBuildingDestroyedCount();
		EvaluateSafetyStatus(populatedTerritory);
	}
	
	void NotifyBuildingDamaged(ARGEO_BuildingPopulationEntity building, BaseDamageContext damageContext)
	{
		string populatedTerritoryID = building.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
		populatedTerritory.IncreaseBuildingDamageCount();
		EvaluateSafetyStatus(populatedTerritory);
	}
	
	//
	// SAFETY
	//
	void EvaluateSafetyStatus(ARGEO_PopulatedTerritory populatedTerritory)
	{
		ARGEO_PopulationSafetyStatus currentStatus = populatedTerritory.GetSafetyStatus();
		ARGEO_PopulationSafetyStatus newStatus = currentStatus;
		
		// TODO make it configurable
		
		bool tense = populatedTerritory.GetBuildingDamageCount() > 0;
		if (newStatus > ARGEO_PopulationSafetyStatus.TENSE && tense)
			newStatus = ARGEO_PopulationSafetyStatus.TENSE;

		bool dangerous = populatedTerritory.GetBuildingDamageCount() / populatedTerritory.GetBuildingHouseholdsCount() > 1
		 || populatedTerritory.GetBuildingDestroyedCount() > 0;
		if (newStatus > ARGEO_PopulationSafetyStatus.DANGEROUS && dangerous)
			newStatus = ARGEO_PopulationSafetyStatus.DANGEROUS;
		
		bool unlivable = populatedTerritory.GetBuildingDestroyedCount() / populatedTerritory.GetBuildingHouseholdsCount() > 0.2;
		if (unlivable)
			newStatus = ARGEO_PopulationSafetyStatus.UNLIVABLE;
		
		if (newStatus != currentStatus)
			populatedTerritory.ChangeSafetyStatus(newStatus);
	}

	//
	// DISPLACEMENT
	//
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

	bool CanNonCombatantBeDischarged(SCR_ChimeraCharacter character)
	{
		ARGEO_CivicCenterEntity civicCenter = GetNearestCivicCenter(character.GetOrigin());
		if (civicCenter)
		{
			float distance = vector.DistanceXZ(character.GetOrigin(), civicCenter.GetOrigin());
			return distance <= civicCenter.GetDischargeRadius();
		}
		return false;
	}
			
	bool CanPrisonerBeDischarged(SCR_ChimeraCharacter character)
	{
		// TODO Implement a system for prison, etc.
		ARGEO_CivicCenterEntity civicCenter = GetNearestCivicCenter(character.GetOrigin());
		if (civicCenter)
			return vector.DistanceXZ(character.GetOrigin(), civicCenter.GetOrigin()) <= civicCenter.GetDischargeRadius();
		return false;
	}
			
	//
	// ACCESSORS
	//
	float GetGlobalBuildingsOccupation()
	{
		return m_fGlobalBuildingsOccupation;
	}
	
	ARGEO_PopulatedTerritory GetPopulatedTerritory(string populatedTerritoryID)
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

	static ARGEO_PopulationComponent GetInstance()
	{
		return s_Instance;
	}
}
