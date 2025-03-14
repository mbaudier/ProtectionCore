[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateStructuresTriggerEntityClass : SCR_BaseTriggerEntityClass
{
}

class ARGEO_PopulateStructuresTriggerEntity : SCR_BaseTriggerEntity
{
	[Attribute(desc: "Faction which will be used to populate. Leave empty for default civilians.", category: "Population")]
	protected FactionKey m_sFactionKey;
	
	private int m_iPopulationCount = 0;

	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);

		if(!m_sFactionKey)
			m_sFactionKey = "CIV";

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
				bool enableSpawn = true;

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
