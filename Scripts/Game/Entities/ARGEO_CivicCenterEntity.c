class ARGEO_CivicCenterEntityClass: ARGEO_BuildingPopulationEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! A building (possibly temporary) where internally displaced persons and prisoners of war
//! can be discharged, registered and managed.
class ARGEO_CivicCenterEntity: ARGEO_BuildingPopulationEntity
{
	protected float m_iDischargeRadius = 200;
	
	[Attribute("0", desc: "Delay the registration as civic center until the editable building is built", category: "Logic")]
	protected bool m_bDelayRegistration;

	[Attribute("{4080EBB873179873}Prefabs/AI/Waypoints/AIWaypoint_RegisterToCivicCenter.et", desc:"Register to this civic center", category: "Waypoints Prefabs")]
	protected ResourceName m_sRegisterWaypointPrefab;

	[Attribute("{FAD1D789EE291964}Prefabs/AI/Waypoints/AIWaypoint_Defend_Large.et", desc:"Loiter near the civic center", category: "Waypoints Prefabs")]
	protected ResourceName m_sLoiterWaypointPrefab;

	[Attribute("{531EC45063C1F57B}Prefabs/AI/Waypoints/AIWaypoint_Wait.et", desc:"Wait near the civic center", category: "Waypoints Prefabs")]
	protected ResourceName m_sWaitWaypointPrefab;

	[Attribute("{C40316EE26846CAB}Prefabs/AI/Waypoints/AIWaypoint_GetOut.et", desc:"Get out of vehicle", category: "Waypoints Prefabs")]
	protected ResourceName m_sLeaveVehicleWaypointPrefab;

	protected SCR_AIWaypoint m_RegisterWP;
	
	protected SCR_AIWaypoint m_LoiterWP;
	
	protected SCR_AIWaypoint m_WaitWP;
	
	protected SCR_AIWaypoint m_LeaveVehicleWP;
	
	protected ref array<SCR_ChimeraCharacter> m_aRegisteredNonCombatants = new array<SCR_ChimeraCharacter>;
	
	protected ref array<SCR_ChimeraCharacter> m_aRegisteredPrisoners = new array<SCR_ChimeraCharacter>;
	
	//
	// LIFECYCLE
	//

	//------------------------------------------------------------------------------------------------
	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		// Waypoints
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = this.GetOrigin();
		m_RegisterWP = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sRegisterWaypointPrefab), null, params));
		m_LoiterWP = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sLoiterWaypointPrefab), null, params));
		m_WaitWP = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sWaitWaypointPrefab), null, params));
		m_LeaveVehicleWP = SCR_AIWaypoint.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sLeaveVehicleWaypointPrefab), null, params));

		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		
		if (!m_bDelayRegistration)
			populationComp.RegisterCivicCenter(this);
	}
	
	//
	// NON-COMBATANTS MANAGEMENT
	//
	
	//------------------------------------------------------------------------------------------------
	//! Registers a displaced person to this civic center.
	void RegisterNonCombatant(IEntity nonCombatant)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(nonCombatant);
		if (!character)
			return;
		
		m_aRegisteredNonCombatants.Insert(character);
		
		ARGEO_CharacterProtectionComponent characterProtectionComp = ARGEO_CharacterProtectionComponent.Cast(character.FindComponent(ARGEO_CharacterProtectionComponent));
		if (characterProtectionComp)
			characterProtectionComp.SetDisplacementStatus(ARGEO_ECharacterDisplacementStatus.DISPLACED, this);
		
		AIControlComponent aiControlComp = AIControlComponent.Cast(character.FindComponent(AIControlComponent));
		if (aiControlComp)
		{
			AIAgent agent = aiControlComp.GetAIAgent();
			if (agent)
			{
				// we assume that an agent always have a group
				agent.GetParentGroup().AddWaypoint(m_LeaveVehicleWP);
				agent.GetParentGroup().AddWaypoint(m_LoiterWP);
			}
		}

	}

	//
	// PRISONERS MANAGEMENT
	//
	
	//------------------------------------------------------------------------------------------------
	//! Registers a prisoner of war to this civic center.
	void RegisterPrisoner(IEntity prisoner)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(prisoner);
		if (!character)
			return;
		
		m_aRegisteredPrisoners.Insert(character);
		
		ARGEO_CharacterProtectionComponent characterProtectionComp = ARGEO_CharacterProtectionComponent.Cast(character.FindComponent(ARGEO_CharacterProtectionComponent));
		if (characterProtectionComp)
			characterProtectionComp.SetPOWStatus(true, this);
		
		// TODO make it disappear after a while
		AIControlComponent aiControlComp = AIControlComponent.Cast(character.FindComponent(AIControlComponent));
		if (aiControlComp)
		{
			AIAgent agent = aiControlComp.GetAIAgent();
			if (agent)
			{
				// we assume that an agent always havea group
				agent.GetParentGroup().AddWaypoint(m_WaitWP);
			}
		}
	}

	//
	// EVENTS
	//
	
	//------------------------------------------------------------------------------------------------
	override void OnBuildingDestroyed(EDamageState state)
	{
		super.OnBuildingDestroyed(state);

		if (state != EDamageState.DESTROYED)
			return;
		
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		populationComp.NotifyCivicCenterDestroyed(this);
	}

	//------------------------------------------------------------------------------------------------
	//! If delayed registration is enabled, the civic center will be usable only at this stage,
	//! when it has been effectively built.
	override void OnCompositionSpawned(bool arg)
	{
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;

		if(m_bDelayRegistration)
			populationComp.RegisterCivicCenter(this);
	}
	
	//
	// ACCESSORS
	//
	
	SCR_AIWaypoint GetFleeToWP()
	{
		return m_RegisterWP;
	}
	
	//------------------------------------------------------------------------------------------------
	//! The maximum distance from which protected persons can be discharged.
	int GetDischargeRadius()
	{
		return m_iDischargeRadius;
	}
}
