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
#include "console/engineAPI.h"
#include "console/consoleTypes.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "T3D/gameBase/gameConnection.h"
#include "T3D/gameBase/gameBase.h"
#include "T3D/vehicles/vehicle.h"
#include "gfx/primBuilder.h"
#include "gfx/gfxDrawUtil.h"
#include "T3D/player.h"
//-----------------------------------------------------------------------------
/// A Speedometer control.
/// This gui displays the speed of the current Vehicle based
/// control object. This control only works if a server
/// connection exists and its control object is a vehicle. If
/// either of these requirements is false, the control is not rendered.
class GuiSpeedometerHud : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;

   F32   mSpeed;        ///< Current speed
   F32   mMaxSpeed;     ///< Max speed at max need pos
   F32   mMaxAngle;     ///< Max pos of needle
   F32   mMinAngle;     ///< Min pos of needle
   Point2F mCenter;     ///< Center of needle rotation
   ColorF mColor;       ///< Needle Color
   F32   mNeedleLength;
   F32   mNeedleWidth;
   F32   mTailLength;
   bool mClampNeedleAngle;	// KGB
   F32 mTorqueUnitScale; // KGB: scale of torque units to meters

   bool mForceRender;	// KGB
   

   StringTableEntry  mNeedleBitmap;
   GFXTexHandle mNeedleTextureHandle;

   GFXStateBlockRef mBlendSB;

public:
   GuiSpeedometerHud();
   bool onWake();

   void setNeedleBitmap(const char *name);
   void draw2DSquare(const Point2F &screenPoint, F32 length, F32 width, GFXTextureObject* texture, F32 spinAngle = 0.0f);
   void renderNeedle(const Point3F &center, F32 spinAngle);
   void onRender( Point2I, const RectI &);
   static void initPersistFields();

   F32 mphToVel(F32 mph);
   F32 kphToVel(F32 kph);

   F32	mCalibrateSpeed;
   F32	mCalibrateAngle;

   DECLARE_CONOBJECT( GuiSpeedometerHud );
   DECLARE_CATEGORY( "Gui Game" );
   DECLARE_DESCRIPTION( "Displays the speed of the current Vehicle-based control object." );
};


//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT( GuiSpeedometerHud );

ConsoleDocClass( GuiSpeedometerHud,
   "@brief Displays the speed of the current Vehicle based control object.\n\n"
   "This control only works if a server connection exists, and its control"
   "object is a either a Vehicle derived class or a Player derived class mounted"
   "to a Vehicle derived class. If these requirements are not met,"
   "the control is not rendered.\n"

   "The control renders the speedometer needle as a colored quad, rotated to "
   "indicate the Vehicle speed as determined by the <i>minAngle</i>, "
   "<i>maxAngle</i>, and <i>maxSpeed</i> properties. This class extends GuiBitmapCtrl "
   "and will render a specified background image representing the speedometer dial.\n\n"

   "Speed variables (maxSpeed/minSpeed) represent raw object velocity. Calculations"
   "for mph/kph are derived using torqueUnitScale. The torqueUnitScale value defaults"
   "to 1 torque unit = 1 meter.\n\n"
   
   "@tsexample\n"
   "new GuiSpeedometerHud()\n"
   "{\n"
   "   bitmap = \"art/gui/speedometer\";\n"
   "   maxSpeed = \"35.7632\";\n" 
   "   minAngle = \"135\";\n"
   "   maxAngle = \"300\";\n"
   "   color = \"1 0.3 0.3 1\";\n"
   "   center = \"130 123\";\n"
   "   length = \"100\";\n"
   "   width = \"2\";\n"
   "   tail = \"0\";\n"
   "   forceRender = \"false\";\n"
   "   clampNeedleAngle = \"false\";\n"
   "   calibrateSpeed = \"0\";\n"
   "   calibrateAngle = \"0\";\n"
   "   torqueUnitScale = \"1\";\n"
   "   needleBitmap = \"art/gui/speedometerNeedle\";\n"
   "   //Properties not specific to this control have been omitted from this example.\n"
   "};\n"
   "@endtsexample\n\n"

   "@ingroup GuiContainers"
);

