[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton coordinating the civilian population.")]
class ARGEO_CampaignPopulationComponentClass : ARGEO_PopulationComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Campaign extensions for population management.
class ARGEO_CampaignPopulationComponent : ARGEO_PopulationComponent
{
	protected ref array<string> m_aEnabledTerritoryIDs = new array<string>;
	protected ref SCR_CampaignMilitaryBaseManager m_BaseManager = null;
	//
	// LIFECYLE
	//
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SCR_MissionHeaderCampaign header = SCR_MissionHeaderCampaign.Cast(GetGame().GetMissionHeader());
		if (header)
		{
			if (header.m_bCustomBaseWhitelist && !header.m_aCampaignCustomBaseList.IsEmpty())
			{		
				array<string> customBaseList = {};
				foreach (SCR_CampaignCustomBase customBase : header.m_aCampaignCustomBaseList)
				{
					string baseName = customBase.GetBaseName();
					IEntity baseEntity = GetGame().FindEntity(baseName);
					if (baseEntity)
					{
						Print("Found custom base entity " + baseEntity);
					}
				}
			}
		}
		super.OnPostInit(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnGameModeStart()
	{
		SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
		if (campaign)
			m_BaseManager = campaign.GetBaseManager();

		SCR_MilitaryBaseSystem baseManager = SCR_MilitaryBaseSystem.GetInstance();
		SCR_MissionHeaderCampaign header = SCR_MissionHeaderCampaign.Cast(GetGame().GetMissionHeader());
		if (baseManager && header)
		{
			if (header.m_bCustomBaseWhitelist && !header.m_aCampaignCustomBaseList.IsEmpty())
			{		
				array<string> customBaseList = {};
				foreach (SCR_CampaignCustomBase customBase : header.m_aCampaignCustomBaseList)
				{
					customBaseList.Insert(customBase.GetBaseName());
				}
			
				array<SCR_MilitaryBaseComponent> bases = {};
				baseManager.GetBases(bases);
				string baseName;
				SCR_CampaignMilitaryBaseComponent campaignBase;
				int listIndex;
				foreach (SCR_MilitaryBaseComponent base : bases)
				{
					campaignBase = SCR_CampaignMilitaryBaseComponent.Cast(base);
					if (!campaignBase)
						continue;
					baseName = campaignBase.GetOwner().GetName();
					listIndex = customBaseList.Find(baseName);
		
					if (listIndex != -1)
					{
						Print("Found custom base " + baseName);
						
					}
				}
			}
		}
		
		super.OnGameModeStart();
	}
	
	//
	// AUTOMATED POPULATION
	//
	
	//------------------------------------------------------------------------------------------------
	//! The main logic of populating territories based on the relative weights of the factions.
	override void ApplyPopulation()
	{
		if (!m_aEnabledTerritoryIDs.IsEmpty())
		{
			array<ARGEO_PopulatedTerritory> currentPopulatedTerritories = {};
			foreach (ARGEO_PopulatedTerritory territory : m_aPopulatedTerritories)
			{
				currentPopulatedTerritories.Insert(territory);
			}
			
			foreach (ARGEO_PopulatedTerritory territory : currentPopulatedTerritories)
			{
				if (!m_aEnabledTerritoryIDs.Contains(territory.GetPopulatedTerritoryID()))
				{
					m_aPopulatedTerritories.RemoveItem(territory);
					Print("Remove disabled populated territory " + territory.GetPopulatedTerritoryID());
				}
			}
		}
		super.ApplyPopulation();
	}
	
	//------------------------------------------------------------------------------------------------
	override void RegisterBuildingHousehold(notnull ARGEO_BuildingHouseholdEntity buildingHousehold)
	{
		if (m_BaseManager && !m_aEnabledTerritoryIDs.Contains(buildingHousehold.GetPopulatedTerritoryID()))
		{
			SCR_CampaignMilitaryBaseComponent campaignBase = m_BaseManager.FindClosestBase(buildingHousehold.GetOrigin());
			if (campaignBase)
			{
				float distanceFromBase = vector.Distance(campaignBase.GetOwner().GetOrigin(), buildingHousehold.GetOrigin());
				
				int consideredRadius = campaignBase.GetRadioRange();
				// source control points have radio range = 1
				if (campaignBase.GetRadioRange() < 500)
				{
					consideredRadius = campaignBase.GetRadius() * 5;
				}
				
				if(distanceFromBase <= consideredRadius)
				{
					m_aEnabledTerritoryIDs.Insert(buildingHousehold.GetPopulatedTerritoryID());
				}
			}
		}
		super.RegisterBuildingHousehold(buildingHousehold);
	}
}
