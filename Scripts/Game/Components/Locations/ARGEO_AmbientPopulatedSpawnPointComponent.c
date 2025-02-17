class ARGEO_AmbientPopulatedSpawnPointComponentClass : SCR_AmbientPatrolSpawnPointComponentClass
{
}

class ARGEO_AmbientPopulatedSpawnPointComponent : SCR_AmbientPatrolSpawnPointComponent
{
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		Print("Populated spawnpoint initialized");
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		Print("Populated spawnpoint post-init");
	}

	override void SpawnPatrol()
	{
		super.SpawnPatrol();
		Print("Populated spawnpoint spawn patrol");
	}
}