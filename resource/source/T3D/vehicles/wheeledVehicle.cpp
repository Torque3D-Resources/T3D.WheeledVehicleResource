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

#include "platform/platform.h"
#include "T3D/vehicles/wheeledVehicle.h"

#include "math/mMath.h"
#include "math/mathIO.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "collision/clippedPolyList.h"
#include "collision/planeExtractor.h"
#include "core/stream/bitStream.h"
#include "core/dnet.h"
#include "T3D/gameBase/gameConnection.h"
#include "ts/tsShapeInstance.h"
#include "T3D/fx/particleEmitter.h"
#include "sfx/sfxSystem.h"
#include "sfx/sfxTrack.h"
#include "sfx/sfxSource.h"
#include "sfx/sfxTypes.h"
#include "scene/sceneManager.h"
#include "core/resourceManager.h"
#include "materials/materialDefinition.h"
#include "materials/baseMatInstance.h"
#include "lighting/lightQuery.h"

#include "console/engineAPI.h"
#include "console/consoleInternal.h"

// Collision masks are used to determine what type of objects the
// wheeled vehicle will collide with.
static U32 sClientCollisionMask =
      TerrainObjectType     | PlayerObjectType  | 
      StaticShapeObjectType | VehicleObjectType | 
      VehicleBlockerObjectType;

// Gravity constant
static F32 sWheeledVehicleGravity = -20;

// Misc. sound constants
static F32 sMinSquealVolume = 0.05f;
static F32 sIdleEngineVolume = 0.2f;


//----------------------------------------------------------------------------
// Vehicle Tire Data Block
//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(WheeledVehicleTire);

ConsoleDocClass( WheeledVehicleTire,
   "@brief Defines the properties of a WheeledVehicle tire.\n\n"
   "Tires act as springs and generate lateral and longitudinal forces to move "
   "the vehicle. These distortion/spring forces are what convert wheel angular "
   "velocity into forces that act on the rigid body.\n"
   "@ingroup Vehicles\n"
);

WheeledVehicleTire::WheeledVehicleTire()
{
   shape = 0;
   shapeName = "";
   staticFriction = 1;
   kineticFriction = 0.5f;
   restitution = 1;
   rollingResistance = 0.125f; // Engine Change
   radius = 0.6f;
   lateralForce = 10;
   lateralDamping = 1;
   lateralRelaxation = 1;
   longitudinalForce = 10;
   longitudinalDamping = 1;
   longitudinalRelaxation = 1;
   mass = 1.f;
}

bool WheeledVehicleTire::preload(bool server, String &errorStr)
{
   // Load up the tire shape.  ShapeBase has an option to force a
   // CRC check, this is left out here, but could be easily added.
   if (shapeName && shapeName[0]) 
   {

      // Load up the shape resource
      shape = ResourceManager::get().load(shapeName);
      if (!bool(shape)) 
      {
         errorStr = String::ToString("WheeledVehicleTire: Couldn't load shape \"%s\"",shapeName);
         return false;
      }

      // Determine wheel radius from the shape's bounding box.
      // The tire should be built with it's hub axis along the
      // object's Y axis.
      radius = shape->bounds.len_z() / 2;
   }

   return true;
}

void WheeledVehicleTire::initPersistFields()
{
   addField( "shapeFile",TypeShapeFilename,Offset(shapeName,WheeledVehicleTire),
      "The path to the shape to use for the wheel." );
   addField( "mass", TypeF32, Offset(mass, WheeledVehicleTire),
      "The mass of the wheel.\nCurrently unused." );
   addField( "radius", TypeF32, Offset(radius, WheeledVehicleTire),
      "@brief The radius of the wheel.\n\n"
      "The radius is determined from the bounding box of the shape provided "
      "in the shapefile field, and does not need to be specified in script. "
      "The tire should be built with its hub axis along the object's Y-axis." );
   addField( "staticFriction", TypeF32, Offset(staticFriction, WheeledVehicleTire),
      "Tire friction when the wheel is not slipping (has traction)." );
   addField( "kineticFriction", TypeF32, Offset(kineticFriction, WheeledVehicleTire),
      "Tire friction when the wheel is slipping (no traction)." );
   addField( "restitution", TypeF32, Offset(restitution, WheeledVehicleTire),
      "Tire restitution.\nCurrently unused." );
   addField( "lateralForce", TypeF32, Offset(lateralForce, WheeledVehicleTire),
      "@brief Tire force perpendicular to the direction of movement.\n\n"
      "Lateral force can in simple terms be considered left/right steering "
      "force. WheeledVehicles are acted upon by forces generated by their tires "
      "and the lateralForce measures the magnitude of the force exerted on the "
      "vehicle when the tires are deformed along the x-axis. With real wheeled "
      "vehicles, tires are constantly being deformed and it is the interplay of "
      "deformation forces which determines how a vehicle moves. In Torque's "
      "simulation of vehicle physics, tire deformation obviously can't be handled "
      "with absolute realism, but the interplay of a vehicle's velocity, its "
      "engine's torque and braking forces, and its wheels' friction, lateral "
      "deformation, lateralDamping, lateralRelaxation, longitudinal deformation, "
      "longitudinalDamping, and longitudinalRelaxation forces, along with its "
      "wheels' angular velocity are combined to create a robust real-time "
      "physical simulation.\n\n"
      "For this field, the larger the value supplied for the lateralForce, the "
      "larger the effect steering maneuvers can have. In Torque tire forces are "
      "applied at a vehicle's wheel hubs." );
   addField( "lateralDamping", TypeF32, Offset(lateralDamping, WheeledVehicleTire),
      "Damping force applied against lateral forces generated by the tire.\n\n"
      "@see lateralForce" );
   addField( "lateralRelaxation", TypeF32, Offset(lateralRelaxation, WheeledVehicleTire),
      "@brief Relaxing force applied against lateral forces generated by the tire.\n\n"
      "The lateralRelaxation force measures how strongly the tire effectively "
      "un-deforms.\n\n@see lateralForce" );
   addField( "longitudinalForce", TypeF32, Offset(longitudinalForce, WheeledVehicleTire),
      "@brief Tire force in the direction of movement.\n\n"
      "Longitudinal force can in simple terms be considered forward/backward "
      "movement force. WheeledVehicles are acted upon by forces generated by "
      "their tires and the longitudinalForce measures the magnitude of the "
      "force exerted on the vehicle when the tires are deformed along the y-axis.\n\n"
      "For this field, the larger the value, the larger the effect "
      "acceleration/deceleration inputs have.\n\n"
      "@see lateralForce" );
   addField( "longitudinalDamping", TypeF32, Offset(longitudinalDamping, WheeledVehicleTire),
      "Damping force applied against longitudinal forces generated by the tire.\n\n"
      "@see longitudinalForce" );
   addField( "longitudinalRelaxation", TypeF32, Offset(longitudinalRelaxation, WheeledVehicleTire),
      "@brief Relaxing force applied against longitudinal forces generated by the tire.\n\n"
      "The longitudinalRelaxation force measures how strongly the tire effectively "
      "un-deforms.\n\n"
      "@see longitudinalForce" );
   addField( "rollingResistance", TypeF32, Offset(rollingResistance, WheeledVehicleTire),
      "@brief (engine change) Dampening factor when wheel is rolling (not under power).\n\n"
      );
   Parent::initPersistFields();
}

void WheeledVehicleTire::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeString(shapeName);
   stream->write(mass);
   stream->write(staticFriction);
   stream->write(kineticFriction);
   stream->write(restitution);
   stream->write(radius);
   stream->write(lateralForce);
   stream->write(lateralDamping);
   stream->write(lateralRelaxation);
   stream->write(longitudinalForce);
   stream->write(longitudinalDamping);
   stream->write(longitudinalRelaxation);
   stream->write(rollingResistance ); // Engine Change
}

void WheeledVehicleTire::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   shapeName = stream->readSTString();
   stream->read(&mass);
   stream->read(&staticFriction);
   stream->read(&kineticFriction);
   stream->read(&restitution);
   stream->read(&radius);
   stream->read(&lateralForce);
   stream->read(&lateralDamping);
   stream->read(&lateralRelaxation);
   stream->read(&longitudinalForce);
   stream->read(&longitudinalDamping);
   stream->read(&longitudinalRelaxation);
   stream->read(&rollingResistance); // Engine Change
}


//----------------------------------------------------------------------------
// Vehicle Spring Data Block
//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(WheeledVehicleSpring);

ConsoleDocClass( WheeledVehicleSpring,
   "@brief Defines the properties of a WheeledVehicle spring.\n\n"
   "@ingroup Vehicles\n"
);

WheeledVehicleSpring::WheeledVehicleSpring()
{
   length = 1;
   force = 10;
   damping = 1;
   antiSway = 1;
}

void WheeledVehicleSpring::initPersistFields()
{
   addField( "length", TypeF32, Offset(length, WheeledVehicleSpring),
      "@brief Maximum spring length. ie. how far the wheel can extend from the "
      "root hub position.\n\n"
      "This should be set to the vertical (Z) distance the hub travels in the "
      "associated spring animation." );
   addField( "force", TypeF32, Offset(force, WheeledVehicleSpring),
      "@brief Maximum spring force (when compressed to minimum length, 0).\n\n"
      "Increasing this will make the vehicle suspension ride higher (for a given "
      "vehicle mass), and also make the vehicle more bouncy when landing jumps." );
   addField( "damping", TypeF32, Offset(damping, WheeledVehicleSpring),
      "@brief Force applied to slow changes to the extension of this spring.\n\n"
      "Increasing this makes the suspension stiffer which can help stabilise "
      "bouncy vehicles." );
   addField( "antiSwayForce", TypeF32, Offset(antiSway, WheeledVehicleSpring),
      "@brief Force applied to equalize extension of the spring on the opposite "
      "wheel.\n\n"
      "This force helps to keep the suspension balanced when opposite wheels "
      "are at different heights." );

   Parent::initPersistFields();
}

void WheeledVehicleSpring::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->write(length);
   stream->write(force);
   stream->write(damping);
   stream->write(antiSway);
}

void WheeledVehicleSpring::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   stream->read(&length);
   stream->read(&force);
   stream->read(&damping);
   stream->read(&antiSway);
}



//----------------------------------------------------------------------------
// Wheeled Vehicle Engine Data Block
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
IMPLEMENT_CO_DATABLOCK_V1(WheeledVehicleEngine);

ConsoleDocClass( WheeledVehicleEngine,
   "@brief Defines the properties of a WheeledVehicle engine/transmission.\n\n"
   "@ingroup Vehicles\n"
);


IMPLEMENT_CALLBACK( WheeledVehicleEngine, onShift, void, ( GameBase* obj, S32 gear, S32 engineRPM ), ( obj, gear, engineRPM ),
   "@brief Called when the object is removed from the scene.\n\n"
   "@param obj the GameBase object\n\n"
   "@see onAdd for an example\n" );


WheeledVehicleEngine::WheeledVehicleEngine()
{   
	minEngineRPM   = 10.0f;
   idleEngineRPM  = 1000.0f;
	maxEngineRPM   = 7000.0f;
	engineDrag     = 0.001f;
   neutralBoost   = 3.5f;
   slowDownRate   = 0.001f;
   useAutomatic   = false;
   fuelFlow       = 0.1f;

   inertiaFactor = 1.0f;
   overRevSlowdown = 0.08f;

	minEnginePitch  = 0.8f;
	maxEnginePitch  = 1.2f;
	minEngineVolume = 0.3f;
	maxEngineVolume = 1.0f;

	
	shiftUpRPM       = 4500.0f;
	shiftDownRPM     = 2000.0f;
	transmissionSlip = 250.0f;

	numGearRatios = 5;
	gearRatios[0] = 3.09F;
	gearRatios[1] = 1.92F;
	gearRatios[2] = 1.40F;
	gearRatios[3] = 1.0F;
	gearRatios[4] = 0.89F;
		
	reverseRatio     = 2.45f;	
	differentalRatio = 3.4F;
	
	numTorqueLevels = 13;
	for( int i = 0; i < MaxTorqueLevels; i++ ) {
		torqueLevels[i] = 0;
		rpmValues[i] = 0;
	}
	/*
	torqueLevels[0]  = 160.0F;
	torqueLevels[1]  = 225.0F;
	torqueLevels[2]  = 298.0F;
	torqueLevels[3]  = 305.0F;
	torqueLevels[4]  = 298.0F;
	torqueLevels[5]  = 290.0F;
	torqueLevels[6]  = 285.0F;
	torqueLevels[7]  = 280.0F;
	torqueLevels[8]  = 275.0F;
	torqueLevels[9]  = 265.0F;
	torqueLevels[10] = 260.0F;
	torqueLevels[11] = 240.0F;
	torqueLevels[12] = 230.0F;
	
	for( int i = 0; i < numTorqueLevels; i++ ) {
		rpmValues[i] = 1000.0 + (500.0 * i );
	}
	*/	
	setup = false;
}

