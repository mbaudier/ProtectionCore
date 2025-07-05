class ARGEO_CivicCenterEntityClass: ARGEO_BuildingPopulationEntityClass
{
}

//! The link between people and a given building.
class ARGEO_CivicCenterEntity: ARGEO_BuildingPopulationEntity
{
	private ref array<SCR_AIWaypoint> m_aWaypoints = new array<SCR_AIWaypoint>;
	private AIWaypoint m_MoveTo;

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
			
			SCR_AIWaypoint waypoint = SCR_AIWaypoint.Cast(processedEntity);
			if (!m_MoveTo)
			{
				// TODO deal with other kind of waypoints
				m_MoveTo = waypoint
			}
			 
			if (waypoint) {
				m_aWaypoints.Insert(waypoint);
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
		
//		if (!m_MoveTo)
//			m_MoveTo = new SCR_AIWaypoint();

		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		populationComp.RegisterCivicCenter(this);
	}

	//
	// EVENTS
	//
	override void OnBuildingDestroyed(EDamageState state)
	{
		super.OnBuildingDestroyed(state);
		
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		populationComp.UnregisterCivicCenter(this);
	}
	
	//
	// ACCESSOR
	//
	AIWaypoint GetMoveToWaypoint()
	{
		return m_MoveTo;
	}
}
