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
#include "T3D/vehicles/wheeledVehicle.h"
#include "gfx/primBuilder.h"
#include "T3D/player.h"
//-----------------------------------------------------------------------------
/// A Tachometer control.
/// This gui works with WheeledVehicleEngine to display the rpm of the
/// engine held in the current Vehicle based
/// control object. This control only works if a server
/// connection exists and the control object is a vehicle or player mounted to a vehicle.
/// If either of these requirements is false, the control is not rendered.
class GuiTachometerHud : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;

   F32   mRPM;        ///< Current RPM
   F32   mMaxRPM;     ///< Max RPM at max need pos
   F32   mMaxAngle;     ///< Max pos of needle
   F32   mMinAngle;     ///< Min pos of needle
   Point2F mCenter;     ///< Center of needle rotation
   ColorF mColor;       ///< Needle Color
   F32   mNeedleLength;
   F32   mNeedleWidth;
   F32   mTailLength;

   GFXStateBlockRef mBlendSB;

public:
   GuiTachometerHud();
   void onRender( Point2I, const RectI &);
   static void initPersistFields();

   // KGB: Debugging/setup flags
   F32	mCalibrateRPM;
   F32	mCalibrateAngle;
   bool mClampNeedleAngle;
   bool mForceBitmapRender;	// KGB
   

   DECLARE_CONOBJECT( GuiTachometerHud );
   DECLARE_CATEGORY( "Gui Game" );
   DECLARE_DESCRIPTION( "Displays the rpm of the current Vehicle-based control object." );
};


//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT( GuiTachometerHud );

ConsoleDocClass( GuiTachometerHud,
   "@brief Displays the RPM of the current Vehicle based control object.\n\n"

   "This control only works if a server connection exists, and its control "
   "object is a Vehicle derived class. If either of these requirements is false, "
   "the control is not rendered.<br>"

   "The control renders the speedometer needle as a colored quad, rotated to "
   "indicate the Vehicle RPM as determined by the <i>minAngle</i>, "
   "<i>maxAngle</i>, and <i>maxRPM</i> properties. This control is normally "
   "placed on top of a GuiBitmapCtrl representing the speedometer dial.\n\n"

   "@tsexample\n"
   "new GuiTachometerHud()\n"
   "{\n"
   "   maxRPM = \"100\";\n"
   "   minAngle = \"215\";\n"
   "   maxAngle = \"0\";\n"
   "   color = \"1 0.3 0.3 1\";\n"
   "   center = \"130 123\";\n"
   "   length = \"100\";\n"
   "   width = \"2\";\n"
   "   tail = \"0\";\n"
   "   //Properties not specific to this control have been omitted from this example.\n"
   "};\n"
   "@endtsexample\n\n"

   "@ingroup GuiContainers"
);

GuiTachometerHud::GuiTachometerHud()
{
   mRPM = 0;
   mMaxRPM = 100;
   mMaxAngle = 0;
   mMinAngle = 200;
   mCenter.set(0,0);
   mNeedleWidth = 3;
   mNeedleLength = 10;
   mTailLength = 5;
   mColor.set(1,0,0,1);

   mClampNeedleAngle = true;	// KGB
   mCalibrateRPM = 0;
   mCalibrateAngle = 0;
   mForceBitmapRender = false;
}

