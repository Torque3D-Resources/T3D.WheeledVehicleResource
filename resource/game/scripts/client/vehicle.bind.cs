// ============================================================
// Project            :  kgb_test
// File               :  .\scripts\client\vehicle.bind.cs
// Copyright          :  
// Author             :  kent
// Created on         :  Tuesday, August 30, 2016 9:27 PM
//
// Editor             :  TorqueDev v. 1.2.5129.4848
//
// Description        :  
//                    :  
//                    :  
// ============================================================
$pref::vehicle::brakeTrigger = 4;


if (isJoystickDetected()){
   //exec("./joystick.cs");
   enableJoystick();
}

if ( isObject( vehicleDriverMap ) )
   vehicleDriverMap.delete();
new ActionMap(vehicleDriverMap);


// Starting vehicle action map code
if ( isObject( vehicleMap ) )
   vehicleMap.delete();
new ActionMap(vehicleMap);



// Similar to "moveforward" but will disengage the parking brake.
function accelerate(%val)
{
   //echo("Script:" @ $movementSpeed);
   %o = LocalClientConnection.player.mVehicle;
   if(%o.parkingBrake == true || %o.getGear()==0){      
      %o.parkingBrake = false;
      //%o.setGear(1);
   }
   $mvForwardAction = %val * $movementSpeed;
}
function decelerate(%val)
{
   //echo("Script:" @ $movementSpeed);
   %o = LocalClientConnection.player.mVehicle;
   if(%o.parkingBrake == true || %o.getGear()==0){      
      %o.parkingBrake = false;
      //%o.setGear(1);
   }
   $mvBackwardAction = %val * $movementSpeed;
}

// Trace a line along the direction the crosshair is pointing
// If you find a car with a player in it...eject them
function carjack()
{
   %player = LocalClientConnection.getControlObject();

   if (%player.getClassName() $= "Player")
   {
      %eyeVec = %player.getEyeVector();

      %startPos = %player.getEyePoint();
      %endPos = VectorAdd(%startPos, VectorScale(%eyeVec, 1000));

      %target = ContainerRayCast(%startPos, %endPos, $TypeMasks::VehicleObjectType);

      if (%target)
      {
         // See if anyone is mounted in the car's driver seat
         %mount = %target.getMountNodeObject(0);

         // Can only carjack bots
         // remove '&& %mount.getClassName() $= "AIPlayer"' to allow you
         // to carjack anyone/anything
         if (%mount && %mount.getClassName() $= "AIPlayer")
         {
            commandToServer('carUnmountObj', %mount);
         }
      }
   }
}

function getOut()
{
   commandToServer('dismountVehicle');
}

function headLights(){
   commandToServer('toggleHeadlights');
}

function brake(%val)
{
   $mvTriggerCount[$pref::vehicle::brakeTrigger]++;
}


// Bind the keys to the carjack command
moveMap.bindCmd(keyboard, "ctrl z", "carjack();", "");

// The key command for flipping the car
vehicleMap.bindCmd(keyboard, "ctrl x", "commandToServer(\'flipCar\');", "");


vehicleMap.bindCmd(keyboard, "escape", "", "handleEscape();");
//vehicleMap.bind( keyboard, w, moveforward ); // Original TGE
vehicleMap.bind( keyboard, w, accelerate );
vehicleMap.bind( keyboard, s, decelerate );
vehicleMap.bind( keyboard, up, moveforward );
vehicleMap.bind( keyboard, down, movebackward );
vehicleMap.bind( mouse, xaxis, yaw );
vehicleMap.bind( mouse, yaxis, pitch );
vehicleMap.bind( mouse, button0, mouseFire );
vehicleMap.bind( mouse, button1, altTrigger );
vehicleMap.bindCmd(keyboard, "ctrl f","getout();","");
vehicleMap.bind(keyboard, space, brake);
vehicleMap.bindCmd(keyboard, "l", "headLights();", "");
vehicleMap.bind( keyboard, v, toggleFreeLook ); // v for vanity
//vehicleMap.bind(keyboard, tab, toggleFirstPerson );
vehicleMap.bind(keyboard, "alt c", toggleCamera);
// bind the left thumbstick for steering
vehicleMap.bind( gamepad, thumblx, "D", "-0.23 0.23", gamepadYaw );
// bind the gas, break, and reverse buttons
vehicleMap.bind( gamepad, btn_a, moveforward );
vehicleMap.bind( gamepad, btn_b, brake );
vehicleMap.bind( gamepad, btn_x, movebackward );
// bind exiting the vehicle to a button
vehicleMap.bindCmd(gamepad, btn_y,"getout();","");
//vehicleMap.bindCmd(keyboard, "ctrl v", "commandToServer('DispEngineGui');", "");
vehicleMap.bind( keyboard, v, toggleFreeLook ); // v for vanity
vehicleMap.bindCmd(keyboard, "ctrl v", "commandToServer('DispEngineGui');", "");