void WheeledVehicleEngine::initPersistFields() {
	Parent::initPersistFields();
	
	addField( "minRPM", TypeF32, Offset( minEngineRPM, WheeledVehicleEngine),"@brief The STALL RPM - must be a positive (non-zero) number");
	addField( "maxRPM", TypeF32, Offset( maxEngineRPM, WheeledVehicleEngine),"@brief Upper limit of RPM output");
   addField( "idleRPM", TypeF32, Offset( idleEngineRPM, WheeledVehicleEngine),"@brief RPM that the running engine naturaly holds without accelertation");	
	addField( "minPitch", TypeF32, Offset( minEnginePitch, WheeledVehicleEngine),"@brief The low-end of engine pitch (At zero value, not IDLE)");
	addField( "maxPitch", TypeF32, Offset( maxEnginePitch, WheeledVehicleEngine),"@brief High-end engine pitch at maxEngineRPM (can overrev higher)");
	addField( "minVolume", TypeF32, Offset( minEngineVolume, WheeledVehicleEngine),"@brief The low-end of engine volume (At zero value, not IDLE)");
	addField( "maxVolume", TypeF32, Offset( maxEngineVolume, WheeledVehicleEngine),"@brief High-end engine volume at maxEngineRPM (can overrev higher)");
	addField( "numGears", TypeS32, Offset( numGearRatios,WheeledVehicleEngine),"@brief Number of gears");
	addField( "gearRatios", TypeF32, Offset( gearRatios,WheeledVehicleEngine), MaxGears, "@brief Gear ratio table" );
	
	
   addField( "shiftUpRPM", TypeF32, Offset( shiftUpRPM, WheeledVehicleEngine ),"@brief Automatic transmission - up shifts at this RPM level");
	addField( "shiftDownRPM", TypeF32, Offset( shiftDownRPM, WheeledVehicleEngine ),"@brief Automatic transmission - down shifts at this RPM level");
   addField( "transmissionSlip", TypeF32, Offset(transmissionSlip, WheeledVehicleEngine),"@brief RPM drop on upshift - simulates power loss");
	addField( "reverseRatio", TypeF32, Offset( reverseRatio, WheeledVehicleEngine),"@brief Gear ratio when car is in reverse");	
	addField( "differentialRatio", TypeF32, Offset( differentalRatio, WheeledVehicleEngine),"@brief Gear ratio of differential (scales wheel RPM to engine RPM");

	addField( "engineDrag", TypeF32, Offset( engineDrag, WheeledVehicleEngine),"@brief The amount of drag that the engine adds when throttle is off");
   addField( "neutralBoost", TypeF32, Offset( neutralBoost, WheeledVehicleEngine),"@brief  Amount of boost to provide when the engine is revved in neutral");
   addField( "slowDownRate", TypeF32, Offset( slowDownRate, WheeledVehicleEngine),"@brief Slowdown rate when negative throttle is applied");
   addField( "inertiaFactor", TypeF32, Offset( inertiaFactor, WheeledVehicleEngine),"@brief Scales the mass resistance calculations (they seem OK, so it defaults to 1).");
   addField( "overRevSlowdown", TypeF32, Offset( overRevSlowdown, WheeledVehicleEngine),"@brief How quickly the engine/wheel RPM react to over-rev (downshift)");
   addField( "fuelFlow", TypeF32, Offset( fuelFlow, WheeledVehicleEngine),"@brief Default fuel flow. This values is used to scales torque to calculate RPM increase");
   addField( "useAutomatic", TypeBool, Offset( useAutomatic, WheeledVehicleEngine),"@brief Default automatic/manual transmission setting");

	addField("numTorqueLevels", TypeS32, Offset( numTorqueLevels, WheeledVehicleEngine),"@brief Number of torque levels. Each level defines the engine torque output for a set rpm");
	
	addField("torqueLevels", TypeF32, Offset( torqueLevels, WheeledVehicleEngine), MaxTorqueLevels, "@brief Torque level array (corresponds to rmpValues array");
	addField("rpmValues", TypeF32, Offset( rpmValues, WheeledVehicleEngine), MaxTorqueLevels, "@brief RPM level array (corresponds to torqueLevel array)");	
	
}

void WheeledVehicleEngine::packData( BitStream *stream ) {
	Parent::packData(stream);
	
	stream->write( minEngineRPM );
	stream->write( maxEngineRPM );
   stream->write( idleEngineRPM );
	stream->write( numGearRatios );
	for( int i = 0; i < numGearRatios; i++ ) {
		stream->write( gearRatios[i] );   
	}
	
	stream->write( reverseRatio );
	stream->write( differentalRatio );	
	stream->write( shiftUpRPM );
	stream->write( shiftDownRPM );
   stream->write( transmissionSlip );

   stream->write( engineDrag );
   stream->write( neutralBoost );
   stream->write( slowDownRate);
   stream->write( inertiaFactor);
   stream->write( overRevSlowdown);
      
   stream->write( fuelFlow );
   stream->writeFlag( useAutomatic );

	stream->write( numTorqueLevels );
	for( int i = 0; i < numTorqueLevels; i++ ) {
		stream->write( torqueLevels[i] );
		stream->write( rpmValues[i] );
	}
		
}

void WheeledVehicleEngine::unpackData( BitStream *stream ) {
	Parent::unpackData(stream);
	
	stream->read(&minEngineRPM );
	stream->read(&maxEngineRPM );
   stream->read(&idleEngineRPM );
	stream->read(&numGearRatios );
	
	for( int i = 0; i < MaxGears; i++ ) {
		gearRatios[i] = 0.0f;
	}
	for( int i = 0; i < numGearRatios; i++ ) {
		stream->read(&gearRatios[i]);
	}
	
	stream->read( &reverseRatio );
	stream->read( &differentalRatio );
	stream->read( &shiftUpRPM );
	stream->read( &shiftDownRPM );
   stream->read( &transmissionSlip );

   stream->read( &engineDrag );
   stream->read( &neutralBoost );
   stream->read( &slowDownRate);
   stream->read( &inertiaFactor);
   stream->read( &overRevSlowdown);

   stream->read( &fuelFlow );
   useAutomatic = stream->readFlag();

	stream->read( &numTorqueLevels );	
	for( int i = 0; i < MaxTorqueLevels; i++ ) {
		torqueLevels[i] = 0.0f;
		rpmValues[i] = 0.0f;
	}
	for( int i = 0; i < numTorqueLevels; i++ ) {
		stream->read(&torqueLevels[i]);
		stream->read(&rpmValues[i]);
	}
		
}

// **KGB: This is a modified version of the original "getTorque" function
// **     that scaled torque based on the transmission settings after the
// **     lookup.  getRPMTorque just does the pure lookup - nicely
// **     interpolated using the original code.
// ================================================================================
F32 WheeledVehicleEngine::getRPMTorque( F32 RPM) {

	if( !setup ) {
		setup = true;
		// Compute torque interpolations....
		for( int i = 0; i < numTorqueLevels-1; i++ ) {
			deltaTorque[i] = torqueLevels[i+1] - torqueLevels[i];
			rpmStep[i] = rpmValues[i+1] - rpmValues[i];
		}
		deltaTorque[numTorqueLevels-1] = 0;
		rpmStep[numTorqueLevels-1] = 1;
	}	
	
   // See if the RPM are below the lowest level if so, then scale the lowest
   // Torque level
   int TorqueIdx = 0;   

	if( RPM < rpmValues[0] ) {
		return torqueLevels[0] * ( RPM / rpmValues[TorqueIdx] );
	}
	for( ; rpmValues[TorqueIdx] < RPM && TorqueIdx < numTorqueLevels; TorqueIdx++ );
	
	F32 partial = 0.0f;
	if( TorqueIdx >= numTorqueLevels )
		TorqueIdx = numTorqueLevels-1;
	else 
	    partial = ((RPM - rpmValues[TorqueIdx]) / rpmStep[TorqueIdx]) * deltaTorque[TorqueIdx];

	F32 baseTorque = torqueLevels[TorqueIdx]  + partial;
	return baseTorque;
}


//----------------------------------------------------------------------------
// Wheeled Vehicle Data Block
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(WheeledVehicleData);

ConsoleDocClass( WheeledVehicleData,
   "@brief Defines the properties of a WheeledVehicle.\n\n"
   "@ingroup Vehicles\n"
);

WheeledVehicleData::WheeledVehicleData()
{
   tireEmitter = 0;
   dustVolume = 1.0f;
   maxWheelSpeed = 40;
   engineTorque = 1;
   engineBrake = 1;
   brakeTorque = 1;
   brakeLightSequence = -1;
   steeringSequence = -1;
   wheelCount = 0;

   for (S32 i = 0; i < MaxSounds; i++)
      sound[i] = 0;
}


//----------------------------------------------------------------------------
/** Load the vehicle shape
   Loads and extracts information from the vehicle shape.

   Wheel Sequences
      spring#        Wheel spring motion: time 0 = wheel fully extended,
                     the hub must be displaced, but not directly animated
                     as it will be rotated in code.
   Other Sequences
      steering       Wheel steering: time 0 = full right, 0.5 = center
      brakeLight     Brake light, time 0 = off, 1 = braking

   Wheel Nodes
      hub#           Wheel hub

   The steering and animation sequences are optional.
*/
bool WheeledVehicleData::preload(bool server, String &errorStr)
{
   if (!Parent::preload(server, errorStr))
      return false;

   // A temporary shape instance is created so that we can
   // animate the shape and extract wheel information.
   TSShapeInstance* si = new TSShapeInstance(mShape, false);

   // Resolve objects transmitted from server
   if (!server) {
      for (S32 i = 0; i < MaxSounds; i++)
         if( !sfxResolve( &sound[ i ], errorStr ) )
            return false;

      if (tireEmitter)
         Sim::findObject(SimObjectId((uintptr_t)tireEmitter),tireEmitter);
   }

   // Extract wheel information from the shape
   TSThread* thread = si->addThread();
   Wheel* wp = wheel;
   char buff[10];
   for (S32 i = 0; i < MaxWheels; i++) {

      // The wheel must have a hub node to operate at all.
      dSprintf(buff,sizeof(buff),"hub%d",i);
      wp->springNode = mShape->findNode(buff);
      if (wp->springNode != -1) {

         // Check for spring animation.. If there is none we just grab
         // the current position of the hub. Otherwise we'll animate
         // and get the position at time 0.
         dSprintf(buff,sizeof(buff),"spring%d",i);
         wp->springSequence = mShape->findSequence(buff);
         if (wp->springSequence == -1)
            si->mNodeTransforms[wp->springNode].getColumn(3, &wp->pos);
         else {
            si->setSequence(thread,wp->springSequence,0);
            si->animate();
            si->mNodeTransforms[wp->springNode].getColumn(3, &wp->pos);

            // Determin the length of the animation so we can scale it
            // according the actual wheel position.
            Point3F downPos;
            si->setSequence(thread,wp->springSequence,1);
            si->animate();
            si->mNodeTransforms[wp->springNode].getColumn(3, &downPos);
            wp->springLength = wp->pos.z - downPos.z;
            if (!wp->springLength)
               wp->springSequence = -1;
         }

         // Match wheels that are mirrored along the Y axis.
         mirrorWheel(wp);
         wp++;
      }
   }
   wheelCount = wp - wheel;

   // Check for steering. Should think about normalizing the
   // steering animation the way the suspension is, but I don't
   // think it's as critical.
   steeringSequence = mShape->findSequence("steering");

   // Brakes
   brakeLightSequence = mShape->findSequence("brakelight");

   // Extract collision planes from shape collision detail level
   if (collisionDetails[0] != -1) {
      MatrixF imat(1);
      SphereF sphere;
      sphere.center = mShape->center;
      sphere.radius = mShape->radius;
      PlaneExtractorPolyList polyList;
      polyList.mPlaneList = &rigidBody.mPlaneList;
      polyList.setTransform(&imat, Point3F(1,1,1));
      si->buildPolyList(&polyList,collisionDetails[0]);
   }

   delete si;
   return true;
}


//----------------------------------------------------------------------------
/** Find a matching lateral wheel
   Looks for a matching wheeling mirrored along the Y axis, within some
   tolerance (current 0.5m), if one is found, the two wheels are lined up.
*/
bool WheeledVehicleData::mirrorWheel(Wheel* we)
{
   we->opposite = -1;
   for (Wheel* wp = wheel; wp != we; wp++)
      if (mFabs(wp->pos.y - we->pos.y) < 0.5) 
      {
         we->pos.x = -wp->pos.x;
         we->pos.y = wp->pos.y;
         we->pos.z = wp->pos.z;
         we->opposite = wp - wheel;
         wp->opposite = we - wheel;
         return true;
      }
   return false;
}


//----------------------------------------------------------------------------

