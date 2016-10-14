//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

datablock SFXProfile(JeepGunSound)
{
   filename = "art/sound/jeep/jeep_gun";
   description = AudioClose3D;
   preload = true;
};

datablock SFXPlayList(JeepGunSoundList)
{
   // Use a looped description so the list playback will loop.
   description = AudioClose3D;

   track[ 0 ] = JeepGunSound;
};

datablock SFXProfile(JeepEngineSound)
{
   preload = "1";
   description = "AudioCloseLoop3D";
   fileName = "art/sound/jeep/jeep_engine";
};

datablock SFXProfile(JeepSqueal)
{
   preload = "1";
   description = "AudioDefault3D";
   fileName = "art/sound/cheetah/cheetah_squeal.ogg";
};

datablock ProjectileData(TurretBulletProjectile)
{
   projectileShapeName = "";

   directDamage        = 50;
   radiusDamage        = 25;
   damageRadius        = 5;
   areaImpulse         = 0.5;
   impactForce         = 1;

   explosion           = BulletDirtExplosion;
   decal               = BulletHoleDecal;

   muzzleVelocity      = 200;
   velInheritFactor    = 1;

   armingDelay         = 0;
   lifetime            = 992;
   fadeDelay           = 800;
   bounceElasticity    = 0;
   bounceFriction      = 0;
   isBallistic         = false;
   gravityMod          = 0.80;
};

function TurretBulletProjectile::onCollision(%this,%obj,%col,%fade,%pos,%normal)
{
   // Apply impact force from the projectile.
   
   // Apply damage to the object all shape base objects
   if ( %col.getType() & $TypeMasks::GameBaseObjectType )
      %col.damage(%obj,%pos,%this.directDamage,"TurretBulletProjectile");
}

datablock ParticleEmitterData(TurretFireSmokeEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 10;
   ejectionVelocity = "0";
   velocityVariance = "0";
   thetaMin         = "0";
   thetaMax         = "0";
   lifetimeMS       = 250;
   particles = "GunFireSmoke";
   blendStyle = "NORMAL";
   softParticles = "0";
   originalName = "GunFireSmokeEmitter";
   alignParticles = "0";
   orientParticles = "0";
};

