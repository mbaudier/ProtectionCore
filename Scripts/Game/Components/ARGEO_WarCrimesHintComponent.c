void ScriptInvokerWarCrimesNotificationMethod(array<SCR_ECrimeNotification> crimeNotifications, float criminalScore);
typedef func ScriptInvokerWarCrimesNotificationMethod;
typedef ScriptInvokerBase<ScriptInvokerWarCrimesNotificationMethod> ScriptInvokerWarCrimesNotification;

[ComponentEditorProps(category: "GameScripted/KickHintComponent", description: "")]
class ARGEO_WarCrimesHintComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Game component providing different hints for the various war crimes. 
//! It is triggered by player statistics.
class ARGEO_WarCrimesHintComponent : ScriptComponent
{
	[Attribute("5", desc: "Duration of the short hints (in s). Longer hints will use twice that duration.")]
	protected float m_fHintDuration;
	
	protected ref ScriptInvokerWarCrimesNotification m_OnCriminalScoreIncreased = new ScriptInvokerWarCrimesNotification();
	
	//------------------------------------------------------------------------------------------------
	//! Called on the client in order to show a detailed hint for a single war crime,
	//! or a list of multiple war crimes if there were more than one.
	void ShowUI(array<SCR_ECrimeNotification> crimeNotifications, float criminalScore)
	{
		if (crimeNotifications.Count() == 1)
		{
			SCR_ECrimeNotification crime = crimeNotifications[0];
			if (SCR_ECrimeNotification.TEAMKILL == crime)
				SCR_HintManagerComponent.ShowCustomHint("#AR-FriendlyFire_Text", "#AR-FriendlyFire_Title", m_fHintDuration * 2);
			else if (SCR_ECrimeNotification.NON_COMBATANT_KILL == crime)
				SCR_HintManagerComponent.ShowCustomHint("#PRTC-WarCrime_ProtectedKill_Text", "#PRTC-WarCrime_Title_Prefix#PRTC-WarCrime_ProtectedKill_Title", m_fHintDuration);
			else if (SCR_ECrimeNotification.HORS_DE_COMBAT_KILL == crime)
				SCR_HintManagerComponent.ShowCustomHint("#PRTC-WarCrime_HdcKill_Text", "#PRTC-WarCrime_Title_Prefix#PRTC-WarCrime_HdcKill_Title", m_fHintDuration);
		}
		else
		{
			string msg = "#PRTC-WarCrime_Multiple_Text\n";
			foreach (SCR_ECrimeNotification crime : crimeNotifications)
			{
				if (SCR_ECrimeNotification.TEAMKILL == crime)
					msg += "\n- #AR-FriendlyFire_Title";
				else if (SCR_ECrimeNotification.NON_COMBATANT_KILL == crime)
					msg += "\n- #PRTC-WarCrime_ProtectedKill_Title (#PRTC-WarCrime_Rule_Nbr 1)";
				else if (SCR_ECrimeNotification.HORS_DE_COMBAT_KILL == crime)
					msg += "\n- #PRTC-WarCrime_HdcKill_Title (#PRTC-WarCrime_Rule_Nbr 47)";
				else if (SCR_ECrimeNotification.KILLING_WHILE_DISGUISED == crime)
					msg += "\n- #PRTC-WarCrime_Disguised_Title (#PRTC-WarCrime_Rule_Nbr 62)";
				else if (SCR_ECrimeNotification.PERFIDY == crime)
					msg += "\n- #PRTC-WarCrime_Perfidy_Title (#PRTC-WarCrime_Rule_Nbr 65)";
			}
			
			SCR_HintManagerComponent.ShowCustomHint(msg, "#PRTC-WarCrime_Multiple_Title", m_fHintDuration * 2);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Notifies new war crimes.
	void NotifyClientCriminalScoreIncreased(array<SCR_ECrimeNotification> crimeNotifications, float criminalScore)
	{
		Rpc(RPC_DoIncreaseCriminalScore, crimeNotifications, criminalScore);
	}
	
	//
	// LIFECYCLE
	//
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		m_OnCriminalScoreIncreased.Insert(ShowUI);
	}

	//
	// RPC
	//
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RPC_DoIncreaseCriminalScore(array<SCR_ECrimeNotification> crimeNotifications, float criminalScore)
	{
		m_OnCriminalScoreIncreased.Invoke(crimeNotifications, criminalScore);
	}
}
