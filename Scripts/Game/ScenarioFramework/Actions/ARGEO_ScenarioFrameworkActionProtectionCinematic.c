//------------------------------------------------------------------------------------------------
//! The helicopters' cinematic logic as a component (instead of a custom entity script),
//! so that it can be used in faction-switched slots.
[BaseContainerProps(), SCR_ContainerActionTitle()]
class ARGEO_ScenarioFrameworkActionProtectionCinematic : SCR_ScenarioFrameworkActionBase
{
	//------------------------------------------------------------------------------------------------
	override void OnActivate(IEntity object)
	{
		SCR_ScenarioFrameworkLayerBase layer = SCR_ScenarioFrameworkLayerBase.Cast(object.FindComponent(SCR_ScenarioFrameworkLayerBase));
		if (!layer)
		{
			Print(string.Format("ScenarioFramework Action: Action %1 attached on %2 is not called from layer and won't do anything.", this, object.GetName()), LogLevel.ERROR);
			return;
		}
		IEntity entity = layer.GetSpawnedEntity();
		
		// Allows helicopter cinematics to be faction-switchable
		VehicleHelicopterSimulation m_Vehicle_s = VehicleHelicopterSimulation.Cast(entity.FindComponent(VehicleHelicopterSimulation));
		if (m_Vehicle_s)
		{
			m_Vehicle_s.EngineStart();
			m_Vehicle_s.SetThrottle(1);
			m_Vehicle_s.RotorSetForceScaleState(0, 2);
			m_Vehicle_s.RotorSetForceScaleState(1, 1);
			
			vector velOrig = entity.GetPhysics().GetVelocity();
			vector rotVector = entity.GetAngles();
			vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * 50, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * 50 };
			entity.GetPhysics().SetVelocity(vel);
		}
	}
}
