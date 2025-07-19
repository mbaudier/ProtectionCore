class ARGEO_RegisterToCivicCenterWaypointClass: SCR_AIWaypointClass
{
};

class ARGEO_RegisterToCivicCenterWaypoint: SCR_AIWaypoint
{
	override SCR_AIWaypointState CreateWaypointState(SCR_AIGroupUtilityComponent groupUtilityComp)
	{
		SCR_AIWaypointState waypointState = new SCR_AIWaypointState(groupUtilityComp, this);
		return waypointState;
	}
	
};

class ARGEO_RegisterToCivicCenterWaypointState: SCR_AIWaypointState
{
	void ARGEO_RegisterToCivicCenterWaypointState(notnull SCR_AIGroupUtilityComponent utility, SCR_AIWaypoint waypoint)
	{
		SCR_AIWaypointState(utility, waypoint);
	}
	
	
	override void OnSelected()
	{
		super.OnSelected();
	}
	
	override void OnDeselected()
	{
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp)
			return;
		
		AIAgent agent = m_Utility.GetOwner();
		if (agent)
		{
			ARGEO_CivicCenterEntity civicCenter = populationComp.CanNonCombatantBeDischarged(agent.GetControlledEntity());
			if (civicCenter)
			{
				civicCenter.RegisterNonCombatant(agent.GetControlledEntity());
			}
		}

		super.OnDeselected();
	}
};