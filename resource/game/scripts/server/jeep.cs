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

function JeepCar::onAdd(%this, %obj)
{
   Parent::onAdd(%this, %obj);

	// ** Add vehicle-specific engine here
	%obj.setEngine( JeepEngine );   
    %obj.fuelFlow = .3;
    %obj.hop = 3000;

   %obj.setWheelTire(0,JeepCarTire);
   %obj.setWheelTire(1,JeepCarTire);
   %obj.setWheelTire(2,JeepCarTire);
   %obj.setWheelTire(3,JeepCarTire);

   // Setup the car with some tires & springs
   for (%i = %obj.getWheelCount() - 1; %i >= 0; %i--)
   {
	  //%obj.setWheelTire(%i,JeepCarTire);
      %obj.setWheelPowered(%i, true);
      %obj.setWheelSpring(%i, JeepCarSpring);
   }

   // Steer with the front tires
   %obj.setWheelSteering(0, 1);
   %obj.setWheelSteering(1, 1);

   // Add tail lights
   %obj.rightBrakeLight = new PointLight() 
   {
      radius = ".5";
      isEnabled = "0";
      color = "1 0 0.141176 1";
      brightness = "2";
      castShadows = "1";
      priority = "1";
      animate = "0";
      animationPeriod = "1";
      animationPhase = "1";
      flareScale = "1";
      attenuationRatio = "0 1 1";
      shadowType = "DualParaboloidSinglePass";
      texSize = "512";
      overDarkFactor = "2000 1000 500 100";
      shadowDistance = "400";
      shadowSoftness = "0.15";
      numSplits = "1";
      logWeight = "0.91";
      fadeStartDistance = "0";
      lastSplitTerrainOnly = "0";
      representedInLightmap = "0";
      shadowDarkenColor = "0 0 0 -1";
      includeLightmappedGeometryInShadow = "0";
      rotation = "1 0 0 0";
      canSave = "1";
      canSaveDynamicFields = "1";
         splitFadeDistances = "10 20 30 40";
   };
   %obj.leftBrakeLight = new PointLight() 
   {
      radius = ".5";
      isEnabled = "0";
      color = "1 0 0.141176 1";
      brightness = "2";
      castShadows = "1";
      priority = "1";
      animate = "0";
      animationPeriod = "1";
      animationPhase = "1";
      flareScale = "1";
      attenuationRatio = "0 1 1";
      shadowType = "DualParaboloidSinglePass";
      texSize = "512";
      overDarkFactor = "2000 1000 500 100";
      shadowDistance = "400";
      shadowSoftness = "0.15";
      numSplits = "1";
      logWeight = "0.91";
      fadeStartDistance = "0";
      lastSplitTerrainOnly = "0";
      representedInLightmap = "0";
      shadowDarkenColor = "0 0 0 -1";
      includeLightmappedGeometryInShadow = "0";
      rotation = "1 0 0 0";
      canSave = "1";
      canSaveDynamicFields = "1";
         splitFadeDistances = "10 20 30 40";
   };
   
      // Add Head lights
   %obj.rightHeadLight = new SpotLight() 
{
   range = "250";
   innerAngle = "40";
   outerAngle = "50";
   isEnabled = "0";
   color = "1 1 1 1";
   brightness = "2";
   castShadows = "1";
   priority = "1";
   animate = "1";
   animationPeriod = "1";
   animationPhase = "1";
   //flareType = "LightFlareExample0";
   //flareScale = "1";
   attenuationRatio = "0 1 1";
   shadowType = "Spot";
   texSize = "512";
   overDarkFactor = "2000 1000 500 100";
   shadowDistance = "400";
   shadowSoftness = "0.15";
   numSplits = "1";
   logWeight = "0.91";
   fadeStartDistance = "0";
   lastSplitTerrainOnly = "0";
   representedInLightmap = "0";
   shadowDarkenColor = "0 0 0 -1";
   includeLightmappedGeometryInShadow = "0";
   position = "-29.4362 -5.86289 5.58602";
   rotation = "1 0 0 0";
};
   %obj.leftHeadLight = new SpotLight() 
{
   range = "250";
   innerAngle = "40";
   outerAngle = "50";
   isEnabled = "0";
   color = "1 1 1 1";
   brightness = "2";
   castShadows = "1";
   priority = "1";
   animate = "1";
   animationPeriod = "1";
   animationPhase = "1";
   //flareType = "LightFlareExample0";
   //flareScale = "1";
   attenuationRatio = "0 1 1";
   shadowType = "Spot";
   texSize = "512";
   overDarkFactor = "2000 1000 500 100";
   shadowDistance = "400";
   shadowSoftness = "0.15";
   numSplits = "1";
   logWeight = "0.91";
   fadeStartDistance = "0";
   lastSplitTerrainOnly = "0";
   representedInLightmap = "0";
   shadowDarkenColor = "0 0 0 -1";
   includeLightmappedGeometryInShadow = "0";
   position = "-29.4362 -5.86289 5.58602";
   rotation = "1 0 0 0";
};

%obj.bulbHeadLight = new PointLight()
{
   radius = ".25";
   isEnabled = "0";
   color = "1 1 1 1";
   brightness = "10";
   castShadows = "1";
   priority = "1";
   animate = "0";
   animationType = "SubtlePulseLightAnim";
   animationPeriod = "3";
   animationPhase = "3";
   flareScale = "1";
   attenuationRatio = "0 1 1";
   shadowType = "DualParaboloidSinglePass";
   texSize = "512";
   overDarkFactor = "2000 1000 500 100";
   shadowDistance = "400";
   shadowSoftness = "0.15";
   numSplits = "1";
   logWeight = "0.91";
   fadeStartDistance = "0";
   lastSplitTerrainOnly = "0";
   splitFadeDistances = "10 20 30 40";
   representedInLightmap = "0";
   shadowDarkenColor = "0 0 0 -1";
   includeLightmappedGeometryInShadow = "1";
   position = "-61.3866 1.69186 5.1464";
   rotation = "1 0 0 0";
};

%obj.bulb2HeadLight = new PointLight()
{
   radius = ".25";
   isEnabled = "0";
   color = "1 1 1 1";
   brightness = "10";
   castShadows = "1";
   priority = "1";
   animate = "0";
   animationType = "SubtlePulseLightAnim";
   animationPeriod = "3";
   animationPhase = "3";
   flareScale = "1";
   attenuationRatio = "0 1 1";
   shadowType = "DualParaboloidSinglePass";
   texSize = "512";
   overDarkFactor = "2000 1000 500 100";
   shadowDistance = "400";
   shadowSoftness = "0.15";
   numSplits = "1";
   logWeight = "0.91";
   fadeStartDistance = "0";
   lastSplitTerrainOnly = "0";
   splitFadeDistances = "10 20 30 40";
   representedInLightmap = "0";
   shadowDarkenColor = "0 0 0 -1";
   includeLightmappedGeometryInShadow = "1";
   position = "-61.3866 1.69186 5.1464";
   rotation = "1 0 0 0";
};

   // Mount a ShapeBaseImageData
   %didMount = %obj.mountImage(JeepTurretImage, %this.turretSlot);

   // Mount the brake lights
   %obj.mountObject(%obj.rightBrakeLight, %this.rightBrakeSlot);
   %obj.mountObject(%obj.leftBrakeLight, %this.leftBrakeSlot);
   
   // Mount the head lamps
   %obj.mountObject(%obj.rightHeadLight, %this.rightHeadSlot);
   %obj.mountObject(%obj.leftHeadLight, %this.leftHeadSlot);
   %obj.mountObject(%obj.bulbHeadLight, %this.bulbHeadSlot);
   %obj.mountObject(%obj.bulb2HeadLight, %this.bulb2HeadSlot);
}

