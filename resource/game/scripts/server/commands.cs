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

//-----------------------------------------------------------------------------
// Misc server commands avialable to clients
//-----------------------------------------------------------------------------

function serverCmdSuicide(%client)
{
   if (isObject(%client.player))
      %client.player.kill("Suicide");
}

function serverCmdPlayCel(%client,%anim)
{
   if (isObject(%client.player))
      %client.player.playCelAnimation(%anim);
}

function serverCmdTestAnimation(%client, %anim)
{
   if (isObject(%client.player))
      %client.player.playTestAnimation(%anim);
}

function serverCmdPlayDeath(%client)
{
   if (isObject(%client.player))
      %client.player.playDeathAnimation();
}

// ----------------------------------------------------------------------------
// Throw/Toss
// ----------------------------------------------------------------------------

function serverCmdThrow(%client, %data)
{
   %player = %client.player;
   if(!isObject(%player) || %player.getState() $= "Dead" || !$Game::Running)
      return;
   switch$ (%data)
   {
      case "Weapon":
         %item = (%player.getMountedImage($WeaponSlot) == 0) ? "" : %player.getMountedImage($WeaponSlot).item;
         if (%item !$="")
            %player.throw(%item);
      case "Ammo":
         %weapon = (%player.getMountedImage($WeaponSlot) == 0) ? "" : %player.getMountedImage($WeaponSlot);
         if (%weapon !$= "")
         {
            if(%weapon.ammo !$= "")
               %player.throw(%weapon.ammo);
         }
      default:
         if(%player.hasInventory(%data.getName()))
            %player.throw(%data);
   }
}

// ----------------------------------------------------------------------------
// Force game end and cycle
// Probably don't want this in a final game without some checks.  Anyone could
// restart a game.
// ----------------------------------------------------------------------------

function serverCmdFinishGame()
{
   cycleGame();
}

// ----------------------------------------------------------------------------
// Cycle weapons
// ----------------------------------------------------------------------------

function serverCmdCycleWeapon(%client, %direction)
{
   %client.getControlObject().cycleWeapon(%direction);
}

// ----------------------------------------------------------------------------
// Unmount current weapon
// ----------------------------------------------------------------------------

function serverCmdUnmountWeapon(%client)
{
   %client.getControlObject().unmountImage($WeaponSlot);
}

// ----------------------------------------------------------------------------
// Weapon reloading
// ----------------------------------------------------------------------------
function serverCmdReloadWeapon(%client)
{
   %player = %client.getControlObject();
   %image = %player.getMountedImage($WeaponSlot);
   
   // Don't reload if the weapon's full.
   if (%player.getInventory(%image.ammo) == %image.ammo.maxInventory)
      return;
      
   if (%image > 0)
      %image.clearAmmoClip(%player, $WeaponSlot);
}

// ----------------------------------------------------------------------------
// Object interaction
// ----------------------------------------------------------------------------
function serverCmdUseObject(%client, %type)
{
   %player = %client.player;

   %eye = %player.getEyeVector();
   if(%player.mounted == true){
      %vec = vectorScale(%eye, 5);
   } else {
      %vec = vectorScale(%eye, 5);
   }
   

   %startPoint = %player.getEyeTransform();
   %endPoint = VectorAdd(%startPoint,%vec);
   %searchMasks = $TypeMasks::ShapeBaseObjectType | $TypeMasks::StaticObjectType;

   %objects = ContainerRayCast (%startPoint, %endPoint, %searchMasks, %player);
   %obj = getword(%objects,0);
   if (%obj){
      echo("Obj:" @ %obj @ "//");
       %db = %obj.getDataBlock();
       // See if this is a vehicle that can be mounted.
      if ((%db.getClassName() $= "WheeledVehicleData" ) && %player.mountVehicle && %player.getState() $= "Move")
      {
         onMountVehicle(%db, %player, %obj);
         
         /*
         // Only mount drivers for now.
         ServerConnection.setFirstPerson(0);

         // For this specific example, only one person can fit
         // into a vehicle
         %mount = %obj.getMountNodeObject(0);         
         if(%mount)
            return;
         
         // For this specific FPS Example, always mount the player
         // to node 0
         %node = 0;
         %obj.mountObject(%player, %node);
         %player.mVehicle = %obj;
         */
         
      } else {
         // Attempt to use against the object datablock
         %ob.getDataBlock().useObject(%ob,%client);   
      }      
   } else {   
      messageClient(%client, 'MsgUseObjectNotInRange', '\c0Not in range');
		echo("No Obj:" @ %ob @ "//");
	}
}