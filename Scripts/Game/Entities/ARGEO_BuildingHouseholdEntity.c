class ARGEO_BuildingHouseholdEntityClass: GenericEntityClass
{
}

//! The link between people and a given building.
//! It typically contains 
class ARGEO_BuildingHouseholdEntity: GenericEntity
{
	private SCR_DestructibleBuildingComponent m_DestructibleBuildingComp;
	
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		//Print("Household initialized");
	}

	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);

		m_DestructibleBuildingComp = FindDestructibleBuildingComp(owner);
		if(m_DestructibleBuildingComp)
		{
			m_DestructibleBuildingComp.GetOnDamageStateChanged().Insert(OnBuildingDestroyed);
		}
		//Print("Household post-init");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called when the related building is destroyed, making the related people displaced persons.
	protected void OnBuildingDestroyed(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		Print("Building destroyed!");
	}
	
	private SCR_DestructibleBuildingComponent FindDestructibleBuildingComp(IEntity current)
	{
		if (!current)
			return NULL;
		
		SCR_DestructibleBuildingComponent res = SCR_DestructibleBuildingComponent.Cast(current.FindComponent(SCR_DestructibleBuildingComponent));
		if (res)
			return res;
		
		IEntity parent = current.GetParent();
		// recursive call
		return FindDestructibleBuildingComp(parent);
	}
}