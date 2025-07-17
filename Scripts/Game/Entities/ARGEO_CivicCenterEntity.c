class ARGEO_CivicCenterEntityClass: ARGEO_BuildingPopulationEntityClass
{
}

//! The link between people and a given building.
class ARGEO_CivicCenterEntity: ARGEO_BuildingPopulationEntity
{
	protected float m_iDischargeRadius = 200;
	
	[Attribute("0", desc: "Force composition to immediately register", category: "Logic")]
	protected bool m_bForceRegister;

	[Attribute("{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et", desc:"Destination to move to", category: "Prefabs")]
	protected ResourceName m_sMoveToWaypointPrefab;


	private SCR_AIWaypoint m_MoveTo;

	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		// Target waypoint for fleeing civilians
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = this.GetOrigin();
		m_MoveTo = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sMoveToWaypointPrefab), null, params));

		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		
		if (m_bForceRegister || !m_CampaignBuildingCompositionComp)
			populationComp.RegisterCivicCenter(this);
	}

	//
	// EVENTS
	//
	override void OnBuildingDestroyed(EDamageState state)
	{
		super.OnBuildingDestroyed(state);

		if (state != EDamageState.DESTROYED)
			return;
		
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		populationComp.UnregisterCivicCenter(this);
	}

	override void OnCompositionSpawned(bool arg)
	{
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;

		populationComp.RegisterCivicCenter(this);
	}
	
	//
	// ACCESSOR
	//
	SCR_AIWaypoint GetMoveToWaypoint()
	{
		return m_MoveTo;
	}
	
	int GetDischargeRadius()
	{
		return m_iDischargeRadius;
	}
}