void WheeledVehicleData::initPersistFields()
{
   addField( "jetSound", TYPEID< SFXTrack >(), Offset(sound[JetSound], WheeledVehicleData),
      "Looping sound played when the vehicle is jetting." );
   addField( "engineSound", TYPEID< SFXTrack >(), Offset(sound[EngineSound], WheeledVehicleData),
      "@brief Looping engine sound.\n\n"
      "The pitch is dynamically adjusted based on the current engine RPM" );
   addField("squealSound", TYPEID< SFXTrack >(), Offset(sound[SquealSound], WheeledVehicleData),
      "@brief Looping sound played while any of the wheels is slipping.\n\n"
      "The volume is dynamically adjusted based on how much the wheels are slipping." );
   addField("WheelImpactSound", TYPEID< SFXTrack >(), Offset(sound[WheelImpactSound], WheeledVehicleData),
      "Sound played when the wheels impact the ground.\nCurrently unused." );

   addField("tireEmitter",TYPEID< ParticleEmitterData >(), Offset(tireEmitter, WheeledVehicleData),
      "ParticleEmitterData datablock used to generate particles from each wheel "
      "when the vehicle is moving and the wheel is in contact with the ground.");
   addField("dustVolume", TypeF32, Offset(dustVolume, WheeledVehicleData),
      "@brief Determines the volume of particles generated by the tireEmitter.\n\n"
      "This can increase/decrease dust effect." );
   addField("maxWheelSpeed", TypeF32, Offset(maxWheelSpeed, WheeledVehicleData),
      "@brief Maximum linear velocity of each wheel.\n\n"
      "This caps the maximum speed of the vehicle." );
   addField("engineTorque", TypeF32, Offset(engineTorque, WheeledVehicleData),
      "@brief Torque available from the engine at 100% throttle.\n\n"
      "This controls vehicle acceleration. ie. how fast it will reach maximum speed." );
   addField("engineBrake", TypeF32, Offset(engineBrake, WheeledVehicleData),
      "@brief Braking torque applied by the engine when the throttle and brake "
      "are both 0.\n\n"
      "This controls how quickly the vehicle will coast to a stop." );
   addField("brakeTorque", TypeF32, Offset(brakeTorque, WheeledVehicleData),
      "@brief Torque applied when braking.\n\n"
      "This controls how fast the vehicle will stop when the brakes are applied." );
   
   Parent::initPersistFields();
}


//----------------------------------------------------------------------------

void WheeledVehicleData::packData(BitStream* stream)
{
   Parent::packData(stream);

   if (stream->writeFlag(tireEmitter))
      stream->writeRangedU32(packed? SimObjectId((uintptr_t)tireEmitter):
         tireEmitter->getId(),DataBlockObjectIdFirst,DataBlockObjectIdLast);

   for (S32 i = 0; i < MaxSounds; i++)
      sfxWrite( stream, sound[ i ] );

stream->write(dustVolume);   
stream->write(maxWheelSpeed);
   stream->write(engineTorque);
   stream->write(engineBrake);
   stream->write(brakeTorque);
}

void WheeledVehicleData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   tireEmitter = stream->readFlag()?
      (ParticleEmitterData*) stream->readRangedU32(DataBlockObjectIdFirst,
         DataBlockObjectIdLast): 0;

   for (S32 i = 0; i < MaxSounds; i++)
      sfxRead( stream, &sound[ i ] );

   stream->read(&dustVolume);   
   stream->read(&maxWheelSpeed);
   stream->read(&engineTorque);
   stream->read(&engineBrake);
   stream->read(&brakeTorque);
}


//----------------------------------------------------------------------------
// Wheeled Vehicle Class
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(WheeledVehicle);

ConsoleDocClass( WheeledVehicle,
   "@brief A wheeled vehicle.\n"
   "@ingroup Vehicles\n"
);

WheeledVehicle::WheeledVehicle()
{
   mDataBlock = 0;
   mBraking = false;
   mJetSound = NULL;
   mEngineSound = NULL;
   mSquealSound = NULL;
   mTailLightThread = 0;
   mSteeringThread = 0;

   for (S32 i = 0; i < WheeledVehicleData::MaxWheels; i++) {
      mWheel[i].springThread = 0;
      mWheel[i].Dy = mWheel[i].Dx = 0;
      mWheel[i].tire = 0;
      mWheel[i].spring = 0;
      mWheel[i].shapeInstance = 0;
      mWheel[i].steering = 0;
      mWheel[i].powered = true;
      mWheel[i].slipping = false;
   }

 /***KGB: Advanced Vehicle Default Settings*/
   mBrakeTrigger = 4;	// TGE/T3D default is 2.

   mCenterSteeringRate = 0.04f;
   mEngine = NULL;
   mFuelFlow = 0.1f;   
   // Transmission Settings
   mUseAutomatic = true;   
   mCurGear = 0;
   mGearDelay = 0;
   mStuckCounter = 0;
   mSlowDown = 0.0f;

   inPark = false;
   // RPM Values for engine and wheels
   mEngineRPM = 500.0f;
   mWheelRPM = 0.0f;
   mWheelVel = 0.0f;
   inPark=true;
   mForceBraking=false;
   /* End of Engine Change */
}

WheeledVehicle::~WheeledVehicle()
{
}

void WheeledVehicle::initPersistFields()
{
	Parent::initPersistFields();
	addField( "centerSteeringRate", TypeF32, Offset(mCenterSteeringRate, WheeledVehicle),"@brief: Rate which steering inputs will return to center. (0 disables return to center)");
	addField( "fuelFlow", TypeF32, Offset(mFuelFlow, WheeledVehicle),"@brief ");
	addField( "parkingBrake", TypeBool, Offset(inPark, WheeledVehicle),"@brief ");
	addField( "forceBrake", TypeBool, Offset(mForceBraking, WheeledVehicle),"@brief ");
	addField( "brakeTrigger", TypeS8, Offset(mBrakeTrigger, WheeledVehicle),"@brief Move manager trigger that brakes should respond to.");
	// addField( "autoTrans", TypeBool, Offset( useAutomatic, WheeledVehicle ),"@brief ");
	// addField( "rpm", TypeF32, Offset( mEngineRPM, WheeledVehicle ),"@brief ");    
}


//----------------------------------------------------------------------------

bool WheeledVehicle::onAdd()
{
   if(!Parent::onAdd())
      return false;

   addToScene();
   if (isServerObject())
      scriptOnAdd();
   return true;
}

void WheeledVehicle::onRemove()
{
   // Delete the wheel resources
   if (mDataBlock != NULL)  {
      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) {
         if (!wheel->emitter.isNull())
            wheel->emitter->deleteWhenEmpty();
         delete wheel->shapeInstance;
      }
   }

   // Stop the sounds
   SFX_DELETE( mJetSound );
   SFX_DELETE( mEngineSound );
   SFX_DELETE( mSquealSound );

   //
   scriptOnRemove();
   removeFromScene();
   Parent::onRemove();
}


//----------------------------------------------------------------------------

bool WheeledVehicle::onNewDataBlock(GameBaseData* dptr, bool reload)
{
   // Delete any existing wheel resources if we're switching
   // datablocks.
   if (mDataBlock) 
   {
      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
      {
         if (!wheel->emitter.isNull()) 
         {
            wheel->emitter->deleteWhenEmpty();
            wheel->emitter = 0;
         }
         delete wheel->shapeInstance;
         wheel->shapeInstance = 0;
      }
   }

   // Load up the new datablock
   mDataBlock = dynamic_cast<WheeledVehicleData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr,reload))
      return false;

   // Set inertial tensor, default for the vehicle is sphere
   if (mDataBlock->massBox.x > 0 && mDataBlock->massBox.y > 0 && mDataBlock->massBox.z > 0)
      mRigid.setObjectInertia(mDataBlock->massBox);
   else
      mRigid.setObjectInertia(mObjBox.maxExtents - mObjBox.minExtents);

   // Initialize the wheels...
   for (S32 i = 0; i < mDataBlock->wheelCount; i++) 
   {
      Wheel* wheel = &mWheel[i];
      wheel->data = &mDataBlock->wheel[i];
      wheel->tire = 0;
      wheel->spring = 0;

      wheel->surface.contact = false;
      wheel->surface.object  = NULL;
      wheel->avel = 0;
      wheel->apos = 0;
      wheel->extension = 1;
      wheel->slip = 0;

      wheel->springThread = 0;
      wheel->emitter = 0;

      // Steering on the front tires by default
      if (wheel->data->pos.y > 0)
         wheel->steering = 1;

      // Build wheel animation threads
      if (wheel->data->springSequence != -1) {
         wheel->springThread = mShapeInstance->addThread();
         mShapeInstance->setSequence(wheel->springThread,wheel->data->springSequence,0);
      }

      // Each wheel get's it's own particle emitter
      if( mDataBlock->tireEmitter && isGhost() )
      {
         wheel->emitter = new ParticleEmitter;
         wheel->emitter->onNewDataBlock( mDataBlock->tireEmitter, false );
         wheel->emitter->registerObject();
      }
   }

   // Steering sequence
   if (mDataBlock->steeringSequence != -1) {
      mSteeringThread = mShapeInstance->addThread();
      mShapeInstance->setSequence(mSteeringThread,mDataBlock->steeringSequence,0);
   }
   else
      mSteeringThread = 0;

   // Brake light sequence
   if (mDataBlock->brakeLightSequence != -1) {
      mTailLightThread = mShapeInstance->addThread();
      mShapeInstance->setSequence(mTailLightThread,mDataBlock->brakeLightSequence,0);
   }
   else
      mTailLightThread = 0;

   if (isGhost()) 
   {
      // Create the sounds ahead of time.  This reduces runtime
      // costs and makes the system easier to understand.

      SFX_DELETE( mEngineSound );
      SFX_DELETE( mSquealSound );
      SFX_DELETE( mJetSound );

      if ( mDataBlock->sound[WheeledVehicleData::EngineSound] )
         mEngineSound = SFX->createSource( mDataBlock->sound[WheeledVehicleData::EngineSound], &getTransform() );

      if ( mDataBlock->sound[WheeledVehicleData::SquealSound] )
         mSquealSound = SFX->createSource( mDataBlock->sound[WheeledVehicleData::SquealSound], &getTransform() );

      if ( mDataBlock->sound[WheeledVehicleData::JetSound] )
         mJetSound = SFX->createSource( mDataBlock->sound[WheeledVehicleData::JetSound], &getTransform() );
   }

   scriptOnNewDataBlock();
   return true;
}


//----------------------------------------------------------------------------

S32 WheeledVehicle::getWheelCount()
{
   // Return # of hubs defined on the car body
   return mDataBlock? mDataBlock->wheelCount: 0;
}

void WheeledVehicle::setWheelSteering(S32 wheel,F32 steering)
{
   AssertFatal(wheel >= 0 && wheel < WheeledVehicleData::MaxWheels,"Wheel index out of bounds");
   mWheel[wheel].steering = mClampF(steering,-1,1);
   setMaskBits(WheelMask);
}

void WheeledVehicle::setWheelPowered(S32 wheel,bool powered)
{
   AssertFatal(wheel >= 0 && wheel < WheeledVehicleData::MaxWheels,"Wheel index out of bounds");
   mWheel[wheel].powered = powered;
   setMaskBits(WheelMask);
}

void WheeledVehicle::setWheelTire(S32 wheel,WheeledVehicleTire* tire)
{
   AssertFatal(wheel >= 0 && wheel < WheeledVehicleData::MaxWheels,"Wheel index out of bounds");
   mWheel[wheel].tire = tire;
   setMaskBits(WheelMask);
}

void WheeledVehicle::setWheelSpring(S32 wheel,WheeledVehicleSpring* spring)
{
   AssertFatal(wheel >= 0 && wheel < WheeledVehicleData::MaxWheels,"Wheel index out of bounds");
   mWheel[wheel].spring = spring;
   setMaskBits(WheelMask);
}

void WheeledVehicle::getWheelInstAndTransform( U32 index, TSShapeInstance** inst, MatrixF* xfrm ) const
{
   AssertFatal( index < WheeledVehicleData::MaxWheels,
      "WheeledVehicle::getWheelInstAndTransform() - Bad wheel index!" );
   
   const Wheel* wheel = &mWheel[index];
   *inst = wheel->shapeInstance;
   
   if ( !xfrm || !wheel->shapeInstance )
      return;
   MatrixF world = getRenderTransform();
   world.scale( mObjScale );
   // Steering & spring extension
   MatrixF hub(EulerF(0,0,mSteering.x * wheel->steering));
   Point3F pos = wheel->data->pos;
   pos.z -= wheel->spring->length * wheel->extension;
   hub.setColumn(3,pos);
   world.mul(hub);

   // Wheel rotation
   MatrixF rot(EulerF(wheel->apos * M_2PI,0,0));
   world.mul(rot);

   // Rotation the tire to face the right direction
   // (could pre-calculate this)
   MatrixF wrot(EulerF(0,0,(wheel->data->pos.x > 0)? M_PI/2: -M_PI/2));
   world.mul(wrot);

   *xfrm = world;
}

// ################################################################################
// Advanced Vehicle utility functions
// ################################################################################

void WheeledVehicle::setEngine( WheeledVehicleEngine *engine ) {
	mEngine = engine;
	if(engine == NULL){		
		return;
	}
   // Copy in any values that are initialized from the datablock but can be overridden
   mFuelFlow = mEngine->fuelFlow;
   mUseAutomatic = mEngine->useAutomatic;
   
	setMaskBits(EngineMask);
}

WheeledVehicleEngine * WheeledVehicle::getEngine() {
	return mEngine;
}
//----------------------------------------------------------------------------
// WheeledVehicle::calcEngineRPM(gear, wRPM)
// This calculates the engine RPM based on the wheel RPM.  Mostly used when upshifting
// to ensure that the motor reacts to the new gear ratios.  This is also where the
// transmission slip is rolled in to calculate power loss.

F32 WheeledVehicle::calcEngineRPM(S32 gear, F32 wRPM){
   // Quick sanity check
   if (gear < -1 || gear > mEngine->numGearRatios) return(0.0f);

   F32 gR = 0;
   if (gear == 0 ) return(mEngine->idleEngineRPM);
   if (gear == -1) gR = mEngine->differentalRatio;
   if (gear > 0) gR = mEngine->gearRatios[gear-1];
   
   F32 g1 = wRPM * gR * mEngine->differentalRatio;
   // Roll in a little simulated power loss
   F32 final = g1 - mEngine->transmissionSlip;
   return(final);                  
}

