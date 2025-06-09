[ComponentEditorProps(category: "GameScripted/GameMode", description: "Singleton coordinating the civilian population.")]
class ARGEO_PopulationComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_PopulationComponent : SCR_BaseGameModeComponent
{
	[Attribute("CIV", desc: "Default civilian faction.", category: "Population")]
	protected FactionKey m_sDefaultCivilianFactionKey;

	[Attribute(defvalue: "0", UIWidgets.Slider, desc: "Buildings occupation ratio, in percentage.", params: "0 100 1", category: "Population")]
	protected float m_fGlobalBuildingsOccupation;

//	[Attribute("0", desc: "Populate with the default faction the buildings outside the explicitely populated areas.", category: "Population")]
//	protected bool m_bPopulateOutsidePopulatedAreas;

	protected static ARGEO_PopulationComponent s_Instance;
	
	
	static ARGEO_PopulationComponent GetInstance()
	{
		return s_Instance;
	}

	override void OnGameModeStart()
	{
		if (!s_Instance)
			s_Instance = this;
		Print("Global building occupation: " + m_fGlobalBuildingsOccupation + "%");
	}
	
	//
	// ACCESSORS
	//
	FactionKey GetDefaultCivilianFactionKey()
	{
		return m_sDefaultCivilianFactionKey;
	}
	
	float GetGlobalBuildingsOccupation()
	{
		return m_fGlobalBuildingsOccupation;
	}
	
//	bool PopulateOutsidePopulatedAreas()
//	{
//		return m_bPopulateOutsidePopulatedAreas;
//	}
}