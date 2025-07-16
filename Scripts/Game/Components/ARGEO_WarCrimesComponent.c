[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class ARGEO_WarCrimesComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_WarCrimesComponent : SCR_BaseGameModeComponent
{
	[Attribute("0", desc: "Killing an unarmed or wounded enemy is a war crime (IHL DB - Rule 47).", category: "War Crimes")]
	protected bool m_bKillingHorsDeCombatIsWarCrime;

	[Attribute("{5A45CA8948A1D825}Prefabs/Systems/WarCrimes/WarCrime_Base.et", desc: "The prefab that will be spawned when a war cime is committed, to be used in triggers or to document atrocities a posteriori.", category: "Integration")]
	protected ResourceName m_sWarCrimePrefab;

	[Attribute("1", desc: "All war crimes will be considered as friendly kills (behavior of Arma Reforger currently).", category: "Compatibility")]
	protected bool m_bTreatAllWarCrimesAsFriendlyKills;

	[Attribute("1", desc: "Force playable factions to be friendly to non-military factions (IHL DB - Rule 1).", category: "Consider Arma Reforger EULAs Before Changing This")]
	protected bool m_bPlayableFactionsFriendlyToNonMilitary;

	protected static ARGEO_WarCrimesComponent s_Instance;
	
	//
	// LIFECYCLE
	//
	override void OnGameModeStart()
	{
		if (!s_Instance)
			s_Instance = this;

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
	
	//
	// NOTIFICATIONS
	//
	void RegisterWarCrime(SCR_ECrimeNotification crime, notnull SCR_InstigatorContextData instigatorContextData)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = instigatorContextData.GetVictimEntity().GetOrigin();

		ARGEO_WarCrimeEntity warCrimeEntity = ARGEO_WarCrimeEntity.Cast(GetGame().SpawnEntityPrefab(Resource.Load(m_sWarCrimePrefab), null, params));
		if (!warCrimeEntity)
			return;
		warCrimeEntity.SetCrime(crime);
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
//		CharacterPerceivableComponent perceivableComp = CharacterPerceivableComponent.Cast(character.FindComponent(CharacterPerceivableComponent));
//		if (perceivableComp)
//		{
//			if(perceivableComp.IsDisarmed())
//				return true;
//		}
		
		CharacterWeaponManagerComponent weaponManager = CharacterWeaponManagerComponent.Cast(character.FindComponent(CharacterWeaponManagerComponent));
		if (!weaponManager)
			return true;

		array<IEntity> weapons = {};
		int weaponsCount = 	weaponManager.GetWeaponsList(weapons);
		bool disarmed = weaponsCount == 0;

		return disarmed;
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
	
	static ARGEO_WarCrimesComponent GetInstance()
	{
		return s_Instance;
	}
}