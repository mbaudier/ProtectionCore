[ComponentEditorProps(category: "GameScripted/Character", description: "Manages attributes related to a person protection.")]
class ARGEO_CharacterProtectionComponentClass : GameComponentClass
{
}

class ARGEO_CharacterProtectionComponent : GameComponent
{
	protected bool m_bProtected = false;
	
	protected Faction m_PreProtectionFaction;
	
	protected ARGEO_ECharacterDisplacementStatus m_DisplacementStatus = ARGEO_ECharacterDisplacementStatus.NORMAL;
	protected bool m_bIsPOW = false;
	
	protected ARGEO_CivicCenterEntity m_PlaceOfRegistration;
	
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
		if (m_PreProtectionFaction)
			m_bProtected = true;
		else
			m_bProtected = false;
	}
	
	
	bool IsProtected()
	{
		return m_bProtected;
	}
	
	ARGEO_ECharacterDisplacementStatus GetDisplacementStatus()
	{
		return m_DisplacementStatus;
	}
	
	void SetDisplacementStatus(ARGEO_ECharacterDisplacementStatus displacementStatus, ARGEO_CivicCenterEntity placeOfRegistration)
	{
		m_DisplacementStatus = displacementStatus;
		m_PlaceOfRegistration = placeOfRegistration;
	}
	
	bool IsPOW()
	{
		return m_bIsPOW;
	}
	
	void SetPOWStatus(bool isPOW, ARGEO_CivicCenterEntity placeOfRegistration)
	{
		m_bIsPOW = isPOW;
		m_PlaceOfRegistration = placeOfRegistration;
	}
	
	ARGEO_CivicCenterEntity GetPlaceOfRegistration()
	{
		return m_PlaceOfRegistration;
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

enum ARGEO_ECharacterDisplacementStatus
{
	FLEEING,
	DISPLACED,
	RESETTLED,
	NORMAL,
}