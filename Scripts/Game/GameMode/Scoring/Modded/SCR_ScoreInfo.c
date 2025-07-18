modded class SCR_ScoreInfo
{
	int m_iNonCombatantKills = 0;
	int m_iHorsDeCombatKills = 0;
	// Note: disguised and perfidy are dealt with more complex systems such as XP
	
	override bool RplSave(ScriptBitWriter writer)
    {
        vanilla.RplSave(writer);
		writer.WriteInt(m_iNonCombatantKills);
		writer.WriteInt(m_iHorsDeCombatKills);
		return true;
    }

    override bool RplLoad(ScriptBitReader reader)
    {
        vanilla.RplLoad(reader);
		reader.ReadInt(m_iNonCombatantKills);
		reader.ReadInt(m_iHorsDeCombatKills);
        return true;
    }
	
	override void Clear()
	{
        vanilla.Clear();
		m_iNonCombatantKills = 0;
		m_iHorsDeCombatKills = 0;
	}
}