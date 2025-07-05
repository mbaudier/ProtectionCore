class ARGEO_BuildingPopulationEntityClass: GenericEntityClass
{
}

//! The link between people and a given building.
class ARGEO_BuildingPopulationEntity: GenericEntity
{
	private SCR_DestructibleBuildingComponent m_DestructibleBuildingComp;

	private string m_sPopulatedTerritoryID;
	
	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);

		m_DestructibleBuildingComp = FindDestructibleBuildingComp(owner);
		if(m_DestructibleBuildingComp)
		{
			m_DestructibleBuildingComp.GetOnDamageStateChanged().Insert(OnBuildingDestroyed);
			m_DestructibleBuildingComp.GetOnDamage().Insert(OnBuildingDamaged);
		}
		//Print("Household post-init");
		
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called when the related building is destroyed, making the related people displaced persons.
	protected void OnBuildingDestroyed(EDamageState state)
	{
		if (state != EDamageState.DESTROYED)
			return;
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp)
			return;
		populationComp.NotifyBuildingDestroyed(this);
	}
	
	protected void OnBuildingDamaged(notnull BaseDamageContext damageContext)
	{
		ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
		if (!populationComp)
			return;
		populationComp.NotifyBuildingDamaged(this, damageContext);
	}
	
	private SCR_DestructibleBuildingComponent FindDestructibleBuildingComp(IEntity current)
	{
		if (!current)
			return null;
		
		SCR_DestructibleBuildingComponent res = SCR_DestructibleBuildingComponent.Cast(current.FindComponent(SCR_DestructibleBuildingComponent));
		if (res)
			return res;
		
		IEntity parent = current.GetParent();
		// recursive call
		return FindDestructibleBuildingComp(parent);
	}
	
	//
	// ACCESSORS
	//
	void SetPopulatedTerritoryID(string populatedTerritoryID)
	{
		if (m_sPopulatedTerritoryID)
			return; // TODO warning?
		m_sPopulatedTerritoryID = populatedTerritoryID;
	}
	
	string GetPopulatedTerritoryID()
	{
		return m_sPopulatedTerritoryID;
	}
}