//----------------------------------------------------------------------------
// WheeledVehicle::upShift - Goes up through all the gears until it reaches the
// higest gear defined.
// 
S32 WheeledVehicle::upShift() {
   if (mCurGear >= mEngine->numGearRatios) return(mEngine->numGearRatios);

   // This should jump up until the number hits the numGears   
   mCurGear++;   
   //Con::executef(mDataBlock,4,"onShift",this, Con::getIntArg(mCurGear), Con::getIntArg(mEngineRPM));
    mEngine->onShift_callback( this, mCurGear, mEngineRPM );
   if (mCurGear>1){
      // Upshift - adjust engine RPM for the new gear ratio.
      mEngineRPM = calcEngineRPM(mCurGear, mWheelRPM);
   }   
   return(mCurGear);

}

//----------------------------------------------------------------------------
// WheeledVehicle::downShift - Drops through all the gears until it reaches reverse
// 
S32 WheeledVehicle::downShift() {   
   if (mCurGear <= -1) return(-1);

   // This should just nail the gear down until it hits reverse
   mCurGear --;
   
   //Con::executef(mDataBlock,4,"onShift",scriptThis(), Con::getIntArg(mCurGear), Con::getIntArg(mEngineRPM));   
   mEngine->onShift_callback( this, mCurGear, mEngineRPM );
   return(mCurGear);
}

//----------------------------------------------------------------------------
// WheeledVehicle::setGear - used to set the gear for the current transmission
// bypassing the upshift/downshift routines.  This routine will return the starting
// gear for the transmission.  Then it executes the onShift script callback if the
//  gear actually changes.  
//
S32 WheeledVehicle::setGear( S32 nGear ) {
	
	S32 rGear = mCurGear;	

	if( nGear <= mEngine->numGearRatios && nGear >= -1 && mCurGear != nGear) {
	   mCurGear = nGear;
      if (mCurGear>1 && mCurGear>rGear){
         // Upshift - adjust engine RPM for the new gear ratio.
         mEngineRPM = calcEngineRPM(mCurGear, mWheelRPM);
      }   
      //Con::executef(mDataBlock,4,"onShift",scriptThis(), Con::getIntArg(mCurGear), Con::getIntArg(mEngineRPM));
	  mEngine->onShift_callback( this, mCurGear, mEngineRPM );
	}

	return rGear;
}

//----------------------------------------------------------------------------
S32 WheeledVehicle::getGear() {
	return mCurGear;
}

//----------------------------------------------------------------------------
// WheeledVehicle::getRPM() - Returns the current engine RPM

S32 WheeledVehicle::getRPM() {
	return mEngineRPM;
}

//----------------------------------------------------------------------------
// WheeledVehicle::getSpeed() - Returns the current vehicle velocity
// 
S32 WheeledVehicle::getSpeed() {
   return getVelocity().len();
}

//----------------------------------------------------------------------------
// WheeledVehicle::isAutomatic() - Returns the setting of the engine's default
// useAutomatic setting
//
bool WheeledVehicle::isAutomatic() {
   return mEngine->useAutomatic;
}


//----------------------------------------------------------------------------
// WheeledVehicle::setAutomatic() - Overrides the engine's default useAutomatic setting.
//
void WheeledVehicle::setAutomatic(bool isAuto) {
   mUseAutomatic = isAuto;
}

// ################################################################################
// End: Advanced Vehicle utility functions
// ################################################################################

void WheeledVehicle::processTick(const Move* move)
{
   Parent::processTick(move);
}

void WheeledVehicle::updateMove(const Move* move)
{
   Parent::updateMove(move);   

   // Brake on trigger
   mBraking = move->trigger[mBrakeTrigger];  // KGB - add flexibility for the move trigger.
   //mBraking = move->trigger[4];  // KGB - moved this from 2 to reduce jump conflict

   // Set the tail brake light thread direction based on the brake state.
   if (mTailLightThread)
      mShapeInstance->setTimeScale(mTailLightThread, mBraking? 1.0f : -1.0f);

	/*** KGB: ***/

   if(!mEngine) return;

   F32 fFlow = mFuelFlow; // Drop fuel flow into a temp variable - this is increased
                          // when in neutral to rev the motor quickly.

   // Check automatic transmission in neutral - transmission should always engage
   // when throttle is applied.
   if (mCurGear == 0 && mEngine->useAutomatic == true && mThrottle < 0) {            
      setGear(-1);      
	  //Con::warnf("Set reverse: %i",mCurGear);
   } else if (mCurGear == 0 && mEngine->useAutomatic == true && mThrottle > 0) {      
      setGear(1);
	  //Con::warnf("First Set: %i",mCurGear);
   }
   
   // Check manual transmission, in neutral, no throttle, engineRPM > idle.   
   // ** Addition:
   // Check automatic transmission, no throttle, no powered contact, engineRPM > idle
   // This makes the reved motor drop RPM when in neutral or upside down/high centered in automatic.
   if ( (mCurGear == 0 && mEngine->useAutomatic == false && mEngineRPM > mEngine->idleEngineRPM && mThrottle == 0) ||
      (mEngine->useAutomatic && mEngineRPM > mEngine->idleEngineRPM && mThrottle == 0 && mContPowered == false) )
   {
      F32 eDrag = (mEngineRPM * mEngine->engineDrag) * mEngine->neutralBoost * 10;
      mEngineRPM -= eDrag;
   }

   // Get the gear ratio for the current gear
   F32 gR=0;
   if(mCurGear == -1 )
      gR = mEngine->reverseRatio;
   else if(mCurGear == 0 ) {  
      if (mEngine->useAutomatic == false)
      {    
         // This makes the motor rev faster in neutral         
         fFlow = mFuelFlow * mEngine->neutralBoost;
      }
      // In neutral, use first gear ratio for base torque
      gR = mEngine->gearRatios[mCurGear]; 
      }
   else if(mCurGear > 0)
      gR = mEngine->gearRatios[mCurGear-1];

   // Find the torque for the current RPM level
   F32 rTq = mEngine->getRPMTorque(mEngineRPM) * gR;  // Less torqe at lower gear ratios   
   F32 tTq = rTq; // Temporary to see where we started for debugging
   
   // OK, this is here to avoid a "stall" condition in first gear (if desired)   
   if (mEngineRPM < mEngine->minEngineRPM) mEngineRPM = mEngine->minEngineRPM;

   // Simulate mass resistance - keeps the lower gear ratios from being used
   // to get acceleration at low speeds.  Note: should add a "bog-down" sound
   if (mCurGear != 0)
   {
      // Simulate start-up intertia      
      F32 tV = getVelocity().len() ;// Basic speed
      if (tV <= 1) tV = 1;          // Make sure start from zero velocity possible
      F32 mV = mMass / tV / gR;        // mass divided by velocity (gets smaller with speed)
      //mV /= gR;                     // mV divided by gear ratio (gets bigger with lower gear ratios)
      mV *= mEngine->inertiaFactor; // Variable to tweak the performance
      F32 aTq = rTq - mV;                    // Subtract the inertia value from calulated torque

      // Avoid a negative engine RPM setting
      if (aTq < 0 && mFabs(aTq) > mEngineRPM)
         rTq = -mEngineRPM;             
      else
         rTq = aTq;      
   }
   
   if (rTq > 0)  // Set the idle speed - unless the gear gives negative torque
   {          
      if (mEngineRPM < mEngine->idleEngineRPM) mEngineRPM = mEngine->idleEngineRPM;
   }

   // Check for over-rev situation from manual down-shift at high speeds   
   if (mEngineRPM > mEngine->maxEngineRPM)
   {      
      mSlowDown = mEngine->overRevSlowdown; // Reverse-trottle slow down (effects the wheels)
   } else{
      mSlowDown = 0.0f; // This is a value to allow reverse throttle to act as a slow brake.
   }

   // Now take the calulated engine torque and process the throttle input to determine
   // engine RPM change and reverse throttle slowdown.
   if (mEngine->useAutomatic == true) 
   {
      // ** Automatic Transmission throttle handling
      if (mThrottle<0){
         // Negative throttle input, 
         if (mCurGear == -1) {
            // We are going backward .... add to RPM
            mEngineRPM += rTq * fFlow;
         } else {
            // This is going forward ....set some braking
            mEngineRPM -= rTq * fFlow;
            mSlowDown += rTq * mEngine->slowDownRate;
            // When they reach 0.  Then switch into reverse.
            if (mWheelRPM <= 5){
               mEngineRPM = 500;
               mGearDelay = 8;
               setGear(-1);
            }
         }
      }else if (mThrottle > 0){
         if (mCurGear == -1) {
            // We are going backward .... bleed off some RPMs
            mEngineRPM -= rTq * fFlow;
            mSlowDown += rTq * mEngine->slowDownRate;
            // When they reach 0.  Then switch into forward.
            if (mWheelRPM <= 5){
               mEngineRPM = 500;
               mGearDelay = 8;
               setGear(0);               
            }
         } else {
            // Going forward ....add to the RPM
            mEngineRPM += rTq * fFlow;
         }      
      }
   }else {
      // ** Manual Transmission throttle handling
      if (mThrottle<0){
         // Negative throttle input - always brake the engine with manual
         mEngineRPM -= rTq * fFlow;
         mSlowDown += rTq * mEngine->slowDownRate;
      } else if (mThrottle > 0){
         // Positive throttle input - always accelerate the engine with manual
         mEngineRPM += rTq * fFlow;       
      }
   } // ** end Automatic / Manual transmission throttle handling
   

   // Make sure the routine does not send out more RPM than the vehicle will allow   
   mEngineRPM = mClampF( mEngineRPM, 0, mEngine->maxEngineRPM );

   // Handle automatic transmission shifting - based on engine RPM
   if( mEngine->useAutomatic ) {
	   if (mGearDelay>0){
			mGearDelay--;
			return;
	   }

         if (mCurGear > 0) {
			   int nGear = mCurGear;
			   if( mEngineRPM < mEngine->shiftDownRPM && mThrottle <= 0) {
				   nGear--;
				   //Con::warnf("Dn Shift: %i",nGear);
			   }
			   if( mEngineRPM > mEngine->shiftUpRPM && !mCurGear == 0 ) {
				   // Up Shift.
				   nGear++;
				   //Con::warnf("Up Shift: %i",nGear);
			   }

				if( nGear > mEngine->numGearRatios )
					nGear = mEngine->numGearRatios;
			   
			   if( mCurGear != nGear ) {
               // This actually shifted
               if (nGear>mCurGear)
               {
                  // Upshift - adjust engine RPM for the new gear ratio.
                  //mEngine->onShift_callback( this, mCurGear, mEngineRPM );
                  mEngineRPM = calcEngineRPM(nGear, mWheelRPM);
               }               
               mCurGear = nGear;               
               //Con::executef(mDataBlock,4,"onShift",scriptThis(), Con::getIntArg(mCurGear), Con::getIntArg(mEngineRPM));               
			   mEngine->onShift_callback( this, mCurGear, mEngineRPM );
				   mGearDelay = 8;
			   }
         } else if (mCurGear <0){
            if( mEngineRPM < mEngine->shiftDownRPM && mThrottle >= 0) {
               // Always shift up into neutral
				//Con::warnf("Up Shift(rev): %i",mCurGear);
               mCurGear = 0;
               //Con::executef(mDataBlock,4,"onShift",scriptThis(), Con::getIntArg(mCurGear), Con::getIntArg(mEngineRPM));
			   mEngine->onShift_callback( this, mCurGear, mEngineRPM );
               mGearDelay = 8;
            }
         }
      		
	}

/* KGB End*/


/***KGB: ***/
   // Steering return-to-center based on original resource by James Jacoby
   if (mCenterSteeringRate >0.0f)
   {
      F32 returnSpeed = mFabs( move->y * mCenterSteeringRate ); // based on forward/backward motion
      if( mSteering.x > 0.0f )
      {
         mSteering.x -= returnSpeed;
         mSteering.x < 0.0f ? 0.0f : mSteering.x;  // KGB: make sure we don't go past zero
      }else if( mSteering.x < 0.0f )
      {
         mSteering.x += returnSpeed;
         mSteering.x > 0.0f ? 0.0f : mSteering.x;  // KGB: make sure we don't go past zero
      }
   }
/* KGB End */
}


//----------------------------------------------------------------------------

void WheeledVehicle::advanceTime(F32 dt)
{
   PROFILE_SCOPE( WheeledVehicle_AdvanceTime );

   Parent::advanceTime(dt);

   // Stick the wheels to the ground.  This is purely so they look
   // good while the vehicle is being interpolated.
   extendWheels();

   // Update wheel angular position and slip, this is a client visual
   // feature only, it has no affect on the physics.
   F32 slipTotal = 0;
   F32 torqueTotal = 0;

   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++)
      if (wheel->tire && wheel->spring) {
         // Update angular position
         wheel->apos += (wheel->avel * dt) / M_2PI;
         wheel->apos -= mFloor(wheel->apos);
         if (wheel->apos < 0)
            wheel->apos = 1 - wheel->apos;

         // Keep track of largest slip
         slipTotal += wheel->slip;
         torqueTotal += wheel->torqueScale;
      }

   // Update the sounds based on wheel slip and torque output
   updateSquealSound(slipTotal / mDataBlock->wheelCount);
   updateEngineSound(sIdleEngineVolume + (1 - sIdleEngineVolume) *
      (1 - (torqueTotal / mDataBlock->wheelCount)));
   updateJetSound();

   updateWheelThreads();
   updateWheelParticles(dt);

   // Update the steering animation: sequence time 0 is full right,
   // and time 0.5 is straight ahead.
   if (mSteeringThread) {
      F32 t = (mSteering.x * mFabs(mSteering.x)) / mDataBlock->maxSteeringAngle;
      mShapeInstance->setPos(mSteeringThread,0.5 - t * 0.5);
   }

   // Animate the tail light. The direction of the thread is
   // set based on vehicle braking.
   if (mTailLightThread)
      mShapeInstance->advanceTime(dt,mTailLightThread);
}


