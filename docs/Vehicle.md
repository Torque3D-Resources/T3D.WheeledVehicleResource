<p>The VehicleData class extends the basic energy/damage functionality provided
   by ShapeBaseData to include damage from collisions, as well as particle
   emitters activated automatically when damage levels reach user specified
   thresholds.</p>

   <div>The example below shows how to setup a Vehicle to:</div>"
   <ul>
     <li>take damage when colliding with another object</li>
     <li>emit gray smoke particles from two locations on the Vehicle when damaged above 50%</li>
     <li>emit black smoke particles from two locations on the Vehicle when damaged above 85%</li>
     <li>emit bubbles when any active damage emitter point is underwater</li>
   </ul>

   <pre>
   // damage from collisions
   collDamageMultiplier = 0.05;
   collDamageThresholdVel = 15;

   // damage levels
   damageLevelTolerance[0] = 0.5;
   damageEmitter[0] = GraySmokeEmitter;     // emitter used when damage is >= 50%
   damageLevelTolerance[1] = 0.85;
   damageEmitter[1] = BlackSmokeEmitter;    // emitter used when damage is >= 85%
   damageEmitter[2] = DamageBubbleEmitter;  // emitter used instead of damageEmitter[0:1]
                                            // when offset point is underwater

   // emit offsets (used for all active damage level emitters)
   damageEmitterOffset[0] = \"0.5 3 1\";
   damageEmitterOffset[1] = \"-0.5 3 1\";
   numDmgEmitterAreas = 2;
   </pre>

<h3>VehicleData console fields</h3>

<table>
<tr>
	<td>jetForce</td>
	<td>F32</td>
	<td>Additional force applied to the vehicle when it is jetting.<br>
      For WheeledVehicles, the force is applied in the forward direction. For
      FlyingVehicles, the force is applied in the thrust direction.</td>
</tr>
<tr>
	<td>jetEnergyDrain</td>
	<td>F32</td>
	<td>Energy amount to drain for each tick the vehicle is jetting.<br>
      Once the vehicle's energy level reaches 0, it will no longer be able to jet.</td>
</tr>
<tr>
	<td>minJetEnergy</td>
	<td>F32</td>
	<td>Minimum vehicle energy level to begin jetting.</td>
</tr>
<tr>
	<td>steeringReturn</td>
	<td>F32</td>
	<td>Rate at which the vehicle's steering returns to forwards when it is moving.</td>
</tr>
<tr>
	<td>steeringReturnSpeedScale</td>
	<td>F32</td>
	<td>Amount of effect the vehicle's speed has on its rate of steering return.</td>
</tr>
<tr>
	<td>powerSteering</td>
	<td>bool</td>
	<td>If true, steering does not auto-centre while the vehicle is being steered by its driver.</td>
</tr>
<tr>
	<td>massCenter</td>
	<td>Point3F</td>
	<td>Defines the vehicle's center of mass (offset from the origin of the model).</td>
</tr>
<tr>
	<td>massBox</td>
	<td>Point3F</td>
	<td>Define the box used to estimate the vehicle's moment of inertia.<br>
      Currently only used by WheeledVehicle; other vehicle types use a unit sphere to compute inertia.
	</td>
</tr>
<tr>
	<td>massCenter</td>
	<td>Point3F</td>
	<td>Defines the vehicle's center of mass (offset from the origin of the model).</td>
