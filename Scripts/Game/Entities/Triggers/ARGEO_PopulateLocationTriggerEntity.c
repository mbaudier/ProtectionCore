[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateLocationTriggerEntityClass : ARGEO_PopulateStructuresTriggerEntityClass
{
}

class ARGEO_PopulateLocationTriggerEntity : ARGEO_PopulateStructuresTriggerEntity
{
	private string m_sLocationName;
	
	private ref ARGEO_PopulatedTerritory m_PopulatedTerritory;
	
	private EventHandlerManagerComponent m_EventHandlerMgr;

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
			
		string parentName = "N/A";
		IEntity parent = owner.GetParent();
		if (parent)
		{
			parentName = parent.GetName();
		}
		m_sLocationName = parentName;
		if (!m_sPopulatedTerritoryID)
			m_sPopulatedTerritoryID = m_sLocationName;

		m_EventHandlerMgr = EventHandlerManagerComponent.Cast(owner.FindComponent(EventHandlerManagerComponent));
	}

	override void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp) // typically in workbnech
			return;
		
		// FIXME better deal with lifecycle and init
		m_PopulatedTerritory = populationComp.GetPopulatedTerritory(m_sPopulatedTerritoryID);
		if (m_PopulatedTerritory && m_EventHandlerMgr)
		{
			m_PopulatedTerritory.SetEventHandler(m_EventHandlerMgr);
		}
	}
}
