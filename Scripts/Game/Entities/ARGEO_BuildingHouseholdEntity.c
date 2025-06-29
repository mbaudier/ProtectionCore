class ARGEO_BuildingHouseholdEntityClass: ARGEO_BuildingPopulationEntityClass
{
}

//! The link between people and a given building.
class ARGEO_BuildingHouseholdEntity: ARGEO_BuildingPopulationEntity
{
	private ref array<ARGEO_PopulatedSpawnPointComponent> m_aSpawnPoints = new array<ARGEO_PopulatedSpawnPointComponent>;

	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);

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
	
	//
	// EVENTS
	//
	void OnSafetyStatusChanged(ARGEO_PopulationSafetyStatus safetyStatus)
	{
		if (safetyStatus <= ARGEO_PopulationSafetyStatus.DANGEROUS)
		{
			ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
			if (!populationComp)
				return;
			foreach (ARGEO_PopulatedSpawnPointComponent spawnPoint : m_aSpawnPoints)
			{
//				if (spawnPoint.GetMembersAlive() > 0)
//				{
					SCR_AIGroup group = spawnPoint.GetSpawnedGroup();
					if (group)
					{
						array<AIWaypoint> wps = {};
						group.GetWaypoints(wps);
						foreach (AIWaypoint wp : wps)
						{	
							group.RemoveWaypoint(wp);			
						}
						ARGEO_CivicCenterEntity civicCenter = populationComp.GetClosestCivicCenter(group.GetCenterOfMass());
						if (civicCenter)
						{
							AIWaypoint fleeTo = civicCenter.GetMoveToWaypoint();
							group.AddWaypoint(fleeTo);
							Print("Civilian fleeing to " + fleeTo.GetOrigin());
						}
					}
//				}
			}
		}
	}
	
	//
	// ACCESSORS
	//
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