GuiSpeedometerHud::GuiSpeedometerHud()
{
   mSpeed = 0;
   mMaxSpeed = 100;
   mMaxAngle = 0;
   mMinAngle = 200;
   mCenter.set(0,0);
   mNeedleWidth = 3;
   mNeedleLength = 10;
   mTailLength = 5;
   mColor.set(1,0,0,1);

   mClampNeedleAngle = false;	// KGB
   mCalibrateSpeed = 0;
   mCalibrateAngle = 0;
   mForceRender = false;
   mTorqueUnitScale = 1;	// 1 torque unit = 1 meter

   mNeedleBitmap = StringTable->insert("");
   mNeedleTextureHandle = NULL;
}


bool GuiSpeedometerHud::onWake()
{
   if (!Parent::onWake())
      return false;
   setActive(true);

   setNeedleBitmap(mNeedleBitmap);
   return true;
}

void GuiSpeedometerHud::initPersistFields()
{
   addGroup("Range");

   addField("maxSpeed", TypeF32, Offset( mMaxSpeed, GuiSpeedometerHud ),
      "Maximum Vehicle speed (in Torque units per second) to represent on the "
      "speedo (Vehicle speeds greater than this are clamped to maxSpeed)." );

   addField("minAngle", TypeF32, Offset( mMinAngle, GuiSpeedometerHud ),
      "Angle (in radians) of the needle when the Vehicle speed is 0. An angle "
      "of 0 points right, 90 points up etc)." );

   addField("maxAngle", TypeF32, Offset( mMaxAngle, GuiSpeedometerHud ),
      "Angle (in radians) of the needle when the Vehicle speed is >= maxSpeed. "
      "An angle of 0 points right, 90 points up etc)." );

   addField("clampNeedleAngle", TypeBool, Offset( mClampNeedleAngle, GuiSpeedometerHud ),
      "Clamp needle to maxAngle when the Vehicle speed is >= maxSpeed. ");

   addField("color", TypeColorF, Offset( mColor, GuiSpeedometerHud ),
      "Color of the needle" );

   addField("center", TypePoint2F, Offset( mCenter, GuiSpeedometerHud ),
      "Center of the needle, offset from the GuiSpeedometerHud control top "
      "left corner" );

   addField("torqueUnitScale", TypeF32, Offset( mTorqueUnitScale, GuiSpeedometerHud ),
      "Meters per torque unit." );

   endGroup("Range");
   addGroup("Rendered Needle");
   addField("length", TypeF32, Offset( mNeedleLength, GuiSpeedometerHud ),
      "Length of the needle from center to end" );

   addField("width", TypeF32, Offset( mNeedleWidth, GuiSpeedometerHud ),
      "Width of the needle" );

   addField("tail", TypeF32, Offset( mTailLength, GuiSpeedometerHud ),
      "Length of the needle from center to tail" );
   endGroup("Rendered Needle");

   addGroup("Needle Bitmap");
   addField("NeedleBitmap", TypeFilename, Offset(mNeedleBitmap, GuiSpeedometerHud),
	   "Bitmap to use for representing the needle");
   endGroup("Needle Bitmap");

   addGroup("Calibration");
   addField("calibrateSpeed", TypeF32, Offset( mCalibrateSpeed, GuiSpeedometerHud ),
      "Set a forced output speed (as object velocity) to be rendered for calibration. Will cause control to render without vehicle mount/control object. Setting to 0 disables." );
   addField("calibrateAngle", TypeF32, Offset( mCalibrateAngle, GuiSpeedometerHud ),
      "Set a forced output angle to be rendered for calibration. Will cause control to render without vehicle mount/control object. Setting to 0 disables." );
   addField("forceRender", TypeBool, Offset( mForceRender, GuiSpeedometerHud ),
      "Force the control to render without a valid control/mount . ");
   endGroup("Calibration");
   Parent::initPersistFields();
}


