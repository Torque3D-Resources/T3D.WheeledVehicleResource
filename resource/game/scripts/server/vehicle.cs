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

// Parenting is in place for WheeledVehicleData to VehicleData.  This should
// make it easier for people to simply drop in new (generic) vehicles.  All that
// the user needs to create is a set of datablocks for the new wheeled vehicle
// to use.  This means that no (or little) scripting should be necessary.

// Special, or unique vehicles however will still require some scripting.  They
// may need to override the onAdd() function in order to mount weapons,
// differing tires/springs, etc., almost everything else is taken care of in the
// WheeledVehicleData and VehicleData methods.  This helps us by not having to
// duplicate the same code for every new vehicle.

// In theory this would work for HoverVehicles and FlyingVehicles also, but
// hasn't been tested or fully implemented for those classes -- yet.

function VehicleData::onAdd(%this, %obj)
{
   %obj.setRechargeRate(%this.rechargeRate);
   %obj.setEnergyLevel(%this.MaxEnergy);
   %obj.setRepairRate(0);

   if (%obj.mountable || %obj.mountable $= "")
      %this.isMountable(%obj, true);
   else
      %this.isMountable(%obj, false);

   if (%this.nameTag !$= "")
      %obj.setShapeName(%this.nameTag);
}

function VehicleData::onRemove(%this, %obj)
{
   //echo("\c4VehicleData::onRemove("@ %this.getName() @", "@ %obj.getClassName() @")");

   // if there are passengers/driver, kick them out
   for(%i = 0; %i < %obj.getDatablock().numMountPoints; %i++)
   {
      if (%obj.getMountNodeObject(%i))
      {
         %passenger = %obj.getMountNodeObject(%i);
         %passenger.getDataBlock().doDismount(%passenger, true);
      }
   }
}

// ----------------------------------------------------------------------------
// Vehicle player mounting and dismounting
// ----------------------------------------------------------------------------

function VehicleData::isMountable(%this, %obj, %val)
{
   %obj.mountable = %val;
}

function VehicleData::mountPlayer(%this, %vehicle, %player)
{
   //echo("\c4VehicleData::mountPlayer("@ %this.getName() @", "@ %vehicle @", "@ %player.client.nameBase @")");

   if (isObject(%vehicle) && %vehicle.getDamageState() !$= "Destroyed")
   {
      %player.startFade(1000, 0, true);
      %this.schedule(1000, "setMountVehicle", %vehicle, %player);
      %player.schedule(1500, "startFade", 1000, 0, false);
   }
}

function VehicleData::setMountVehicle(%this, %vehicle, %player)
{
   //echo("\c4VehicleData::setMountVehicle("@ %this.getName() @", "@ %vehicle @", "@ %player.client.nameBase @")");

   if (isObject(%vehicle) && %vehicle.getDamageState() !$= "Destroyed")
   {
      %node = %this.findEmptySeat(%vehicle, %player);
      if (%node >= 0)
      {
         //echo("\c4Mount Node: "@ %node);
         %vehicle.mountObject(%player, %node);
         //%player.playAudio(0, MountVehicleSound);
         %player.mVehicle = %vehicle;
      }
   }
}

function VehicleData::findEmptySeat(%this, %vehicle, %player)
{
   //echo("\c4This vehicle has "@ %this.numMountPoints @" mount points.");

   for (%i = 0; %i < %this.numMountPoints; %i++)
   {
      %node = %vehicle.getMountNodeObject(%i);
      if (%node == 0)
         return %i;
   }
   return -1;
}

function VehicleData::switchSeats(%this, %vehicle, %player)
{
   for (%i = 0; %i < %this.numMountPoints; %i++)
   {
      %node = %vehicle.getMountNodeObject(%i);
      if (%node == %player || %node > 0)
         continue;

      if (%node == 0)
         return %i;
   }
   return -1;
}

// ********************************************************** //

// *******************************************
// Function: findNextFreeSeat
//
// Inputs: Client connection object
//         Vehicle object
//         Vehicle datablock
//
// Outputs: FALSE if a seat is not found
//          The next free seat number
//
// This function searches the vehicle for a
// free seat.
// *******************************************
function findNextFreeSeat(%client, %vehicle, %vehicleblock)
{
   // Check the next seat
   %seat = %client.player.mSeat + 1;

   if (%seat == %vehicleblock.numMountPoints)
   {
      // Check from seat 0, we've run out of seats to check.
      %seat = 0;
   }

   // Reset the flag.
   %found = false;

   // Search through the seats.
   while ((%seat != %client.player.mSeat) && (%found == false))
   {
      if (%seat >= %vehicleblock.numMountPoints)
      {
         // Go back to the first (driver's) seat
         %seat = 0;
      }

      // Is this seat free?
      %node = %vehicle.getMountNodeObject(%seat);

      if (%node == 0)
      {
         // Yes it is free.
         %found = true;
      }
      if (%found == false)
      {
         // We couldn't find a free seat.
         %seat++;
      }
   }

   if (%found == false)
      return -1;
   else
      return %seat;
}

