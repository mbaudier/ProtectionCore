//------------------------------------------------------------------------------------------------
class ARGEO_BaseAddAIGroupCommand : SCR_BaseGroupCommand
{
	[Attribute(defvalue: "0", desc: "Apply to the whole AI group of the target")]
	protected bool m_bApplyToGroup;
	
	// caches to avoid testing repeatedly whether a protected faction is available
	private ARGEO_ProtectionFactionManagerComponent m_ProtectionFactionManagerComponent = NULL;
	private bool m_bProtectionEnabled = true;
	
	/// To be overridden
	protected bool IsFeatureEnabled()
	{
		return false;
	}
	
	/// To be overridden
	protected bool CanBeShownForFaction(notnull SCR_Faction controlledEntityFaction, notnull SCR_Faction faction)
	{
		return false;
	}
	
	/// To be overridden
	protected bool CanBeShownForCharacter(notnull SCR_ChimeraCharacter targetCharacter)
	{
		return true;
	}
	
	protected bool ProcessPlayer(notnull SCR_ChimeraCharacter character, int targetPlayerID)
	{
		return false;
	}
	
	/// Will set the faction to protected faction
	protected bool IsProtecting()
	{
		return true;
	}

	/// To be overridden
	protected void PostRecruitment(int playerID, int count)
	{
	}
	

	// TODO comment
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
		
		// faction has now been set to recruiter's faction, set it to either PROTECTED or original:
		Faction factionToSet = currentFaction;// can be NULL
		if (IsProtecting() && m_bProtectionEnabled)
		{
			Faction protectedFaction = GetProtectedFaction();
			if (protectedFaction)
			{
				factionToSet = protectedFaction;
				ARGEO_CharacterProtectionComponent characterProtectionComponent = ARGEO_CharacterProtectionComponent.Cast(character.FindComponent(ARGEO_CharacterProtectionComponent));
				if (characterProtectionComponent)
				{
					// TODO use updated faction callback
					characterProtectionComponent.SetPreProtectionFaction(currentFaction);
					
					//commandedGroup.GetOnAgentRemoved().Insert(OnAgentRemoved);
				}
			}
		}
		factionAffiliation.SetAffiliatedFaction(factionToSet);
		
		if (GroupContainsProtected(commandedGroup))
		{
			// set column formation when protecting
			// FIXME understand why it is not working
			AIFormationComponent aiFormation = AIFormationComponent.Cast(commandedGroup.FindComponent(AIFormationComponent));
			if (aiFormation)
			{
				string formationName = SCR_Enum.GetEnumName(SCR_EAIGroupFormation, SCR_EAIGroupFormation.Column);
				aiFormation.SetFormation(formationName);
			}
			AIGroupMovementComponent groupMovement = AIGroupMovementComponent.Cast(commandedGroup.FindComponent(AIGroupMovementComponent));
			if (groupMovement)
				groupMovement.SetFormationDisplacement(0);
			//aiFormation.SetFormation("Column");
			// TODO sort military before and after protected
		}
	}

	protected void OnAgentRemoved(SCR_AIGroup group, AIAgent agent)
	{
		IEntity character = agent.GetControlledEntity();
		ARGEO_CharacterProtectionComponent characterProtectionComponent = ARGEO_CharacterProtectionComponent.Cast(character.FindComponent(ARGEO_CharacterProtectionComponent));
		if (characterProtectionComponent && characterProtectionComponent.IsProtected())
		{
			// TODO auto-reset faction?
		}
	}		
	//
	// FORKED LOGIC
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
//			commandedGroup.SetFaction(playerController.GetLocalControlledEntityFaction());
	
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
				
			// DEBUG list the whole group
//			array<AIAgent> commandedA = {};
//			commandedGroup.GetAgents(commandedA);
//			int index = 0;
//			foreach (AIAgent a : commandedA)
//			{
//				SCR_ChimeraCharacter c = SCR_ChimeraCharacter.Cast(a.GetControlledEntity());
//				Print(" " + index + " - " + c.GetFactionKey());
//				index++;
//			}		
			
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
		//in case there is a limit on how many AIs can be in single group.
		if (maxAI != -1 && slaveGroup.GetAgentsCount() >= maxAI)
			return false;
		
		if (!CanRoleShow())
			return false;
		
		//
		// Protection specific
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
		//
				
		int playerID = GetGame().GetPlayerController().GetPlayerId();
		
//		SCR_Faction playerFaction;
//		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
//		if (factionManager)
//		{
//			playerFaction = SCR_Faction.Cast(factionManager.GetPlayerFaction(playerID));
//		}
		
		// Protection: check that it is not already controlled by any faction
		if (IsCharacterInAnyGroup(groupController, character))
			return false;
		
		return true;
	}
	
	//
	// UTILITIES
	//
	
	private bool GroupContainsProtected(AIGroup commandedGroup)
	{
		if (commandedGroup) {
			Faction protectedFaction = GetProtectedFaction();
			array<AIAgent> agents = {};
			commandedGroup.GetAgents(agents);
			foreach (AIAgent agent:agents) {
				FactionAffiliationComponent fac = FactionAffiliationComponent.Cast(agent.GetControlledEntity().FindComponent(FactionAffiliationComponent));
				Faction faction = fac.GetAffiliatedFaction();
				if (protectedFaction == faction)
					return true;
			}
		}
		return false;
	}
	
	private Faction GetProtectedFaction()
	{
		if (!m_bProtectionEnabled)
			return NULL;
		
		if (!m_ProtectionFactionManagerComponent)
		{
			FactionManager factionManager = GetGame().GetFactionManager();
			m_ProtectionFactionManagerComponent = ARGEO_ProtectionFactionManagerComponent.Cast(factionManager.FindComponent(ARGEO_ProtectionFactionManagerComponent));
			if (m_ProtectionFactionManagerComponent)
			{
				Faction protectionFaction = m_ProtectionFactionManagerComponent.GetProtectedFaction();
				if (!protectionFaction)
					m_bProtectionEnabled = false;
				else
					return protectionFaction;
			}
			else
			{
				m_bProtectionEnabled = false;
			}
		}
		return m_ProtectionFactionManagerComponent.GetProtectedFaction();
	}
	
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