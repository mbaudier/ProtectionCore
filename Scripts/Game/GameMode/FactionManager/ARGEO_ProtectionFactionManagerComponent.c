[ComponentEditorProps(category: "GameScripted/FactionManager/Components", description: "")]
class ARGEO_ProtectionFactionManagerComponentClass: SCR_BaseFactionManagerComponentClass
{
};

class ARGEO_ProtectionFactionManagerComponent : SCR_BaseFactionManagerComponent
{
	[Attribute("CIV", desc: "Possibly virtual faction temporarily assigned when civilians, POW, etc. are protected.", category: "Protection")]
	private FactionKey m_sProtectedFaction;
	
	[Attribute("1", desc: "Force the protected faction to be friendly to all.", category: "Protection")]
	private bool m_bForceProtectedFactionFriendlyToAll;
	
	[Attribute("0", desc: "Soldiers from friendly military factions can be recruited.", category: "Protection")]
	private bool m_bAlliesCanBeRecruited;
	
	private SCR_Faction m_ProtectedFaction;

	override void OnFactionsInit(array<Faction> factions)
	{
		// find protected faction
		for (int i = 0; i < factions.Count(); i++)
		{		
			Faction f = factions.Get(i);
			if (m_sProtectedFaction == f.GetFactionKey())
			{
				m_ProtectedFaction = SCR_Faction.Cast(f);
			}
		}
		
		if (m_ProtectedFaction)
		{
			if (!m_bForceProtectedFactionFriendlyToAll)
				return;
		
			// force all factions friendly to the protected faction
			for (int i = 0; i < factions.Count(); i++)
			{		
				SCR_Faction f = SCR_Faction.Cast(factions.Get(i));
				if (m_ProtectedFaction != f)
				{
					m_ProtectedFaction.SetFactionFriendly(f);
					f.SetFactionFriendly(m_ProtectedFaction);
					// no need to notify as we are still initializing
				}
			}
		}
		else
		{
			Print("Virtual protected faction not found", LogLevel.WARNING);
		}
	}
	
	Faction GetProtectedFaction()
	{
		return m_ProtectedFaction;
	}
	
	bool CanAlliesCanBeRecruited()
	{
		return m_bAlliesCanBeRecruited;
	}
}