//----------------------------------------------------------------------------
/** Update the rigid body forces on the vehicle
   This method calculates the forces acting on the body, including gravity,
   suspension & tire forces.
*/
void WheeledVehicle::updateForces(F32 dt)
{
   PROFILE_SCOPE( WheeledVehicle_UpdateForces );

   if(mEngine){
	   updateEngineForces(dt);
	   return;
   }
   extendWheels();

   F32 aMomentum = mMass / mDataBlock->wheelCount;

   // Get the current matrix and extact vectors
   MatrixF currMatrix;
   mRigid.getTransform(&currMatrix);

   Point3F bx,by,bz;
   currMatrix.getColumn(0,&bx);
   currMatrix.getColumn(1,&by);
   currMatrix.getColumn(2,&bz);

   // Steering angles from current steering wheel position
   F32 quadraticSteering = -(mSteering.x * mFabs(mSteering.x));
   F32 cosSteering,sinSteering;
   mSinCos(quadraticSteering, sinSteering, cosSteering);

   // Calculate Engine and brake torque values used later by in
   // wheel calculations.
   F32 engineTorque,brakeVel;
   if (mBraking) 
   {
      brakeVel = (mDataBlock->brakeTorque / aMomentum) * dt;
      engineTorque = 0;
   }
   else 
   {
      if (mThrottle) 
      {
         engineTorque = mDataBlock->engineTorque * mThrottle;
         brakeVel = 0;
         // Double the engineTorque to help out the jets
         if (mThrottle > 0 && mJetting)
            engineTorque *= 2;
      }
      else 
      {
         // Engine brake.
         brakeVel = (mDataBlock->engineBrake / aMomentum) * dt;
         engineTorque = 0;
      }
   }

   // Integrate forces, we'll do this ourselves here instead of
   // relying on the rigid class which does it during movement.
   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   mRigid.force.set(0, 0, 0);
   mRigid.torque.set(0, 0, 0);

   // Calculate vertical load for friction.  Divide up the spring
   // forces across all the wheels that are in contact with
   // the ground.
   U32 contactCount = 0;
   F32 verticalLoad = 0;
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      if (wheel->tire && wheel->spring && wheel->surface.contact) 
      {
         verticalLoad += wheel->spring->force * (1 - wheel->extension);
         contactCount++;
      }
   }
   if (contactCount)
      verticalLoad /= contactCount;

   // Sum up spring and wheel torque forces
   for (Wheel* wheel = mWheel; wheel < wend; wheel++)  
   {
      if (!wheel->tire || !wheel->spring)
         continue;

      F32 Fy = 0;
      if (wheel->surface.contact) 
      {

         // First, let's compute the wheel's position, and worldspace velocity
         Point3F pos, r, localVel;
         currMatrix.mulP(wheel->data->pos, &pos);
         mRigid.getOriginVector(pos,&r);
         mRigid.getVelocity(r, &localVel);

         // Spring force & damping
         F32 spring  = wheel->spring->force * (1 - wheel->extension);

         if (wheel->extension == 0) //spring fully compressed
         {
            // Apply impulses to the rigid body to keep it from
            // penetrating the surface.
            F32 n = -mDot(localVel,Point3F(0,0,1));
            if (n >= 0)
            {
               // Collision impulse, straight forward force stuff.
               F32 d = mRigid.getZeroImpulse(r,Point3F(0,0,1));
               F32 j = n * (1 + mRigid.restitution) * d;
               mRigid.force += Point3F(0,0,1) * j;
            }
         }

         F32 damping = wheel->spring->damping * -(mDot(bz, localVel) / wheel->spring->length);
         if (damping < 0)
            damping = 0;

         // Anti-sway force based on difference in suspension extension
         F32 antiSway = 0;
         if (wheel->data->opposite != -1)
         {
            Wheel* oppositeWheel = &mWheel[wheel->data->opposite];
            if (oppositeWheel->surface.contact)
               antiSway = ((oppositeWheel->extension - wheel->extension) *
                  wheel->spring->antiSway);
            if (antiSway < 0)
               antiSway = 0;
         }

         // Spring forces act straight up and are applied at the
         // spring's root position.
         Point3F t, forceVector = bz * (spring + damping + antiSway);
         mCross(r, forceVector, &t);
         mRigid.torque += t;
         mRigid.force += forceVector;

         // Tire direction vectors perpendicular to surface normal
         Point3F wheelXVec = bx * cosSteering;
         wheelXVec += by * sinSteering * wheel->steering;
         Point3F tireX, tireY;
         mCross(wheel->surface.normal, wheelXVec, &tireY);
         tireY.normalize();
         mCross(tireY, wheel->surface.normal, &tireX);
         tireX.normalize();

         // Velocity of tire at the surface contact
         Point3F wheelContact, wheelVelocity;
         mRigid.getOriginVector(wheel->surface.pos,&wheelContact);
         mRigid.getVelocity(wheelContact, &wheelVelocity);

         F32 xVelocity = mDot(tireX, wheelVelocity);
         F32 yVelocity = mDot(tireY, wheelVelocity);

         // Tires act as springs and generate lateral and longitudinal
         // forces to move the vehicle. These distortion/spring forces
         // are what convert wheel angular velocity into forces that
         // act on the rigid body.

         // Longitudinal tire deformation force
         F32 ddy = (wheel->avel * wheel->tire->radius - yVelocity) -
            wheel->tire->longitudinalRelaxation *
            mFabs(wheel->avel) * wheel->Dy;
         wheel->Dy += ddy * dt;
         Fy = (wheel->tire->longitudinalForce * wheel->Dy +
            wheel->tire->longitudinalDamping * ddy);

         // Lateral tire deformation force
         F32 ddx = xVelocity - wheel->tire->lateralRelaxation *
            mFabs(wheel->avel) * wheel->Dx;
         wheel->Dx += ddx * dt;
         F32 Fx = -(wheel->tire->lateralForce * wheel->Dx +
            wheel->tire->lateralDamping * ddx);

         // Vertical load on the tire
         verticalLoad = spring + damping + antiSway;
         if (verticalLoad < 0)
            verticalLoad = 0;

         // Adjust tire forces based on friction
         F32 surfaceFriction = 1;
         F32 mu = surfaceFriction * (wheel->slipping ? wheel->tire->kineticFriction : wheel->tire->staticFriction);
         F32 Fn = verticalLoad * mu; Fn *= Fn;
         F32 Fw = Fx * Fx + Fy * Fy;
         if (Fw > Fn) 
         {
            F32 K = mSqrt(Fn / Fw);
            Fy *= K;
            Fx *= K;
            wheel->Dy *= K;
            wheel->Dx *= K;
            wheel->slip = 1 - K;
            wheel->slipping = true;
         }
         else 
         {
            wheel->slipping = false;
            wheel->slip = 0;
         }

         // Tire forces act through the tire direction vectors parallel
         // to the surface and are applied at the wheel hub.
         forceVector = (tireX * Fx) + (tireY * Fy);
         pos -= bz * (wheel->spring->length * wheel->extension);
         mRigid.getOriginVector(pos,&r);
         mCross(r, forceVector, &t);
         mRigid.torque += t;
         mRigid.force += forceVector;
      }
      else 
      {
         // Wheel not in contact with the ground
         wheel->torqueScale  = 0;
         wheel->slip = 0;

         // Relax the tire deformation
         wheel->Dy += (-wheel->tire->longitudinalRelaxation *
                       mFabs(wheel->avel) * wheel->Dy) * dt;
         wheel->Dx += (-wheel->tire->lateralRelaxation *
                       mFabs(wheel->avel) * wheel->Dx) * dt;
      }

      // Adjust the wheel's angular velocity based on engine torque
      // and tire deformation forces.
      if (wheel->powered) 
      {
         F32 maxAvel = mDataBlock->maxWheelSpeed / wheel->tire->radius;
         wheel->torqueScale = (mFabs(wheel->avel) > maxAvel) ? 0 :
            1 - (mFabs(wheel->avel) / maxAvel);
      }
      else
         wheel->torqueScale = 0;
      wheel->avel += (((wheel->torqueScale * engineTorque) - Fy *
         wheel->tire->radius) / aMomentum) * dt;

      // Adjust the wheel's angular velocity based on brake torque.
      // This is done after avel update to make sure we come to a
      // complete stop.
      if (brakeVel > mFabs(wheel->avel))
         wheel->avel = 0;
      else
         if (wheel->avel > 0)
            wheel->avel -= brakeVel;
         else
            wheel->avel += brakeVel;
   }

   // Jet Force
   if (mJetting)
      mRigid.force += by * mDataBlock->jetForce;

   // Add in force from physical zones...
   mRigid.force += mAppliedForce;

   // Container drag & buoyancy
   mRigid.force  += Point3F(0, 0, -mBuoyancy * sWheeledVehicleGravity * mRigid.mass);
   mRigid.force  -= mRigid.linVelocity * mDrag;
   mRigid.torque -= mRigid.angMomentum * mDrag;

   // If we've added anything other than gravity, then we're no
   // longer at rest. Could test this a little more efficiently...
   if (mRigid.atRest && (mRigid.force.len() || mRigid.torque.len()))
      mRigid.atRest = false;

   // Gravity
   mRigid.force += Point3F(0, 0, sWheeledVehicleGravity * mRigid.mass);

   // Integrate and update velocity
   mRigid.linMomentum += mRigid.force * dt;
   mRigid.angMomentum += mRigid.torque * dt;
   mRigid.updateVelocity();

   // Since we've already done all the work, just need to clear this out.
   mRigid.force.set(0, 0, 0);
   mRigid.torque.set(0, 0, 0);

   // If we're still atRest, make sure we're not accumulating anything
   if (mRigid.atRest)
      mRigid.setAtRest();
}

