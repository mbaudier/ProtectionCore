[ComponentEditorProps(category: "GameScripted/Character", description: "Manages attributes related to a person protection.")]
class ARGEO_CharacterProtectionComponentClass : GameComponentClass
{
}

class ARGEO_CharacterProtectionComponent : GameComponent
{
	protected bool m_bProtected = false;
	
	protected Faction m_PreProtectionFaction;
	
	protected ARGEO_CharacterDisplacementStatus m_DisplacementStatus = ARGEO_CharacterDisplacementStatus.NORMAL;
	
	//
	// ACCESSORS
	//
	Faction GetPreProtectionFaction()
	{
		return m_PreProtectionFaction;
	}
	
	void SetPreProtectionFaction(Faction preProtectionFaction)
	{
		m_PreProtectionFaction = preProtectionFaction;
		if (preProtectionFaction)
			m_bProtected = true;
		else
			m_bProtected = false;
	}
	
	bool IsProtected()
	{
		return m_bProtected;
	}
	
	ARGEO_CharacterDisplacementStatus GetDisplacementStatus()
	{
		return m_DisplacementStatus;
	}
	
	void SetDisplacementStatus(ARGEO_CharacterDisplacementStatus displacementStatus)
	{
		m_DisplacementStatus = displacementStatus;
	}

	//
	// UTILITIES
	//
	static ARGEO_CharacterProtectionComponent FindFromAgent(AIAgent agent)
	{
		if (!agent)
			return null;
		IEntity controlledEntity = agent.GetControlledEntity();
		if (!controlledEntity)
			return null;
		return ARGEO_CharacterProtectionComponent.Cast(controlledEntity.FindComponent(ARGEO_CharacterProtectionComponent));
	}
}

enum ARGEO_CharacterDisplacementStatus
{
	FLEEING,
	DISPLACED,
	RESETTLED,
	NORMAL,
}