//------------------------------------------------------------------------------------------------
//! Extends player statistics with the additional Protection war crimes.
modded enum SCR_EDataStats
{
	LEGAL_KILLS, //!< Legal enemy human kills
	LEGAL_AI_KILLS, //!< Legal enemy AI kills
	ALLIED_KILLS, //!< Military allied human kills
	ALLIED_AI_KILLS, //!< Military allied AI kills
	PROTECTED_KILLS, //!< Protected human kills
	PROTECTED_AI_KILLS, //!< Protected AI kills
	HDC_KILLS, //!< Hors de combat human kills
	HDC_AI_KILLS, //!< Hors de combat AI kills
	
	DISGUISED_KILLER, //!< Killer was disguised as enemy
	PERFIDY_KILLER, //!< Killer was disguised as protected

	WARCRIME_HARMING_PROTECTED, //!< Harming protected
	WARCRIME_HARMING_HDC, //!< Harming hors de combat
	WARCRIME_DISGUISED, //!< Killing while disguised as enemy
	WARCRIME_PERFIDY, //!< Killing while disguised as protected
}