void GuiTachometerHud::initPersistFields()
{
   addGroup("Needle");

   addField("maxRPM", TypeF32, Offset( mMaxRPM, GuiTachometerHud ),
      "Maximum Vehicle RPM (in Torque units per second) to represent on the "
      "speedo (Vehicle RPMs greater than this are clamped to maxRPM)." );

   addField("minAngle", TypeF32, Offset( mMinAngle, GuiTachometerHud ),
      "Angle (in radians) of the needle when the Vehicle RPM is 0. An angle "
      "of 0 points right, 90 points up etc)." );

   addField("maxAngle", TypeF32, Offset( mMaxAngle, GuiTachometerHud ),
      "Angle (in radians) of the needle when the Vehicle RPM is >= maxRPM. "
      "An angle of 0 points right, 90 points up etc)." );

   addField("clampNeedleAngle", TypeBool, Offset( mClampNeedleAngle, GuiTachometerHud ),
      "Clamp needle to maxAngle when the Vehicle speed is >= maxSpeed. ");

   addField("color", TypeColorF, Offset( mColor, GuiTachometerHud ),
      "Color of the needle" );

   addField("center", TypePoint2F, Offset( mCenter, GuiTachometerHud ),
      "Center of the needle, offset from the GuiTachometerHud control top "
      "left corner" );

   addField("length", TypeF32, Offset( mNeedleLength, GuiTachometerHud ),
      "Length of the needle from center to end" );

   addField("width", TypeF32, Offset( mNeedleWidth, GuiTachometerHud ),
      "Width of the needle" );

   addField("tail", TypeF32, Offset( mTailLength, GuiTachometerHud ),
      "Length of the needle from center to tail" );

   endGroup("Needle");
   addGroup("Calibration");
   addField("calibrateRPM", TypeF32, Offset( mCalibrateRPM, GuiTachometerHud),
      "Set a forced output RPM to be rendered for calibration. Will cause control to render without vehicle mount/control object. Setting to 0 disables." );
   addField("calibrateAngle", TypeF32, Offset( mCalibrateAngle, GuiTachometerHud),
      "Set a forced output angle to be rendered for calibration. Will cause control to render without vehicle mount/control object. Setting to 0 disables." );
   addField("forceBitmapRender", TypeBool, Offset( mForceBitmapRender, GuiTachometerHud ),
      "Clamp needle to maxAngle when the Vehicle speed is >= maxSpeed. ");

   endGroup("Calibration");

   Parent::initPersistFields();
}


//-----------------------------------------------------------------------------
/**
   Gui onRender method.
   Renders a health bar with filled background and border.
*/
void GuiTachometerHud::onRender(Point2I offset, const RectI &updateRect)
{
	if(mForceBitmapRender) Parent::onRender(offset,updateRect);	// Show bitmap no matter what if flag is set
	WheeledVehicle* vehicle = NULL;
	if(mCalibrateRPM == 0 && mCalibrateAngle == 0){
		// Must have a connection
		GameConnection* conn = GameConnection::getConnectionToServer();
		if (!conn)
			return;
		// Requires either a vehicle control object or a vehicle-mounted player		
		vehicle = dynamic_cast<WheeledVehicle*>(conn->getControlObject());
		if(!vehicle){
			Player * player = dynamic_cast<Player*>(conn->getControlObject());
			if(!player) return;
			if (!player->isMounted()) return;
			vehicle = dynamic_cast<WheeledVehicle*>(player->getObjectMount());
			if(!vehicle) return;
		}
	}
	if(!mForceBitmapRender) Parent::onRender(offset,updateRect);	// If not already done, show base bitmap.

	// Set up the needle vertex list
	Point3F vertList[5];
	vertList[0].set(+mNeedleLength,-mNeedleWidth,0);
	vertList[1].set(+mNeedleLength,+mNeedleWidth,0);
	vertList[2].set(-mTailLength  ,+mNeedleWidth,0);
	vertList[3].set(-mTailLength  ,-mNeedleWidth,0);   
	vertList[4].set(+mNeedleLength,-mNeedleWidth,0); //// Get back to the start!
	if(mCalibrateRPM > 0 || !vehicle){
		mRPM = mCalibrateRPM;
	}else{
		mRPM = vehicle->mEngineRPM;
	}
	 //mSpeed = vehicle->getVelocity().len();
	// Calculate center point if none specified
	Point2F center = mCenter;
	if (mIsZero(center.x) && mIsZero(center.y))
	{
		center.x = getExtent().x / 2.0f;
		center.y = getExtent().y / 2.0f;
	}
	// Calculate view center with all offsets
	F32 fillOffset = GFX->getFillConventionOffset(); // Find the fill offset
	Point2F viewCenter(offset.x + fillOffset + center.x, offset.y + fillOffset + center.y);

	// Handle rotation calculations
	F32 rcos,rsin;
	F32 rotation;
	rotation = mCalibrateAngle > 0 ? mCalibrateAngle : mMinAngle + ((mMaxAngle - mMinAngle) * (mRPM / mMaxRPM));
	if(rotation>mMaxAngle && mClampNeedleAngle) rotation = mMaxAngle; 	// Clamp the angle if desired and necessary
	mSinCos(mDegToRad(rotation),rsin,rcos);
	
	// Translate and render the needle
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
		Point2F pt( rcos * vertList[k].x  - rsin * vertList[k].y,
				    rsin * vertList[k].x  + rcos * vertList[k].y);
	   PrimBuild::vertex2f(pt.x + viewCenter.x, pt.y + viewCenter.y);
	}
	PrimBuild::end();
}