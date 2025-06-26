[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton coordinating the civilian population.")]
class ARGEO_PopulationComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_PopulationComponent : SCR_BaseGameModeComponent
{
	[Attribute("CIV", desc: "Default civilian faction.", category: "Population")]
	protected FactionKey m_sDefaultCivilianFactionKey;

	[Attribute(defvalue: "0", UIWidgets.Slider, desc: "Buildings occupation ratio, in percentage.", params: "0 100 1", category: "Population")]
	protected float m_fGlobalBuildingsOccupation;

//	[Attribute("0", desc: "Populate with the default faction the buildings outside the explicitely populated areas.", category: "Population")]
//	protected bool m_bPopulateOutsidePopulatedAreas;

	protected static ARGEO_PopulationComponent s_Instance;
	
	protected ref array<ref Faction> m_aPopulationFactions = new array<ref Faction>();
	
	protected ref array<ref ARGEO_PopulatedTerritory> m_aPopulatedTerritories = new array<ref ARGEO_PopulatedTerritory>;
	//protected ref ARGEO_PopulatedTerritory m_IsolatedPopulation = new ARGEO_PopulatedTerritory("");
	
	private bool m_bPopulationAppliedOnce = false;
	
	void RegisterBuildingHousehold(ARGEO_BuildingHouseholdEntity buildingHousehold)
	{
		string populatedTerritoryID = buildingHousehold.GetPopulatedTerritoryID();
		ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
		if (!populatedTerritory)
			return;
			//populatedTerritory = m_IsolatedPopulation;
		populatedTerritory.AddBuildingHousehold(buildingHousehold);
		Print("Registered household in " + populatedTerritoryID + " with " + buildingHousehold.GetSpawnPointsCount() + " spawn points");
		
		if (m_bPopulationAppliedOnce)
			ApplyPopulation(); // update 
	}
	
	void ApplyPopulation()
	{
		int totalSpawnPoints = 0;
		int totalWeight = 0;
		foreach (ARGEO_PopulatedTerritory populatedTerritory : m_aPopulatedTerritories)
		{
			totalSpawnPoints = populatedTerritory.GetSpawnPointsCount();
			totalWeight = populatedTerritory.GetTotalWeight();
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
	// LIFE CYLE
	//
	override void OnGameModeStart()
	{
		if (!s_Instance)
			s_Instance = this;
		Print("Global building occupation: " + m_fGlobalBuildingsOccupation + "%");
		

		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		SCR_SortedArray<SCR_Faction> factions = new SCR_SortedArray<SCR_Faction>();
		factionManager.GetSortedFactionsList(factions);
		// TODO check whether default civilian faction is listed

		for (int i = 0; i < factions.Count(); i++)
		{		
			SCR_Faction f = factions.Get(i);
			SCR_FactionHomeTerritoryConfig homeTerritoryConfig = f.GetFactionHomeTerritoryConfig();
			// we use the same IDs and weights as the vanilla home territories
			if (homeTerritoryConfig && homeTerritoryConfig.IsFactionPopulation())
			{
				Faction faction = f;
				m_aPopulationFactions.Insert(faction);
				
				// TODO make it configurable
				//if (faction.GetFactionKey() == m_sDefaultCivilianFactionKey)
				//	m_IsolatedPopulation.SetFactionWeight(faction.GetFactionKey(), 0);
				
				for (int j = 0; j < homeTerritoryConfig.GetHomeTerritoriesCount(); j++)
				{
					SCR_FactionHomeTerritory homeTerritory = homeTerritoryConfig.GetHomeTerritory(j);
					// we assume homeTerritory will never be null
					string populatedTerritoryID = homeTerritory.m_sID;
					ARGEO_PopulatedTerritory populatedTerritory = GetPopulatedTerritory(populatedTerritoryID);
					if (!populatedTerritory)
					{
						populatedTerritory = new ARGEO_PopulatedTerritory(populatedTerritoryID);
						m_aPopulatedTerritories.Insert(populatedTerritory);
					}
					
					// configure populated territory
					populatedTerritory.SetFactionWeight(faction.GetFactionKey(), homeTerritory.m_iWeight);
				}
			}
		}
		
		// apply population after the backend delay also used by the ambient patrol system
		//GetGame().GetCallqueue().CallLater(ApplyPopulation, SCR_GameModeCampaign.BACKEND_DELAY);
		GetGame().GetCallqueue().CallLater(ApplyPopulation, 10000);
	}
	
	//
	// ACCESSORS
	//
	FactionKey GetDefaultCivilianFactionKey()
	{
		return m_sDefaultCivilianFactionKey;
	}
	
	float GetGlobalBuildingsOccupation()
	{
		return m_fGlobalBuildingsOccupation;
	}
	
//	bool PopulateOutsidePopulatedAreas()
//	{
//		return m_bPopulateOutsidePopulatedAreas;
//	}
	
	ARGEO_PopulatedTerritory GetPopulatedTerritory(string populatedTerritoryID)
	{
		if (!populatedTerritoryID)
			return null;
		
		foreach (ARGEO_PopulatedTerritory t : m_aPopulatedTerritories)
			if (t.GetPopulatedTerritoryID() == populatedTerritoryID)
				return t;
		return null;
	}

	static ARGEO_PopulationComponent GetInstance()
	{
		return s_Instance;
	}
}

class ARGEO_PopulatedTerritory
{
	string m_sID;
	
	protected ref map<FactionKey, int> m_mFactionWeights = new map<FactionKey, int>;
	protected ref array<ARGEO_BuildingHouseholdEntity> m_aBuildingHouseholds = new array<ARGEO_BuildingHouseholdEntity>;
	
	private int m_iSpawnPointsCount = 0;
	private int m_iTotalWeight = 0;
	
	void ARGEO_PopulatedTerritory(string populatedTerritoryID)
	{
		m_sID = populatedTerritoryID;
	}
	
	void SetFactionWeight(FactionKey factionKey, int weight)
	{
		m_mFactionWeights.Set(factionKey, weight);
		m_iTotalWeight += weight;
	}
	
	void AddBuildingHousehold(ARGEO_BuildingHouseholdEntity buildingHousehold)
	{
		m_aBuildingHouseholds.Insert(buildingHousehold);
		m_iSpawnPointsCount += buildingHousehold.GetSpawnPointsCount();
	}
	
	int PopulateRandomSpawnPoints(int toPopulateSpawnPointsCount)
	{
		array<ARGEO_PopulatedSpawnPointComponent> spawnPoints = {};
		foreach (ARGEO_BuildingHouseholdEntity buildingHousehold : m_aBuildingHouseholds)
		{
			buildingHousehold.GetSpawnPoints(spawnPoints);
		}
		
		int spawnPointCount = 0;
		for (int i = 0; i < toPopulateSpawnPointsCount; i++)
		{
			if(spawnPoints.Count() == 0)
			{
				Print("Not enough spawn points to populate territory " + m_sID + " (" + toPopulateSpawnPointsCount + " > "+ spawnPointCount + ")", LogLevel.WARNING);
				break; // toPopulateSpawnPointsCount
			}
			
			int index = Math.RandomInt(0, spawnPoints.Count());
			ARGEO_PopulatedSpawnPointComponent spawnPoint = spawnPoints[index];
			GenericEntity owner = spawnPoint.GetOwner();
			FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));		
			if (factionAffiliation)
			{
				int randomWeight = Math.RandomIntInclusive(0, m_iTotalWeight);
		
				int checkedWeight = 0;
				
				FactionKey factionKey;
				foreach (FactionKey f, int weight : m_mFactionWeights)
				{
					checkedWeight += weight;
					if (randomWeight <= checkedWeight)
					{
						factionKey = f;
						break; // m_mFactionWeights
					}
				}

				if(factionKey)
					factionAffiliation.SetAffiliatedFactionByKey(factionKey);		
			}
			spawnPoint.EnableSpawn();
			
			spawnPointCount++;
			spawnPoints.Remove(index);
		}
		
		// in case we are updating, make sure all other spawn points are disabled
		foreach (ARGEO_PopulatedSpawnPointComponent spawnPoint : spawnPoints)
		{
			spawnPoint.DisableSpawn();
		}
		
		return spawnPointCount;
	}
	
	//
	// ACCESSORS
	//
	string GetPopulatedTerritoryID()
	{
		return m_sID;
	}
	
	int GetSpawnPointsCount()
	{
		return m_iSpawnPointsCount;
	}
	
	int GetTotalWeight()
	{
		return m_iTotalWeight;
	}
}