[BaseContainerProps(), SCR_BaseGroupCommandTitleField("m_sCommandName")]
class ARGEO_ProtectionFollowGroupCommand: SCR_FollowGroupCommand
{
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
			// If the group contain unarmed characters, we suppress autonmoumous behavior
			// other they keep scattering around, while they are actually not atrgeted (since they are protected).
			// Moreover, commanded allied AIs are supposed to stay close to the protected characters.
			m_bForceCommand = true;
			
			AIGroupMovementComponent commandedGroupMovementComp = AIGroupMovementComponent.Cast(commandedGroup.FindComponent(AIGroupMovementComponent));
			if (commandedGroupMovementComp)
				commandedGroupMovementComp.SetGroupCharactersWantedMovementType(EMovementType.RUN);

			// set formation to column
			// FIXME doesn't seem to work
			AIFormationComponent aiFormation = AIFormationComponent.Cast(commandedGroup.FindComponent(AIFormationComponent));
			if (aiFormation)
				aiFormation.SetFormation(SCR_Enum.GetEnumName(SCR_EAIGroupFormation, SCR_EAIGroupFormation.Column));
		}
		else
		{
			m_bForceCommand = false;
		}
		
		return super.Execute(cursorTarget, target, targetPosition, playerID, isClient);
	}
}