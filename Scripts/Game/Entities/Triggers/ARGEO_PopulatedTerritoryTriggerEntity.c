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

	//------------------------------------------------------------------------------------------------
	//! During the first activation, configures the structural population-related entities,
	//! such as houshols and ambient vehicles.
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
