class ARGEO_PopulatedSpawnPointComponentClass : SCR_AmbientPatrolSpawnPointComponentClass
{
}

class ARGEO_PopulatedSpawnPointComponent : SCR_AmbientPatrolSpawnPointComponent
{
	protected AIWaypoint m_ToHomeWP;
	protected AIWaypointCycle m_DailyLifeWP;

	protected SCR_TimedWaypoint m_StayAtHomeWP;
	
	private bool m_bSpawnEnabled = false;
	
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

	override void ActivateGroup()
	{
		// since there may be a lot of civilians in the same area, smooth activation
		bool smoothActivation = false;
		if (smoothActivation)
		{
			int randomDelay = Math.RandomInt(0, 5000);
			GetGame().GetCallqueue().CallLater(DoActivateGroup, randomDelay, false);
		}
		else
		{
			DoActivateGroup();
		}
	}
	
	private void DoActivateGroup()
	{
		super.ActivateGroup();
		Print("Populated spawn point - " + GetOwner().GetID() + " - activated", LogLevel.DEBUG);
	}

	override void DeactivateGroup()
	{
		super.DeactivateGroup();
		Print("Populated spawn point - " + GetOwner().GetID() + " - deactivated", LogLevel.DEBUG);
	}
		
	bool IsSpawnEnabled()
	{
		return m_bSpawnEnabled;
	}
	
	void DisableSpawn()
	{
		SCR_AmbientPatrolSystem ambientPatrolSystem = SCR_AmbientPatrolSystem.GetInstance();
		if (ambientPatrolSystem)
			ambientPatrolSystem.UnregisterPatrol(this);
		m_bSpawnEnabled = false;
	}
	
	void EnableSpawn()
	{
		SCR_AmbientPatrolSystem ambientPatrolSystem = SCR_AmbientPatrolSystem.GetInstance();
		if (ambientPatrolSystem)
			ambientPatrolSystem.RegisterPatrol(this);
		m_bSpawnEnabled = true;
		
		// add random delay so that not all civilians spawn at the same time
//		ChimeraWorld world =  GetOwner().GetWorld();
//		WorldTimestamp currentTime = world.GetServerTimestamp();
//		int randomDelay = Math.RandomInt(5,30);
//		SetRespawnTimestamp(currentTime.PlusSeconds(randomDelay));
		
		if(m_iMembersAlive == 0)
		{	
			// workaround for when bases cleared it as remnants
			m_iMembersAlive = -1;
		}
	}
}