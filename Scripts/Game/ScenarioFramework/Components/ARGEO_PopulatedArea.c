[EntityEditorProps(category: "GameScripted/ScenarioFramework", description: "")]
class ARGEO_PopulatedAreaClass : SCR_ScenarioFrameworkAreaClass
{
}

class ARGEO_PopulatedArea : SCR_ScenarioFrameworkArea
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		//Print("Populated area initialized");
	}
	
	override void OnAreaTriggerActivated(IEntity entity)
	{
		//Print("Populated area trigger activated "+entity);
		super.OnAreaTriggerActivated(entity);
	}
	
	override void SpawnTrigger()
	{
		super.SpawnTrigger();
		//ARGEO_PopulateStructuresTriggerEntity populateStructureTrigger = ARGEO_PopulateStructuresTriggerEntity.Cast(GetTrigger());		
		ARGEO_PopulateStructuresTriggerEntity populateStructureTrigger = NULL;		
		if (!populateStructureTrigger)
			return;
		FactionKey areaFactionKey = GetFactionKey();
		populateStructureTrigger.SetFactionKey(areaFactionKey);
		//Print("Spawned area trigger");
	}
}