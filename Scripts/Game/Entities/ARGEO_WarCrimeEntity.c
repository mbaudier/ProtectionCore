class ARGEO_WarCrimeEntityClass: GenericEntityClass
{
}

//! A located war crime, so that it can activate trigger or be documented.
class ARGEO_WarCrimeEntity: GenericEntity
{
	private SCR_ECrimeNotification m_iCrime;
	
	void SetCrime(SCR_ECrimeNotification crime)
	{
		m_iCrime = crime;
	}
	
	bool SerializationSave(BaseSerializationSaveContext context)
	{
		if (!context.IsValid())
			return false;

		context.WriteValue("", GetOrigin());
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