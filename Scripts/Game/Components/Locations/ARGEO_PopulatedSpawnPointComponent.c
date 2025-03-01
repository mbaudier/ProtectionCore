class ARGEO_PopulatedSpawnPointComponentClass : SCR_AmbientPatrolSpawnPointComponentClass
{
}

class ARGEO_PopulatedSpawnPointComponent : SCR_AmbientPatrolSpawnPointComponent
{
	protected float m_fDefaultAILimitThreshold = 0.95;
	
	protected AIWaypoint m_ToHomeWP;
	protected AIWaypointCycle m_DailyLifeWP;

	protected SCR_TimedWaypoint m_StayAtHomeWP;

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_fDefaultAILimitThreshold = m_fAILimitThreshold;
		//DisableSpawn();// disable spawn by default
		//Print("Populated spawnpoint initialized");
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		//Print("Populated spawnpoint post-init");
	}

	override void PrepareWaypoints()
	{
		super.PrepareWaypoints();
		
		// !! we need at least two waypoints in prefabs hierarchy so that a cycle waypoint is created
		AIWaypointCycle wp = AIWaypointCycle.Cast(m_Waypoint);
		if(wp)
		{	
			array<AIWaypoint> waypoints = {};
			wp.GetWaypoints(waypoints);
			
			// TODO make it more robust
			m_ToHomeWP = waypoints[0];
			m_DailyLifeWP = AIWaypointCycle.Cast(waypoints[1]);
			
			// checks
			SCR_AmbientPatrolSpawnPointComponentClass componentData = SCR_AmbientPatrolSpawnPointComponentClass.Cast(GetComponentData(GetOwner()));
			if (!componentData)
				return;			
			Resource waypointResource = Resource.Load(componentData.GetDefaultWaypointPrefab());			
			if (!waypointResource || !waypointResource.IsValid())
				return;
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = GetOwner().GetOrigin();
			
			m_StayAtHomeWP = SCR_TimedWaypoint.Cast(GetGame().SpawnEntityPrefab(waypointResource, null, params));
			
			float meanTimeAtHome = 380;
			float timeAtHome =  Math.RandomGaussFloat(Math.Sqrt(meanTimeAtHome), meanTimeAtHome);
			m_StayAtHomeWP.SetHoldingTime(timeAtHome);
			// stay at home is before daily life
			waypoints.InsertAt(m_StayAtHomeWP, 1);
			wp.SetWaypoints(waypoints);
		}
	}

	override void SpawnPatrol()
	{
		super.SpawnPatrol();
		//Print("Populated spawn point - " + GetOwner().GetID() + " - spawned " + m_SavedFaction.GetFactionKey());

		// start deactivated
		//super.DeactivateGroup();
	}
	
	override void DespawnPatrol()
	{
		super.DespawnPatrol();
		Print("Populated spawn point - " + GetOwner().GetID() + " - despawned ");
	}
	
	override void ActivateGroup()
	{
		super.ActivateGroup();
				
		Print("Populated spawn point - " + GetOwner().GetID() + " - activated " + m_SavedFaction.GetFactionKey());
	}
	
	override void DeactivateGroup()
	{
		super.DeactivateGroup();
		if(m_Group)
			Print("Populated spawn point - " + GetOwner().GetID() + " - deactivated " + m_SavedFaction.GetFactionKey());
	}
	
	void DisableSpawn()
	{
		SetAILimitThreshold(0);
	}
	
	void EnableSpawn()
	{
		SetAILimitThreshold(m_fDefaultAILimitThreshold);
		if(m_iMembersAlive == 0)
		{	
			// workaround for when bases cleared it as remnants
			m_iMembersAlive = -1;
		}
	}
	
	protected void SetAILimitThreshold(float threshold)
	{
		m_fAILimitThreshold = threshold;
	}
}