// ============================================================
// Project            :  KB Clean
// File               :  .\kgb.fps1\client\scripts\vehicleSupport.cs
// Copyright          :  
// Author             :  Administrator
// Created on         :  Wednesday, February 13, 2008 12:49 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================
vehicleEditGui.curPane = "";

function vehicleEditGui::onAdd(%this){
   %this.clearFields();
}

function vehicleEditGui::onWake(%this){
	if(%this.curPane $= "")
	   %this-->OptGraphicsButton.performClick();
}

function vehicleEditGui::setPane(%this, %pane)
{
   %this-->VeditEnginePane.setVisible(false);
   %this-->VeditVehiclePane.setVisible(false);

   %this.findObjectByInternalName( "Vedit" @ %pane @ "Pane", true ).setVisible(true);
   %this.curPane = %pane;
}

function vehicleEditGui::updateDatablocks(%this){
	saveEngineGuiData(%this.curVehicle,	%this.curEngine, %this.curTire, %tire, %this.curSpring);	
	Canvas.popDialog(vehicleEditGui);
}

function vehicleEditGui::saveToFile(%this){
	echo("SaveStart");
	%base = ve_datablockPrefix.getText();
	%oname = %base @ "Container";
	if(isObject(%oname)){
		%oname.delete();
	}
	%grp = new SimGroup(%oname){};

	%vehicle = %this.curVehicle.clone();
	%vehicle.name = %base @ "Car";
	%engine = %this.curEngine.clone();
	%engine.name = %base @ "Engine";
	%tire = %this.curTire.clone();
	%tire.name = %base @ "Tire";
	%spring = %this.curSpring.clone();
	%spring.name = %base @ "Spring";	
			
	echo("Vehicle Datablocks Created for"@%base@".");
   if(isObject(%vehicle.damageEmitter[0])){
      if(isObject(%vehicle.damageEmitter[0].particles))
         %grp.add(%vehicle.damageEmitter[0].particles);
      %grp.add(%vehicle.tireEmitter);      
   }else{
      %vehicle.damageEmitter[0]="";
   }
   if(isObject(%vehicle.damageEmitter[1])){
      if(isObject(%vehicle.damageEmitter[1].particles))
         %grp.add(%vehicle.damageEmitter[1].particles);
      %grp.add(%vehicle.tireEmitter);      
   }else{
      %vehicle.damageEmitter[1]="";
   }
   if(isObject(%vehicle.damageEmitter[2])){
      if(isObject(%vehicle.damageEmitter[2].particles))
         %grp.add(%vehicle.damageEmitter[2].particles);
      %grp.add(%vehicle.tireEmitter);      
   }else{
      %vehicle.damageEmitter[2]="";
   }
   if(isObject(%vehicle.splashEmitter)){
      if(isObject(%vehicle.splashEmitter.particles))
         %grp.add(%vehicle.splashEmitter.particles);
      %grp.add(%vehicle.splashEmitter);
   } else {
      %vehicle.splashEmitter="";
   }
         
   if(isObject(%vehicle.tireEmitter)){
      if(isObject(%vehicle.tireEmitter.particles))
         %grp.add(%vehicle.tireEmitter.particles);
      %grp.add(%vehicle.tireEmitter);
   }else {
      %vehicle.tireEmitter="";
   }
   
   if(isObject(%vehicle.engineSound))
      %grp.add(%vehicle.engineSound);
   
   if(isObject(%vehicle.squealSound))
      %grp.add(%vehicle.squealSound);
            
   if(isObject(%vehicle.softImpactSound))
      %grp.add(%vehicle.softImpactSound);
      
   if(isObject(%vehicle.hardImpactSound))
      %grp.add(%vehicle.hardImpactSound);

	%grp.add(%engine);
	%grp.add(%tire);
	%grp.add(%spring);
	%grp.add(%vehicle);
	
	saveEngineGuiData(%vehicle, %engine, %tire, %spring);
	%fn = "./"@%base@".cs";
	warn("Saving:"@%fn);
	%grp.save(%fn);
}

