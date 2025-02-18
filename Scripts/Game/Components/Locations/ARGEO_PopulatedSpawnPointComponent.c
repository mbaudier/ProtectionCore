class ARGEO_PopulatedSpawnPointComponentClass : SCR_AmbientPatrolSpawnPointComponentClass
{
}

class ARGEO_PopulatedSpawnPointComponent : SCR_AmbientPatrolSpawnPointComponent
{
	protected float m_fDefaultAILimitThreshold = 0.95;

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		m_fDefaultAILimitThreshold = m_fAILimitThreshold;
		DisableSpawn();
		//Print("Populated spawnpoint initialized");
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		//Print("Populated spawnpoint post-init");
	}

	override void SpawnPatrol()
	{
		super.SpawnPatrol();
		Print("Populated spawn point - " + GetOwner().GetID() + " - spawned " + m_SavedFaction.GetFactionKey());
	}
	
	override void DespawnPatrol()
	{
		super.DespawnPatrol();
		Print("Populated spawn point - " + GetOwner().GetID() + " - despawned " + m_SavedFaction.GetFactionKey());
	}
	
	override void ActivateGroup()
	{
		super.ActivateGroup();
		Print("Populated spawn point - " + GetOwner().GetID() + " - activated " + m_SavedFaction.GetFactionKey());
	}
	
	override void DeactivateGroup()
	{
		super.DeactivateGroup();
		Print("Populated spawn point - " + GetOwner().GetID() + " - deactivated " + m_SavedFaction.GetFactionKey());
	}
	
	void DisableSpawn()
	{
		SetAILimitThreshold(0);
	}
	
	void EnableSpawn()
	{
		SetAILimitThreshold(m_fDefaultAILimitThreshold);
		if(m_iMembersAlive == 0)
		{	
			// workaround for when bases cleared it as remnants
			m_iMembersAlive = -1;
		}
	}
	
	protected void SetAILimitThreshold(float threshold)
	{
		m_fAILimitThreshold = threshold;
	}
}