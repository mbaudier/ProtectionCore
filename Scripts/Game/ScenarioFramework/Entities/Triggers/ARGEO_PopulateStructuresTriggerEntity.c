[EntityEditorProps(category: "GameScripted/Triggers", description: "")]
class ARGEO_PopulateStructuresTriggerEntityClass : SCR_BaseTriggerEntityClass
{
}

class ARGEO_PopulateStructuresTriggerEntity : SCR_BaseTriggerEntity
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		Print("Populate - trigger initialized");
	}

	override protected event void OnActivate(IEntity ent)
	{
		super.OnActivate(ent);
		//Print("Populated trigger activated "+ent);
		
	 	EntityPrefabData prefabData =	ent.GetPrefabData();
		ResourceName resourceName = prefabData.GetPrefabName();
		Print("Populate - found "+resourceName+" - "+ent);

	}
	
	override event protected void OnQueryFinished(bool bIsEmpty)
	{
		Print("Populate - query finished");
		EnablePeriodicQueries(false);
	}
}
