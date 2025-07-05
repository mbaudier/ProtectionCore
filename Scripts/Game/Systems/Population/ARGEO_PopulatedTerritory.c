class ARGEO_PopulatedTerritory
{
	static const string EVENT_SAFETY_STATUS_CHANGED = "OnSafetyStatusChanged";
	
	private string m_sID;
	
	protected EventHandlerManagerComponent m_EventHandlerMgr;
	
	protected ref map<FactionKey, int> m_mFactionWeights = new map<FactionKey, int>;
	protected ref array<ARGEO_BuildingHouseholdEntity> m_aBuildingHouseholds = new array<ARGEO_BuildingHouseholdEntity>;
	
	private int m_iSpawnPointsCount = 0;
	private int m_iTotalWeight = 0;
	
	private ARGEO_PopulationSafetyStatus m_SafetyStatus = ARGEO_PopulationSafetyStatus.SAFE;
	
	private int m_iBuildingDestroyedCount = 0;
	private int m_iBuildingDamageCount = 0;
	
	void ARGEO_PopulatedTerritory(string populatedTerritoryID)
	{
		m_sID = populatedTerritoryID;
	}
	
	void SetEventHandler(EventHandlerManagerComponent eventHandlerMgr)
	{
		if (m_EventHandlerMgr)
			if(m_EventHandlerMgr == eventHandlerMgr)
				return;
			else
				Print("Event handler already set for populated territory " + m_sID, LogLevel.ERROR);
		else 
			m_EventHandlerMgr = eventHandlerMgr;
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
		
		if (m_EventHandlerMgr)
			m_EventHandlerMgr.RegisterScriptHandler(EVENT_SAFETY_STATUS_CHANGED, buildingHousehold, buildingHousehold.OnSafetyStatusChanged);
	}
	
	int PopulateRandomSpawnPoints(int toPopulateSpawnPointsCount)
	{
		ARGEO_PopulationComponent populationComponent = ARGEO_PopulationComponent.GetInstance();
		
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
			FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(spawnPoint.GetOwner().FindComponent(FactionAffiliationComponent));		
			if (factionAffiliation)
			{
				populationComponent.OptionallySetPopulationFactionByKey(factionAffiliation, GetRandomFactionKey());		
				spawnPoint.EnableSpawn();
				
				spawnPointCount++;
				spawnPoints.Remove(index);
			}
		}
		
		// make sure all other spawn points are disabled
		foreach (ARGEO_PopulatedSpawnPointComponent spawnPoint : spawnPoints)
		{
			spawnPoint.DisableSpawn();
		}
		
		return spawnPointCount;
	}

	FactionKey GetRandomFactionKey()
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
		return factionKey;	
	}
	
	//
	// SAFETY
	//
	void ChangeSafetyStatus(ARGEO_PopulationSafetyStatus safetyStatus)
	{
		if (m_SafetyStatus == safetyStatus)
			return;
		ARGEO_PopulationSafetyStatus previousStatus = m_SafetyStatus;
		m_SafetyStatus = safetyStatus;
		Print("Safety status of territory " + m_sID + " changed from " + previousStatus + " to " + m_SafetyStatus);
		
		m_EventHandlerMgr.RaiseEvent(EVENT_SAFETY_STATUS_CHANGED, 1, m_SafetyStatus);
	}
	
	float GetBuildingDestroyedCount()
	{
		return m_iBuildingDestroyedCount;
	}
	
	void IncreaseBuildingDestroyedCount()
	{
		m_iBuildingDestroyedCount++;
	}
	
	float GetBuildingDamageCount()
	{
		return m_iBuildingDamageCount;
	}
	
	void IncreaseBuildingDamageCount()
	{
		m_iBuildingDamageCount++;
	}
	
	//
	// ACCESSORS
	//
	string GetPopulatedTerritoryID()
	{
		return m_sID;
	}
	
	float GetSpawnPointsCount()
	{
		return m_iSpawnPointsCount;
	}
	
	float GetBuildingHouseholdsCount()
	{
		return m_aBuildingHouseholds.Count();
	}
	
	int GetTotalWeight()
	{
		return m_iTotalWeight;
	}
	
	ARGEO_PopulationSafetyStatus GetSafetyStatus()
	{
		return m_SafetyStatus;
	}
}

enum ARGEO_PopulationSafetyStatus
{
	UNLIVABLE,
	DANGEROUS,
	TENSE,
	SAFE,
	PEACE
}