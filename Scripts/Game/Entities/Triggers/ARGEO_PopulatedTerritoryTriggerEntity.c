[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulatedTerritoryTriggerEntityClass : ScriptedGameTriggerEntityClass
{
}

//------------------------------------------------------------------------------------------------
//! A trigger related to a populated territory and responsible for populating the buildings.
class ARGEO_PopulatedTerritoryTriggerEntity : ScriptedGameTriggerEntity
{
	[Attribute(desc: "ID of the populated territory to attach to.", category: "Population")]
	protected ARGEO_PopulatedTerritoryID m_sPopulatedTerritoryID;	

	[Attribute("{C72F956E4AC6A6E7}Prefabs/Systems/ScenarioFramework/Components/Area.et", "Scenario Framework")]
	protected ResourceName m_sAreaPrefab;

	[Attribute("{5F9FFF4BF027B3A3}Prefabs/Systems/ScenarioFramework/Components/Layer.et", "Scenario Framework")]
	protected ResourceName m_sLayerPrefab;

	[Attribute("{2956EC953C36C760}Prefabs/Systems/ScenarioFramework/Components/SlotPerson.et", "Scenario Framework")]
	protected ResourceName m_sSlotPersonPrefab;
	
	protected SCR_ScenarioFrameworkArea m_SFArea;
	protected SCR_ScenarioFrameworkLayerBase m_SFLayer;
	protected IEntity m_SFLayerEntity;

	//
	// LIFECYCLE
	//
	
	//------------------------------------------------------------------------------------------------
	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		// Scenario Framework
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = this.GetOrigin();
		
		IEntity area = GetGame().SpawnEntityPrefab(Resource.Load(m_sAreaPrefab), null, params);
		m_SFArea = SCR_ScenarioFrameworkArea.Cast(area.FindComponent(SCR_ScenarioFrameworkArea));
		if (m_SFArea)
		{
			m_SFArea.Init();
			m_SFLayerEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sLayerPrefab), null, params);
			if (m_SFLayerEntity)
			{
				area.AddChild(m_SFLayerEntity, -1);
				m_SFLayer = SCR_ScenarioFrameworkLayerBase.Cast(m_SFLayerEntity.FindComponent(SCR_ScenarioFrameworkLayerBase));
				m_SFLayer.Init(m_SFArea);
				// TODO optimize
				//m_SFArea.DynamicReinit();
			}
		}
	}
	
	//
	// EVENTS
	//
	//------------------------------------------------------------------------------------------------
	//! During the first activation, configures the structural population-related entities,
	//! such as housholds and ambient vehicles.
	//! Then monitor for populated territory events such as war crimes.
	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		
		ARGEO_PopulationComponent populationComponent = ARGEO_PopulationComponent.GetInstance();
		if (!populationComponent) // typically in workbench
			return;
		
		//
		// Structural - Happens only once
		//
		
		// Check if faction forced
		FactionKey triggerFactionKey;
		FactionAffiliationComponent triggerFactionAffiliation = FactionAffiliationComponent.Cast(this.FindComponent(FactionAffiliationComponent));
		if (triggerFactionAffiliation)
		{
			triggerFactionKey = triggerFactionAffiliation.GetAffiliatedFactionKey();
		}

		// Structural entities
		ARGEO_BuildingHouseholdEntity buildingHousehold = ARGEO_BuildingHouseholdEntity.Cast(ent);
		if (buildingHousehold && !buildingHousehold.GetPopulatedTerritoryID())
		{	
			if (triggerFactionKey)
			{
				array<ARGEO_PopulatedSpawnPointComponent> spawnPoints = {};
				buildingHousehold.GetSpawnPoints(spawnPoints);
				foreach (ARGEO_PopulatedSpawnPointComponent spawnPoint : spawnPoints)
				{
					FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(spawnPoint.GetOwner().FindComponent(FactionAffiliationComponent));		
					populationComponent.OptionallySetPopulationFactionByKey(factionAffiliation, triggerFactionKey);
				}
			}
			buildingHousehold.SetPopulatedTerritoryID(m_sPopulatedTerritoryID);
			populationComponent.RegisterBuildingHousehold(buildingHousehold);
			return;
		}

		//ARGEO_PopulatedBedEntity bed = ARGEO_PopulatedBedEntity.Cast(ent);
		ARGEO_PopulatedBedEntity bed = null;
		if (bed && !bed.GetPopulatedTerritoryID())
		{	
			Print("Found bed in " + m_sPopulatedTerritoryID);
			bed.SetPopulatedTerritoryID(m_sPopulatedTerritoryID);
			
			if (m_SFLayerEntity)
			{
				// Scenario Framework
				EntitySpawnParams params = EntitySpawnParams();
				params.TransformMode = ETransformMode.WORLD;
				params.Transform[3] = ent.GetOrigin();
				
				IEntity person = GetGame().SpawnEntityPrefab(Resource.Load(m_sSlotPersonPrefab), null, params);
				m_SFLayerEntity.AddChild(person, -1);
				SCR_ScenarioFrameworkLayerBase personSlot =  SCR_ScenarioFrameworkLayerBase.Cast(person.FindComponent(SCR_ScenarioFrameworkLayerBase));
				personSlot.Init(m_SFArea);
				// TODO optimize
				//m_SFArea.DynamicReinit();
			}

//			populationComponent.RegisterBuildingHousehold(buildingHousehold);
			return;
		}
		
		// Ambient vehicle
		SCR_AmbientVehicleSpawnPointComponent vehicleSpawnPoint = SCR_AmbientVehicleSpawnPointComponent.Cast(ent.FindComponent(SCR_AmbientVehicleSpawnPointComponent));
		if (vehicleSpawnPoint)
		{
			if (triggerFactionKey)
			{
				FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(vehicleSpawnPoint.GetOwner().FindComponent(FactionAffiliationComponent));		
				populationComponent.OptionallySetPopulationFactionByKey(factionAffiliation, triggerFactionKey);
			}
			populationComponent.RegisterAmbientVehicle(m_sPopulatedTerritoryID, vehicleSpawnPoint);
		}
		
		//
		// Monitoring - Can happen any time
		//
		
		// War crime entities
		ARGEO_WarCrimeEntity warCrimeEntity = ARGEO_WarCrimeEntity.Cast(ent);
		if (warCrimeEntity)
		{
			OnNewWarCrime(warCrimeEntity);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notified when a war crime entity has appeared in this trigger.
	protected void OnNewWarCrime(notnull ARGEO_WarCrimeEntity warCrimeEntity)
	{
		Print("New war crime in " + m_sPopulatedTerritoryID);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Unused for the time being.
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		//EnablePeriodicQueries(false);
	}
}
