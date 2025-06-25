[ComponentEditorProps(category: "GameScripted/GameMode", description: "Lore settings.")]
class ARGEO_ProtectionLoreComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_ProtectionLoreComponent : SCR_BaseGameModeComponent
{
	[Attribute("1989", UIWidgets.Slider, "Year", "1900 2200 1", category: "Date")]
	protected int m_iYear;

	[Attribute("11", UIWidgets.Slider, "Month of the year", "1 12 1", category: "Date")]
	protected int m_iMonthOfTheYear;

	[Attribute("9", UIWidgets.Slider, "Day of the month", "1 31 1", category: "Date")]
	protected int m_iDayOfTheMonth;

	protected static ARGEO_ProtectionLoreComponent s_Instance;

	void SetupDate(int year, int month, int day = 1)
	{
		ChimeraWorld world = ChimeraWorld.CastFrom(GetOwner().GetWorld());
		if (!world)
			return;

		TimeAndWeatherManagerEntity manager = world.GetTimeAndWeatherManager();
		if (!manager)
			return;
		
		if (manager.SetDate(year, month, day, true))
		{
			Print("Date changed: " + year + "-" + month + "-" + day);
		}
		else
		{
			Print("Invalid date: " + year + "-" + month + "-" + day);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);

		if (!Replication.IsServer() || !GetGame().InPlayMode())
			return;

		if (s_Instance != this)
		{
			Print("Multiple instances of ARGEO_ProtectionLoreComponent detected.", LogLevel.WARNING);
			return;
		}

		SetupDate(m_iYear, m_iMonthOfTheYear, m_iDayOfTheMonth);
	}

	override void OnPostInit(IEntity owner)
	{
		// Allow only one instance
		if (s_Instance || !GetGame().InPlayMode())
			return;
		s_Instance = this;
	}
}