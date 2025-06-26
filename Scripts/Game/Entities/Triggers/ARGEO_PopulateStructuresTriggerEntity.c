[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateStructuresTriggerEntityClass : ScriptedGameTriggerEntityClass
{
}

class ARGEO_PopulateStructuresTriggerEntity : ScriptedGameTriggerEntity
{
	//private static ref RandomGenerator s_Rng = new RandomGenerator();
	//private static int s_iTotalPopulation = 0;

	[Attribute(desc: "ID of the populated territory to attach to.", category: "Population")]
	protected string m_sPopulatedTerritoryID;

	// TODO use FactionAffiliationComponent
//	[Attribute(desc: "Force population to this faction.", category: "Population")]
//	protected FactionKey m_sFactionKey;
	
	
	//private int m_iPopulationCount = 0;
	
	private string m_sLocationName;
	

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
/*		
		float buildingsOccupation = populationComponent.GetGlobalBuildingsOccupation();
		
		if(!m_sFactionKey)
			m_sFactionKey = populationComponent.GetDefaultCivilianFactionKey();

		// Find ambient spawn points
		array<ARGEO_PopulatedSpawnPointComponent> spawnPoints = {};
		
		array<IEntity> queue = {ent};
		ARGEO_PopulatedSpawnPointComponent spawnPoint;
		IEntity processedEntity;
		IEntity nextInHierarchy;
		
		while (!queue.IsEmpty())
		{
			processedEntity = queue[0];
			queue.Remove(0);
			
			spawnPoint = ARGEO_PopulatedSpawnPointComponent.Cast(processedEntity.FindComponent(ARGEO_PopulatedSpawnPointComponent));
			 
			if (spawnPoint) {
				
				FactionKey factionKey = m_sFactionKey;	
				bool enableSpawn = false;
				if(buildingsOccupation >= 100)
				{
					// TODO deal with overpopulation
					enableSpawn = true;
				}
				else
				{
					float rnd = s_Rng.RandFloat01() * 100;
					//int rnd = s_Rng.RandInt(0, 100);
					// strictly inferior, otherwise occupation 0 does not disable
					enableSpawn = (rnd < buildingsOccupation);
				}
				
				SCR_FactionAffiliationComponent factionAffiliation = SCR_FactionAffiliationComponent.Cast(processedEntity.FindComponent(SCR_FactionAffiliationComponent));		
				if (factionAffiliation)
				{
					factionAffiliation.SetAffiliatedFactionByKey(factionKey);		
				}
				
				if(enableSpawn)
				{
					spawnPoint.EnableSpawn();
					spawnPoints.Insert(spawnPoint);
				}
				else
				{
					spawnPoint.DisableSpawn();
					// FIXME hack until we understand how to deactivate properly
					//processedEntity.GetParent().RemoveChild(processedEntity);
				}
				
				nextInHierarchy = NULL;// no need to go deeper
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

		foreach(ARGEO_PopulatedSpawnPointComponent sp: spawnPoints)
		{
			// TODO setup waypoints, etc.
			// TODO count and track population
			m_iPopulationCount++;
		}
*/	
	}
	
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		//s_iTotalPopulation = s_iTotalPopulation + m_iPopulationCount;
		//Print("Populate structures trigger - " + m_sLocationName + " : " + m_iPopulationCount + " people " + m_sFactionKey+ " - Total: " + s_iTotalPopulation);
		EnablePeriodicQueries(false);
	}

	override event protected event void OnInit(IEntity owner)
	{
		string parentName = "N/A";
		IEntity parent = owner.GetParent();
		if (parent)
		{
			parentName = parent.GetName();
		}
		m_sLocationName = parentName;
		if (!m_sPopulatedTerritoryID)
			m_sPopulatedTerritoryID = m_sLocationName;
	}
		
//	void SetFactionKey(FactionKey factionKey)
//	{
//		m_sFactionKey = factionKey;
//		//Print("Populated trigger set faction key "+m_sFactionKey);
//	}
}
