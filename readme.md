<h1>Wheeled Vehicle Resource</h1>
<p>A collection of upgrades for Wheeled Vehicle behavior ported from TGE to Torque 3D.</p>
<p>This combines and extends several classic engine resources to provide several core improvements. A simulated drive train allows vehicles to be created with a wide range of performence characteristics. A number of GUI elements have been added or improved and script handling for multiple mounts is more fleshed out.</p>

<img src="./media/screenshot_001.png">

<ul>
<li>RPM-based engine simulation</li>
<li>Definable transmission (up to 7 gears)</li>
<li>Automatic and Manual transmission handling.</li>
<li>Programmable power output.</li>
<li>Tachometer gui</li>
<li>Scripted gear indicator gui</li>
<li>In-game vehicle datablock/settings editor gui</li>
</ul>
<hr width=75%>
<h2>WheeledVehicleEnging</h2>
<p>The WheeledVehicleEngine is a datablock subclass with fields to define the parameters of a vehicle's drive train.</p>
<table>
  <tr>
  <thead colspan=2 style="text-align:center;">WheeledVehicleEngine Console Fields</thead>
  </tr>
  <tr>
    <td>minRPM</td>
    <td>Lower floor to clamp RPM value.</td>
  </tr>  
  <tr>
    <td>maxRPM</td>
    <td>Highest RPM in normal operating range.</td>
  </tr>
  <tr>
    <td>ildeRPM</td>
    <td>RPM value at resting engine idle.</td>
  </tr>
  
  <tr>
    <td>numTorqueLevels</td>
    <td></td>
  </tr>
  <tr>
    <td>rmpValues</td>
    <td></td>
  </tr>  
  <tr>
    <td>torqueLevels</td>
    <td></td>
  </tr>  
  
  <tr>
    <td>numGears</td>
    <td></td>
  </tr>     
  <tr>
    <td>gearRatios[]</td>
    <td></td>
  </tr>
  <tr>
    <td>reverseRatio</td>
    <td></td>
  </tr>  
  <tr>
    <td>diffRatio</td>
    <td></td>
  </tr>     
  <tr>
    <td>shiftUpRPM</td>
    <td></td>
  </tr>
  <tr>
    <td>shiftDownRPM</td>
    <td></td>
  </tr>
  <tr>
    <td>transmissionSlip</td>
    <td></td>
  </tr>  
  <tr>
    <td>engineDrag</td>
    <td></td>
  </tr>
  <tr>
    <td>neutralBoost</td>
    <td></td>
  </tr>
  <tr>
    <td>slowDownRate</td>
    <td></td>
  </tr>
  <tr>
    <td>inertiaFactor</td>
    <td></td>
  </tr>
  <tr>
    <td>overRevSlowdown</td>
    <td></td>
  </tr>
  
  <tr>
    <td>minPitch</td>
    <td></td>
  </tr>
  <tr>
    <td>maxPitch</td>
    <td></td>
  </tr> 
  <tr>
    <td>minVolume</td>
    <td></td>
  </tr>
  <tr>
    <td>maxVolume</td>
    <td></td>
  </tr>

  
  <tr>
    <td>fuelFlow</td>
    <td></td>
  </tr>
  <tr>
    <td>useAutomatic</td>
    <td></td>
  </tr>  

</table>
