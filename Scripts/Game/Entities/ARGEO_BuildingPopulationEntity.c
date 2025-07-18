class ARGEO_BuildingPopulationEntityClass: GenericEntityClass
{
}

//! The link between people and a given building.
class ARGEO_BuildingPopulationEntity: GenericEntity
{
	protected SCR_DestructibleBuildingComponent m_DestructibleBuildingComp;
	protected SCR_CampaignBuildingCompositionComponent m_CampaignBuildingCompositionComp;

	private ARGEO_PopulatedTerritoryID m_sPopulatedTerritoryID;
	
	//
	// LIFECYCLE
	//
	
	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);

		m_DestructibleBuildingComp = FindDestructibleBuildingComp(owner);
		if (m_DestructibleBuildingComp)
		{
			m_DestructibleBuildingComp.GetOnDamageStateChanged().Insert(OnBuildingDestroyed);
			m_DestructibleBuildingComp.GetOnDamage().Insert(OnBuildingDamaged);
		}
		
		m_CampaignBuildingCompositionComp = FindCampaignBuildingCompositionComp(owner);
		if (m_CampaignBuildingCompositionComp)
		{
			m_CampaignBuildingCompositionComp.GetOnCompositionSpawned().Insert(OnCompositionSpawned);
		}
		
	}
	
	//
	// CALLBACKS
	//
	
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
	
	//! To be overridden
	protected void OnCompositionSpawned(bool arg)
	{
	}
	
	//
	// UTILITIES
	//
	
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
	
	private SCR_CampaignBuildingCompositionComponent FindCampaignBuildingCompositionComp(IEntity current)
	{
		if (!current)
			return null;
		
		SCR_CampaignBuildingCompositionComponent res = SCR_CampaignBuildingCompositionComponent.Cast(current.FindComponent(SCR_CampaignBuildingCompositionComponent));
		if (res)
			return res;
		
		IEntity parent = current.GetParent();
		// recursive call
		return FindCampaignBuildingCompositionComp(parent);
	}
	
	//
	// ACCESSORS
	//
	void SetPopulatedTerritoryID(ARGEO_PopulatedTerritoryID populatedTerritoryID)
	{
		if (m_sPopulatedTerritoryID)
			return; // TODO warning?
		m_sPopulatedTerritoryID = populatedTerritoryID;
	}
	
	ARGEO_PopulatedTerritoryID GetPopulatedTerritoryID()
	{
		return m_sPopulatedTerritoryID;
	}
}