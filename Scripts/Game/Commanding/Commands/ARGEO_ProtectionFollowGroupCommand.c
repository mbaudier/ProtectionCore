//------------------------------------------------------------------------------------------------
//! Follow command which suppresses autonomous behavior
//! when protected agents are in the commanded AI group.
[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_ProtectionFollowGroupCommand: SCR_FollowGroupCommand
{
	//------------------------------------------------------------------------------------------------
	override bool Execute(IEntity cursorTarget, IEntity target, vector targetPosition, int playerID, bool isClient)
	{
		if (!m_sWaypointPrefab || !target || !targetPosition)
			return false;
		
		if (isClient)
		{
			//place to place a logic that would be executed for other players
			return true;
		}
		
		SCR_AIGroup commandedGroup = SCR_AIGroup.Cast(target);
		if (!commandedGroup)
			return false;
		
		if (ARGEO_ProtectionFactionManagerComponent.GroupContainsProtected(commandedGroup))
		{
			// If the group contains unarmed characters, we suppress autonomous behavior
			// otherwise they keep scattering around, while they are not actually targeted (since they are in protected status).
			// Moreover, commanded allied AIs are supposed to stay close to the protected characters.
			m_bForceCommand = true;
			
			AIGroupMovementComponent commandedGroupMovementComp = AIGroupMovementComponent.Cast(commandedGroup.FindComponent(AIGroupMovementComponent));
			if (commandedGroupMovementComp)
				commandedGroupMovementComp.SetGroupCharactersWantedMovementType(EMovementType.RUN);
		}
		else
		{
			m_bForceCommand = false;
		}
		
		return super.Execute(cursorTarget, target, targetPosition, playerID, isClient);
	}
}