void WheeledVehicle::updateEngineForces(F32 dt)
{
   PROFILE_SCOPE( WheeledVehicle_UpdateEngineForces );

   extendWheels();

   F32 aMomentum = mMass / mDataBlock->wheelCount;

   // Get the current matrix and extact vectors
   MatrixF currMatrix;
   mRigid.getTransform(&currMatrix);

   Point3F bx,by,bz;
   currMatrix.getColumn(0,&bx);
   currMatrix.getColumn(1,&by);
   currMatrix.getColumn(2,&bz);

   // Steering angles from current steering wheel position
   F32 quadraticSteering = -(mSteering.x * mFabs(mSteering.x));
   F32 cosSteering,sinSteering;
   mSinCos(quadraticSteering, sinSteering, cosSteering);

   // Calculate Engine and brake torque values used later in
   // wheel calculations.
   F32 brakeVel;

/*** KGB */   
   if( mBraking || mForceBraking ) {
	   brakeVel = (mDataBlock->brakeTorque / aMomentum) * dt;      
   } else {
	   brakeVel = 0;
   }      
   if( inPark ) {
	   brakeVel += ((2.0f*mDataBlock->brakeTorque)/aMomentum)*dt;      
   }   

   // Instead of one monster equation, I broke this out so it made more sense   
   F32 gR;     // Gear ratio for current gear
   F32 tRPM;   // RPM from transmission
   F32 dRPM;   // Transmission and differental combo
   F32 rMass;  // Resistance = mass/(speed * gear ratio)
   F32 wRPM;   // Final wheel RPM
   if (mCurGear == -1){
      gR = mEngine->reverseRatio;
   } else if (mCurGear == 0){
      gR = mEngine->gearRatios[mCurGear];
   }else{
      gR = mEngine->gearRatios[mCurGear-1];
   }   
   tRPM = mEngineRPM / gR;  // This is the ratio of Engine to transmission
   dRPM = tRPM / mEngine->differentalRatio; // This is the ratio of Transmission to differental
   rMass = mMass / (getVelocity().len() + tRPM);	// TODO: Should be dRPM?
   if ( rMass > dRPM){
      wRPM = 0.0f;
   }else{
      wRPM = dRPM - rMass;
   }
   if( inPark ) {
	   wRPM = 0.0f;
   }
/*** KGB End*/
   /*
   F32 engineTorque,brakeVel;
   if (mBraking) 
   {
      brakeVel = (mDataBlock->brakeTorque / aMomentum) * dt;
      engineTorque = 0;
   }
   else 
   {
      if (mThrottle) 
      {
         engineTorque = mDataBlock->engineTorque * mThrottle;
         brakeVel = 0;
         // Double the engineTorque to help out the jets
         if (mThrottle > 0 && mJetting)
            engineTorque *= 2;
      }
      else 
      {
         // Engine brake.
         brakeVel = (mDataBlock->engineBrake / aMomentum) * dt;
         engineTorque = 0;
      }
   }
*/

   // Integrate forces, we'll do this ourselves here instead of
   // relying on the rigid class which does it during movement.
   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   mRigid.force.set(0, 0, 0);
   mRigid.torque.set(0, 0, 0);

   // Calculate vertical load for friction.  Divide up the spring
   // forces across all the wheels that are in contact with
   // the ground.
   U32 contactCount = 0;
   F32 verticalLoad = 0;

   mContPowered = false;  // KGB: To track if a powered wheel has contact

   F32 wR=0; // Wheel radius dammit
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      if (wheel->tire){
		  wR = wheel->tire->radius;
		  if(wheel->spring && wheel->surface.contact) 
			{
	         verticalLoad += wheel->spring->force * (1 - wheel->extension);
		     contactCount++;
			 if (wheel->powered) mContPowered = true; // KGB: for collision stuff
		  }
      }
   }
   if (contactCount)
      verticalLoad /= contactCount;
   
   /***KGB: Stuff I need both in the loop at at the bottom */
   F32 tAvel = 0.0f;      
   // One round-about way to translate wheel RPM into distance traveled
   F32 wC = M_2PI * wR; // Wheel Circumference
   F32 wD = wC * wRPM;  // This is the distance traveled in a minute
   F32 wV = wD / 60.0f;

   // Sum up spring and wheel torque forces
   for (Wheel* wheel = mWheel; wheel < wend; wheel++)  
   {
      if (!wheel->tire || !wheel->spring)
         continue;

      F32 Fy = 0;
      if (wheel->surface.contact) 
      {

         // First, let's compute the wheel's position, and worldspace velocity
         Point3F pos, r, localVel;
         currMatrix.mulP(wheel->data->pos, &pos);
         mRigid.getOriginVector(pos,&r);
         mRigid.getVelocity(r, &localVel);

         // Spring force & damping
         F32 spring  = wheel->spring->force * (1 - wheel->extension);

         if (wheel->extension == 0) //spring fully compressed
         {
            // Apply impulses to the rigid body to keep it from
            // penetrating the surface.
            F32 n = -mDot(localVel,Point3F(0,0,1));
            if (n >= 0)
            {
               // Collision impulse, straight forward force stuff.
               F32 d = mRigid.getZeroImpulse(r,Point3F(0,0,1));
               F32 j = n * (1 + mRigid.restitution) * d;
               mRigid.force += Point3F(0,0,1) * j;
            }
         }

         F32 damping = wheel->spring->damping * -(mDot(bz, localVel) / wheel->spring->length);
         if (damping < 0)
            damping = 0;

         // Anti-sway force based on difference in suspension extension
         F32 antiSway = 0;
         if (wheel->data->opposite != -1)
         {
            Wheel* oppositeWheel = &mWheel[wheel->data->opposite];
            if (oppositeWheel->surface.contact)
               antiSway = ((oppositeWheel->extension - wheel->extension) *
                  wheel->spring->antiSway);
            if (antiSway < 0)
               antiSway = 0;
         }

         // Spring forces act straight up and are applied at the
         // spring's root position.
         Point3F t, forceVector = bz * (spring + damping + antiSway);
         mCross(r, forceVector, &t);
         mRigid.torque += t;
         mRigid.force += forceVector;

         // Tire direction vectors perpendicular to surface normal
         Point3F wheelXVec = bx * cosSteering;
         wheelXVec += by * sinSteering * wheel->steering;
         Point3F tireX, tireY;
         mCross(wheel->surface.normal, wheelXVec, &tireY);
         tireY.normalize();
         mCross(tireY, wheel->surface.normal, &tireX);
         tireX.normalize();

         // Velocity of tire at the surface contact
         Point3F wheelContact, wheelVelocity;
         mRigid.getOriginVector(wheel->surface.pos,&wheelContact);
         mRigid.getVelocity(wheelContact, &wheelVelocity);

         F32 xVelocity = mDot(tireX, wheelVelocity);
         F32 yVelocity = mDot(tireY, wheelVelocity);

         // Tires act as springs and generate lateral and longitudinal
         // forces to move the vehicle. These distortion/spring forces
         // are what convert wheel angular velocity into forces that
         // act on the rigid body.

         // Longitudinal tire deformation force
         F32 ddy = (wheel->avel * wheel->tire->radius - yVelocity) -
            wheel->tire->longitudinalRelaxation *
            mFabs(wheel->avel) * wheel->Dy;
         wheel->Dy += ddy * dt;
         Fy = (wheel->tire->longitudinalForce * wheel->Dy +
            wheel->tire->longitudinalDamping * ddy);

         // Lateral tire deformation force
         F32 ddx = xVelocity - wheel->tire->lateralRelaxation *
            mFabs(wheel->avel) * wheel->Dx;
         wheel->Dx += ddx * dt;
         F32 Fx = -(wheel->tire->lateralForce * wheel->Dx +
            wheel->tire->lateralDamping * ddx);

         // Vertical load on the tire
         verticalLoad = spring + damping + antiSway;
         if (verticalLoad < 0)
            verticalLoad = 0;

         // Adjust tire forces based on friction
         F32 surfaceFriction = 1;
         F32 mu = surfaceFriction * (wheel->slipping ? wheel->tire->kineticFriction : wheel->tire->staticFriction);
         F32 Fn = verticalLoad * mu; Fn *= Fn;
         F32 Fw = Fx * Fx + Fy * Fy;
         if (Fw > Fn) 
         {
            F32 K = mSqrt(Fn / Fw);
            Fy *= K;
            Fx *= K;
            wheel->Dy *= K;
            wheel->Dx *= K;
            wheel->slip = 1 - K;
            wheel->slipping = true;
         }
         else 
         {
            wheel->slipping = false;
            wheel->slip = 0;
         }

         // Tire forces act through the tire direction vectors parallel
         // to the surface and are applied at the wheel hub.
         forceVector = (tireX * Fx) + (tireY * Fy);
         pos -= bz * (wheel->spring->length * wheel->extension);
         mRigid.getOriginVector(pos,&r);
         mCross(r, forceVector, &t);
         mRigid.torque += t;
         mRigid.force += forceVector;
      }
      else 
      {
         // Wheel not in contact with the ground
         wheel->torqueScale  = 0;
         wheel->slip = 0;

         // Relax the tire deformation
         wheel->Dy += (-wheel->tire->longitudinalRelaxation *
                       mFabs(wheel->avel) * wheel->Dy) * dt;
         wheel->Dx += (-wheel->tire->lateralRelaxation *
                       mFabs(wheel->avel) * wheel->Dx) * dt;
      }

      // Adjust the wheel's angular velocity based on engine torque
      // and tire deformation forces.

      F32 resistVel = 0.0f;
	   if( wheel->surface.contact ) {
		  resistVel = mFabs(wheel->avel) * wheel->tire->rollingResistance * dt;
	   }

/*** KGB */      
      F32 eDrag = 0.0f;
      if (wheel->powered && mCurGear != 0){         
         // The wheel RPMs should have increased the velocity of the vehicle
         // If the vehicle is roughly stopped, we should stop the wheel.

         if (mWheelRPM > 500 && getVelocity().len() < 7 && mCurGear != 0){
            // The wheels are moving, but the vehicle is not, stop the wheels?            
            F32 nVel = wV - (mEngine->overRevSlowdown *2);
            if (mCurGear == -1){
               wheel->avel = -nVel;
            }else{
               wheel->avel = nVel;                  
            }
            mEngineRPM = mEngine->minEngineRPM;
            mThrottle = 0;
            //mEngineRPM = mEngine->idleEngineRPM;
            // setGear(0);
         } else {


            if(mThrottle!=0 ||  mCurGear == 1 || mCurGear == -1)
            //if(mThrottle!=0)
            {
               // This is the powered wheel velocity, if the wheels are already going 
               // faster, don't slow them down
               if (wV > mFabs(wheel->avel)){
                  if (mCurGear == -1){
                     wheel->avel = -wV;
                  }else{
                     wheel->avel = wV;                  
                  }
               }else{
                  eDrag = mEngineRPM * mEngine->engineDrag * dt;
               }
            } else {
               // The throttle is not on, calculate the engine drag
                  eDrag = mEngineRPM * mEngine->engineDrag * dt;
            }
         }
      }

      // Check speed, and copy in the lateral and deformation forces
      if ( (getVelocity().len() < .5 && mThrottle == 0 && (mEngine->useAutomatic == true || mCurGear > 1)) || ((wheel->avel <0 && mCurGear >0) || (wheel->avel >0 && mCurGear <0)) ) {
         // Stop if movement gets insanely slow
         wheel->avel  = 0;
      }else{         
         wheel->avel += (( - Fy * wheel->tire->radius) / aMomentum) * dt ;
      }
   
      // Now we've increased wheel RPMs and converted to velocity if the engine
      // has provided extra power.  Next we need to subtract out any drag or 
      // braking from the wheel velocity.

      // Adjust the wheel's angular velocity based on brake torque.
      // This is done after avel update to make sure we come to a
      // complete stop.
      F32 lbrakeVel = mFabs(brakeVel + resistVel + eDrag + mSlowDown);            
	   if (lbrakeVel > mFabs(wheel->avel))
		   wheel->avel = 0;
	   else
		   if (wheel->avel > 0)
			   wheel->avel -= lbrakeVel;
	      else
		      wheel->avel += lbrakeVel;

      // Keep track of the greatest angular wheel velocity
      if (mFabs(wheel->avel) > mFabs(tAvel) )
         tAvel = wheel->avel;


   }
   mWheelVel = tAvel;   // Highest wheel angular velocity - use to check forward/backward 
                        // wheel direction, regardless of gear selection

   // At this stage, the rigid body has been updated with last tick's forces and should
   // roughly match the wheel RPMs until that value is updated.


   // Take the adjusted aVel and calculate the wheel RPM going into the next
   // tick to use in the calculation of RPM drop on shift.
   F32 d1 = mFabs(mWheelVel) * 60;   // velocity traveled in 60 seconds
   mWheelRPM = (d1 / wC)>=1?d1/wC:0;   // divided by the circumference of the wheel
   
   // Finally, check the wheel RPMs and slow down the motor RPMs to match
   // if the throttle is not on - or if the engine is over-revved
   bool fT = false; // Forward Throttle Check
   if ( (mEngine->useAutomatic == true &&(mThrottle < 0 && mCurGear < 0) || (mThrottle > 0 && mCurGear > 0))
            ||  (mEngine->useAutomatic == false && mThrottle >0) )
   {
      fT = true;
   }
   
   F32 neRPM = mWheelRPM * gR * mEngine->differentalRatio;  // Calculated new engine RPM
   if ((!fT && mContPowered && mWheelRPM > 0 && !mCurGear == 0) || neRPM > mEngine->maxEngineRPM ){
      mEngineRPM = neRPM;
   }



/*** KGB End   */


   // Jet Force
   if (mJetting)
      mRigid.force += by * mDataBlock->jetForce;

   // Add in force from physical zones...
   mRigid.force += mAppliedForce;

   // Container drag & buoyancy
   mRigid.force  += Point3F(0, 0, -mBuoyancy * sWheeledVehicleGravity * mRigid.mass);
   mRigid.force  -= mRigid.linVelocity * mDrag;
   mRigid.torque -= mRigid.angMomentum * mDrag;

   // If we've added anything other than gravity, then we're no
   // longer at rest. Could test this a little more efficiently...
   if (mRigid.atRest && (mRigid.force.len() || mRigid.torque.len()))
      mRigid.atRest = false;

   // Gravity
   mRigid.force += Point3F(0, 0, sWheeledVehicleGravity * mRigid.mass);

   // Integrate and update velocity
   mRigid.linMomentum += mRigid.force * dt;
   mRigid.angMomentum += mRigid.torque * dt;
   mRigid.updateVelocity();

   // Since we've already done all the work, just need to clear this out.
   mRigid.force.set(0, 0, 0);
   mRigid.torque.set(0, 0, 0);

   // If we're still atRest, make sure we're not accumulating anything
   if (mRigid.atRest)
      mRigid.setAtRest();
}

//----------------------------------------------------------------------------
/** Extend the wheels
   The wheels are extended until they contact a surface. The extension
   is instantaneous.  The wheels are extended before force calculations and
   also on during client side interpolation (so that the wheels are glued
   to the ground).
*/
void WheeledVehicle::extendWheels(bool clientHack)
{
   PROFILE_SCOPE( WheeledVehicle_ExtendWheels );

   disableCollision();

   MatrixF currMatrix;
   
   if(clientHack)
      currMatrix = getRenderTransform();
   else
      mRigid.getTransform(&currMatrix);
   

   // Does a single ray cast down for now... this will have to be
   // changed to something a little more complicated to avoid getting
   // stuck in cracks.
   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      if (wheel->tire && wheel->spring) 
      {
         wheel->extension = 1;

         // The ray is cast from the spring mount point to the tip of
         // the tire.  If there is a collision the spring extension is
         // adjust to remove the tire radius.
         Point3F sp,vec;
         currMatrix.mulP(wheel->data->pos,&sp);
         currMatrix.mulV(VectorF(0,0,-wheel->spring->length),&vec);
         F32 ts = wheel->tire->radius / wheel->spring->length;
         Point3F ep = sp + (vec * (1 + ts));
         ts = ts / (1+ts);

         RayInfo rInfo;
         if (mContainer->castRay(sp, ep, sClientCollisionMask & ~PlayerObjectType, &rInfo)) 
         {
            wheel->surface.contact  = true;
            wheel->extension = (rInfo.t < ts)? 0: (rInfo.t - ts) / (1 - ts);
            wheel->surface.normal   = rInfo.normal;
            wheel->surface.pos      = rInfo.point;
            wheel->surface.material = rInfo.material;
            wheel->surface.object   = rInfo.object;
         }
         else 
         {
            wheel->surface.contact = false;
            wheel->slipping = true;
         }
      }
   }
   enableCollision();
}


