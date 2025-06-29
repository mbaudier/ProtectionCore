class ARGEO_PopulatedTerritory
{
	string m_sID;
	
	protected EventHandlerManagerComponent m_EventHandlerMgr;
	
	protected ref map<FactionKey, int> m_mFactionWeights = new map<FactionKey, int>;
	protected ref array<ARGEO_BuildingHouseholdEntity> m_aBuildingHouseholds = new array<ARGEO_BuildingHouseholdEntity>;
	
	private int m_iSpawnPointsCount = 0;
	private int m_iTotalWeight = 0;
	
	private ARGEO_PopulationSafetyStatus m_SafetyStatus = ARGEO_PopulationSafetyStatus.SAFE;
	
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
			m_EventHandlerMgr.RegisterScriptHandler("OnSafetyStatusChanged", buildingHousehold, buildingHousehold.OnSafetyStatusChanged);
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
	
	void ChangeSafetyStatus(ARGEO_PopulationSafetyStatus safetyStatus)
	{
		if (m_SafetyStatus == safetyStatus)
			return;
		m_SafetyStatus = safetyStatus;
		
		m_EventHandlerMgr.RaiseEvent("OnSafetyStatusChanged", 1, safetyStatus);
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

enum ARGEO_PopulationSafetyStatus
{
	UNLIVABLE,
	DANGEROUS,
	TENSE,
	SAFE,
	PEACEFUL
}