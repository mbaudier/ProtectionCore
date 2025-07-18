class ARGEO_WarCrimeEntityClass: GenericEntityClass
{
}

//! A located war crime, so that it can activate trigger or be documented.
sealed class ARGEO_WarCrimeEntity: GenericEntity
{
	private SCR_ECrimeNotification m_iCrime;
	
	// criminal
	private FactionKey m_sCriminalFactionKey;
	private string m_sCriminalSurname;
	private string m_sCriminalGivenName;
	
	// victim
	private FactionKey m_sVictimFactionKey;
	private string m_sVictimSurname;
	private string m_sVictimGivenName;
	private SCR_EIdentityGender m_iVictimGender;
	private ref SCR_ExtendedIdentity m_VictimExtendedIdentity;
	private ref SCR_IdentityBio m_VictimIdentityBio;
	
	private bool immutable = false;
	
	//
	// ACCESSORS
	//
	void SetCrime(SCR_ECrimeNotification crime)
	{
		if (CheckImmutable())
			return;
		m_iCrime = crime;
	}
	
	SCR_ECrimeNotification GetCrime()
	{
		return m_iCrime;
	}
	
	void SetCriminalFactionKey(FactionKey criminalFactionKey)
	{
		if (CheckImmutable())
			return;
		m_sCriminalFactionKey = criminalFactionKey;
	}
	
	SCR_ECrimeNotification GetCriminalFactionKey()
	{
		return m_sCriminalFactionKey;
	}
	
	void SetVictimFactionKey(FactionKey victimFactionKey)
	{
		if (CheckImmutable())
			return;
		m_sVictimFactionKey = victimFactionKey;
	}
	
	SCR_ECrimeNotification GetVictimFactionKey()
	{
		return m_sVictimFactionKey;
	}

	void SetCriminalIdentity(string surname, string givenName)
	{
		m_sCriminalSurname = surname;
		m_sCriminalGivenName = givenName;
	}
	
	string GetCriminalSurname()
	{
		return m_sCriminalSurname;
	}
	
	string GetCriminalGivenName()
	{
		return m_sCriminalGivenName;
	}	
		
	void SetVictimIdentity(string surname, string givenName, SCR_EIdentityGender gender, SCR_ExtendedIdentity extendedIdentity, SCR_IdentityBio identityBio)
	{
		m_sVictimSurname = surname;
		m_sVictimGivenName = givenName;
		m_iVictimGender = gender;
		m_VictimExtendedIdentity = extendedIdentity;
		m_VictimIdentityBio = identityBio;
	}
		
	string GetVictimSurname()
	{
		return m_sVictimSurname;
	}
	
	string GetVictimGivenName()
	{
		return m_sVictimGivenName;
	}	
	
	SCR_EIdentityGender GetVictimGender()
	{
		return m_iVictimGender;
	}
		
	SCR_ExtendedIdentity GetVictimExtendedIdentity()
	{
		return m_VictimExtendedIdentity;
	}
		
	SCR_IdentityBio GetVictimIdentityBio()
	{
		return m_VictimIdentityBio;
	}
		
	private bool CheckImmutable()
	{
		if (immutable)
			Print("War crime cannot be modified a posterior.", LogLevel.WARNING);
		return immutable;
	}
	
	void SetImmutable()
	{
		if (immutable)
			return;
		immutable = true;
	}
	
	//
	// SERIALIZATION
	//
	
	bool SerializationSave(BaseSerializationSaveContext context)
	{
		if (!context.IsValid())
			return false;

		context.WriteValue("position", GetOrigin());
		context.WriteValue("crime", m_iCrime);

		return true;
	}

	bool SerializationLoad(BaseSerializationLoadContext context)
	{
		if (!context.IsValid())
			return false;

		vector position;
		context.ReadValue("position", position);
		context.ReadValue("crime", m_iCrime);

		return true;
	}
}