vehicleMap.bindCmd(keyboard, "e", "commandToServer('UpShift');", "");
vehicleMap.bindCmd(keyboard, "q", "commandToServer('DnShift');", "");
vehicleMap.bindCmd(keyboard, "d", "commandToServer('CarHop');", "");


vehicleDriverMap.bindCmd(joystick0, "button4", "commandToServer('DnShift');", "");
vehicleDriverMap.bindCmd(joystick0, "button5", "commandToServer('UpShift');", "");
vehicleDriverMap.bindCmd(joystick0, "button6", "commandToServer('carHop');", "");

vehicleMap.bind(joystick0, "xaxis", gamepadYaw);
//vehicleMap.bind(joystick0, "rzaxis", joyAccel);
vehicleMap.bind(joystick0, "button1", accelerate);
vehicleMap.bind(joystick0, "button0", decelerate);
vehicleMap.bind(joystick0, "button2", brake);
vehicleMap.bind(joystick0, "button2", headLights);
vehicleMap.bindCmd(joystick0, "button7", "commandToServer(\'DismountVehicle\');", "");
//vehicleDriverMap.bind(joystick0, "button4",  "commandToServer(\'dnShift\');");
//vehicleDriverMap.bind(joystick0, "button5",  "commandToServer(\'upShift\');");
vehicleMap.bindCmd(joystick0, "button4", "commandToServer('DnShift');", "");
vehicleMap.bindCmd(joystick0, "button5", "commandToServer('UpShift');", "");
vehicleMap.bindCmd(joystick0, "button6", "commandToServer('carHop');", "");


vehicleMap.bind(keyboard, tab, toggleFirstPerson );

/******************************************************
                  Passenger Map
******************************************************* */
if ( isObject( vehiclePassengerMap ) )
   vehiclePassengerMap.delete();
new ActionMap(vehiclePassengerMap);

vehicleMap.bindCmd(keyboard, "escape", "", "handleEscape();");
vehiclePassengerMap.bindcmd(keyboard, "F2", "", "PlayerListGui.toggle();");
vehiclePassengerMap.bind(keyboard, c, toggleMessageHud );
vehiclePassengerMap.bind(keyboard, "pageUp", pageMessageHudUp );
vehiclePassengerMap.bind(keyboard, "pageDown", pageMessageHudDown );
vehiclePassengerMap.bind(keyboard, "p", resizeMessageHud );
vehiclePassengerMap.bind( keyboard, F3, startRecordingDemo );
vehiclePassengerMap.bind( keyboard, F4, stopRecordingDemo );
moveMap.bind(keyboard, "alt p", doScreenShotHudless);

vehiclePassengerMap.bind(keyboard, tab, toggleFirstPerson );

vehiclePassengerMap.bindCmd(keyboard, "q", "commandToServer(\'FindNextFreeSeat\');", "");
vehicleMap.bindCmd(keyboard, "ctrl f","getout();","");
vehiclePassengerMap.bind(keyboard, "alt c", toggleCamera);
vehiclePassengerMap.bindCmd(keyboard, "ctrl k", "commandToServer('suicide');", "");
