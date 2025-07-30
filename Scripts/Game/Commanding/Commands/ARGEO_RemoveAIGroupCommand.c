//------------------------------------------------------------------------------------------------
//! Remove AIs from the commanded AI group, possibly discharging them if they are protected.
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_RemoveAIGroupCommand : SCR_BaseGroupCommand
{
	//
	// FORKED VANILLA LOGIC
	//
	
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (isClient)
		{
			//place to place a logic that would be executed for other players
			return true;
		}		
		
		SCR_PlayerController playerController =SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerID));
		if (!playerController)
			return false;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
			return false;
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(cursorTarget);
		if (!character)
			return false;
		
		if (GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character) == 0)
			groupController.RequestRemoveAgent(character, playerID);

		//
		// Protection-specific
		//
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(character.FindComponent(FactionAffiliationComponent));
		ARGEO_CharacterProtectionComponent characterProtectionComponent = ARGEO_CharacterProtectionComponent.Cast(character.FindComponent(ARGEO_CharacterProtectionComponent));
		if (factionAffiliation && characterProtectionComponent && characterProtectionComponent.IsProtected())
		{
			ARGEO_PopulationComponent populationComp = ARGEO_PopulationComponent.GetInstance();
			SCR_XPHandlerComponent xpComp = SCR_XPHandlerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_XPHandlerComponent));

			SCR_Faction controlledEntityFaction = SCR_Faction.Cast(playerController.GetLocalControlledEntityFaction());

			// TODO use updated faction callback
			Faction preProtectionFaction = characterProtectionComponent.GetPreProtectionFaction();
			if (controlledEntityFaction && controlledEntityFaction.IsFactionEnemy(preProtectionFaction)) // prisoner
			{
				ARGEO_CivicCenterEntity civicCenter;
				if (populationComp)
				{
					civicCenter = populationComp.CanNonCombatantBeDischarged(character);
					if (civicCenter)
					{
						civicCenter.RegisterPrisoner(character);
					}
				}

				if (civicCenter)
				{
					if (xpComp)
						xpComp.AwardXP(playerID, SCR_EXPRewards.DISCHARGE_PRISONER);
					
					// stay PROTECTED and wait
					return true;
				}
				else
				{
					if (xpComp)
						xpComp.AwardXP(playerID, SCR_EXPRewards.ABANDON_PRISONER);
				}
			}
			else // non-combatants
			{
				ARGEO_CivicCenterEntity civicCenter;
				if (populationComp)
				{
					civicCenter = populationComp.CanNonCombatantBeDischarged(character);
					if (civicCenter)
					{
						civicCenter.RegisterNonCombatant(character);
					}
				}
				
				if (civicCenter)
					xpComp.AwardXP(playerID, SCR_EXPRewards.DISCHARGE_NON_COMBATANT);
				else
					xpComp.AwardXP(playerID, SCR_EXPRewards.ABANDON_NON_COMBATANT);
			}
			
			// set faction back to original
			ARGEO_ProtectionFactionManagerComponent.UnsetProtected(character);
		}
		//
		// End of Protection-specific
		//
				
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShown()
	{
		if (!CanBeShownInCurrentLifeState())
			return false;
		
		PlayerCamera camera = GetGame().GetPlayerController().GetPlayerCamera();
		if (!camera)
			return false;
		
		IEntity cursorTarget = camera.GetCursorTarget();
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(cursorTarget);
		if (!character)
			return false;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();
		if (!groupManager)
			return false;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		if (!groupController)
			return false;
		
		if (!CanRoleShow())
			return false;
		
		int playerID = GetGame().GetPlayerController().GetPlayerId();
		SCR_AIGroup playerGroup = groupManager.GetPlayerGroup(playerID);
		
		return playerGroup.IsAIControlledCharacterMember(character);
	}
}