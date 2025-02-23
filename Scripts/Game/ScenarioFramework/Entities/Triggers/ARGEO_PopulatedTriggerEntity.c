[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulatedTriggerEntityClass : SCR_ScenarioFrameworkTriggerEntityClass
{
}

class ARGEO_PopulatedTriggerEntity : SCR_ScenarioFrameworkTriggerEntity
{
	
	private FactionKey m_sFactionKey;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		//Print("Populated trigger initialized");
	}

	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		//Print("Populated trigger activated "+ent);
		
		FactionKey factionKey = m_sFactionKey;
		
		bool enableSpawn = true;
		
		if(enableSpawn)
		{
			SCR_FactionAffiliationComponent factionAffiliation = SCR_FactionAffiliationComponent.Cast(ent.FindComponent(SCR_FactionAffiliationComponent));		
			if (factionAffiliation)
			{
				factionAffiliation.SetAffiliatedFactionByKey(factionKey);		
				//Print("Populated trigger - " + ent.GetID() + " - set faction " + factionKey);
			}

			ARGEO_PopulatedSpawnPointComponent populatedSpawnPoint = ARGEO_PopulatedSpawnPointComponent.Cast(ent.FindComponent(ARGEO_PopulatedSpawnPointComponent));
			if(populatedSpawnPoint)
			{
				populatedSpawnPoint.EnableSpawn();
				//Print("Populated trigger - " + ent.GetID() + " - enable spawn of " + factionKey);
			}
			else
			{		
				Print("Populated trigger - " + ent.GetID() + " - " + ent);
			}
		}
	}
	
	void SetFactionKey(FactionKey factionKey)
	{
		m_sFactionKey = factionKey;
		//Print("Populated trigger set faction key "+m_sFactionKey);
	}
}