function JeepCar::onRemove(%this, %obj)
{
   echo("Removing Jeep");
   Parent::onRemove(%this, %obj);

   if(isObject(%obj.rightBrakeLight))
      %obj.rightBrakeLight.delete();

   if(isObject(%obj.leftBrakeLight))
      %obj.leftBrakeLight.delete();
      
   if(isObject(%obj.rightHeadLight))
      %obj.rightHeadLight.delete();

   if(isObject(%obj.leftHeadLight))
      %obj.leftHeadLight.delete();
      
   if(isObject(%obj.bulbHeadLight))
      %obj.bulbHeadLight.delete();

   if(isObject(%obj.bulb2HeadLight))
      %obj.bulb2HeadLight.delete();

   if(isObject(%obj.turret))
      %obj.turret.delete();
      
   
}

function JeepCar::onBrake(%this, %car, %type){
   if(%type == 0)
   {
      %car.rightBrakeLight.setLightEnabled(0);
      %car.leftBrakeLight.setLightEnabled(0);
   }
   else
   {
      %car.rightBrakeLight.setLightEnabled(1);
      %car.leftBrakeLight.setLightEnabled(1);
   }
}

function JeepCar::toggleHeadlights(%this, %car){
   if(%car.rightHeadLight.isEnabled)
   {
      %car.rightHeadLight.setLightEnabled(0);
      %car.leftHeadLight.setLightEnabled(0);
      %car.bulbHeadLight.setLightEnabled(0);
      %car.bulb2HeadLight.setLightEnabled(0);
   }
   else
   {
      %car.rightHeadLight.setLightEnabled(1);
      %car.leftHeadLight.setLightEnabled(1);
      %car.bulbHeadLight.setLightEnabled(1);
      %car.bulb2HeadLight.setLightEnabled(1);
   }   
}

// Callback invoked when an input move trigger state changes when the JeepCar
// is the control object
function JeepCar::onTrigger(%this, %obj, %index, %state)
{
   // Pass trigger states on to TurretImage (to fire weapon)
   switch ( %index )
   {
      case 0:  %obj.setImageTrigger( %this.turretSlot, %state );
      case 1:  %obj.setImageAltTrigger( %this.turretSlot, %state );
   }
}

function JeepTurretImage::onMount(%this, %obj, %slot)
{
   // Load the gun
   %obj.setImageAmmo(%slot, true);
}
