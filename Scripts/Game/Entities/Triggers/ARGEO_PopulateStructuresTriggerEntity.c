[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateStructuresTriggerEntityClass : SCR_BaseTriggerEntityClass
{
}

class ARGEO_PopulateStructuresTriggerEntity : SCR_BaseTriggerEntity
{
	static ref RandomGenerator s_Rng = new RandomGenerator();

	[Attribute(desc: "Faction which will be used to populate. Leave empty for default civilians.", category: "Population")]
	protected FactionKey m_sFactionKey;
	
	private int m_iPopulationCount = 0;

	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);

		ARGEO_PopulationComponent populationComponent = ARGEO_PopulationComponent.GetInstance();
		int buildingsOccupation = populationComponent.GetGlobalBuildingsOccupation();
		
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
				bool enableSpawn;
				if(buildingsOccupation >= 100)
				{
					// TODO deal with overpopulation
					enableSpawn = true;
				}
				else
				{
					int rnd = s_Rng.RandInt(0, 100);
					enableSpawn = (rnd <= buildingsOccupation);
				}
				
				spawnPoints.Insert(spawnPoint);
				SCR_FactionAffiliationComponent factionAffiliation = SCR_FactionAffiliationComponent.Cast(processedEntity.FindComponent(SCR_FactionAffiliationComponent));		
				if (factionAffiliation)
				{
					factionAffiliation.SetAffiliatedFactionByKey(factionKey);		
					//Print("Populated trigger - " + ent.GetID() + " - set faction " + factionKey);
				}
				
				if(enableSpawn)
				{
					spawnPoint.EnableSpawn();
				}
				else
				{
					spawnPoint.DisableSpawn();
				}

				// FIXME hack until we understand how to deactivate properly
				if(!enableSpawn)		
					processedEntity.GetParent().RemoveChild(processedEntity);
			}
			
			nextInHierarchy = processedEntity.GetChildren();
			

			
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

	}
	
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		Print("Populate structures trigger - " + m_iPopulationCount + " people " + m_sFactionKey);
		EnablePeriodicQueries(false);
	}

	void SetFactionKey(FactionKey factionKey)
	{
		m_sFactionKey = factionKey;
		//Print("Populated trigger set faction key "+m_sFactionKey);
	}
}
