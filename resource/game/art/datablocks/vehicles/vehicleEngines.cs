// ============================================================
// Project            :  kgb_test
// File               :  .\scripts\server\vehicleEngies.cs
// Copyright          :  
// Author             :  kent
// Created on         :  Saturday, September 17, 2016 1:06 PM
//
// Editor             :  TorqueDev v. 1.2.5129.4848
//
// Description        :  
//                    :  
//                    :  
// ============================================================
datablock WheeledVehicleEngine(DefaultEngine) {
	// Default Engine info...	
	MinRPM = 10.0;
	MaxRPM = 6000.0;
	idleRPM = 500.0;
   
	engineDrag = 0.001;
	neutralBoost = 3.5;
	slowDownRate = 0.001;
	inertiaFactor = 1;
	overRevSlowdown = 0.8;
	fuelFlow = 0.1;
	
	throttleIdle = 0.1;
	useAutomatic = true;
	numGears = 5;
	
	gearRatios[0] = 3.78;
	gearRatios[1] = 2.06;
	gearRatios[2] = 1.35;
	gearRatios[3] = 0.97;
	gearRatios[4] = 0.74;
	

/*	gearRatios[0] = 10;
	gearRatios[1] = 9;
	gearRatios[2] = 8;
	gearRatios[3] = 7;
	gearRatios[4] = 6;
*/
   
  	differentialRatio = 2.9;
	reverseRatio = 3.60;
	
	shiftUpRPM = 4000.0;
	shiftDownRPM = 1800.0;
	transmissionSlip = 250;

	numTorqueLevels = 9;
	
	rpmValues[0] = 500.0;
	torqueLevels[0] = 170.0;	
	rpmValues[1] = 1000.0;
	torqueLevels[1] = 175.0;	
	rpmValues[2] = 1500.0;
	torqueLevels[2] = 80.0;	
	rpmValues[3] = 2000.0;
	torqueLevels[3] = 85.0;
	rpmValues[4] = 2500.0;
	torqueLevels[4] = 90.0;	
	rpmValues[5] = 3000.0;
	torqueLevels[5] = 100.0;	
	rpmValues[6] = 3500.0;
	torqueLevels[6] = 95.0;
	rpmValues[7] = 4000.0;
	torqueLevels[7] = 110.0;	
	rpmValues[8] = 4500.0;
	torqueLevels[8] = 200.0;
	
};

datablock WheeledVehicleEngine(JeepEngine) {
	// Default Engine info...	
	MinRPM = 10.0;
	MaxRPM = 6000.0;
	idleRPM = 500.0;
   
	engineDrag = 0.001;
	neutralBoost = 3.5;
	slowDownRate = 0.001;
	inertiaFactor = 1;
	overRevSlowdown = 0.8;
	fuelFlow = 0.1;
	
	throttleIdle = 0.1;
	useAutomatic = true;
	numGears = 5;
	
	gearRatios[0] = 3.78;
	gearRatios[1] = 2.06;
	gearRatios[2] = 1.35;
	gearRatios[3] = 0.97;
	gearRatios[4] = 0.74;
	

/*	gearRatios[0] = 10;
	gearRatios[1] = 9;
	gearRatios[2] = 8;
	gearRatios[3] = 7;
	gearRatios[4] = 6;
*/
   
  	differentialRatio = 2.9;
	reverseRatio = 3.60;
	
	shiftUpRPM = 4000.0;
	shiftDownRPM = 1800.0;
	transmissionSlip = 250;

	numTorqueLevels = 9;
	
	rpmValues[0] = 500.0;
	torqueLevels[0] = 170.0;	
	rpmValues[1] = 1000.0;
	torqueLevels[1] = 175.0;	
	rpmValues[2] = 1500.0;
	torqueLevels[2] = 80.0;	
	rpmValues[3] = 2000.0;
	torqueLevels[3] = 85.0;
	rpmValues[4] = 2500.0;
	torqueLevels[4] = 90.0;	
	rpmValues[5] = 3000.0;
	torqueLevels[5] = 100.0;
	rpmValues[6] = 3500.0;
	torqueLevels[6] = 95.0;
	rpmValues[7] = 4000.0;
	torqueLevels[7] = 110.0;	
	rpmValues[8] = 4500.0;
	torqueLevels[8] = 200.0;
	
};

