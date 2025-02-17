[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulatedTriggerEntityClass : SCR_ScenarioFrameworkTriggerEntityClass
{
}

class ARGEO_PopulatedTriggerEntity : SCR_ScenarioFrameworkTriggerEntity
{
	
	FactionKey m_sFactionKey;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		Print("Populated trigger initialized");
	}

	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		Print("Populated trigger activated "+ent);
		
		SCR_FactionAffiliationComponent comp = SCR_FactionAffiliationComponent.Cast(ent.FindComponent(SCR_FactionAffiliationComponent));		
		if (!comp)
			return;
		if(m_sFactionKey)
			comp.SetAffiliatedFactionByKey(m_sFactionKey);		
	}
	
	void SetFactionKey(FactionKey factionKey)
	{
		m_sFactionKey = factionKey;
	}
}
