datablock SFXProfile(hardImpactSound)
{
   preload = "1";
   description = "AudioDefault3D";
   fileName = "art/sound/cheetah/hardImpact.ogg";
};

datablock SFXProfile(softImpactSound)
{
   preload = "1";
   description = "AudioDefault3D";
   fileName = "art/sound/cheetah/softImpact.ogg";
};

datablock SFXProfile(DirtKickup)
{
   preload = "1";
   description = "AudioDefault3D";
   fileName = "art/sound/cheetah/softImpact.ogg";
};

datablock SFXProfile(DefaultEngineSound)
{
   preload = "1";
   description = "AudioDefault3D";
   fileName = "art/sound/cheetah/cheetah_engine.ogg";
};




datablock ParticleData(DefaultTireParticle)
{
   textureName          = "art/particles/dustParticle";
   dragCoefficient      = "1.99902";
   gravityCoefficient   = "-0.100122";
   inheritedVelFactor   = "0.0998043";
   constantAcceleration = 0.0;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 400;
   colors[0]            = "0.456693 0.354331 0.259843 1";
   colors[1]            = "0.456693 0.456693 0.354331 0";
   sizes[0]             = "0.997986";
   sizes[1]             = "3.99805";
   sizes[2]             = "1.0";
   sizes[3]             = "1.0";
   times[0]             = "0.0";
   times[1]             = "1";
   times[2]             = "1";
   times[3]             = "1";
};

// Default Gray Smoke
datablock ParticleData(GraySmokeParticle)
{
   textureName          = "art/particles/gray_smoke";
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = -0.30;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 300;
   useInvAlpha =  true;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "0.16 0.16 0.16 1.0";
   colors[1]     = "0.1 0.1 0.1 1.0";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.0;
   sizes[1]      = 2.0;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};


datablock ParticleEmitterData(DefaultTireEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 10;
   ejectionVelocity = "14.57";
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 60;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "DefaultTireParticle";
   blendStyle = "ADDITIVE";
};


datablock ParticleEmitterData(GraySmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 4;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 60.0;
   //lifetimeMS       = 250;
   particles = "GraySmokeParticle";
   blendStyle = "ADDITIVE";
   overrideAdvance = false;
};

// Default Black Smoke
datablock ParticleData(BlackSmokeParticle)
{
   textureName          = "art/particles/black_smoke";
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = -0.30;
   lifetimeMS           = 1200;
   lifetimeVarianceMS   = 300;
   useInvAlpha =  true;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "0.56 0.36 0.26 1.0";
   colors[1]     = "0.2 0.2 0.2 1.0";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 4.0;
   sizes[1]      = 2.5;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(BlackSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 4;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   //lifetimeMS       = 250;
   particles = "GraySmokeParticle";
   blendStyle = "ADDITIVE";
   overrideAdvance = false;   
};

// Default bubble emitter.
datablock ParticleData(DefaultVehicleBubbleParticle)
{
   textureName          = "art/particles/bubble";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = -0.25;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 1500;
   lifetimeVarianceMS   = 600;
   useInvAlpha          = false;
   spinRandomMin        = -100.0;
   spinRandomMax        =  100.0;

   colors[0]     = "0.7 0.8 1.0 0.4";
   colors[1]     = "0.7 0.8 1.0 0.4";
   colors[2]     = "0.7 0.8 1.0 0.0";

   sizes[0]      = 0.3;
   sizes[1]      = 0.3;
   sizes[2]      = 0.3;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(DefaultVehicleBubbleEmitter)
{
   ejectionPeriodMS = 9;
   periodVarianceMS = 0;
   ejectionVelocity = 1;
   ejectionOffset   = 0.1;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 80.0;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   particles = "DefaultVehicleBubbleParticle";
};

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

