//------------------------------------------------------------------------------------------------
//! Extends editable entity labels with the additional Protection factions.
//! Factions currently being developed are already listed here,
//! in order to "reserve" their labels and numerical IDs.
modded enum EEditableEntityLabel
{
	//
	// AVAILABLE
	//
	FACTION_PROTECTED = 910212, //!< Virtual faction to which all factions are friendly
	FACTION_RURAL = 910213, //!< Rural civilians
	FACTION_URBAN = 910214, //!< Urban civilians
	
	FACTION_GANG = 910215, //!< Urban militia (equivalent to FIA, then considered as a rural militia)
	FACTION_INDPEACE = 910216, //!< Private military contractors specialized in humanitarian tasks
	FACTION_BLUPEACE = 910217, //!< Rich countries peacekeepers
	FACTION_REDPEACE = 910218, //!< Developing countries peacekeepers

	//
	// UNDER DEVELOPMENT
	//
	FACTION_NGO = 910219, //!< Humanitarian NGO
	FACTION_PRESS = 910220, //!< War reporters
	FACTION_IMPERIAL = 910221, //!< Expansionist opposition force
};

modded enum EEditableEntityLabelGroup
{
	BRANCH_BLUPEACE = 217000,
	BRANCH_IMPERIAL = 221000,
};