</tr>
  <tr>
    <td>bodyRestitution</td>
    <td>F32</td>
    <td>Collision 'bounciness'.<br>Normally in the range 0 (not bouncy at all) to 1 (100% bounciness).</td>
  </tr>
  <tr>
    <td>bodyFriction</td>
    <td>F32</td>
    <td>Collision friction coefficient.<br>How well this object will slide against objects it collides with.</td>
  </tr>
  <tr>
    <td>softImpactSound</td>
    <td>SFXProfile</td>
    <td>Sound to play on a 'soft' impact.<br>
      This sound is played if the impact speed is < hardImpactSpeed and >= softImpactSpeed.
    </td>
  </tr>
  <tr>
    <td>hardImpactSound</td>
    <td>SFXProfile</td>
    <td>Sound to play on a 'hard' impact.<br>
      This sound is played if the impact speed is >= hardImpactSpeed.
    </td>
  </tr>
  <tr>
    <td>minImpactSpeed</td>
    <td>F32</td>
    <td>Minimum collision speed for the onImpact callback to be invoked.</td>
  </tr>
  <tr>
    <td>minImpactSpeed</td>
    <td>F32</td>
    <td>Minimum collision speed for the onImpact callback to be invoked.</td>
  </tr>
  <tr>
    <td>softImpactSpeed</td>
    <td>F32</td>
    <td>Minimum collision speed for the softImpactSound to be played.</td>
  </tr>
  <tr>
    <td>hardImpactSpeed</td>
    <td>F32</td>
    <td>Minimum collision speed for the hardImpactSound to be played.</td>
  </tr>
  <tr>
    <td>maxSteeringAngle</td>
    <td>F32</td>
    <td>Maximum yaw (horizontal) and pitch (vertical) steering angle in radians.</td>
  </tr>
  <tr>
    <td>maxDrag</td>
    <td>F32</td>
    <td>Maximum drag coefficient.<br>Currently unused.</td>
  </tr>
  <tr>
    <td>minDrag</td>
    <td>F32</td>
    <td>Minimum drag coefficient.<br>Currently only used by FlyingVehicle.</td>
  </tr>
  <tr>
    <td>integration</td>
    <td>S32</td>
    <td>Number of integration steps per tick.<br>
	Increase this to improve simulationstability (also increases simulation processing time)
    </td>
  </tr>
  <tr>
    <td>collisionTol</td>
    <td>F32</td>
    <td>Minimum distance between objects for them to be considered as colliding.</td>
  </tr>
  <tr>
    <td>contactTol</td>
    <td>F32</td>
    <td>Maximum relative velocity between objects for collisions to be resolved as contacts.<br>
	Velocities greater than this are handled as collisions.
    </td>
  </tr>
  <tr>
    <td>collDamageThresholdVel</td>
    <td>F32</td>
    <td>Minimum collision velocity to cause damage to this vehicle.<br>Currently unused.</td>
  </tr>
  <tr>
    <td>CollDamageMultiplier</td>
    <td>F32</td>
    <td>Damage to this vehicle after a collision (multiplied by collision velocity).<br>Currently unused.</td>
  </tr>


  <tr>
    <td>cameraRoll</td>
    <td>bool</td>
    <td>If true, the camera will roll with the vehicle. If false, the camera will always have the positive Z axis as up.</td>
  </tr>
  <tr>
    <td>cameraLag</td>
    <td>F32</td>
    <td>How much the camera lags behind the vehicle depending on vehicle speed.<br><br>
      Increasing this value will make the camera fall further behind the vehicle as it accelerates away.</td>
  </tr>
  <tr>
    <td>cameraDecay</td>
    <td>F32</td>
    <td>How quickly the camera moves back towards the vehicle when stopped.</td>
  </tr>
  <tr>
    <td>cameraOffset</td>
    <td>F32</td>
    <td>Vertical (Z axis) height of the camera above the vehicle.</td>
  </tr>
  <tr>
    <td>damageEmitter[]</td>
    <td>ParticleEmitterData</td>
    <td>Array of particle emitters used to generate damage (dust, smoke etc) effects.<br><br>
      Currently, the first two emitters (indices 0 and 1) are used when the damage
      level exceeds the associated damageLevelTolerance. The 3rd emitter is used
      when the emitter point is underwater.</td>
  </tr>
  <tr>
    <td>damageLevelTolerance</td>
    <td>F32</td>
    <td>Damage levels (as a percentage of maxDamage) above which to begin emitting particles from the associated damageEmitter.<br><br>
      Levels should be in order of increasing damage.</td>
  </tr>
  <tr>
    <td>numDmgEmitterAreas</td>
    <td>S32</td>
    <td>Number of damageEmitterOffset values to use for each damageEmitter.</td>
  </tr>
  <tr>
    <td>damageEmitterOffset[]</td>
    <td>Point3F</td>
    <td>Object space \"x y z\" offsets used to emit particles for the active damageEmitter.<br><br>
	Example:
	<pre>
      // damage levels
      damageLevelTolerance[0] = 0.5;
      damageEmitter[0] = SmokeEmitter;
      // emit offsets (used for all active damage level emitters)
      damageEmitterOffset[0] = \"0.5 3 1\";
      damageEmitterOffset[1] = \"-0.5 3 1\";
      numDmgEmitterAreas = 2;
	</pre>
</td>
  </tr>
</table>

<pre>
   addField( "minRollSpeed", TypeF32, Offset(minRollSpeed, VehicleData),
      "Unused" );


   addField( "dustEmitter", TYPEID< ParticleEmitterData >(), Offset(dustEmitter, VehicleData),
      "Dust particle emitter.\n\n@see triggerDustHeight\n\n@see dustHeight");
   addField( "triggerDustHeight", TypeF32, Offset(triggerDustHeight, VehicleData),
      "@brief Maximum height above surface to emit dust particles.\n\n"
      "If the vehicle is less than triggerDustHeight above a static surface "
      "with a material that has 'showDust' set to true, the vehicle will emit "
      "particles from the dustEmitter." );
   addField( "dustHeight", TypeF32, Offset(dustHeight, VehicleData),
      "Height above ground at which to emit particles from the dustEmitter." );

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

</pre>

<h3>VehicleDatablock Callbacks:</h3>
<div>onEnterLiquid()</div>
<div>onLeaveLiquid()</div>


<h3>Vehicle Console Variables:</h3>
<table>
<tr>
	<td>$vehicle::workingQueryBoxStaleThreshold</td>
	<td>S32</td>
	<td>The maximum number of ticks that go by before the mWorkingQueryBox is considered stale and needs updating.<br><br>"
      Other factors can cause the collision working query box to become invalidated, such as the vehicle moving far
      enough outside of this cached box.  The smaller this number, the more times the working list of triangles that are
      considered for collision is refreshed.  This has the greatest impact with colliding with high triangle count meshes.<br>
      @note Set to -1 to disable any time-based forced check.
	</td>
</tr>
<tr>
	<td>$vehicle::workingQueryBoxSizeMultiplier</td>
	<td>F32</td>
	<td>How much larger the mWorkingQueryBox should be made when updating the working collision list.<br>
      The larger this number the less often the working list will be updated due to motion, but any non-static shape that
      moves into the query box will not be noticed.
	</td>
</tr>
</table>

<h3>Vehicle Persistant Fields:</h3>
disableMove bool When this flag is set, the vehicle will ignore throttle changes.

<h3>Vehicle Callbacks:</h3>
onCollision(%this, %object, %vector, %magnitude);