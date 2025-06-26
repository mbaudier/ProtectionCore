class ARGEO_BuildingHouseholdEntityClass: GenericEntityClass
{
}

//! The link between people and a given building.
class ARGEO_BuildingHouseholdEntity: GenericEntity
{
	private SCR_DestructibleBuildingComponent m_DestructibleBuildingComp;
	
	private ref array<ARGEO_PopulatedSpawnPointComponent> m_aSpawnPoints = new array<ARGEO_PopulatedSpawnPointComponent>;

	private string m_sPopulatedTerritoryID;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		//Print("Household initialized");
	}

	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);

		m_DestructibleBuildingComp = FindDestructibleBuildingComp(owner);
		if(m_DestructibleBuildingComp)
		{
			m_DestructibleBuildingComp.GetOnDamageStateChanged().Insert(OnBuildingDestroyed);
		}
		//Print("Household post-init");
		
		// Find populated spawn points
		array<IEntity> queue = {owner};
		IEntity processedEntity;
		IEntity nextInHierarchy;
		
		while (!queue.IsEmpty())
		{
			processedEntity = queue[0];
			queue.Remove(0);
			
			ARGEO_PopulatedSpawnPointComponent spawnPoint = ARGEO_PopulatedSpawnPointComponent.Cast(processedEntity.FindComponent(ARGEO_PopulatedSpawnPointComponent));
			 
			if (spawnPoint) {
				m_aSpawnPoints.Insert(spawnPoint);
				nextInHierarchy = null;// no need to go deeper
			}
			else
			{
				nextInHierarchy = processedEntity.GetChildren();
			}
			
			while (nextInHierarchy)
			{
				queue.Insert(nextInHierarchy);
				nextInHierarchy = nextInHierarchy.GetSibling();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called when the related building is destroyed, making the related people displaced persons.
	protected void OnBuildingDestroyed(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		Print("Building destroyed!");
	}
	
	private SCR_DestructibleBuildingComponent FindDestructibleBuildingComp(IEntity current)
	{
		if (!current)
			return null;
		
		SCR_DestructibleBuildingComponent res = SCR_DestructibleBuildingComponent.Cast(current.FindComponent(SCR_DestructibleBuildingComponent));
		if (res)
			return res;
		
		IEntity parent = current.GetParent();
		// recursive call
		return FindDestructibleBuildingComp(parent);
	}
	
	//
	// ACCESSORS
	//
	void SetPopulatedTerritoryID(string populatedTerritoryID)
	{
		if (m_sPopulatedTerritoryID)
			return; // TODO warning?
		m_sPopulatedTerritoryID = populatedTerritoryID;
	}
	
	string GetPopulatedTerritoryID()
	{
		return m_sPopulatedTerritoryID;
	}
	
	int GetSpawnPointsCount()
	{
		return m_aSpawnPoints.Count();
	}
	
	int GetSpawnPoints(out array<ARGEO_PopulatedSpawnPointComponent> spawnPoints)
	{
		spawnPoints.InsertAll(m_aSpawnPoints);
		return m_aSpawnPoints.Count();
	}
}