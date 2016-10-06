<h1>Tuning up a Wheeled Vehicle</h1>

The Wheeled vehicle is a complex object based on several modular datablocks which combine to form a simulated vehicle system. The core vehicle is defined with a WheeledVehicleData datablock which is derived from VehicleData, which itself is derived from ShapebaseData. 

<h2>WheeledVehicleData</h2>
<p>WheeledVehicleData defines the properties of the vehicle body along with audio and camera handling. It derives much capacity from the classes common to all vehicles: VehicleData/Vehicle. These classes are not instantiated directly and serve as parent to all mountable vehicles. The full list of console fields and commands associated with VehicleData can be found <a href=./Vehicle.md>here</a>.</p> We will focus on the subset of fields important to Torque script implementations of WheeledVehicleData that pertain to the physics/driving simulation.


<table valgin=top>
  <tr>
  <thead colspan=3 style="text-align:center;"><center><b>WheeledVehicleData Console Fields</b></center></thead>
  </tr>
  <tr>
    <td>mass</td>
    <td>F32</td>
    <td>Shape mass. Used in simulation of moving objects.</td>
  </tr>
  <tr>
    <td>drag</td>
    <td>F32</td>
    <td>Drag factor. Reduces velocity of moving objects.</td>
  </tr>
  <tr>
    <td>density</td>
    <td>F32</td>
    <td>Shape density.\nUsed when computing buoyancy when in water.</td>
  </tr>
  <tr>
    <td>massCenter</td>
    <td>Point3F</td>
    <td>Defines the vehicle's center of mass (offset from the origin of the model).</td>
  </tr>
  <tr>
    <td>massBox</td>
    <td>Point3F</td>
    <td> Define the box used to estimate the vehicle's moment of inertia. Defaults to object box.</td>
  </tr>
  <tr>
    <td>massBox</td>
    <td>Point3F</td>
    <td>Define the box used to estimate the vehicle's moment of inertia. Defaults to object box.</td>
  </tr>
  <tr>
    <td>bodyRestitution</td>
    <td>F32</td>
    <td>Collision 'bounciness'.<br>Normally in the range 0 (not bouncy at all) to 1 (100% bounciness).</td>
  </tr>
  <tr>
    <td>bodyRestitution</td>
    <td>F32</td>
    <td>Collision friction coefficient.<br>How well this object will slide against objects it collides with.</td>
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
    <td>Bool</td>
    <td>If true, steering does not auto-centre while the vehicle is being steered by its driver.</td>
  </tr>

  <tr>
    <td>jetForce</td>
    <td>F32</td>
    <td>Additional force applied to the vehicle when it is jetting. For WheeledVehicles, the force is applied in the forward direction..</td>
  </tr>
  <tr>
    <td>jetEnergyDrain</td>
    <td>F32</td>
    <td>Energy amount to drain for each tick the vehicle is jetting. Once the vehicle's energy level reaches 0, it will no longer be able to jet.</td>
  </tr>
  <tr>
    <td>brakeTorque</td>
    <td>F32</td>
    <td>Torque applied when braking.<br><br>
      This controls how fast the vehicle will stop when the brakes are applied.</td>
  </tr>
  <tr><td colspan=3 style="text-align:center;">TGE Engine Fields*</td></tr>
  <tr>
    <td>maxWheelSpeed</td>
    <td>F32</td>
    <td>Maximum linear velocity of each wheel.<br><Br>
      This caps the maximum speed of the vehicle.</td>
  </tr>
  <tr>
    <td>engineTorque</td>
    <td>F32</td>
    <td>Torque available from the engine at 100% throttle. This controls vehicle acceleration. ie. how fast it will reach maximum speed.</td>
  </tr>
  <tr>
    <td>engineBrake</td>
    <td>F32</td>
    <td>Braking torque applied by the engine when the throttle and brake are both 0. This controls how quickly the vehicle will coast to a stop.</td>
  </tr>

</table>

<h4>*Note: These fields control the function of the original T3D engine functions. In the event no WheeledVehicleEngine datablock has been assigned to a vehicle, the operation will fall back to utilizing these routines.</h4>


<h3>VehicleDatablock Callbacks:</h3>
onEnterLiquid() - Vehicle
onLeaveLiquid() - Vehicle

<h3>Wheeled Vehicle Engine Callbacks:</h3>
onShift()

<h3>Wheeled Vehicle Console Functions</h3>
<ul>
<li>setWheelSteering(S32 wheel, F32 steering)</li>
<li>setWheelPowered(S32 wheel,bool powered)</li>
<li>setWheelTire(S32 wheel, WheeledVehicleTire * tire)</li>
<li>getTire(S32 wheel)</li>
<li>setWheelSpring(S32 wheel, WheeledVehicleSpring * tire)</li>
<li>getSpring(S32 wheel)</li>
<li>getWheelCount()</li>
<li>setEngine(WheeledVehicleEngine * engine)</li>
<li>getEngine()</li>
<li>setGear()</li>
<li>upShift()</li>
<li>downShift()</li>
<li>getGear()</li>
<li>getSpeed()</li>
<li>getRPM()</li>
<li>setAuto(bool isAuto)</li>
<li>isDefaultAuto()</li>
<li>isAuto()</li>
</ul>