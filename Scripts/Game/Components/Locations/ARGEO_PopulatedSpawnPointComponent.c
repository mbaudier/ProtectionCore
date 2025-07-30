class ARGEO_PopulatedSpawnPointComponentClass : SCR_AmbientPatrolSpawnPointComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Population as single-person ambient patrols.
class ARGEO_PopulatedSpawnPointComponent : SCR_AmbientPatrolSpawnPointComponent
{
	protected AIWaypoint m_ToHomeWP;
	
	protected AIWaypointCycle m_DailyLifeWP;

	protected SCR_TimedWaypoint m_StayAtHomeWP;
	
	private bool m_bSpawnEnabled = false;
	
	protected AIAgent m_Person;
	
	//
	// AMBIENT PATROL
	//
	
	//------------------------------------------------------------------------------------------------
	override void SpawnPatrol()
	{
		super.SpawnPatrol();
		
		if (!m_Group)
			return;
		m_Group.GetOnAgentAdded().Insert(OnAgentAdded);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called when an AI agent is effectively added. Should happen only once.
	protected void OnAgentAdded(AIAgent agent)
	{
		if (m_Person)
		{
			Print("Person agent is already set", LogLevel.ERROR);
			return;
		}
		m_Person = agent;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Whether this spawn point will be populated.
	bool IsSpawnEnabled()
	{
		return m_bSpawnEnabled;
	}
	
	//------------------------------------------------------------------------------------------------
	//! This spawn point won't be populated.
	void DisableSpawn()
	{
		SCR_AmbientPatrolSystem ambientPatrolSystem = SCR_AmbientPatrolSystem.GetInstance();
		if (ambientPatrolSystem)
			ambientPatrolSystem.UnregisterPatrol(this);
		m_bSpawnEnabled = false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! This spawn point will be populated.
	void EnableSpawn()
	{
		SCR_AmbientPatrolSystem ambientPatrolSystem = SCR_AmbientPatrolSystem.GetInstance();
		if (ambientPatrolSystem)
			ambientPatrolSystem.RegisterPatrol(this);
		m_bSpawnEnabled = true;
		
		if(m_iMembersAlive == 0)
		{	
			// workaround for when bases cleared it as remnants
			m_iMembersAlive = -1;
		}
	}
	
	//
	// DAILY LIFE
	//
	
	//------------------------------------------------------------------------------------------------
	//! Prepare waypoints so that the population can perform various activites in a peaceful context.
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
	
	//
	// ACCESSORS
	//
	
	//------------------------------------------------------------------------------------------------
	//! The single agent related to this spawnpoint, or null if there is none.
	AIAgent GetAgent()
	{
		SCR_ChimeraAIAgent chimeraAgent = SCR_ChimeraAIAgent.Cast(m_Person);
		return m_Person;
	}
}