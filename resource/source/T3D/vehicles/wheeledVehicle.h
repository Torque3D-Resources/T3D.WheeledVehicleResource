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

#ifndef _WHEELEDVEHICLE_H_
#define _WHEELEDVEHICLE_H_

#ifndef _VEHICLE_H_
#include "T3D/vehicles/vehicle.h"
#endif

#ifndef _CLIPPEDPOLYLIST_H_
#include "collision/clippedPolyList.h"
#endif

class ParticleEmitter;
class ParticleEmitterData;
struct CameraQuery;

//----------------------------------------------------------------------------

struct WheeledVehicleTire: public SimDataBlock 
{
   typedef SimDataBlock Parent;

   //
   StringTableEntry shapeName;// Max shape to render

   // Physical properties
   F32 mass;                  // Mass of the whole wheel
   F32 kineticFriction;       // Tire friction coefficient
   F32 staticFriction;        // Tire friction coefficient
   F32 restitution;           // Currently not used

   F32 rollingResistance;     //Rolling resistance factor. *Engine Change*

   // Tires act as springs and generate lateral and longitudinal
   // forces to move the vehicle. These distortion/spring forces
   // are what convert wheel angular velocity into forces that
   // act on the rigid body.
   F32 lateralForce;          // Spring force
   F32 lateralDamping;        // Damping force
   F32 lateralRelaxation;     // The tire will relax if left alone
   F32 longitudinalForce;
   F32 longitudinalDamping;
   F32 longitudinalRelaxation;

   // Shape information initialized in the preload
   Resource<TSShape> shape;   // The loaded shape
   F32 radius;                // Tire radius
   F32 circumference;         // Tire circumference

   //
   WheeledVehicleTire();
   DECLARE_CONOBJECT(WheeledVehicleTire);
   static void initPersistFields();
   bool preload(bool, String &errorStr);
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};


//----------------------------------------------------------------------------

struct WheeledVehicleSpring: public SimDataBlock 
{
   typedef SimDataBlock Parent;

   F32 length;                // Travel distance from root hub position
   F32 force;                 // Spring force
   F32 damping;               // Damping force
   F32 antiSway;              // Opposite wheel anti-sway

   //
   WheeledVehicleSpring();
   DECLARE_CONOBJECT(WheeledVehicleSpring);
   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};


//----------------------------------------------------------------------------
/***KGB: Advanced Vehicle - Engine Definition			*/
//----------------------------------------------------------------------------
struct WheeledVehicleEngine: public SimDataBlock {
	typedef SimDataBlock Parent;
	
public: 
	enum Constants {
		MaxGears = 7,
		MaxTorqueLevels = 20
	};
	F32 fuelFlow;		 // Scales torque to calculate RPM increase
	F32 minEngineRPM;	 // The STALL RPM - must be a positive (non-zero) number
	F32 maxEngineRPM;	 // Upper limit of RPM output
	F32 idleEngineRPM;	 // RPM that the running engine naturaly sets to
	F32 engineDrag;		 // The amount of drag that the engine adds when throttle is off
	F32 neutralBoost;	 // Amount of boost to provide when the engine is revved in neutral
	F32 slowDownRate;	 // Slowdown rate when negative throttle is applied
	F32 inertiaFactor;	 // Scales the mass resistance calculations (they seem OK, so it defaults to 1).
	F32 overRevSlowdown; // How quickly the engine/wheel RPM react to over-rev (downshift)

	F32 minEnginePitch;  // The low-end of engine pitch (At zero value, not IDLE)
	F32 maxEnginePitch;  // High-end engine pitch at maxEngineRPM (can overrev higher)
	F32 minEngineVolume; // The low-end of engine volume (At zero value, not IDLE)
	F32 maxEngineVolume; // High-end engine volume at maxEngineRPM (can overrev higher)

	S32 numGearRatios;
	F32 gearRatios[MaxGears];
	F32 reverseRatio;
	F32 differentalRatio;	
	
	F32 shiftUpRPM;			// Automatic transmission - up shifts at this RPM level
	F32 shiftDownRPM;		// Automatic transmission - down shifts at this RPM level
	F32 transmissionSlip;	// Built in RPM drop on upshift - simulates power loss

	S32 numTorqueLevels;
	F32 torqueLevels[MaxTorqueLevels];
	F32 rpmValues[MaxTorqueLevels];
			
	bool useAutomatic;