//-----------------------------------------------------------------------------
/**
   Gui onRender method.
   Will only render if the controlObject is a vehicle or vehicle-mounted player
   unless mCalibrateSpeed || mCalibrateAngle > 0 or mForceBitmapRender = true.
*/
void GuiSpeedometerHud::onRender(Point2I offset, const RectI &updateRect)
{
   if(mForceRender) Parent::onRender(offset,updateRect);	// Show bitmap no matter what if flag is set
   Vehicle* vehicle = NULL;
   if(mCalibrateSpeed == 0 && mCalibrateAngle == 0){
      // Must have a connection
      GameConnection* conn = GameConnection::getConnectionToServer();
      if (!conn)
         return;
      // Requires either a vehicle control object or a vehicle-mounted player		
      vehicle = dynamic_cast<Vehicle*>(conn->getControlObject());
      if(!vehicle){
         Player * player = dynamic_cast<Player*>(conn->getControlObject());
         if(!player) return;
         if (!player->isMounted()) return;
         vehicle = dynamic_cast<Vehicle*>(player->getObjectMount());
         if(!vehicle) return;
      }
   }
   if(!mForceRender) Parent::onRender(offset,updateRect);	// If not already done, show base bitmap.

   if(mCalibrateSpeed > 0 || !vehicle){
      mSpeed = mCalibrateSpeed;
   }else{
      mSpeed = vehicle->getVelocity().len() * mTorqueUnitScale;
   }

   // Calculate center point if none specified
   Point2F center = mCenter;
   if (mIsZero(center.x) && mIsZero(center.y))
   {
      center.x = getExtent().x / 2.0f;
      center.y = getExtent().y / 2.0f;
   }
   // Calculate view center with all offsets
   F32 fillOffset = GFX->getFillConventionOffset(); // Find the fill offset
   Point3F viewCenter(offset.x + fillOffset + center.x, offset.y + fillOffset + center.y,0);

   // Handle rotation calculations	
   F32 rotation, spinAngle;
   rotation = mCalibrateAngle > 0 ? mCalibrateAngle : mMinAngle + ((mMaxAngle - mMinAngle) * (mSpeed / mMaxSpeed));
   if(rotation>mMaxAngle && mClampNeedleAngle) rotation = mMaxAngle; 	// Clamp the angle if desired and necessary
   spinAngle = mDegToRad(rotation);

   // Render the needle
   if(mNeedleTextureHandle){
      draw2DSquare(center, mNeedleLength, mNeedleWidth, mNeedleTextureHandle, spinAngle);
      return;
   } else {
      renderNeedle(viewCenter,spinAngle);
   }
}

void GuiSpeedometerHud::renderNeedle(const Point3F &center, F32 spinAngle){
   // Set up the needle vertex list
   Point3F vertList[5];
   vertList[0].set(+mNeedleLength,-mNeedleWidth,0);
   vertList[1].set(+mNeedleLength,+mNeedleWidth,0);
   vertList[2].set(-mTailLength  ,+mNeedleWidth,0);
   vertList[3].set(-mTailLength  ,-mNeedleWidth,0);   
   vertList[4].set(+mNeedleLength,-mNeedleWidth,0); //// Get back to the start!

   MatrixF rotMatrix(EulerF(0.0, 0.0, spinAngle));
   if (mBlendSB.isNull())
   {
      GFXStateBlockDesc desc;
      desc.setBlend(true, GFXBlendSrcAlpha, GFXBlendInvSrcAlpha);
      desc.samplersDefined = true;
      desc.samplers[0].textureColorOp = GFXTOPDisable;
      mBlendSB = GFX->createStateBlock(desc);
   }
   GFX->setStateBlock(mBlendSB);
   GFX->setTexture(0, NULL);

   PrimBuild::color4f(mColor.red, mColor.green, mColor.blue, mColor.alpha);
   PrimBuild::begin(GFXLineStrip, 5);
   for(int k=0; k<5; k++){
      rotMatrix.mulP(vertList[k]);
      PrimBuild::vertex2f(vertList[k].x + center.x, vertList[k].y + center.y);
   }
   PrimBuild::end();
}

