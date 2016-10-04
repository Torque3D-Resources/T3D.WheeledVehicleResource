"The VehicleData class extends the basic energy/damage functionality provided "
   "by ShapeBaseData to include damage from collisions, as well as particle "
   "emitters activated automatically when damage levels reach user specified "
   "thresholds.\n\n"

   "The example below shows how to setup a Vehicle to:\n"
   "<ul>\n"
   "  <li>take damage when colliding with another object\n"
   "  <li>emit gray smoke particles from two locations on the Vehicle when damaged above 50%</li>\n"
   "  <li>emit black smoke particles from two locations on the Vehicle when damaged above 85%</li>\n"
   "  <li>emit bubbles when any active damage emitter point is underwater</li>\n"
   "</ul>\n\n"

   "@tsexample\n"
   "// damage from collisions\n"
   "collDamageMultiplier = 0.05;\n"
   "collDamageThresholdVel = 15;\n\n"

   "// damage levels\n"
   "damageLevelTolerance[0] = 0.5;\n"
   "damageEmitter[0] = GraySmokeEmitter;     // emitter used when damage is >= 50%\n"
   "damageLevelTolerance[1] = 0.85;\n"
   "damageEmitter[1] = BlackSmokeEmitter;    // emitter used when damage is >= 85%\n"
   "damageEmitter[2] = DamageBubbleEmitter;  // emitter used instead of damageEmitter[0:1]\n"
   "                                         // when offset point is underwater\n"
   "// emit offsets (used for all active damage level emitters)\n"
   "damageEmitterOffset[0] = \"0.5 3 1\";\n"
   "damageEmitterOffset[1] = \"-0.5 3 1\";\n"
   "numDmgEmitterAreas = 2;\n"
   "@endtsexample\n"



   addField( "jetForce", TypeF32, Offset(jetForce, VehicleData),
      "@brief Additional force applied to the vehicle when it is jetting.\n\n"
      "For WheeledVehicles, the force is applied in the forward direction. For "
      "FlyingVehicles, the force is applied in the thrust direction." );
   addField( "jetEnergyDrain", TypeF32, Offset(jetEnergyDrain, VehicleData),
      "@brief Energy amount to drain for each tick the vehicle is jetting.\n\n"
      "Once the vehicle's energy level reaches 0, it will no longer be able to jet." );
   addField( "minJetEnergy", TypeF32, Offset(minJetEnergy, VehicleData),
      "Minimum vehicle energy level to begin jetting." );

   addField( "steeringReturn", TypeF32, Offset(steeringReturn, VehicleData),
      "Rate at which the vehicle's steering returns to forwards when it is moving." );
   addField( "steeringReturnSpeedScale", TypeF32, Offset(steeringReturnSpeedScale, VehicleData),
      "Amount of effect the vehicle's speed has on its rate of steering return." );
   addField( "powerSteering", TypeBool, Offset(powerSteering, VehicleData),
      "If true, steering does not auto-centre while the vehicle is being steered by its driver." );

   addField( "massCenter", TypePoint3F, Offset(massCenter, VehicleData),
      "Defines the vehicle's center of mass (offset from the origin of the model)." );
   addField( "massBox", TypePoint3F, Offset(massBox, VehicleData),
      "@brief Define the box used to estimate the vehicle's moment of inertia.\n\n"
      "Currently only used by WheeledVehicle; other vehicle types use a "
      "unit sphere to compute inertia." );
   addField( "bodyRestitution", TypeF32, Offset(body.restitution, VehicleData),
      "Collision 'bounciness'.\nNormally in the range 0 (not bouncy at all) to "
      "1 (100% bounciness)." );
   addField( "bodyFriction", TypeF32, Offset(body.friction, VehicleData),
      "Collision friction coefficient.\nHow well this object will slide against "
      "objects it collides with." );
   addField( "softImpactSound", TYPEID< SFXProfile >(), Offset(body.sound[Body::SoftImpactSound], VehicleData),
      "@brief Sound to play on a 'soft' impact.\n\n"
      "This sound is played if the impact speed is < hardImpactSpeed and >= "
      "softImpactSpeed.\n\n"
      "@see softImpactSpeed" );
   addField( "hardImpactSound", TYPEID< SFXProfile >(), Offset(body.sound[Body::HardImpactSound], VehicleData),
      "@brief Sound to play on a 'hard' impact.\n\n"
      "This sound is played if the impact speed >= hardImpactSpeed.\n\n"
      "@see hardImpactSpeed" );

   addField( "minImpactSpeed", TypeF32, Offset(minImpactSpeed, VehicleData),
      "Minimum collision speed for the onImpact callback to be invoked." );
   addField( "softImpactSpeed", TypeF32, Offset(softImpactSpeed, VehicleData),
      "Minimum collision speed for the softImpactSound to be played." );
   addField( "hardImpactSpeed", TypeF32, Offset(hardImpactSpeed, VehicleData),
      "Minimum collision speed for the hardImpactSound to be played." );
   addField( "minRollSpeed", TypeF32, Offset(minRollSpeed, VehicleData),
      "Unused" );
   addField( "maxSteeringAngle", TypeF32, Offset(maxSteeringAngle, VehicleData),
      "Maximum yaw (horizontal) and pitch (vertical) steering angle in radians." );

   addField( "maxDrag", TypeF32, Offset(maxDrag, VehicleData),
      "Maximum drag coefficient.\nCurrently unused." );
   addField( "minDrag", TypeF32, Offset(minDrag, VehicleData),
      "Minimum drag coefficient.\nCurrently only used by FlyingVehicle." );
   addField( "integration", TypeS32, Offset(integration, VehicleData),
      "Number of integration steps per tick.\nIncrease this to improve simulation "
      "stability (also increases simulation processing time)." );
   addField( "collisionTol", TypeF32, Offset(collisionTol, VehicleData),
      "Minimum distance between objects for them to be considered as colliding." );
   addField( "contactTol", TypeF32, Offset(contactTol, VehicleData),
      "Maximum relative velocity between objects for collisions to be resolved "
      "as contacts.\nVelocities greater than this are handled as collisions." );

   addField( "cameraRoll", TypeBool, Offset(cameraRoll, VehicleData),
      "If true, the camera will roll with the vehicle. If false, the camera will "
      "always have the positive Z axis as up." );
   addField( "cameraLag", TypeF32, Offset(cameraLag, VehicleData),
      "@brief How much the camera lags behind the vehicle depending on vehicle speed.\n\n"
      "Increasing this value will make the camera fall further behind the vehicle "
      "as it accelerates away.\n\n@see cameraDecay." );
   addField("cameraDecay",  TypeF32, Offset(cameraDecay, VehicleData),
      "How quickly the camera moves back towards the vehicle when stopped.\n\n"
      "@see cameraLag." );
   addField("cameraOffset", TypeF32, Offset(cameraOffset, VehicleData),
      "Vertical (Z axis) height of the camera above the vehicle." );

   addField( "dustEmitter", TYPEID< ParticleEmitterData >(), Offset(dustEmitter, VehicleData),
      "Dust particle emitter.\n\n@see triggerDustHeight\n\n@see dustHeight");
   addField( "triggerDustHeight", TypeF32, Offset(triggerDustHeight, VehicleData),
      "@brief Maximum height above surface to emit dust particles.\n\n"
      "If the vehicle is less than triggerDustHeight above a static surface "
      "with a material that has 'showDust' set to true, the vehicle will emit "
      "particles from the dustEmitter." );
   addField( "dustHeight", TypeF32, Offset(dustHeight, VehicleData),
      "Height above ground at which to emit particles from the dustEmitter." );

   addField( "damageEmitter", TYPEID< ParticleEmitterData >(), Offset(damageEmitterList, VehicleData), VC_NUM_DAMAGE_EMITTERS,
      "@brief Array of particle emitters used to generate damage (dust, smoke etc) "
      "effects.\n\n"
      "Currently, the first two emitters (indices 0 and 1) are used when the damage "
      "level exceeds the associated damageLevelTolerance. The 3rd emitter is used "
      "when the emitter point is underwater.\n\n"
      "@see damageEmitterOffset" );
   addField( "damageEmitterOffset", TypePoint3F, Offset(damageEmitterOffset, VehicleData), VC_NUM_DAMAGE_EMITTER_AREAS,
      "@brief Object space \"x y z\" offsets used to emit particles for the "
      "active damageEmitter.\n\n"
      "@tsexample\n"
      "// damage levels\n"
      "damageLevelTolerance[0] = 0.5;\n"
      "damageEmitter[0] = SmokeEmitter;\n"
      "// emit offsets (used for all active damage level emitters)\n"
      "damageEmitterOffset[0] = \"0.5 3 1\";\n"
      "damageEmitterOffset[1] = \"-0.5 3 1\";\n"
      "numDmgEmitterAreas = 2;\n"
      "@endtsexample\n" );
   addField( "damageLevelTolerance", TypeF32, Offset(damageLevelTolerance, VehicleData), VC_NUM_DAMAGE_LEVELS,
      "@brief Damage levels (as a percentage of maxDamage) above which to begin "
      "emitting particles from the associated damageEmitter.\n\n"
      "Levels should be in order of increasing damage.\n\n"
      "@see damageEmitterOffset" );
   addField( "numDmgEmitterAreas", TypeF32, Offset(numDmgEmitterAreas, VehicleData),
      "Number of damageEmitterOffset values to use for each damageEmitter.\n\n"
      "@see damageEmitterOffset" );

   addField( "splashEmitter", TYPEID< ParticleEmitterData >(), Offset(splashEmitterList, VehicleData), VC_NUM_SPLASH_EMITTERS,
      "Array of particle emitters used to generate splash effects." );
   addField( "splashFreqMod",  TypeF32, Offset(splashFreqMod, VehicleData),
      "@brief Number of splash particles to generate based on vehicle speed.\n\n"
      "This value is multiplied by the current speed to determine how many "
      "particles to generate each frame." );
   addField( "splashVelEpsilon", TypeF32, Offset(splashVelEpsilon, VehicleData),
      "Minimum speed when moving through water to generate splash particles." );

   addField( "exitSplashSoundVelocity", TypeF32, Offset(exitSplashSoundVel, VehicleData),
      "Minimum velocity when leaving the water for the exitingWater sound to play." );
   addField( "softSplashSoundVelocity", TypeF32, Offset(softSplashSoundVel, VehicleData),
      "Minimum velocity when entering the water for the imapactWaterEasy sound "
      "to play.\n\n@see impactWaterEasy" );
   addField( "mediumSplashSoundVelocity", TypeF32, Offset(medSplashSoundVel, VehicleData),
      "Minimum velocity when entering the water for the imapactWaterMedium sound "
      "to play.\n\n@see impactWaterMedium" );
   addField( "hardSplashSoundVelocity", TypeF32, Offset(hardSplashSoundVel, VehicleData),
      "Minimum velocity when entering the water for the imapactWaterHard sound "
      "to play.\n\n@see impactWaterHard" );
   addField( "exitingWater", TYPEID< SFXProfile >(), Offset(waterSound[ExitWater], VehicleData),
      "Sound to play when exiting the water." );
   addField( "impactWaterEasy", TYPEID< SFXProfile >(), Offset(waterSound[ImpactSoft], VehicleData),
      "Sound to play when entering the water with speed >= softSplashSoundVelocity "
      "and < mediumSplashSoundVelocity." );
   addField( "impactWaterMedium", TYPEID< SFXProfile >(), Offset(waterSound[ImpactMedium], VehicleData),
      "Sound to play when entering the water with speed >= mediumSplashSoundVelocity "
      "and < hardSplashSoundVelocity." );
   addField( "impactWaterHard", TYPEID< SFXProfile >(), Offset(waterSound[ImpactHard], VehicleData),
      "Sound to play when entering the water with speed >= hardSplashSoundVelocity." );
   addField( "waterWakeSound", TYPEID< SFXProfile >(), Offset(waterSound[Wake], VehicleData),
      "Looping sound to play while moving through the water." );

   addField( "collDamageThresholdVel", TypeF32, Offset(collDamageThresholdVel, VehicleData),
      "Minimum collision velocity to cause damage to this vehicle.\nCurrently unused." );
   addField( "collDamageMultiplier", TypeF32, Offset(collDamageMultiplier, VehicleData),
      "@brief Damage to this vehicle after a collision (multiplied by collision "
      "velocity).\n\nCurrently unused." );




<h3>VehicleDatablock Callbacks:</h3>
onEnterLiquid() - Vehicle
onLeaveLiquid() - Vehicle


Vehicle Console Variables:
$vehicle::workingQueryBoxStaleThreshold, S32, The maximum number of ticks that go by before the mWorkingQueryBox is considered stale and needs updating.\n\n"
      "Other factors can cause the collision working query box to become invalidated, such as the vehicle moving far "
      "enough outside of this cached box.  The smaller this number, the more times the working list of triangles that are "
      "considered for collision is refreshed.  This has the greatest impact with colliding with high triangle count meshes.\n\n"
      "@note Set to -1 to disable any time-based forced check.

$vehicle::workingQueryBoxSizeMultiplier",TypeF32,How much larger the mWorkingQueryBox should be made when updating the working collision list.\n\n"
      "The larger this number the less often the working list will be updated due to motion, but any non-static shape that "
      "moves into the query box will not be noticed.\n\n"

Vehicle Persistant Fields:
disableMove bool When this flag is set, the vehicle will ignore throttle changes.
Vehicle Callbacks:
onCollision(%this, %object, %vector, %magnitude);