	bool running;
	
	
private:
	//
	bool setup;
	F32 deltaTorque[MaxTorqueLevels];
	F32 rpmStep[MaxTorqueLevels];
public:
	//
	WheeledVehicleEngine();
	DECLARE_CONOBJECT(WheeledVehicleEngine);
	static void initPersistFields();
	virtual void packData(BitStream *stream);
	virtual void unpackData(BitStream *stream );
		
	F32 getRPMTorque( F32 RPM);  // Returns the correct torque for requested Engine RPM

	   /// @name Callbacks
   /// @{
   DECLARE_CALLBACK( void, onShift, ( GameBase* obj, S32 gear, S32 engineRPM ) );
   /// @}
};
/*** KGB End ***/
// ------------------------------------------------------

struct WheeledVehicleData: public VehicleData 
{
   typedef VehicleData Parent;

   enum Constants 
   {
      MaxWheels = 8,
      MaxWheelBits = 3
   };

   enum Sounds 
   {
      JetSound,
      EngineSound,
      SquealSound,
      WheelImpactSound,
      MaxSounds,
   };
   SFXTrack* sound[MaxSounds];

   ParticleEmitterData* tireEmitter;
   F32 dustVolume;				 //***KGB: Dust emitter is scaled based on wheel speed         

   F32 maxWheelSpeed;            // Engine torque is scale based on wheel speed
   F32 engineTorque;             // Engine force controlled through throttle
   F32 engineBrake;              // Break force applied when throttle is 0
   F32 brakeTorque;              // Maximum brake force

   F32 brakeRate;                // Torque per second increase of braking force
   F32 brakeIncrease;            // Scale to increase base brake rate when brakes are applied (brakeRate *dt) + (brakeRate * brakeIncrease *dt);

   F32 freefallGravity;          // Multiplier for gravity acceleration in free-fall
   S32 freefallContact;          // Multiplier for gravity acceleration in free-fall

   // Initialized onAdd
   struct Wheel 
   {
      S32 opposite;              // Opposite wheel on Y axis (or -1 for none)
      Point3F pos;               // Root pos of spring
      S32 springNode;            // Wheel spring/hub node
      S32 springSequence;        // Suspension animation
      F32 springLength;          // Suspension animation length
   } wheel[MaxWheels];
   U32 wheelCount;
   ClippedPolyList rigidBody;    // Extracted from shape
   S32 brakeLightSequence;       // Brakes
   S32 steeringSequence;         // Steering animation

   //
   WheeledVehicleData();
   DECLARE_CONOBJECT(WheeledVehicleData);
   static void initPersistFields();
   bool preload(bool, String &errorStr);
   bool mirrorWheel(Wheel* we);
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);

   DECLARE_CALLBACK( void, onBrake, ( GameBase* obj, S32 brake, S32 type, F32 brakeLevel ) );
};


//----------------------------------------------------------------------------

class WheeledVehicle: public Vehicle
{
   typedef Vehicle Parent;

   enum MaskBits 
   {
      WheelMask    = Parent::NextFreeMask << 0,
      EngineMask   = Parent::NextFreeMask << 1, // *Engine Change *
      NextFreeMask = Parent::NextFreeMask << 2
   };

   WheeledVehicleData* mDataBlock;

   bool mBraking;
   TSThread* mTailLightThread;
   SFXSource* mJetSound;
   SFXSource* mEngineSound;
   SFXSource* mSquealSound;

   struct Wheel 
   {
      WheeledVehicleTire *tire;
      WheeledVehicleSpring *spring;
      WheeledVehicleData::Wheel* data;

      F32 extension;          // Spring extension (0-1)
      F32 avel;               // Angular velocity
      F32 apos;               // Anuglar position (client side only)
      F32 Dy,Dx;              // Current tire deformation

      struct Surface 
      {
         bool contact;        // Wheel is touching a surface
         Point3F normal;      // Surface normal
         BaseMatInstance* material; // Surface material
         Point3F pos;         // Point of contact
         SceneObject* object; // Object in contact with
      } surface;

      TSShapeInstance* shapeInstance;
      TSThread* springThread;

      F32 steering;           // Wheel steering scale
      bool powered;           // Powered by engine
      bool slipping;          // Traction on last tick
      F32 torqueScale;        // Max torque % applied to wheel (0-1)
      F32 slip;               // Amount of wheel slip (0-1)
      F32 rpm;
      F32 rot;
      