//----------------------------------------------------------------------------
/** Update wheel steering and suspension threads.
   These animations are purely cosmetic and this method is only invoked
   on the client.
*/
void WheeledVehicle::updateWheelThreads()
{
   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      if (wheel->tire && wheel->spring && wheel->springThread) 
      {
         // Scale the spring animation time to match the current
         // position of the wheel.  We'll also check to make sure
         // the animation is long enough, if it isn't, just stick
         // it at the end.
         F32 pos = wheel->extension * wheel->spring->length;
         if (pos > wheel->data->springLength)
            pos = 1;
         else
            pos /= wheel->data->springLength;
         mShapeInstance->setPos(wheel->springThread,pos);
      }
   }
}

//----------------------------------------------------------------------------
/** Update wheel particles effects
   These animations are purely cosmetic and this method is only invoked
   on the client.  Particles are emitted as long as the moving.
*/
void WheeledVehicle::updateWheelParticles(F32 dt)
{
   // OMG l33t hax
   extendWheels(true);
   
   Point3F vel = Parent::getVelocity();
   F32 speed = vel.len();

   // Don't bother if we're not moving.
   if (speed > 1.0f)  
   {
      Point3F axis = vel;
      axis.normalize();

      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
      {
         // Is this wheel in contact with the ground?
         if (wheel->tire && wheel->spring && !wheel->emitter.isNull() &&
               wheel->surface.contact && wheel->surface.object )
         {
            Material* material = ( wheel->surface.material ? dynamic_cast< Material* >( wheel->surface.material->getMaterial() ) : 0 );

            if( material)//&& material->mShowDust )
            {
               ColorF colorList[ ParticleData::PDC_NUM_KEYS ];

               for( U32 x = 0; x < getMin( Material::NUM_EFFECT_COLOR_STAGES, ParticleData::PDC_NUM_KEYS ); ++ x )
                  colorList[ x ] = material->mEffectColor[ x ];
               for( U32 x = Material::NUM_EFFECT_COLOR_STAGES; x < ParticleData::PDC_NUM_KEYS; ++ x )
                  colorList[ x ].set( 1.0, 1.0, 1.0, 0.0 );

               wheel->emitter->setColors( colorList );

               // Emit the dust, the density (time) is scaled by the
               // the vehicles velocity.
               //wheel->emitter->emitParticles( wheel->surface.pos, true,
               //   axis, vel, (U32)(3/*dt * (speed / mDataBlock->maxWheelSpeed) * 1000 * wheel->slip*/));

               // **KGB Changed this up - got rid of "Max Wheel Speed" and used vehicle speed 
               // **    mulitplied by a dust volume variable.  Extra dust for slip is then
               // **    ADDED. Also Set the dust particles to generate a bit off the ground
               // **    (this could be made better by setting the height of dust based on
               // **    wheel radius instead of a hard-code .5).

               U32 dVol;            
               dVol = speed * mDataBlock->dustVolume;
               if (wheel->slip>0){
                  // Increase the dust volume if we are slipping
                  dVol += (wheel->slip * 20);
               }
               Point3F wp = wheel->surface.pos;
               wp.z += .5;            
               wheel->emitter->emitParticles(wp, true, axis, vel, dVol);
            } 
         }
      }
   }
}



//#include "gfx/gfxDrawUtil.h"
//#include "gfx/primBuilder.h"
//#include "gfx/bitmap/gBitmap.h"
#include "gui/3d/guiTSControl.h"
#include "T3D/gameFunctions.h"
//#include "terrain/terrData.h"

//----------------------------------------------------------------------------
/** Update engine sound
   This method is only invoked by clients.
*/
void WheeledVehicle::updateEngineSound(F32 level = 0)
{
   if ( !mEngineSound )
      return;

   if ( !mEngineSound->isPlaying() )
      mEngineSound->play();

	if(!mEngine){
		mEngineSound->setTransform( getTransform() );
		mEngineSound->setVelocity( getVelocity() );
		//mEngineSound->setVolume( level );

		// Adjust pitch
		F32 pitch = ((level-sIdleEngineVolume) * 1.3f);
		if (pitch < 0.4f)  
			pitch = 0.4f;

		mEngineSound->setPitch( pitch );
		return;
	}

      // **KGB: Updated this to work with the transmission better.  Figures a percentage
      // **     of maxEngineRPM based on current engineRPM.  Then range is calculated for
      // **     pitch and volume based on defined minimum and maximum values.  This range
      // **     is multiplied by the percent of RPM to calculate correct pitch and volume
      
      // This scales from 0 - technically, the stall(min) RPM is slightly higher than 0      
      // ** TODO: Calculate this once when the vehicle is created (and edited)
      F32 rScale = (mEngineRPM / mEngine->maxEngineRPM); // Scalar for current RPM level

      // Volume calculations
      F32 vRange = mEngine->maxEngineVolume - mEngine->minEngineVolume;  // Low Volume to high volume level range
      F32 vol = mEngine->minEngineVolume + (vRange * rScale); // Volume increase from minimum by scaled amount
      vol < mEngine->minEngineVolume ? mEngine->minEngineVolume : vol;  // Keep above the minimum if RPMs go wacky
      // pitch calculations
      F32 pRange = mEngine->maxEnginePitch - mEngine->minEnginePitch;  // Low to high pitch level range
      F32 pitch = mEngine->minEnginePitch + (pRange * rScale);         // Pitch increase from minimum by scaled amount
      pitch < mEngine->minEnginePitch ? mEngine->minEnginePitch : pitch;  // Keep above the minimum if RPMs go wacky
      
      // **KGB: If the player mounted to the vehicle, set the sound position to
      // **     the camera so that sound is consistent when players are mounted
      // **     to the vehicle for 1st and 3rd person views.
      // **     <<There is probably a cleaner way to do this!>>
      //


      GameConnection *con = GameConnection::getConnectionToServer();      
      GameBase *co = NULL;
      if(con && ( (co = con->getControlObject()) != NULL) )
      {
         if(co == this || co->getObjectMount() == this)
         {
            // **TODO: Move camera sound matrix to interpolateTick and use for
            //         skidSound as well.
            CameraQuery camera;
            GameProcessCameraQuery(&camera);
            // If the source isn't backed off from the camera a bit,
            // the sound does not react to the vol setting and is LOUD!
            MatrixF mat = camera.cameraMatrix;
/*
			Point3F p;
            mat.getColumn(3,&p);            
            p.z -= 0.5f;
            mat.setColumn(3,p);
			Point3F p2 = getPosition();
            // Set the sound position
            //alxSourceMatrixF(mEngineSound, &mat);
*/
			mEngineSound->setTransform( mat );
            //vol = vol * 0.25f;   // Even backed off, the sound is too lound - tone it down a bit.            
         } else {            
            //alxSourceMatrixF(mEngineSound, &getTransform());
			mEngineSound->setTransform( getTransform() );
         }
      } else {            
            //alxSourceMatrixF(mEngineSound, &getTransform());
            mEngineSound->setTransform( getTransform() );
      }


      // assign the sound values to the audio object
      //alxSourcef(mEngineSound, AL_GAIN_LINEAR, vol);
      mEngineSound->setVolume( vol );
      //alxSourcef(mEngineSound, AL_PITCH, pitch);
      mEngineSound->setPitch( pitch );

//   mEngineSound->setTransform( getTransform() );
   mEngineSound->setVelocity( getVelocity() );

   ////mEngineSound->setVolume( level );
   // Adjust pitch
   //F32 pitch = ((level-sIdleEngineVolume) * 1.3f);
   //if (pitch < 0.4f)  
   //      pitch = 0.4f;
   //mEngineSound->setPitch( pitch );
}


//----------------------------------------------------------------------------
/** Update wheel skid sound
   This method is only invoked by clients.
*/
void WheeledVehicle::updateSquealSound(F32 level)
{
   if ( !mSquealSound )
      return;

   if ( level < sMinSquealVolume ) 
   {
      mSquealSound->stop();
      return;
   }

   if ( !mSquealSound->isPlaying() )
      mSquealSound->play();

   mSquealSound->setTransform( getTransform() );
   mSquealSound->setVolume( level );
}


//----------------------------------------------------------------------------
/** Update jet sound
   This method is only invoked by clients.
*/
void WheeledVehicle::updateJetSound()
{
   if ( !mJetSound )
      return;

   if ( !mJetting ) 
   {
      mJetSound->stop();
      return;
   }

   if ( !mJetSound->isPlaying() )
      mJetSound->play();

   mJetSound->setTransform( getTransform() );
}


//----------------------------------------------------------------------------

U32 WheeledVehicle::getCollisionMask()
{
   return sClientCollisionMask;
}


//----------------------------------------------------------------------------
/** Build a collision polylist
   The polylist is filled with polygons representing the collision volume
   and the wheels.
*/
bool WheeledVehicle::buildPolyList(PolyListContext context, AbstractPolyList* polyList, const Box3F& box, const SphereF& sphere)
{
   PROFILE_SCOPE( WheeledVehicle_BuildPolyList );

   // Parent will take care of body collision.
   Parent::buildPolyList(context, polyList,box,sphere);

   // Add wheels as boxes.
   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) {
      if (wheel->tire && wheel->spring) {
         Box3F wbox;
         F32 radius = wheel->tire->radius;
         wbox.minExtents.x = -(wbox.maxExtents.x = radius / 2);
         wbox.minExtents.y = -(wbox.maxExtents.y = radius);
         wbox.minExtents.z = -(wbox.maxExtents.z = radius);
         MatrixF mat = mObjToWorld;

         Point3F sp,vec;
         mObjToWorld.mulP(wheel->data->pos,&sp);
         mObjToWorld.mulV(VectorF(0,0,-wheel->spring->length),&vec);
         Point3F ep = sp + (vec * wheel->extension);
         mat.setColumn(3,ep);
         polyList->setTransform(&mat,Point3F(1,1,1));
         polyList->addBox(wbox);
      }
   }
   return !polyList->isEmpty();
}

void WheeledVehicle::prepBatchRender(SceneRenderState* state, S32 mountedImageIndex )
{
   Parent::prepBatchRender( state, mountedImageIndex );

   if ( mountedImageIndex != -1 )
      return;

   // Set up our render state *here*, 
   // before the push world matrix, so 
   // that wheel rendering will be correct.
   TSRenderState rdata;
   rdata.setSceneState( state );

   // We might have some forward lit materials
   // so pass down a query to gather lights.
   LightQuery query;
   query.init( getWorldSphere() );
   rdata.setLightQuery( &query );

   // Shape transform
   GFX->pushWorldMatrix();

   MatrixF mat = getRenderTransform();
   mat.scale( mObjScale );
   GFX->setWorldMatrix( mat );

   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      if (wheel->shapeInstance) 
      {
         GFX->pushWorldMatrix();

         // Steering & spring extension
         MatrixF hub(EulerF(0,0,mSteering.x * wheel->steering));
         Point3F pos = wheel->data->pos;
         pos.z -= wheel->spring->length * wheel->extension;
         hub.setColumn(3,pos);

         GFX->multWorld(hub);

         // Wheel rotation
         MatrixF rot(EulerF(wheel->apos * M_2PI,0,0));
         GFX->multWorld(rot);

         // Rotation the tire to face the right direction
         // (could pre-calculate this)
         MatrixF wrot(EulerF(0,0,(wheel->data->pos.x > 0)? M_PI/2: -M_PI/2));
         GFX->multWorld(wrot);

         // Render!
         wheel->shapeInstance->animate();
         wheel->shapeInstance->render( rdata );

         if (mCloakLevel != 0.0f)
            wheel->shapeInstance->setAlphaAlways(1.0f - mCloakLevel);
         else
            wheel->shapeInstance->setAlphaAlways(1.0f);

         GFX->popWorldMatrix();
      }
   }

   GFX->popWorldMatrix();

}

//----------------------------------------------------------------------------

void WheeledVehicle::writePacketData(GameConnection *connection, BitStream *stream)
{
   Parent::writePacketData(connection, stream);
   stream->writeFlag(mBraking);

   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      stream->write(wheel->avel);
      stream->write(wheel->Dy);
      stream->write(wheel->Dx);
      stream->writeFlag(wheel->slipping);
   }
   // *Engine change
   stream->write( mCurGear );
   stream->write( mGearDelay );
   stream->write( mStuckCounter );
   stream->write( mSlowDown );   
   stream->write( mEngineRPM );
   stream->write( mWheelRPM );
   stream->write( mWheelVel );
   stream->write( mFuelFlow );   
   //stream->writeFlag( useAutomatic );
   stream->writeFlag(mContPowered); 
   stream->writeFlag( inPark );
}

