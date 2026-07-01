/**
 * \class SCR_NameTagData
 * \brief Limits AI unit nametags to display their first name only, resolving localization keys.
 */
modded class SCR_NameTagData
{
    override void GetName(out string name, out notnull array<string> nameParams)
    {
        // Fetch the default profile or identity name key from the engine
        super.GetName(name, nameParams);
        
        if (!m_Entity)
            return;

        // Check if the target is an AI (human players have a positive ID)
        if (m_iPlayerID <= 0)
        {
            // Resolve string key (e.g. #AR_Name) into literal text (e.g. "John Doe")
            string translatedName = WidgetManager.Translate(name);

            // Split the translated text string wherever a space character occurs
            array<string> nameParts = {};
            translatedName.Split(" ", nameParts, false);

            // Overwrite the output name with only the first extracted element
            if (nameParts.Count() > 0)
            {
                name = nameParts[0];
            }
        }
    }
}