datablock ShapeBaseImageData(JeepTurretImage)
{
   // Basic Item properties
   shapeFile = "art/shapes/Jeep/Gun/jeep_gun.cached.dts";
   emap = true;

   // Specify mount point & offset for 3rd person, and eye offset
   // for first person rendering.
   mountPoint = 1;
   firstPerson = false;
   useEyeOffset = true;  
   eyeOffset = "0 0 -2";

   // When firing from a point offset from the eye, muzzle correction
   // will adjust the muzzle vector to point to the eye LOS point.
   // Since this weapon doesn't actually fire from the muzzle point,
   // we need to turn this off.
   correctMuzzleVector = false;

   // Add the WeaponImage namespace as a parent, WeaponImage namespace
   // provides some hooks into the inventory system.
   class = "WeaponImage";

   usesEnergy = true;  
   minEnergy = 10;  
   infiniteAmmo = 1;

   projectile = TurretBulletProjectile;
   projectileType = Projectile;
   projectileSpread = "0.005";

   // Weapon lights up while firing
   lightColor = "0.992126 0.968504 0.685039 1";
   lightRadius = "4";
   lightDuration = "100";
   lightType = "WeaponFireLight";
   lightBrightness = 2;

   // Shake camera while firing.
   shakeCamera = false;

   // Images have a state system which controls how the animations
   // are run, which sounds are played, script callbacks, etc. This
   // state system is downloaded to the client so that clients can
   // predict state changes and animate accordingly.  The following
   // system supports basic ready->fire->reload transitions as
   // well as a no-ammo->dryfire idle state.

   useRemainderDT = true;

   // Initial start up state
   stateName[0]                     = "Preactivate";
   stateTransitionOnLoaded[0]       = "Activate";
   stateTransitionOnNoAmmo[0]       = "NoAmmo";

   // Activating the gun.  Called when the weapon is first
   // mounted and there is ammo.
   stateName[1]                     = "Activate";
   stateTransitionOnTimeout[1]      = "Ready";
   stateTimeoutValue[1]             = 0.5;
   stateSequence[1]                 = "Activate";

   // Ready to fire, just waiting for the trigger
   
   stateName[2]                     = "Ready";
   stateTimeoutValue[2]             = 10;
   stateWaitForTimeout[2]           = false;
   stateScaleAnimation[2]           = false;
   stateScaleAnimationFP[2]         = false;
   stateTransitionOnNoAmmo[2]       = "NoAmmo";
   stateTransitionOnTriggerDown[2]  = "Fire";
   
   

   // Fire the weapon. Calls the fire script which does
   // the actual work.
   stateName[3]                     = "Fire";
   stateTransitionOnTimeout[3]      = "NewRound";
   stateTimeoutValue[3]             = 0.1;
   stateFire[3]                     = true;
   stateRecoil[3]                   = "";
   stateAllowImageChange[3]         = false;
   stateSequence[3]                 = "fire";
   stateSequenceRandomFlash[3]      = true;        // use muzzle flash sequence
   stateScript[3]                   = "onFire";
   stateSound[3]                    = JeepGunSoundList;
   stateEmitter[3]                  = TurretFireSmokeEmitter;
   stateEmitterTime[3]              = 0.025;
   



   // No ammo in the weapon, just idle until something
   // shows up. Play the dry fire sound if the trigger is
   // pulled.
   stateName[4]                     = "NoAmmo";
   stateTransitionOnAmmo[4]         = "Ready";
   stateSequence[4]                 = "idle";
   stateTransitionOnTriggerDown[4]  = "DryFire";

   // No ammo dry fire
   stateName[5]                     = "DryFire";
   stateTimeoutValue[5]             = .7;
   stateTransitionOnTimeout[5]      = "NoAmmo";
   stateScript[5]                   = "onDryFire";
   
      // Put another round into the chamber if one is available
   stateName[6]                     = "NewRound";
   stateTransitionOnNoAmmo[6]       = "NoAmmo";
   stateTransitionOnTimeout[6]      = "Ready";
   stateWaitForTimeout[6]           = "0";
   stateTimeoutValue[6]             = 0.05;
   stateAllowImageChange[6]         = false;
   stateEjectShell[6]               = true;
};

//-----------------------------------------------------------------------------
// Information extacted from the shape.
//
// Wheel Sequences
//    spring#        Wheel spring motion: time 0 = wheel fully extended,
//                   the hub must be displaced, but not directly animated
//                   as it will be rotated in code.
// Other Sequences
//    steering       Wheel steering: time 0 = full right, 0.5 = center
//    breakLight     Break light, time 0 = off, 1 = breaking
//
// Wheel Nodes
//    hub#           Wheel hub, the hub must be in it's upper position
//                   from which the springs are mounted.
//
// The steering and animation sequences are optional.
// The center of the shape acts as the center of mass for the car.

//-----------------------------------------------------------------------------
datablock WheeledVehicleTire(JeepCarTire)
{
   // Tires act as springs and generate lateral and longitudinal
   // forces to move the vehicle. These distortion/spring forces
   // are what convert wheel angular velocity into forces that
   // act on the rigid body.
   shapeFile = "art/shapes/Jeep/Wheel/wheel.DAE";
   staticFriction = 14.2;
   kineticFriction = "10";

   // Spring that generates lateral tire forces
   lateralForce = 18000;
   lateralDamping = 6000;
   lateralRelaxation = 1;

   // Spring that generates longitudinal tire forces
   longitudinalForce = 14000;
   longitudinalDamping = 4000;
   longitudinalRelaxation = 1;
   radius = "0.609998";
};

