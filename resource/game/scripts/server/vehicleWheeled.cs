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

// This file contains script methods unique to the WheeledVehicle class.  All
// other necessary methods are contained in "../server/scripts/vehicle.cs" in
// which the "generic" Vehicle class methods that are shared by all vehicles,
// (flying, hover, and wheeled) can be found.

// Parenting is in place for WheeledVehicleData to VehicleData.  This should
// make it easier for people to simply drop in new (generic) vehicles.  All that
// the user needs to create is a set of datablocks for the new wheeled vehicle
// to use.  This means that no (or little) scripting should be necessary.

function WheeledVehicleData::onAdd(%this, %obj)
{
   Parent::onAdd(%this, %obj);
/*
   // Setup the car with some tires & springs
   for (%i = %obj.getWheelCount() - 1; %i >= 0; %i--)
   {
      %obj.setWheelTire(%i, DefaultCarTire);
      %obj.setWheelSpring(%i, DefaultCarSpring);
      %obj.setWheelPowered(%i, false);
   }

   // Steer with the front tires
   %obj.setWheelSteering(0, 1);
   %obj.setWheelSteering(1, 1);

   // Only power the two rear wheels... assuming there are only 4 wheels.
   %obj.setWheelPowered(2, true);
   %obj.setWheelPowered(3, true);
*/
	%obj.moveMaps[0] = "vehicleDriverMap";
	%obj.moveMaps[1] = "vehiclePassengerMap";
	%obj.moveMaps[2] = "vehiclePassengerMap";
	%obj.moveMaps[3] = "vehiclePassengerMap";
		
}

function WheeledVehicleData::onBrake(%this, %obj, %type){
   echo("Brakes Pressed("@%obj SPC %obj.name@"):"@ %type);
   %obj.onBrake(%type);    // Just pass it through to the vehicle for now.
}

function WheeledVehicle::onBrake(%this, %type){
   // Override in subclasses to handle shape-specific animations/lighting, etc.   
}


// Something has hit us
function WheeledVehicleData::onCollision(%this, %obj, %col, %vec, %speed)
{
   // Collision with other objects, including items
   warn("Collision:"@%obj@" was hit by "@%col@" @ "@%speed@" from "@%vec);
}

// We have hit something
function WheeledVehicleData::onImpact(%this, %obj, %col, %vec, %speed)
{
   // Collision with other objects, including items
   warn("Impact:"@%obj@" hit something going "@ %speed);
}



function WheeledVehicleEngine::onShift(%this, %obj, %gear, %rpm){
	echo("(engine)Shifted to:" @ %gear SPC "RPM:" @ %rpm);
	echo("%obj:" SPC %obj SPC %obj.cli);
	commandToClient(%obj.getControllingClient(), 'updateGearDisplay', %gear);	// Tell the client to update their screen score.
}

function upshiftVehicle(%player)
{
   %co = %player.getControlObject();
   if (%co) {
      // There is a valid control object
      %co.upShift();
   }
}
function downshiftVehicle(%player)
{
   %co = %player.getControlObject();
   if (%co) {
      // There is a valid control object
      %co.downShift();
   }
}





// Used to kick the players out of the car that your crosshair is over
function serverCmdcarUnmountObj(%client, %obj)
{
   %obj.unmount();
   %obj.setControlObject(%obj);

   %ejectpos = %obj.getPosition();
   %ejectpos = VectorAdd(%ejectpos, "0 0 5");
   %obj.setTransform(%ejectpos);

   %ejectvel = %obj.mVehicle.getVelocity();
   %ejectvel = VectorAdd(%ejectvel, "0 0 10");
   %ejectvel = VectorScale(%ejectvel, %obj.getDataBlock().mass);
   %obj.applyImpulse(%ejectpos, %ejectvel);
}

// Used to flip the car over if it manages to get stuck upside down
function serverCmdflipCar(%client)
{
   %car = %client.player.getControlObject();

   if (%car.getClassName() $= "WheeledVehicle")
   {
      %carPos = %car.getPosition();
      %carPos = VectorAdd(%carPos, "0 0 3");

      %car.setTransform(%carPos SPC "0 0 1 0");
   }
}

function serverCmdsetPlayerControl(%client)
{
     %client.setControlObject(%client.player);
}

function serverCmddismountVehicle(%client)
{
   %car = %client.player.getControlObject();
   
   %passenger = %car.getMountNodeObject(0);
   %passenger.getDataBlock().doDismount(%passenger, true);
   
   %client.setControlObject(%client.player);
}





function serverCmdDispEngineGui(%client){
   // This is so we can check to see if the player is in a car and if so
   // to set the gear to 0 and parking brake to true.
   %p = %client.player;
   echo(">"@%p SPC %p.name SPC %p.mVehicle);
   if (%p.mVehicle){
      %o = %p.mVehicle;      
      echo("o:" @ %o SPC "Gear:" @ %o.getGear() SPC "RPM:" @ %o.rpm SPC "Speed:" @ %o.getSpeed());
      %o.setgear(0);
      %o.parkingBrake=true;
      %d = %o.getDatablock();
      %e = %o.getEngine();
      %t = %o.getTire(0);
      %s = %o.getSpring(0);
      
      loadEngineGui(%o.getDatablock(),%o.getEngine(),%o.getTire(0),%o.getSpring(0));
   } else {
      loadEngineGui(JeepCar,DefaultEngine,JeepCarTire,JeepCarSpring);   
   }
   
   

}

function serverCmdUpShift(%client)
{
   %p = %client.player;
   echo(">"@%p);
   if (%p.isMounted == true){
      %o = %p.mVehicle;      
      echo("o:" @ %o SPC "Gear:" @ %o.getGear() SPC "RPM:" @ %o.rpm SPC "Speed:" @ %o.getSpeed());      
   }
   error("UPSHIFT Command");
   upshiftVehicle(%client.player);   
   
}
function serverCmdDnShift(%client)
{
   error("Downshift Command");
   downshiftVehicle(%client.player);
}

function serverCmdCarHop(%client)
{
   error("Hop");
   %v = %client.player.mVehicle;
   echo("Player:"@%client.player SPC "/ Vehicle:" @%v);
   //if(isObject(%v)){
      %v.applyImpulse(%v.position,"0 0" SPC %v.hop);
   //}
}


function serverCmdtoggleHeadLights(%client)
{
   %car = %client.player.getControlObject();
   if (%car.getClassName() $= "WheeledVehicle")
   {   
      %datablosk = %car.getDatablock();
      %datablosk.toggleHeadlights(%car);
   }
}