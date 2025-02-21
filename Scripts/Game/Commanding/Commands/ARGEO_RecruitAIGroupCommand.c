//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ARGEO_RecruitAIGroupCommand : SCR_RecruitAIGroupCommand
{
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
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(cursorTarget);
		if (!character)
			return false;
		
		
		if (GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(character) != 0)
			return false;
		
		Faction playerFaction = playerController.GetLocalControlledEntityFaction();
		groupController.RequestAddAIAgent(character, playerID);

		// force player faction on recruited
		FactionAffiliationComponent factionAffiliation = FactionAffiliationComponent.Cast(character.FindComponent(FactionAffiliationComponent))	;
		factionAffiliation.SetAffiliatedFaction(playerFaction);
		
		// TODO reset group faction
		// TODO if possible, order military before and after non combattants
		
		return true;
	}
}