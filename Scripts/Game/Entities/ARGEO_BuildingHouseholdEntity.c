class ARGEO_BuildingHouseholdEntityClass: ARGEO_BuildingPopulationEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! The link between a given building and its inhabitants.
//! Members of an household will also flee together as a group if they have to.
class ARGEO_BuildingHouseholdEntity: ARGEO_BuildingPopulationEntity
{
	[Attribute("{BBF1B3890A3FAFD7}Prefabs/AI/Groups/Displaced_Group.et", "General")]
	protected ResourceName m_sDisplacedGroupPrefab;
	
	[Attribute("{B049D4C74FBC0C4D}Prefabs/AI/Waypoints/AIWaypoint_GetInNearest.et", desc:"Find a vehicle near the household", category: "Waypoints Prefabs")]
	protected ResourceName m_sFindVehicleWaypointPrefab;
	
	private ref array<ARGEO_PopulatedSpawnPointComponent> m_aSpawnPoints = new array<ARGEO_PopulatedSpawnPointComponent>;

	// Household is displaced together (also for performance reasons, reducing the number of active groups)
	protected SCR_AIGroup m_DisplacedGroup = null;
	
	protected SCR_AIWaypoint m_FindVehicleWP;
	
	//
	// LIFECYCLE
	//
	
	//------------------------------------------------------------------------------------------------
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
			 
			if (spawnPoint)
			{
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
		
		// Waypoints
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = this.GetOrigin();
		m_FindVehicleWP = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sFindVehicleWaypointPrefab), null, params));
	}
	
	//
	// EVENTS
	//
	
	//------------------------------------------------------------------------------------------------
	//! Notified when the safety status of this household's territory has changed.
	void OnSafetyStatusChanged(ARGEO_EPopulationSafetyStatus safetyStatus)
	{
		if (HasFled())
			return;
		
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp)
			return;
		
		bool shouldFlee = populationComp.ShouldFlee(this);
		if (shouldFlee)
		{
			foreach (ARGEO_PopulatedSpawnPointComponent spawnPoint : m_aSpawnPoints)
			{
				AIAgent agent = spawnPoint.GetAgent();
				ARGEO_CharacterProtectionComponent characterComp = ARGEO_CharacterProtectionComponent.FindFromAgent(agent);
				if (!characterComp || characterComp.GetDisplacementStatus() < ARGEO_ECharacterDisplacementStatus.NORMAL)
					continue; // m_aSpawnPoints
				
				if (!m_DisplacedGroup) {
					m_DisplacedGroup = CreateDisplacedGroup();
					if (!m_DisplacedGroup)
					{
						Print("Cannot create displaced group", LogLevel.ERROR);
					}
					
					UpdateFleeingTarget(true);
				}
				// We assume it will be removed from ambient group
				// TODO verify the assumption
				m_DisplacedGroup.AddAgent(agent);
				m_DisplacedGroup.ActivateAllMembers();
				characterComp.SetDisplacementStatus(ARGEO_ECharacterDisplacementStatus.FLEEING, null);
				
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notified when a new civic center has been created.
	void OnCivicCenterCreated(ARGEO_CivicCenterEntity civicCenter)
	{
		if (m_DisplacedGroup)
			UpdateFleeingTarget();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notified when a civic center has been destroyed.
	void OnCivicCenterDestroyed(ARGEO_CivicCenterEntity civicCenter)
	{
		if (m_DisplacedGroup)
			UpdateFleeingTarget();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Possibly update the current fleeing target, typically if a civic center has been built nearby.
	protected void UpdateFleeingTarget(bool initFlight = false)
	{
		if (!m_DisplacedGroup)
			return;
		
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp)
			return;
		
		ARGEO_CivicCenterEntity civicCenter = populationComp.GetNearestCivicCenter(m_DisplacedGroup.GetCenterOfMass());
		if (civicCenter)
		{
			SCR_AIWaypoint fleeTo = civicCenter.GetFleeToWP();
			if (fleeTo != m_DisplacedGroup.GetCurrentWaypoint())
			{
				ClearGroupWPs(m_DisplacedGroup);
				
				if (initFlight)
					m_DisplacedGroup.AddWaypoint(m_FindVehicleWP);
				m_DisplacedGroup.AddWaypoint(fleeTo);

				Print("Civilian household fleeing to " + fleeTo.GetOrigin(), LogLevel.NORMAL);
			}
		}
		else // no civic center available
		{
			// TODO scatter them?
			ClearGroupWPs(m_DisplacedGroup);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Creates the group that will be used by this houshold while being displaced.
	protected SCR_AIGroup CreateDisplacedGroup()
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = this.GetOrigin();

		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sDisplacedGroupPrefab), null, params);
		if (!groupEntity)
			return null;
		
		SCR_AIGroup group = SCR_AIGroup.Cast(groupEntity);
		if (!group)
			return null;
		return group;
	}
	
	//
	// UTILITIES
	//
	
	//------------------------------------------------------------------------------------------------
	private void ClearGroupWPs(SCR_AIGroup group)
	{
		array<AIWaypoint> wps = {};
		group.GetWaypoints(wps);
		foreach (AIWaypoint wp : wps)
		{	
			group.RemoveWaypoint(wp);			
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
	
	bool HasFled()
	{
		return m_DisplacedGroup != null;
	}	
}
