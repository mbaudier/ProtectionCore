//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ARGEO_ProtectAIGroupCommand : SCR_BaseGroupCommand
{
	[Attribute(defvalue: "0", desc: "Will the whole target AI group be protected?")]
	protected bool m_bProtectGroup;

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
		
		
		// character to be protected
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(cursorTarget);
		if (!character)
			return false;		

		if (GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character) != 0)
			return false;

		if(IsCharacterInAnyGroup(groupController, character))
			return false;

		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(character.FindComponent(FactionAffiliationComponent))	;
		Faction currentFaction = factionAffiliation.GetAffiliatedFaction();
		SCR_AIGroup currentGroup = GetGroupFromCharacter(character);

		// add targeted person
		groupController.RequestAddAIAgent(character, playerID);
		factionAffiliation.SetAffiliatedFaction(currentFaction);

		// make sure the commanded group will belong to player faction 
		// otherwise the first agent sets the faction of the commanded group
		SCR_AIGroup commandedGroup = groupController.GetPlayersGroup().GetSlave();
		array<AIAgent> commandedAgents = {};
		commandedGroup.GetAgents(commandedAgents);
//		if(commandedAgents.GetSizeOf() == 1) {					
//			commandedGroup.SetFaction(playerController.GetLocalControlledEntityFaction());
//			factionAffiliation.SetAffiliatedFaction(currentFaction);
//		}

		int count = 1;
		if(m_bProtectGroup)
		{
			// add the whole group
			if(currentGroup) {
				array<AIAgent> agents = {};
				currentGroup.GetAgents(agents);
				foreach(AIAgent agent:agents) {
					SCR_ChimeraCharacter c = SCR_ChimeraCharacter.Cast(agent.GetControlledEntity());
					if(!IsCharacterInAnyGroup(groupController, c))
					{
						FactionAffiliationComponent fa = FactionAffiliationComponent.Cast(c.FindComponent(FactionAffiliationComponent))	;
						Faction currentF = fa.GetAffiliatedFaction();
						groupController.RequestAddAIAgent(c, playerID);
						factionAffiliation.SetAffiliatedFaction(currentF);
						count++;
					}
				}
			}
			else
			{		
				return false;
			}
			
		}
		
		if(count == 1)
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Civilian protected", "Protected", 3.0);	
		else	
			SCR_HintManagerComponent.GetInstance().ShowCustomHint("Group of civilians protected", "Protected", 3.0);	
	
	
		// DEBUG list the whole group
		array<AIAgent> commandedA = {};
		commandedGroup.GetAgents(commandedA);
		int index = 0;
		foreach(AIAgent a:commandedA) {
			SCR_ChimeraCharacter c = SCR_ChimeraCharacter.Cast(a.GetControlledEntity());
			Print(" " + index + " - " + c.GetFactionKey());
			index++;
		}		
		
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
		
		if (character.IsRecruited() || !character.IsRecruitable())
			return false;

		// do not show protect group if he/she is alone
		if(m_bProtectGroup)
		{
			SCR_AIGroup currentGroup = GetGroupFromCharacter(character);
			if(!currentGroup)
				return false;
			if(currentGroup.GetAgentsCount() < 2)
				return false;
		}
		
		// protect only non-military factions
		SCR_Faction faction = SCR_Faction.Cast(character.GetFaction());
		if(faction && faction.IsMilitary())
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
		
		// check that it is not already protected
		if(IsCharacterInAnyGroup(groupController, character))
			return false;
		
		return true;
	}
	
	private bool IsCharacterInAnyGroup(SCR_PlayerControllerGroupComponent groupController, SCR_ChimeraCharacter character)
	{
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		SCR_GroupsManagerComponent groupManager = SCR_GroupsManagerComponent.GetInstance();// workaround
		if (factionManager && groupManager)
		{
			array<Faction> factions = {};
			factionManager.GetFactionsList(factions);
			foreach(Faction f : factions)
			{		
				SCR_Faction ff = SCR_Faction.Cast(f);
				array<SCR_AIGroup> groups = groupManager.GetPlayableGroupsByFaction(ff);
				if(groups)
				{		
					// FIXME understand why it sometimes crashes (groups is null)
					if(ff.IsPlayable() && ff.IsMilitary() && groupController.IsAICharacterInAnyGroup(character, ff))
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