function loadEngineGui(%vehicle, %engine, %tire, %spring)
{
	vehicleEditGui.curVehicle = %vehicle;
	vehicleEditGui.curEngine = %engine;
	vehicleEditGui.curTire= %tire;
	vehicleEditGui.curSpring = %spring;
	
   
   E_EngineName.setText("(" @ %engine.name @ ")");
   C_DatablockName.setText("(" @ %vehicle.name @ ")");
   W_TireName.setText("(" @ %tire.name @ ")");
   S_SpringName.setText("(" @ %spring.name @ ")");
   
	E_Idle.setText(%engine.throttleIdle);
	E_MinRpm.setText(%engine.minRPM);
	E_MaxRpm.setText(%engine.maxRPM);
	E_IdleRpm.setText(%engine.IdleRPM);
	E_EngineDrag.setText(%engine.engineDrag);
	E_NeutralBoost.setText(%engine.neutralBoost);
	E_SlowDownRate.setText(%engine.slowdownRate);
	E_UseAutomatic.setText(%engine.useAutomatic);
	E_FuelFlow.setText(%engine.fuelFlow);
	E_InertiaFactor.setText(%engine.inertiaFactor);
	E_OverRevSlowdown.setText(%engine.overRevSlowdown);
	E_MinPitch.setText(%engine.minPitch);
	E_MaxPitch.setText(%engine.maxPitch);
	E_MinVolume.setText(%engine.minVolume);
	E_MaxVolume.setText(%engine.maxVolume);
	E_TransmissionSlip.setText(%engine.transmissionSlip);
	
	E_UpShift.setText(%engine.shiftUpRPM);
	E_DnShift.setText(%engine.shiftDownRPM);
	E_DiffRatio.setText(%engine.differentialRatio);
	E_RevRatio.setText(%engine.reverseRatio);
	E_NumGears.setText(%engine.numGears);
	for(%i=0; %i<7; %i++){
		//echo("E_GearRatio"@%i@".setText(%engine.gearRatios["@%i@"]);");
		//echo(%engine.gearRatios[%i]);
		eval("E_GearRatio"@%i@".setText(%engine.gearRatios["@%i@"]);");
	}
	
	E_NumTorqueLvl.setText(%engine.numTorqueLevels);
	for(%i=0; %i<12; %i++){
		eval("E_TL_RPM"@%i@".setText(%engine.RPMValues["@%i@"]);");
		eval("E_TL_Torque"@%i@".setText(%engine.TorqueLevels["@%i@"]);");
	}
	
	//C_MaxWheel.setText(%vehicle.maxWheelSpeed);
	C_DustVolume.setText(%vehicle.DustVolume);
	C_BrakeTorque.setText(%vehicle.brakeTorque);
	C_MaxSteer.setText(%vehicle.maxSteeringAngle);
	C_Mass.setText(%vehicle.mass);
	C_MassCenter.setText(%vehicle.massCenter);
	C_Drag.setText(%vehicle.drag);
	C_BodyFriction.setText(%vehicle.bodyFriction);
	C_BodyRestitution.setText(%vehicle.bodyRestitution);
   C_MassBox.setText(%vehicle.massBox);
   
   
   
   C_SteeringReturn.setText(%vehicle.steeringReturn);
	C_SteeringReturnSpeedScale.setText(%vehicle.steeringReturnSpeedScale);
	C_PowerSteering.setText(%vehicle.powerSteering);

//	C_SteerBoostSpeed.setText(%vehicle.steeringBoostVelocity);
//	C_SteerBoostAngle.setText(%vehicle.steeringBoost);


	
	S_len.setText(%spring.length);
	S_force.setText(%spring.force);
	S_antiSway.setText(%spring.antiSwayForce);
	S_Damping.setText(%spring.damping);
	
	W_LatForce.setText(%tire.lateralForce);
	W_LatDamping.setText(%tire.lateralDamping);
	W_LatRelax.setText(%tire.lateralRelaxation);
	W_LonForce.setText(%tire.longitudinalForce);
	W_LonDamping.setText(%tire.longitudinalDamping);
	W_LonRelax.setText(%tire.longitudinalRelaxation);
	W_StaFriction.setText(%tire.staticFriction);
	W_KenFriction.setText(%tire.kineticFriction);
	
	Canvas.pushDialog(vehicleEditGui);
}

