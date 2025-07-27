[ComponentEditorProps(category: "GameScripted/FactionManager/Components", description: "")]
class ARGEO_ProtectionFactionManagerComponentClass: SCR_BaseFactionManagerComponentClass
{
};

class ARGEO_ProtectionFactionManagerComponent : SCR_BaseFactionManagerComponent
{
	[Attribute("CIV", desc: "Possibly virtual faction temporarily assigned when civilians, POW, etc. are protected.", category: "Protection")]
	private FactionKey m_sProtectedFaction;
	
	[Attribute("0", desc: "Force the protected faction to be friendly to all. DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING.", category: "Protection")]
	private bool m_bForceProtectedFactionFriendlyToAll;
	
	[Attribute("0", desc: "Soldiers from friendly military factions can be recruited.", category: "Commanding")]
	private bool m_bAlliesCanBeRecruited;

	[Attribute("0", desc: "Non-combattants can be protected (\"recruited\" into the AI commanded group).", category: "Commanding")]
	private bool m_bNonCombattantsCanBeProtected;
	
	[Attribute("0", desc: "Enemy soldiers can be captured (\"recruited\" into the AI commanded group).", category: "Commanding")]
	private bool m_bEnemiesCanBeCaptured;
	
	private static SCR_Faction s_ProtectedFaction;

	override void OnFactionsInit(array<Faction> factions)
	{
		// find protected faction
		for (int i = 0; i < factions.Count(); i++)
		{		
			Faction f = factions.Get(i);
			if (m_sProtectedFaction == f.GetFactionKey())
			{
				s_ProtectedFaction = SCR_Faction.Cast(f);
			}
		}
		
		if (s_ProtectedFaction)
		{
			if (!m_bForceProtectedFactionFriendlyToAll)
				return;
		
			// force all factions friendly to the protected faction
			for (int i = 0; i < factions.Count(); i++)
			{		
				SCR_Faction f = SCR_Faction.Cast(factions.Get(i));
				if (s_ProtectedFaction != f)
				{
					s_ProtectedFaction.SetFactionFriendly(f);
					f.SetFactionFriendly(s_ProtectedFaction);
					// no need to notify as we are still initializing
				}
			}
		}
		else
		{
			Print("Virtual protected faction not found", LogLevel.WARNING);
		}
	}
	
	static Faction GetProtectedFaction()
	{
		return s_ProtectedFaction;
	}
	
	bool CanNonCombattantsBeProtected()
	{
		return m_bNonCombattantsCanBeProtected;
	}

	bool CanEnemiesBeCaptured()
	{
		return m_bEnemiesCanBeCaptured;
	}

	bool CanAlliesCanBeRecruited()
	{
		return m_bAlliesCanBeRecruited;
	}
	
	//
	// STATIC
	//
	static void SetProtected(IEntity owner, Faction preProtectionFaction)
	{
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));
		if (!factionAffiliation)
			return;
		
		Faction protectedFaction = GetProtectedFaction();
		if (!protectedFaction)
		{
			factionAffiliation.SetAffiliatedFaction(preProtectionFaction);
			return;
		}
		factionAffiliation.SetAffiliatedFaction(protectedFaction);

		ARGEO_CharacterProtectionComponent characterProtectionComp = ARGEO_CharacterProtectionComponent.Cast(owner.FindComponent(ARGEO_CharacterProtectionComponent));
		if (!characterProtectionComp)
			return;
		characterProtectionComp.SetPreProtectionFaction(preProtectionFaction);
		
		SCR_CharacterFactionAffiliationComponent characterFactionAffiliationComp = SCR_CharacterFactionAffiliationComponent.Cast(factionAffiliation);
		if (characterFactionAffiliationComp)
			characterFactionAffiliationComp.DisableUpdatingPerceivedFaction_S();
		
		PerceivableComponent perceivableComponent = PerceivableComponent.Cast(owner.FindComponent(PerceivableComponent));
		if (perceivableComponent)
			perceivableComponent.SetPerceivedFactionOverride(protectedFaction);
	}
	
	static void UnsetProtected(IEntity owner)
	{
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));
		if (!factionAffiliation)
			return;

		ARGEO_CharacterProtectionComponent characterProtectionComp = ARGEO_CharacterProtectionComponent.Cast(owner.FindComponent(ARGEO_CharacterProtectionComponent));
		if (!characterProtectionComp || !characterProtectionComp.IsProtected())
			return;

		Faction preProtectionFaction = characterProtectionComp.GetPreProtectionFaction();
		factionAffiliation.SetAffiliatedFaction(preProtectionFaction);
		characterProtectionComp.SetPreProtectionFaction(null);

		SCR_CharacterFactionAffiliationComponent characterFactionAffiliationComp = SCR_CharacterFactionAffiliationComponent.Cast(factionAffiliation);
		if (characterFactionAffiliationComp)
			characterFactionAffiliationComp.InitPlayerOutfitFaction_S();

		PerceivableComponent perceivableComponent = PerceivableComponent.Cast(owner.FindComponent(PerceivableComponent));
		if (perceivableComponent)
			perceivableComponent.SetPerceivedFactionOverride(null);
	}

	static bool GroupContainsProtected(AIGroup commandedGroup)
	{
		if (commandedGroup) {
			Faction protectedFaction = ARGEO_ProtectionFactionManagerComponent.GetProtectedFaction();
			array<AIAgent> agents = {};
			commandedGroup.GetAgents(agents);
			foreach (AIAgent agent : agents) {
				FactionAffiliationComponent fac = FactionAffiliationComponent.Cast(agent.GetControlledEntity().FindComponent(FactionAffiliationComponent));
				if (!fac)
					continue; // agents
				Faction faction = fac.GetAffiliatedFaction();
				if (protectedFaction == faction)
					return true;
			}
		}
		return false;
	}
}