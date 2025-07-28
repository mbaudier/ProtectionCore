//------------------------------------------------------------------------------------------------
//! Abstract class from which "recruiting" commands are inherited.
class ARGEO_BaseAddAIGroupCommand : SCR_BaseGroupCommand
{
	[Attribute(defvalue: "0", desc: "Apply to the whole AI group of the target")]
	protected bool m_bApplyToGroup;
	
	//
	// API
	//
	
	//------------------------------------------------------------------------------------------------
	//! Whether this command is enabled at game level. To be overridden.
	protected bool IsFeatureEnabled()
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Whether this command is relevant for this factions combination. To be overridden.
	//! \param controlledEntityFaction Faction of the commanding entity.
	//! \param faction Faction of the target entity.
	protected bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Whether this command is relevant for this character. To be overridden.
	//! \param targetCharacter Character targeted by the commanding.
	protected bool CanBeShownForCharacter(notnull SCR_ChimeraCharacter targetCharacter)
	{
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Whether this command applies to a real player. Default is false. To be overridden.
	//! \param character The player character.
	//! \param targetPlayerID The player ID.
	protected bool ProcessPlayer(notnull SCR_ChimeraCharacter character, int targetPlayerID)
	{
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! If true, target will be affiliated to the (possibly virtual) protected faction. To be overridden.
	protected bool IsProtecting()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Callback after the recruitment has happened. To be overridden.
	//! \param playerID The player ID.
	//! \param count How many AI agents were added.
	protected void PostRecruitment(int playerID, int count)
	{
	}

	//
	// PROTECTION LOGIC
	//
	
	//------------------------------------------------------------------------------------------------
	//! The main logic of adding an AI agent to the commanded group and protecting it.
	//! \param groupController The player group controller.
	//! \param playerID The player ID.
	//! \param character The target character.
	protected void AddAIAgent(SCR_PlayerControllerGroupComponent groupController, int playerID, SCR_ChimeraCharacter character)
	{
		Faction currentFaction = NULL;
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(character.FindComponent(FactionAffiliationComponent));
		if (factionAffiliation)
		{
			currentFaction = factionAffiliation.GetAffiliatedFaction();
		}
		
		groupController.RequestAddAIAgent(character, playerID);
		SCR_AIGroup commandedGroup = groupController.GetPlayersGroup().GetSlave();
		
		// faction has been forced to recruiter's faction, set it to either PROTECTED or original:
		if (IsProtecting())
		{
			ARGEO_ProtectionFactionManagerComponent.SetProtected(character, currentFaction);
		}
		else // set back to original faction
		{
			factionAffiliation.SetAffiliatedFaction(currentFaction);
		}
	}
	
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
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerID));
		if (!playerController)
			return false;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.Cast(playerController.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!groupController)
			return false;
		
		// character to be considered
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(cursorTarget);
		if (!character)
			return false;		

		int targetPlayerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character);
		if (targetPlayerID != 0) // player
		{
			if (ProcessPlayer(character, targetPlayerID))
			{
				PostRecruitment(playerID, 1);
				return true;
			}
			else
			{
				return false;
			}
		}
		else // AI
		{
			if (IsCharacterInAnyGroup(groupController, character))
				return false;
			AddAIAgent(groupController, playerID, character);
	
			// make sure the commanded group will belong to player faction 
			// otherwise the first agent sets the faction of the commanded group
			SCR_AIGroup commandedGroup = groupController.GetPlayersGroup().GetSlave();
			array<AIAgent> commandedAgents = {};
			commandedGroup.GetAgents(commandedAgents);
	
			int count = 1;
			if (m_bApplyToGroup)
			{
				// add the whole group
				SCR_AIGroup currentGroup = GetGroupFromCharacter(character);
				if (currentGroup) {
					array<AIAgent> agents = {};
					currentGroup.GetAgents(agents);
					foreach (AIAgent agent:agents) {
						SCR_ChimeraCharacter c = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
						if (!IsCharacterInAnyGroup(groupController, c))
						{
							AddAIAgent(groupController, playerID, c);
							count++;
						}
					}
				}
				else
				{
					// TODO log exception?
					return false;
				}
				
			}
			
			PostRecruitment(playerID, count);	
			
			return true;
		}
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
		
		if (character.IsRecruited() || !character.IsRecruitable())
			return false;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		SCR_RespawnSystemComponent respawnComponent = SCR_RespawnSystemComponent.GetInstance();
		if (!respawnComponent)
			return false;
		
		SCR_PlayerControllerGroupComponent groupController = SCR_PlayerControllerGroupComponent.GetLocalPlayerControllerGroupComponent();
		if (!groupController)
			return false;
		
		SCR_AIGroup group = groupController.GetPlayersGroup();
		if (!group)
			return false;
		
		SCR_AIGroup slaveGroup = group.GetSlave();
		if (!slaveGroup)
			return false;
		
		SCR_CommandingManagerComponent commandingManager = SCR_CommandingManagerComponent.GetInstance();
		if (!commandingManager)
			return false;
		
		int maxAI = commandingManager.GetMaxAIPerGroup();
		// in case there is a limit on how many AIs can be in single group.
		if (maxAI != -1 && slaveGroup.GetAgentsCount() >= maxAI)
			return false;
		
		if (!CanRoleShow())
			return false;
		
		//
		// Protection-specific
		//
		if (!IsFeatureEnabled())
			return false;
						
		if (m_bApplyToGroup) // do not show whole group command if alone
		{
			SCR_AIGroup currentGroup = GetGroupFromCharacter(character);
			if(!currentGroup)
				return false;
			if(currentGroup.GetAgentsCount() < 2)
				return false;
		}
		
		SCR_Faction faction = SCR_Faction.Cast(character.GetFaction());
		if (!faction)
			return false;
		
		SCR_Faction controlledEntityFaction = SCR_Faction.Cast(playerController.GetLocalControlledEntityFaction());
		if (!controlledEntityFaction)
			return false;
		
		if (!CanBeShownForFaction(controlledEntityFaction, faction))
			return false;
		
		if (!CanBeShownForCharacter(character))
			return false;
		
		// Protection: check that it is not already controlled by any faction
		if (IsCharacterInAnyGroup(groupController, character))
			return false;
		//
		// End of Protection-specific
		//
		
		return true;
	}
	
	//
	// UTILITIES
	//
	
	//------------------------------------------------------------------------------------------------
	private bool IsCharacterInAnyGroup(SCR_PlayerControllerGroupComponent groupController, SCR_ChimeraCharacter character)
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();// workaround
		if (factionManager && groupManager)
		{
			array<Faction> factions = {};
			factionManager.GetFactionsList(factions);
			foreach (Faction f : factions)
			{		
				SCR_Faction ff = SCR_Faction.Cast(f);
				array<SCR_AIGroup> groups = groupManager.GetPlayableGroupsByFaction(ff);
				if (groups)
				{		
					if (ff.IsPlayable() && ff.IsMilitary() && groupController.IsAICharacterInAnyGroup(character, ff))
						return true;
				}
			}
		}
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	private SCR_AIGroup GetGroupFromCharacter(SCR_ChimeraCharacter character)
	{
		AIControlComponent aiContr = AIControlComponent.Cast(character.FindComponent(AIControlComponent));	
		if (!aiContr)
			return null;
		SCR_ChimeraAIAgent chimeraAgent = SCR_ChimeraAIAgent.Cast(aiContr.GetAIAgent());
		if (!chimeraAgent)
			return null;
		SCR_AIGroup groupToJoin = SCR_AIGroup.Cast(chimeraAgent.GetParentGroup());
		return groupToJoin;
	}
}