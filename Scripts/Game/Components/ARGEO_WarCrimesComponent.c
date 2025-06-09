[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class ARGEO_WarCrimesComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_WarCrimesComponent : SCR_BaseGameModeComponent
{

	[Attribute("0", desc: "Killing an unarmed or wounded enemy is a war crime (IHL DB - Rule 47).", category: "War Crimes")]
	protected bool m_bKillingHorsDeCombatIsWarCrime;

	[Attribute("1", desc: "Force playable factions to be friendly to non-military factions (IHL DB - Rule 1).", category: "Consider Arma Reforger EULAs before changing")]
	protected bool m_bPlayableFactionsFriendlyToNonMilitary;

	[Attribute("1", desc: "All war crimes will be considered as friendly kills (behavior of Arma Reforger currently).", category: "Compatibility")]
	protected bool m_bTreatAllWarCrimesAsFriendlyKills;

	protected static ARGEO_WarCrimesComponent s_Instance;
	
	
	static ARGEO_WarCrimesComponent GetInstance()
	{
		return s_Instance;
	}

	override void OnGameModeStart()
	{
		if (!s_Instance)
			s_Instance = this;

		Print("Protection manager game mode started");
		if (m_bPlayableFactionsFriendlyToNonMilitary)
		{
			SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
			SCR_SortedArray<SCR_Faction> factions = new SCR_SortedArray<SCR_Faction>();
			factionManager.GetSortedFactionsList(factions);

			array<SCR_Faction> nonMilitary = new array<SCR_Faction>();
			array<SCR_Faction> playable = new array<SCR_Faction>();
			for (int i = 0; i < factions.Count(); i++)
			{		
				SCR_Faction f = factions.Get(i);
				if (!f.IsMilitary())
				{			
					nonMilitary.Insert(f);
				}
				if (f.IsPlayable())
				{			
					playable.Insert(f);
				}
			}
			
			foreach (SCR_Faction nm : nonMilitary)
			{
				foreach (SCR_Faction p : playable)
				{
					factionManager.SetFactionsFriendly(nm, p, -1, false);
				}
			}
			factionManager.RequestUpdateAllTargetsFactions();
		}
	}
	
	override void OnGameStateChanged(SCR_EGameModeState state)
	{
		Print("Protection manager game state changed " + state);
	}

	//
	// RULES
	//
	bool IsProtected(IEntity entity)
	{		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if(character)
		{
			SCR_Faction faction = SCR_Faction.Cast(character.GetFaction());
			if(faction && faction.IsMilitary())
				return false;
	
			return IsDisarmed(character);
		}
	
		// TODO deal with buildings
		return false; 
	}

	bool IsHorsDecombat(IEntity entity)
	{		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(entity);
		if(character)
		{
			SCR_Faction faction = SCR_Faction.Cast(character.GetFaction());
			if(faction && !faction.IsMilitary())
				return false;
			// TODO check health, unconsciousness
			return IsDisarmed(character);
		}
	
		return false; 
	}


	bool IsDisarmed(SCR_ChimeraCharacter character)
	{
		CharacterPerceivableComponent perceivableComp = CharacterPerceivableComponent.Cast(character.FindComponent(CharacterPerceivableComponent));
		if (perceivableComp)
		{
			return perceivableComp.IsDisarmed();
		}
		else
		{
			CharacterWeaponManagerComponent weaponManager = CharacterWeaponManagerComponent.Cast(character.FindComponent(CharacterWeaponManagerComponent));
			WeaponSlotComponent weapon = weaponManager.GetCurrentSlot();
			// TODO check other visible slots
			bool unarmed = !weapon;
			return unarmed;
		}
	}
	
	//
	// ACCESSORS
	//
	bool IsKillingHorsDeCombatWarCrime()
	{
		return m_bKillingHorsDeCombatIsWarCrime;
	}
	
	bool TreatAllWarCrimesAsFriendlyKills()
	{
		return m_bTreatAllWarCrimesAsFriendlyKills;
	}
}