<h1>Wheeled Vehicle Resource Documentation</h1>
<p>The Wheeled vehicle implementation in Torque draws from both parameters and model geometry to produce the simulation. When properly balanced, the results can be compelling. This is somewhat famously demonstrated in the vehicles from Tribes 2 - which were implemented with this code.</p>

<p>Unfortunately, getting that balance is also reasonably complex in the fullest implementation. In addition to setting up particle effects, damage/destruction sequences, audio profiles, etc. the datablocks and fields for physics aspects such as tires and springs must be set up properly create a realistic driving experience when applied to a specific model. This resource adds to the complexity by introducing a simulated transmission and power band. This increases the realism of the acceleration behavior, but requires setting up gear ratios and an RPM/Power profile along with defining the basics of the transmission behavior.</p>

With all of this in mind, to properly tune a vehicle it is necessary to see the results of changes in a significant number of parameters. As such, central to the resource is a GUI that allows all major physics-related parameters to be adjusted and applied to a vehicle model in real time.

<p>This document is designed to focus primarily on fields and datablocks that control the physics simulation and their impact.</p>
<hr width=75%>
<h3><a href="./VehicleEditGui.md">VehicleEditGui</a></h3>
<h3><a href="./WheeledVehicle_sim.md">WheeledVehicle Simulation Fields</a></h3>
<h3><a href="./WheeledVehicleSuspension.md">WheeledVehicle Suspension Settings</a></h3>
<h3><a href="./WheeledVehicleEngine.md">WheeledVehicleEngine Settings</a></h3>
<h3><a href="./WheeledVehicle_art.md">Particles and Audio</a></h3>