void GuiSpeedometerHud::draw2DSquare(const Point2F &screenPoint, F32 length, F32 width, GFXTextureObject* texture, F32 spinAngle)
{
   if (texture == NULL)
      return;

   width *= 0.5;

   Point3F offset(screenPoint.x, screenPoint.y, 0.0);

   GFXVertexBufferHandle<GFXVertexPCT> verts(GFX, 4, GFXBufferTypeVolatile);
   verts.lock();

   verts[0].point.set(-width, -length, 0.0f);
   verts[1].point.set(-width, length, 0.0f);
   verts[2].point.set(width, -length, 0.0f);
   verts[3].point.set(width, length, 0.0f);
   
   GFXVertexColor mBitmapModulation;
   mBitmapModulation.set(0xFF, 0xFF, 0xFF, 0xFF);
   verts[0].color = verts[1].color = verts[2].color = verts[3].color = mBitmapModulation;

   if (spinAngle == 0.0f)
   {
      for (S32 i = 0; i < 4; i++)
         verts[i].point += offset;
   }
   else
   {
      MatrixF rotMatrix(EulerF(0.0, 0.0, spinAngle));

      for (S32 i = 0; i < 4; i++)
      {
         rotMatrix.mulP(verts[i].point);
         verts[i].point += offset;
      }
   }

   verts[0].texCoord.set(0, 0);
   verts[1].texCoord.set(1, 0);
   verts[2].texCoord.set(0, 1);
   verts[3].texCoord.set(1, 1);


   verts.unlock();
   GFX->setVertexBuffer(verts);

   GFXStateBlockDesc rectFill;
   rectFill.setCullMode(GFXCullNone);
   rectFill.setZReadWrite(false);
   rectFill.setBlend(true, GFXBlendSrcAlpha, GFXBlendInvSrcAlpha);
   GFXStateBlockRef mRectFillSB = GFX->createStateBlock(rectFill);   
   GFX->setStateBlock(mRectFillSB);
   
   GFX->setTexture(0, texture);
   GFX->setupGenericShaders(GFXDevice::GSModColorTexture);
   GFX->drawPrimitive(GFXTriangleStrip, 0, 2);
}

//---------------------------------------------------------------------------
void GuiSpeedometerHud::setNeedleBitmap(const char *name)
{
   if (name == NULL){
      mNeedleTextureHandle = NULL;
      mNeedleBitmap = StringTable->insert("");
      return;
   }
   mNeedleBitmap = StringTable->insert(name);

   if (*mNeedleBitmap)
      mNeedleTextureHandle = GFXTexHandle(mNeedleBitmap, &GFXDefaultStaticDiffuseProfile, "GUISpeedometerHud::NeedleBitmap");
   else
      // Reset handles if UI object is hidden
      mNeedleTextureHandle = NULL;

   setUpdate();
}

//------------------------------------------------------------------------


F32 GuiSpeedometerHud::kphToVel(F32 kph){
	return ((kph * 1000 * mTorqueUnitScale) / 60) / 60;
}

F32 GuiSpeedometerHud::mphToVel(F32 mph){
	F32 kph = kphToVel(mph * 1.609344);
	return kph;
}

DefineEngineMethod( GuiSpeedometerHud, calibrateMPH, void, (F32 mph),,
	"@brief Set the calibration speed (in MPH)\n\n"
   "Set the calibration speed, first converting input from mph to velocity\n"
   "Will result in the needle being rendered as if the controlObject is moving at the set speed.\n"
   "@param F32 mph\n"
   "@return void\n\n" )
{
	object->mCalibrateSpeed = object->mphToVel(mph);
}

DefineEngineMethod( GuiSpeedometerHud, calibrateKPH, void, (F32 kph),,
	"@brief Set the calibration speed (in KPH)\n\n"
   "Set the calibration speed, first converting input from kph to velocity\n"
   "Will result in the needle being rendered as if the controlObject is moving at the set speed.\n"
   "@param F32 mph\n"
   "@return void\n\n" )
{
	object->mCalibrateSpeed = object->kphToVel(kph);
}

DefineEngineMethod( GuiSpeedometerHud, kphToVel, F32, (F32 kph),,
   "@brief Convert KPH to approximate object velocity\n\n"   
   "@param F32 speed in kph\n"
   "@return F32 linear velocity\n\n" )
{
	return object->kphToVel(kph);
}

DefineEngineMethod( GuiSpeedometerHud, mphToVel, F32, (F32 mph),,
   "@brief Convert MPH to approximate object velocity\n\n"   
   "@param F32 speed in mph\n"
   "@return F32 linear velocity\n\n" )
{
	return object->mphToVel(mph);
}


// script interface
ConsoleMethod(GuiSpeedometerHud, setNeedleBitmap, void, 3, 3, "(string filename)"
   "Set the bitmap for the needle.")
{
   object->setNeedleBitmap(argv[2]);
}