datablock WheeledVehicleSpring(JeepCarSpring)
{
   // Wheel suspension properties
   length = 0.28;             // Suspension travel
   force = 2800;              // Spring force
   damping = 3600;             // Spring damping
   antiSwayForce = 5;         // Lateral anti-sway force
};

datablock WheeledVehicleData(JeepCar)
{
   category = "Vehicles";
   shapeFile = "art/shapes/Jeep/Body/jeep.DAE";
   emap = 1;

   mountPose[0] = sitting;
   numMountPoints = 2;

   useEyePoint = true;  // Use the vehicle's camera node rather than the player's

   maxSteeringAngle = 0.525;  // Maximum steering angle, should match animation

   // 3rd person camera settings
   cameraRoll = false;        // Roll the camera with the vehicle
   cameraMaxDist = 4;       // Far distance from vehicle
   cameraOffset = .8;        // Vertical offset from camera mount point
   cameraLag = "0.3";           // Velocity lag of camera
   cameraDecay = 1.25;        // Decay per sec. rate of velocity lag

   // Rigid Body
   mass = "400";
   massCenter = "0 0 0";    // Center of mass for rigid body
   massBox = "0 0 0";         // Size of box used for moment of inertia,
                              // if zero it defaults to object bounding box
   drag = 0.6;                // Drag coefficient
   bodyFriction = 0.6;
   bodyRestitution = 0.4;
   minImpactSpeed = 5;        // Impacts over this invoke the script callback
   softImpactSpeed = 5;       // Play SoftImpact Sound
   hardImpactSpeed = 15;      // Play HardImpact Sound
   integration = 8;           // Physics integration: TickSec/Rate
   collisionTol = "0.1";        // Collision distance tolerance
   contactTol = "0.4";          // Contact velocity tolerance

   // Engine
   engineTorque = 5600;       // Engine power
   engineBrake = "500";         // Braking when throttle is 0
   brakeTorque = "4500";        // When brakes are applied
   maxWheelSpeed = 50;        // Engine scale by current speed / max speed

   // Energy
   maxEnergy = 100;
   jetForce = 3000;
   minJetEnergy = 30;
   jetEnergyDrain = 2;

   // Sounds
   engineSound = JeepEngineSound;
   squealSound = JeepSqueal;
   softImpactSound = softImpactSound;
   hardImpactSound = hardImpactSound;

   // Dynamic fields accessed via script
   nameTag = 'Jeep';
   maxDismountSpeed = 10;
   maxMountSpeed = 5;
   mountPose0 = "sitting";
   tireEmitter = "DefaultTireEmitter";
   //dustEmitter = "CheetahTireEmitter";
   //dustHeight = ".5";

   // Mount slots
   turretSlot = 1;
   rightBrakeSlot = 2;
   leftBrakeSlot = 3;
   rightHeadSlot = 4;
   leftHeadSlot = 5;
   bulbHeadSlot = 6;
   bulb2HeadSlot = 7;
   
   // damage from collisions
collDamageMultiplier = 0.01;
collDamageThresholdVel = 10;
numDamageLevels = 2;

// damage levels
damageLevelTolerance[0] = 0.5;
damageEmitter[0] = GraySmokeEmitter;     // emitter used when damage is >= 50%
//damageEmitter[0] = DefaultTireEmitter;     // emitter used when damage is >= 50%
damageLevelTolerance[1] = 0.85;
damageEmitter[1] = BlackSmokeEmitter;    // emitter used when damage is >= 85%
damageEmitter[2] = DefaultVehicleBubbleEmitter;  // emitter used instead of damageEmitter[0:1]
                                                 // when offset point is underwater

// emit offsets (used for all active damage level emitters)
damageEmitterOffset[0] = "0 1.75 1";
//damageEmitterOffset[1] = ".75 -1.75 1";
numDmgEmitterAreas = 1;
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