void WheeledVehicle::readPacketData(GameConnection *connection, BitStream *stream)
{
   Parent::readPacketData(connection, stream);
   mBraking = stream->readFlag();

   Wheel* wend = &mWheel[mDataBlock->wheelCount];
   for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
   {
      stream->read(&wheel->avel);
      stream->read(&wheel->Dy);
      stream->read(&wheel->Dx);
      wheel->slipping = stream->readFlag();
   }

   // Rigid state is transmitted by the parent...
   setPosition(mRigid.linPosition,mRigid.angPosition);
   mDelta.pos = mRigid.linPosition;
   mDelta.rot[1] = mRigid.angPosition;

   // *Engine change
   stream->read( &mCurGear );	   
   stream->read( &mGearDelay );
   stream->read( &mStuckCounter );
   stream->read( &mSlowDown );   
   stream->read( &mEngineRPM );
   stream->read( &mWheelRPM );
   stream->read( &mWheelVel );
   stream->read( &mFuelFlow );   
   // useAutomatic = stream->readFlag();
   mContPowered = stream->readFlag();
   inPark = stream->readFlag();
}


//----------------------------------------------------------------------------

U32 WheeledVehicle::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);

   // *Engine Change
  if( stream->writeFlag( mask & EngineMask ) ) {
	  U32 engineid = 0;
	  if(mEngine){
		  stream->writeFlag(true);
		  stream->writeRangedU32( mEngine->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast );
	  } else {
		  stream->writeFlag(false);
	  }
	  //stream->writeRangedU32( mEngine->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast );
	  //stream->writeRangedU32( engineid, DataBlockObjectIdFirst, DataBlockObjectIdLast );
   }
   // *End Engine Change

   // Update wheel datablock information
   if (stream->writeFlag(mask & WheelMask)) 
   {
      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
      {
         if (stream->writeFlag(wheel->tire && wheel->spring)) 
         {
            stream->writeRangedU32(wheel->tire->getId(),
               DataBlockObjectIdFirst,DataBlockObjectIdLast);
            stream->writeRangedU32(wheel->spring->getId(),
               DataBlockObjectIdFirst,DataBlockObjectIdLast);
            stream->writeFlag(wheel->powered);

            // Steering must be sent with full precision as it's
            // used directly in state force calculations.
            stream->write(wheel->steering);
         }
      }
   }

   // The rest of the data is part of the control object packet update.
   // If we're controlled by this client, we don't need to send it.
   if (stream->writeFlag(getControllingClient() == con && !(mask & InitialUpdateMask)))
      return retMask;

   stream->writeFlag(mBraking);

   if (stream->writeFlag(mask & PositionMask)) 
   {
      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
      {
         stream->write(wheel->avel);
         stream->write(wheel->Dy);
         stream->write(wheel->Dx);
      }
   }
   // *Engine Change
   stream->write( mCurGear );
   stream->write( mGearDelay );
   stream->write( mStuckCounter );
   stream->write( mSlowDown );
   stream->write( mEngineRPM );
   stream->write( mWheelRPM );
   stream->write( mWheelVel );
   // stream->writeFlag( useAutomatic );
   stream->writeFlag(mUseAutomatic);
   stream->writeFlag(mContPowered);   
   stream->writeFlag( inPark );

   return retMask;
}

void WheeledVehicle::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con,stream);
   // *Engine Change
   if( stream->readFlag() ) {
	   if( stream->readFlag() ) {
		   SimObjectId eid = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast );
		   if( !Sim::findObject( eid, mEngine ) ) {
			   con->setLastError("Invalid packet WheeledVehicle::unpackUpdate()");
		   }
	   } else {
		   mEngine = NULL;
	   }
   }
   // *End Engine Change

   // Update wheel datablock information
   if (stream->readFlag()) 
   {
      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
      {
         if (stream->readFlag()) 
         {
            SimObjectId tid = stream->readRangedU32(DataBlockObjectIdFirst,DataBlockObjectIdLast);
            SimObjectId sid = stream->readRangedU32(DataBlockObjectIdFirst,DataBlockObjectIdLast);
            if (!Sim::findObject(tid,wheel->tire) || !Sim::findObject(sid,wheel->spring)) 
            {
               con->setLastError("Invalid packet WheeledVehicle::unpackUpdate()");
               return;
            }
            wheel->powered = stream->readFlag();
            stream->read(&wheel->steering);

            // Create an instance of the tire for rendering
            delete wheel->shapeInstance;
            wheel->shapeInstance = (wheel->tire->shape == NULL) ? 0:
               new TSShapeInstance(wheel->tire->shape);
         }
      }
   }

   // After this is data that we only need if we're not the
   // controlling client.
   if (stream->readFlag())
      return;

   mBraking = stream->readFlag();

   if (stream->readFlag()) 
   {
      Wheel* wend = &mWheel[mDataBlock->wheelCount];
      for (Wheel* wheel = mWheel; wheel < wend; wheel++) 
      {
         stream->read(&wheel->avel);
         stream->read(&wheel->Dy);
         stream->read(&wheel->Dx);
      }
   }
   // *Engine Change
   stream->read( &mCurGear );	   
   stream->read( &mGearDelay );
   stream->read( &mStuckCounter );
   stream->read( &mSlowDown );   
   stream->read( &mEngineRPM );
   stream->read( &mWheelRPM );
   stream->read( &mWheelVel );
   mUseAutomatic = stream->readFlag();
   mContPowered = stream->readFlag();
   inPark = stream->readFlag();

}
// ***********************************************************

WheeledVehicleTire * WheeledVehicle::getTire(S32 wheel) {
	if (wheel >= WheeledVehicleData::MaxWheels){
		Con::errorf("WheeledVehicle::getTire() - invalid wheel value.");
		return NULL;
	}
	return mWheel[wheel].tire;
}
WheeledVehicleSpring * WheeledVehicle::getSpring(S32 wheel) {
	if (wheel >= WheeledVehicleData::MaxWheels){
		Con::errorf("WheeledVehicle::getSpring() - invalid wheel value.");
		return NULL;
	}
	return mWheel[wheel].spring;
}

// ***************************************************************

//----------------------------------------------------------------------------
// Console Methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

DefineEngineMethod( WheeledVehicle, setWheelSteering, bool, ( S32 wheel, F32 steering ),,
   "@brief Set how much the wheel is affected by steering.\n\n"
   "The steering factor controls how much the wheel is rotated by the vehicle "
   "steering. For example, most cars would have their front wheels set to 1.0, "
   "and their rear wheels set to 0 since only the front wheels should turn.\n\n"
   "Negative values will turn the wheel in the opposite direction to the steering "
   "angle.\n"
   "@param wheel index of the wheel to set (hub node #)\n"
   "@param steering steering factor from -1 (full inverse) to 1 (full)\n"
   "@return true if successful, false if failed\n\n" )
{
   if ( wheel >= 0 && wheel < object->getWheelCount() ) {
      object->setWheelSteering( wheel, steering );
      return true;
   }
   else
      Con::warnf("setWheelSteering: wheel index %d out of bounds, vehicle has %d hubs",
         wheel, object->getWheelCount());
   return false;
}

DefineEngineMethod( WheeledVehicle, setWheelPowered, bool, ( S32 wheel, bool powered ),,
   "@brief Set whether the wheel is powered (has torque applied from the engine).\n\n"
   "A rear wheel drive car for example would set the front wheels to false, "
   "and the rear wheels to true.\n"
   "@param wheel index of the wheel to set (hub node #)\n"
   "@param powered flag indicating whether to power the wheel or not\n"
   "@return true if successful, false if failed\n\n" )
{
   if ( wheel >= 0 && wheel < object->getWheelCount() ) {
      object->setWheelPowered( wheel, powered );
      return true;
   }
   else
      Con::warnf("setWheelPowered: wheel index %d out of bounds, vehicle has %d hubs",
         wheel, object->getWheelCount());
   return false;
}

DefineEngineMethod( WheeledVehicle, setWheelTire, bool, ( S32 wheel, WheeledVehicleTire* tire ),,
   "@brief Set the WheeledVehicleTire datablock for this wheel.\n"
   "@param wheel index of the wheel to set (hub node #)\n"
   "@param tire WheeledVehicleTire datablock\n"
   "@return true if successful, false if failed\n\n"
   "@tsexample\n"
   "%obj.setWheelTire( 0, FrontTire );\n"
   "@endtsexample\n" )
{
   if (wheel >= 0 && wheel < object->getWheelCount()) {
      object->setWheelTire(wheel,tire);
      return true;
   }
   else {
      Con::warnf("setWheelTire: invalid tire datablock or wheel index, vehicle has %d hubs",
         object->getWheelCount());
      return false;
   }
}

DefineEngineMethod( WheeledVehicle, setWheelSpring, bool, ( S32 wheel, WheeledVehicleSpring* spring ),,
   "@brief Set the WheeledVehicleSpring datablock for this wheel.\n"
   "@param wheel index of the wheel to set (hub node #)\n"
   "@param spring WheeledVehicleSpring datablock\n"
   "@return true if successful, false if failed\n\n"
   "@tsexample\n"
   "%obj.setWheelSpring( 0, FrontSpring );\n"
   "@endtsexample\n" )
{
   if (spring && wheel >= 0 && wheel < object->getWheelCount()) {
      object->setWheelSpring(wheel,spring);
      return true;
   }
   else {
      Con::warnf("setWheelSpring: invalid spring datablock or wheel index, vehicle has %d hubs",
         object->getWheelCount());
      return false;
   }
}

DefineEngineMethod( WheeledVehicle, getWheelCount, S32, (),,
   "@brief Get the number of wheels on this vehicle.\n"
   "@return the number of wheels (equal to the number of hub nodes defined in the model)\n\n" )
{
   return object->getWheelCount();
}



// #############################################################################
// Advanced Vehicle console methods
// #############################################################################

DefineEngineMethod(WheeledVehicle, setEngine, bool, (WheeledVehicleEngine* engine ),,
	"@brief Assign a new WheeledVehicleEngine datablock for the vehicle" )
{
	if( engine || engine == NULL) {
		object->setEngine( engine );
		return true;
	} else {
		Con::warnf("setEngine: datablock does not exist (or is not an engine)");
	}
	return false;
}

DefineEngineMethod(WheeledVehicle, getEngine, S32, (void),,
	"@brief Get the WheeledVehicleEngine datablock for the vehicle" )
{
	WheeledVehicleEngine * engine = object->getEngine();	
	return engine->getId();
}

DefineEngineMethod(WheeledVehicle, getTire, S32, (S32 wheel),,
	"@brief Get the WheeledVehicleSpring datablock for the specified wheel." )
{
	WheeledVehicleTire * tire = object->getTire(wheel);
	//S32 rv = SimObjectId((uintptr_t)tire);
	return tire->getId();
}

DefineEngineMethod(WheeledVehicle, getSpring, S32, (S32 wheel),,
	"@brief Get the WheeledVehicleSpring datablock for the specified wheel." )
{
	WheeledVehicleSpring * spring = object->getSpring(wheel);
	//S32 rv = SimObjectId((uintptr_t)spring);
	return spring->getId();
}

DefineEngineMethod( WheeledVehicle, setGear, S32, ( S32 nGear ),,
   "@brief Set current transmission gear.\n\n"
   "This will set the current transmission to the specified gear\n"
   "@param gear transmission should be set to\n"
   "@return gear number if successful, 0 if failed (or gear 0 is set)\n\n" )
{
	if( nGear >= -1 ) {
		// Can't check upper gear bound here, just do it in the functions.
		return object->setGear( nGear );
	}

	return 0;
}

DefineEngineMethod( WheeledVehicle, upShift, S32, (),,
   "@brief Increment the active transmission gear.\n\n"
   "This will increase the value of the currently selected gear by 1.\n"
   "If the transmission is in reverse (gear -1), this will switch to neutral (gear 0).\n"
   "@return gear number selected by the transmission\n\n" )

{
	return object->upShift();
}

DefineEngineMethod( WheeledVehicle, downShift, S32,(),,
   "@brief Decrement the active transmission gear.\n\n"
   "This will decrease the value of the currently selected gear by 1.\n"
   "If the transmission is in first gear, this will switch to neutral (gear 0).\n"
   "@return gear number selected by the transmission\n\n" )

{
	return object->downShift();
}

DefineEngineMethod( WheeledVehicle, getGear, S32, (),,
   "@brief \n\n"
   "...\n"
   "@param \n"
   "@return .. if successful, .. if failed ()\n\n" ) 
{
	return object->getGear();
}

DefineEngineMethod( WheeledVehicle, getSpeed, S32, (),,
   "@brief \n\n"
   "...\n"
   "@param \n"
   "@return .. if successful, .. if failed ()\n\n" )
{
	return object->getSpeed();
}

DefineEngineMethod( WheeledVehicle, getRPM, S32, (),,
   "@brief \n\n"
   "...\n"
   "@param \n"
   "@return .. if successful, .. if failed ()\n\n" )
{
	return object->getRPM();
}

DefineEngineMethod( WheeledVehicle, setAuto, void, (bool isAuto),,
   "@brief \n\n"
   "...\n"
   "@param \n"
   "@return .. if successful, .. if failed ()\n\n" )
{
   object->setAutomatic(isAuto);
}

DefineEngineMethod( WheeledVehicle, isDefaultAuto, bool, (),,
   "@brief \n\n"
   "...\n"
   "@param \n"
   "@return .. if successful, .. if failed ()\n\n" )
{
	return object->isAutomatic();
}

DefineEngineMethod( WheeledVehicle, isAuto, bool, (),,
   "@brief \n\n"
   "...\n"
   "@param \n"
   "@return .. if successful, .. if failed ()\n\n" ) 
{
	return object->mUseAutomatic;
}
