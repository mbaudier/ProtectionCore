[ComponentEditorProps(category: "GameScripted/Character", description: "Manages attributes related to a person protection.")]
class ARGEO_CharacterProtectionComponentClass : GameComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Character component managing Protection-related attributes, notably the protected status.
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
	
	//------------------------------------------------------------------------------------------------
	//! Convenience method to search from the protection component of an agent.
	//! \param agent The agent whose controlled entity may own the component.
	//! \return The protection component, or null if not available.
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

//------------------------------------------------------------------------------------------------
//! List of displacement statuses, from worst to best.
enum ARGEO_ECharacterDisplacementStatus
{
	FLEEING, //!< On the run, either toward a civic center or without goal
	DISPLACED, //!< Registered to a civic center and thus provided with temporary housing
	RESETTLED, //!< Resettled in a proper building in another populated territory
	NORMAL, //!< At home in own populated territory
}