// *******************************************
// Function: findPreviousFreeSeat
//
// Inputs: Client connection object
//         Vehicle object
//         Vehicle datablock
//
// Outputs: FALSE if a seat is not found
//          The previous free seat number
//
// This function searches the vehicle for a
// free seat. It searches in the opposite
// direction to the findNextFreeSeat function.
// *******************************************
function findPreviousFreeSeat(%client, %vehicle, %vehicleblock)
{
   // Check the previous seat
   %seat = %client.player.mSeat - 1;

   if (%seat == -1)
   {
      // Check from the last seat
      echo("Checking from seat " @ %seat);
   }

   %found = false;

   // Search through the seats.
   while ((%seat != %client.player.mSeat) && (%found == false))
   {
      if (%seat < 0)
      {
         %seat = %vehicleblock.numMountPoints - 1;
      }

      %node = %vehicle.getMountNodeObject(%seat);

      if (%node == 0)
      {
         %found = true;
      }
      if (%found == false)
         %seat--;
   }

   if (%found == false)
      return -1;
   else
      return %seat;
}

// *******************************************
// Function: setActiveSeat
//
// Inputs: Client connection object
//         Vehicle object
//         Vehicle datablock
//         Seat number
//
// Outputs: none
//
// This function re-seats the player into the
// given seat. The player is rotated to face
// the direction defined for the seat. The
// player's pose is also modified and the
// client is ordered to use the correct
// movement map.
// *******************************************
function setActiveSeat(%client, %vehicle, %vehicleblock, %seat)
{
   %client.setTransform(%vehicle.getDataBlock().mountPointTransform[%seat]);
   %vehicle.mountObject(%client,%seat);
   %client.mVehicle = %vehicle;

   CommandToClient(%obj.client, 'PopActionMap', moveMap);
   CommandToClient(%obj.client, 'PushActionMap', $Vehicle::moveMaps[%seat]);

   %client.mSeat = %seat;

   %client.setActionThread(%vehicle.getDatablock().mountPose[%seat],true,true);

   // Are we driving this vehicle?
   if (%seat == 0)
   {
      %client.setControlObject(%vehicle);
      %client.setArmThread("sitting");
   }
   else
   {
      %client.setControlObject(%client);
      %client.setArmThread("looknw");
   }
}

// *******************************************
// Function: isVehicleMoving
//
// Inputs: Vehicle object
//
// Outputs: FALSE if the vehicle is NOT moving
//          TRUE if the vehicle is moving
//
// This function calcuates if the vehicle is
// moving according to a threshold value
// defined on a per-vehicle basis.
// *******************************************
function isVehicleMoving(%vehicle)
{
   // Calculate the vehicle's velocity
   %vel = %vehicle.getVelocity();
   %speed = vectorLen(%vel);

   // Determine if the vehicle is moving according to the
   // threshold value defined in the vehicle's datablock.
   if (%speed > %vehicle.getDataBlock().stationaryThreshold)
      return true;
   else
      return false;
}

// *******************************************
// Function: getVehicleSpeed
//
// Inputs: Vehicle object
//
// Outputs: Vehicle's speed
//
// This utility function calculates the
// vehicle's velocity. Note that the speed
// does not define the vehicle's direction.
// *******************************************
function getVehicleSpeed(%vehicle)
{
   %vel = %vehicle.getVelocity();
   %speed = vectorLen(%vel);
   return %speed;
}

// *******************************************
// Function: dumpMounts
//
// Inputs: Vehicle object
//         Vehicle datablock
//
// Outputs: None
//
// This utility function simply dumps the
// mounts for a given vehicle. It is used to
// check which players the game engine thinks
// are currently mounted for each seat.
// *******************************************
function dumpMounts(%vehicle, %vehicleBlock)
{
   echo("**************");
   echo("Dumping mounts");
   echo("--------------");
   for (%ii=0; %ii<%vehicleblock.numMountPoints;%ii++)
   {
     echo(%ii @ ": " @ %vehicle.getMountNodeObject(%ii));
   }
   echo("**************");
}



// *******************************************
// Function: serverCmdAddCar
//
// Inputs: Client connection object
//
// Outputs: None
//
// This function creates a new WheeledVehicle
// instance near the player when a key is
// pressed. This is testing code, and is no
// longer needed now that vehicles can be
// added to the mission file.
// *******************************************
function serverCmdAddCar(%client)
{	
   if (%client.player $= "" )
      return;
   if (%client.player) {
      if (%client.player.isMounted())
         return;
   }
   %car = new WheeledVehicle(){
      datablock = DefaultCar;
   };
   %car.mountable = true;
   %car.setEnergyLevel(60);

   // Get the player's position. We will use this to
   // place the car near the player.
   %pos = %client.player.getTransform();
   %x = getWord(%pos, 0);
   %y = getWord(%pos, 1);
   %z = getWord(%pos, 2);
   %x += 5.0;
   %y += 5.0;
   %z += 0.5;
   %car.setTransform(%x SPC %y SPC %z);
   %car.parkingBrake=true;
   MissionCleanup.add(%car);
}


// *********************************** //



function serverCmdFindNextFreeSeat(%client)
{
   echo("serverCmdFindNextFreeSeat " @ %client.nameBase);

   // Is the vehicle moving? If so, prevent the player from switching seats
   if (isVehicleMoving(%client.player.mvehicle) == true)
      return;

   %newSeat = findNextFreeSeat(%client,
                               %client.player.mvehicle,
                               %client.player.mvehicle.getDataBlock());

   if (%newSeat != -1)
   {
      echo("Found new seat " @ %newSeat);

      setActiveSeat(%client.player,
                    %client.player.mvehicle,
                    %client.player.mvehicle.getDataBlock(),
                    %newSeat);
   }
   else
   {
      echo("No next free seat");
   }
}
