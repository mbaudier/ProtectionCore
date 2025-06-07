[ComponentEditorProps(category: "GameScripted/Character", description: "Manages attributes related to a person protection.")]
class ARGEO_CharacterProtectionComponentClass : GameComponentClass
{
}

class ARGEO_CharacterProtectionComponent : GameComponent
{
	private Faction m_PreProtectionFaction;
	
	Faction GetPreProtectionFaction()
	{
		return m_PreProtectionFaction;
	}
	
	void SetPreProtectionFaction(Faction preProtectionFaction)
	{
		m_PreProtectionFaction = preProtectionFaction;
	}

}