      SimObjectPtr<ParticleEmitter> emitter;
   };
   Wheel mWheel[WheeledVehicleData::MaxWheels];
   TSThread* mSteeringThread;

   //
   bool onNewDataBlock( GameBaseData *dptr, bool reload );
   void processTick(const Move *move);
   void updateMove(const Move *move);
   void updateForces(F32 dt);
   void updateEngineForces(F32 dt); // KGB: Engine forces calculation override
   void extendWheels(bool clientHack = false);
   void prepBatchRender( SceneRenderState *state, S32 mountedImageIndex );

   // Client sounds & particles
   void updateWheelThreads();
   void updateWheelParticles(F32 dt);
   void updateEngineSound(F32 level);
   void updateSquealSound(F32 level);
   void updateJetSound();

   virtual U32 getCollisionMask();

/***KGB: Advanced Vehicle private variables */   
    WheeledVehicleEngine *mEngine;	// Actual engine for this vehicle
    bool inPark;		// Flag to allow parking brake to be set
	bool mForceBraking;
	  	
	F32 mFuelFlow;		// Local setting for fuel flow

	F32 mSlowDown;		// Current amount of slow-down to apply to wheel velocity
	F32 mWheelVel;		// Velocity of the single wheel with greatest magnitude (debug value mostly)
	
	S32 mGearDelay;		// Counter for Pause after shift (may be obsolete)
	S32 mStuckCounter;	// Counter for impact
	bool mContPowered;
   F32 mGravityAccum;    // Accumulate time in free-fall.
/*** KGB End */

public:
   DECLARE_CONOBJECT(WheeledVehicle);
   static void initPersistFields();

   WheeledVehicle();
   ~WheeledVehicle();

   bool onAdd();
   void onRemove();
   void advanceTime(F32 dt);
   bool buildPolyList(PolyListContext context, AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);

   S32 getWheelCount();
   Wheel *getWheel(U32 index) {return &mWheel[index];}
   void setWheelSteering(S32 wheel,F32 steering);
   void setWheelPowered(S32 wheel,bool powered);
   void setWheelTire(S32 wheel,WheeledVehicleTire*);
   void setWheelSpring(S32 wheel,WheeledVehicleSpring*);

   void getWheelInstAndTransform( U32 wheel, TSShapeInstance** inst, MatrixF* xfrm ) const;

   void writePacketData(GameConnection * conn, BitStream *stream);
   void readPacketData(GameConnection * conn, BitStream *stream);
   U32  packUpdate(NetConnection * conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection * conn, BitStream *stream);

/***KGB: Advanced Vehicle public variables and functions */
	F32 mCenterSteeringRate; // Rate for return to center function
	bool mUseAutomatic;	// Local setting for transmission type

	S32 mCurGear;		// Current transmission gear - used for real-time calculations
	F32 mEngineRPM;		// Current engine RPM - used for real-time calculations
	F32 mWheelRPM;		// Current wheel RPM - used for real-time calculations

	WheeledVehicleTire * getTire(S32 wheel);
	WheeledVehicleSpring * getSpring(S32 wheel);
	


	void setEngine( WheeledVehicleEngine *);	// Adds the engine to the vehicle
	WheeledVehicleEngine * getEngine();	// Adds the engine to the vehicle

	F32 calcEngineRPM(S32 gear, F32 wRPM);			// Calculates engine RPM based on current wheel RPM
	S32 setGear( S32 nGear );		// Set the transmission gear
	S32 upShift();					// Increment the transmission gear (to max)
	S32 downShift();				// Decrement the transmission gear (to reverse)
	S32 getGear();					// Return the current gear
	S32 getRPM();					// Return the current Engine RPM
	S32 getSpeed();					// Return the current vehicle velocity
	bool isAutomatic();				// Returns if the DATABLOCK is supposed to be automatic
	void setAutomatic(bool isAuto);	// Sets the LOCAL transmission automatic setting

	S8 mBrakeTrigger;	// Movemanager trigger that brakes should respond to.
   F32 mBrakeLevel;  // Current level of brake application;


   F32 distPerSec(F32 rpm, F32 circumference) { return (rpm * circumference) * 0.0166666666666667; }
   F32 rpmFromVel(F32 vel, F32 circumference);
   F32 applyTransmissionRatios(F32 engineRPM);
   F32 gearRatio();
   F32 gearRatio(S32 gear);
//*** KGB End */

};


#endif
