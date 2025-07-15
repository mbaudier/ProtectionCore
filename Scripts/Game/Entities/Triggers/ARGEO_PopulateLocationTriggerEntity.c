[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateLocationTriggerEntityClass : ARGEO_PopulateStructuresTriggerEntityClass
{
}

class ARGEO_PopulateLocationTriggerEntity : ARGEO_PopulateStructuresTriggerEntity
{
	private string m_sLocationName;
	private string m_sLocationPrefabName;
	
	private ref ARGEO_PopulatedTerritory m_PopulatedTerritory;
	
	private EventHandlerManagerComponent m_EventHandlerMgr;

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
			
		// Try to detect territory ID based on location prefab
		IEntity parent = owner.GetParent();
		if (parent)
		{
			m_sLocationName = parent.GetName();
			
			EntityPrefabData parentPrefabData = parent.GetPrefabData();
			if (parentPrefabData)
			{
				string fullName = parentPrefabData.GetPrefabName();
				int slashIndex = fullName.LastIndexOf("/");
				int dotIndex = fullName.LastIndexOf(".");
				if (dotIndex > slashIndex)
					m_sLocationPrefabName = fullName.Substring(slashIndex + 1, dotIndex - slashIndex - 1);
			}
		}
		
		if (!m_sPopulatedTerritoryID) // Everon
			m_sPopulatedTerritoryID = m_sLocationPrefabName;
		
		if (!m_sPopulatedTerritoryID) // Arland
			m_sPopulatedTerritoryID = m_sLocationName;
		
		// TODO Use home trritory config for the logic
		// TODO Use Levenstein, etc. distances provided by SCR_StringHelper?

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
