[ComponentEditorProps(category: "GameScripted/GameMode", description: "")]
class ARGEO_ProtectionManagerComponentClass : SCR_BaseGameModeComponentClass
{
}

class ARGEO_ProtectionManagerComponent : SCR_BaseGameModeComponent
{
	protected static ARGEO_ProtectionManagerComponent s_Instance;
	
	
	//------------------------------------------------------------------------------------------------
	//! \return
	static ARGEO_ProtectionManagerComponent GetInstance()
	{
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	// constructor
	//! \param[in] src
	//! \param[in] ent
	//! \param[in] parent
	void ARGEO_ProtectionManagerComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		if (!s_Instance)
			s_Instance = this;
		Print("Protection manager instance available");
	}
	
	override void OnGameModeStart()
	{
		Print("Protection manager game mode started");

	}
	
	override void OnGameStateChanged(SCR_EGameModeState state)
	{
		Print("Protection manager game state changed "+state);
	}

}