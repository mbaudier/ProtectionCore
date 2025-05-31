[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton coordinating the civilian population.")]
class ARGEO_PopulationComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_PopulationComponent : SCR_BaseGameModeComponent
{
	[Attribute("CIV", desc: "Default civilian faction.", category: "Population")]
	protected FactionKey m_sDefaultCivilianFactionKey;

	[Attribute("100", desc: "Buildings occupation ratio, in percentage.", category: "Population")]
	protected int m_iGlobalBuildingsOccupation;

	[Attribute("0", desc: "Populate with the default faction the buildings outside the explicitely populated areas.", category: "Population")]
	protected bool m_bPopulateOutsidePopulatedAreas;

	protected static ARGEO_PopulationComponent s_Instance;
	
	
	static ARGEO_PopulationComponent GetInstance()
	{
		return s_Instance;
	}

	override void OnGameModeStart()
	{
		if (!s_Instance)
			s_Instance = this;
	}
	
	//
	// ACCESSORS
	//
	FactionKey GetDefaultCivilianFactionKey()
	{
		return m_sDefaultCivilianFactionKey;
	}
	
	int GetGlobalBuildingsOccupation()
	{
		return m_iGlobalBuildingsOccupation;
	}
	
	bool PopulateOutsidePopulatedAreas()
	{
		return m_bPopulateOutsidePopulatedAreas;
	}
}