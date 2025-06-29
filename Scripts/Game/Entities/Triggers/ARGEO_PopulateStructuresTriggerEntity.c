[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateStructuresTriggerEntityClass : ScriptedGameTriggerEntityClass
{
}

class ARGEO_PopulateStructuresTriggerEntity : ScriptedGameTriggerEntity
{
	[Attribute(desc: "ID of the populated territory to attach to.", category: "Population")]
	protected string m_sPopulatedTerritoryID;	

	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		
		FactionKey triggerFactionKey;
		FactionAffiliationComponent triggerFactionAffiliation = FactionAffiliationComponent.Cast(this.FindComponent(FactionAffiliationComponent));
		if (triggerFactionAffiliation)
		{
			triggerFactionKey = triggerFactionAffiliation.GetAffiliatedFactionKey();
		}
		
		ARGEO_BuildingHouseholdEntity buildingHousehold = ARGEO_BuildingHouseholdEntity.Cast(ent);
		if (!buildingHousehold || buildingHousehold.GetPopulatedTerritoryID())
			return;
		buildingHousehold.SetPopulatedTerritoryID(m_sPopulatedTerritoryID);

		ARGEO_PopulationComponent populationComponent = ARGEO_PopulationComponent.GetInstance();
		populationComponent.RegisterBuildingHousehold(buildingHousehold);
		if (triggerFactionKey)
		{
			array<ARGEO_PopulatedSpawnPointComponent> spawnPoints = {};
			buildingHousehold.GetSpawnPoints(spawnPoints);
			foreach (ARGEO_PopulatedSpawnPointComponent spawnPoint : spawnPoints)
			{
				FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(spawnPoint.GetOwner().FindComponent(FactionAffiliationComponent));		
				if (factionAffiliation)
				{
					factionAffiliation.SetAffiliatedFactionByKey(triggerFactionKey);
				}
			}
		}
	}
	
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		//s_iTotalPopulation = s_iTotalPopulation + m_iPopulationCount;
		//Print("Populate structures trigger - " + m_sLocationName + " : " + m_iPopulationCount + " people " + m_sFactionKey+ " - Total: " + s_iTotalPopulation);
		EnablePeriodicQueries(false);
	}

}