function saveEngineGuiData(%vehicle, %engine, %tire, %spring)
{	
	//%engine.throttleIdle = E_Idle.getText();
	%engine.minRPM = E_MinRpm.getText();
	%engine.maxRPM = E_MaxRpm.getText();
	%engine.idleRPM = E_IdleRPM.getText();
	%engine.engineDrag = E_EngineDrag.getText();
	%engine.neutralBoost = E_NeutralBoost.getText();
	%engine.slowdownRate = E_SlowdownRate.getText();
	%engine.useAutomatic = E_UseAutomatic.getText();
	%engine.fuelFlow = E_FuelFlow.getText();
	%engine.inertiaFactor = E_IntertiaFactor.getText();
	%engine.overRevSlowdown = E_OverRevSlowdown.getText();
	%engine.minPitch = E_MinPitch.getText();
	%engine.maxPitch = E_MaxPitch.getText();
	%engine.minVolume = E_MinVolume.getText();
	%engine.maxVolume = E_MaxVolume.getText();
	%engine.transmissionSlip = E_TransmissionSlip.getText();
	%engine.shiftUpRPM = E_UpShift.getText();
	%engine.shiftDownRPM = E_DnShift.getText();
	%engine.differentialRatio = E_DiffRatio.getText();
	%engine.reverseRatio = E_RevRatio.getText();
	%engine.numGears = E_NumGears.getText();
	
	for(%i=0; %i<7; %i++){
		eval("%engine.gearRatios["@%i@"]=E_GearRatio"@%i@".getText();");
	}
	
	%engine.numTorqueLevels=E_NumTorqueLvl.getText();
	for(%i=0; %i<12; %i++){
		eval("%engine.RPMValues["@%i@"] = E_TL_RPM"@%i@".getText();");
		eval("%engine.TorqueLevels["@%i@"] = E_TL_Torque"@%i@".getText();");
	}

	%vehicle.DustVolume = C_DustVolume.getText();
	%vehicle.brakeTorque = C_BrakeTorque.getText();
	%vehicle.maxSteeringAngle = C_MaxSteer.getText();
	%vehicle.mass = C_Mass.getText();
	%vehicle.massCenter = C_MassCenter.getText();
	%vehicle.bodyFriction = C_BodyFriction.getText();
	%vehicle.bodyRestitution = C_BodyRestitution.getText();
   %vehicle.centerSteeringRate = C_CenterSteeringRate.getText();
   %vehicle.drag = C_Drag.getText();
   %vehicle.massBox = C_MassBox.getText();
   
   %vehicle.steeringReturn = C_SteeringReturn.getText();
	%vehicle.steeringReturnSpeedScale = C_SteeringReturnSpeedScale.getText();
	%vehicle.powerSteering = C_PowerSteering.getText();

	//%vehicle.steeringBoostVelocity = C_SteerBoostSpeed.getText();
	//%vehicle.steeringBoost = C_SteerBoostAngle.getText();

   
	%spring.length = S_len.getText();
	%spring.force = S_force.getText();
	%spring.antiSwayForce = S_antiSway.getText();
	%spring.damping = S_Damping.getText();
	
	%tire.lateralForce = W_LatForce.getText();
	%tire.lateralDamping = W_LatDamping.getText();
	%tire.lateralRelaxation = W_LatRelax.getText();
	%tire.longitudinalForce = W_LonForce.getText();
	%tire.longitudinalDamping = W_LonDamping.getText();
	%tire.longitudinalRelaxation = W_LonRelax.getText();
	%tire.staticFriction = W_StaFriction.getText();
	%tire.kineticFriction = W_KenFriction.getText();
}


function vehicleEditGui::clearFields(%this){
	vehicleEditGui.curVehicle = "";
	vehicleEditGui.curEngine = "";
	vehicleEditGui.curTire= "";
	vehicleEditGui.curSpring = "";
	
   
   E_EngineName.setText("(engine)");
   C_DatablockName.setText("(vehicle)");
   W_TireName.setText("(tire)");
   S_SpringName.setText("(spring)");
   
	E_Idle.setText("");
	E_MinRpm.setText("");
	E_MaxRpm.setText("");
	E_IdleRpm.setText("");
	E_EngineDrag.setText("");
	E_NeutralBoost.setText("");
	E_SlowDownRate.setText("");
	E_UseAutomatic.setText("");
	E_FuelFlow.setText("");
	E_InertiaFactor.setText("");
	E_OverRevSlowdown.setText("");
	E_MinPitch.setText("");
	E_MaxPitch.setText("");
	E_MinVolume.setText("");
	E_MaxVolume.setText("");
	E_TransmissionSlip.setText("");
	
	E_UpShift.setText("");
	E_DnShift.setText("");
	E_DiffRatio.setText("");
	E_RevRatio.setText("");
	E_NumGears.setText("");
   %s="";
	for(%i=0; %i<7; %i++){
		//echo("E_GearRatio"@%i@".setText(%engine.gearRatios["@%i@"]);");
		//echo(%engine.gearRatios[%i]);      
		eval("E_GearRatio"@%i@".setText(\"\");");
	}
	
	E_NumTorqueLvl.setText(%engine.numTorqueLevels);
	for(%i=0; %i<12; %i++){
		eval("E_TL_RPM"@%i@".setText(\"\");");
		eval("E_TL_Torque"@%i@".setText(\"\");");
	}
	
	//C_MaxWheel.setText(%vehicle.maxWheelSpeed);
	C_DustVolume.setText("");
	//C_SteerBoostSpeed.setText("");
	//C_SteerBoostAngle.setText("");
	C_BrakeTorque.setText("");
	C_MaxSteer.setText("");
	C_Mass.setText("");
	C_MassCenter.setText("");
   C_MassBox.setText("");
	C_Drag.setText("");
	C_BodyFriction.setText("");
	C_BodyRestitution.setText("");
   C_CenterSteeringRate.setText("");

   C_SteeringReturn.setText("");
	C_SteeringReturnSpeedScale.setText("");
	C_PowerSteering.setText("");



	
	S_len.setText("");
	S_force.setText("");
	S_antiSway.setText("");
	S_Damping.setText("");
	
	W_LatForce.setText("");
	W_LatDamping.setText("");
	W_LatRelax.setText("");
	W_LonForce.setText("");
	W_LonDamping.setText("");
	W_LonRelax.setText("");
	W_StaFriction.setText("");
	W_KenFriction.setText("");

}
