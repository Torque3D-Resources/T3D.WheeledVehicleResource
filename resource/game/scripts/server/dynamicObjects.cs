// ============================================================
// Project            :  kgb_test
// File               :  ..\..\..\..\..\T3D_clean\My Projects\demo.fps\game\scripts\server\dynamicObjects.cs
// Copyright          :  
// Author             :  kent
// Created on         :  Thursday, September 15, 2016 1:21 AM
//
// Editor             :  TorqueDev v. 1.2.5129.4848
//
// Description        :  
//                    :  
//                    :  
// ============================================================
function addDynamicObjects(){
   switch$ (theLevelInfo.levelName){
      case "Empty Terrain":
         addCars();
      case "Vehicle Lab":
         addCars2();
      default:
         echo("No dynamic objects");
   }
}
function addBuggy(){
   %obj = new WheeledVehicle(buggy1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "TgeBuggyCar";
      position = "-7.74116 15 230.357";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);
}
   
function addCars(){
   %obj = new WheeledVehicle(buggy1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "TgeBuggyCar";
      position = "-7.74116 15 230.357";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);
   
   %obj = new WheeledVehicle(jeep1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "JeepCar";
      position = "-7.74116 20 230.357";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);
   
   %obj = new WheeledVehicle(cheetah1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "CheetahCar";
      position = "-7.74116 25 230.357";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);   
}



function addCars2(){
   %obj = new WheeledVehicle(buggy1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "TgeBuggyCar";
      position = "-7.74116 15 5.25";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);
   
   %obj = new WheeledVehicle(jeep1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "JeepCar";
      position = "-7.74116 20 5.25";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);
   
   %obj = new WheeledVehicle(cheetah1) {
      disableMove = "0";
      isAIControlled = "0";
      dataBlock = "CheetahCar";
      position = "-7.74116 25 5.25";
      rotation = "-0.0221432 -0.0185072 -0.999584 79.8009";
      scale = "1 1 1";
      canSave = "1";
      canSaveDynamicFields = "1";
      centerSteeringRate = "0.04";
      fuelFlow = "0.2";
   };
   MissionCleanup.add(%obj);   
}