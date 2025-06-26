[BaseContainerProps(configRoot: true)]
modded class SCR_FactionHomeTerritoryConfig
{
	[Attribute("0", desc: "Whether this faction is a basis for population")]
	protected bool m_bIsFactionPopulation;
	
	bool IsFactionPopulation()
	{
		return m_bIsFactionPopulation;
	}
	
	int GetTotalWeight()
	{
		return m_iTotalWeight;
	}
	
	int GetHomeTerritoriesCount()
	{
		if (!m_aFactionHomeTerritories)
			return 0;
		return m_aFactionHomeTerritories.Count();
	}
	
	ref SCR_FactionHomeTerritory GetHomeTerritory(int index)
	{
		if (!m_aFactionHomeTerritories || index >= m_aFactionHomeTerritories.Count())
			return null;
		return m_aFactionHomeTerritories[index];
	}
}