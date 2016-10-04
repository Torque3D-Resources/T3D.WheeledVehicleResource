<h1>EngineEditGui</h1>
<p>A basic GUI that allows real-time access to vehicle-related  datablock parameters.</p>

<h3>Drivetrain Fields</h3>
<img src=./media/vedit_engine.png>

The description for drivetrain fields can be found in the <a href=./WheeledVehicleEngine.md>WheeledVehicleEngine documentation</a>.

<h3>Steering/Suspension Fields</h3>
<img src=./media/vedit_vehicle.png>

The description for general Vehicle fields can be found in the <a href=./WheeledVehicle_sim.md>WheeledVehicle vehicle documentation</a>.

<h3>General Usage</h3>
<p>The vehicle edit GUI can be invoked in-game with the Ctrl-V keyboard command. The screen will load with data pulled from the currently mounted vehicle's datablocks.</p>

<p>Changes can be applied in real-time to the mounted vehicle by pressing the "Apply" button. </p>

<p>The "cancel" button will exit the screen without applying any changes.</p>

<p>The vehicle settings can be dumped to a file for later editing. A master datablock name should be provided in the "Datablock Name" field. Clicking on the "Save to File" button will write datablocks a .cs file with the datablock objects named to match the master (i.e. DatablockCarTire, etc.).</p> 

<p>Note: saving datablocks to file does not apply them to the in-game vehicle.</p>