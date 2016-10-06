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
// PlayGui is the main TSControl through which the game is viewed.
// The PlayGui also contains the hud controls.
//-----------------------------------------------------------------------------
PlayGui.actionMap = moveMap;

function PlayGui::onWake(%this)
{
   // Turn off any shell sounds...
   // sfxStop( ... );
	if (!isObject(PlayGui.actionMap)) PlayGui.actionMap = moveMap;	// Make sure there is an action map.
   $enableDirectInput = "1";
   activateDirectInput();

   // Message hud dialog
   if ( isObject( MainChatHud ) )
   {
      Canvas.pushDialog( MainChatHud );
      chatHud.attach(HudMessageVector);
   }      
   
   // just update the action map here
   //moveMap.push();
	PlayGui.actionMap.push();
	
   // hack city - these controls are floating around and need to be clamped
   if ( isFunction( "refreshCenterTextCtrl" ) )
      schedule(0, 0, "refreshCenterTextCtrl");
   if ( isFunction( "refreshBottomTextCtrl" ) )
      schedule(0, 0, "refreshBottomTextCtrl");
	//echo("extent:" SPC %this.extent);
	//echo("cluster:" SPC instrumentCluster.extent);
	//echo(Canvas.extent);
	%ox = Canvas.extent.x - instrumentCluster.extent.x;
	%oy = Canvas.extent.y - instrumentCluster.extent.y;
	instrumentCluster.setPosition(%ox,%oy);
}

function PlayGui::onSleep(%this)
{
   if ( isObject( MainChatHud ) )
      Canvas.popDialog( MainChatHud );
   
   // pop the keymaps
   //moveMap.pop();
	PlayGui.actionMap.pop();
}

function PlayGui::clearHud( %this )
{
   Canvas.popDialog( MainChatHud );

   while ( %this.getCount() > 0 )
      %this.getObject( 0 ).delete();
}




// ---------------------------------------------------------------------------
// KGB: Swap current action map with new map
function PlayGui::setActionMap(%this, %map){
	if(!isObject(%map)) return;	// Don't try to push an invalid map
	%omap = PlayGui.actionMap;
	PlayGui.actionMap.pop();
	echo("map:"@%map SPC %map.name);
	%map.push();
	PlayGui.actionMap = %map;
}

//-----------------------------------------------------------------------------

function PlayGui::updateGear(%this, %gear)
{
   
   //Remember the base position of the shifter
   if (!gearIndicator.basePos){
      gearIndicator.basePos = gearIndicator.position;
   }
   //gearIndicator.basePos = gearIndicator.getBasePosition();
	//warn("Basepos:" @ gearIndicator.basePos);
	
   // Positive goes left to right
   %px = getWord(gearIndicator.basePos,0);
   %dx = %px + (%gear * gearIndicator.shiftX);

   // Positive goes bottom to top
   %py = getWord(gearIndicator.basePos,1);   
   %dy = %py - (%gear * gearIndicator.shiftY);
   
   
   //LapCounter.setText("Gear:" SPC %gear);
   //%p = 557 - (%gear * 25);
   //%gp = "767 "@%p;

   %gp = %dx SPC %dy;
   //error("DriveGui::updateGear:" @ %gear SPC " x/y:" @ %gp);
   //gearIndicator.position = %gp;
	 gearIndicator.setPosition(%dx,%dy);
}


function clientCmdDispDriveGUI()
{
   showDriveDialog();
}
function showDriveDialog()
{
   canvas.setContent(DriveGui);
}

function clientCmdDispPlayGUI()
{
   showPlayDialog();
}
function showPlayDialog()
{
   canvas.setContent(PlayGui);
}

function clientCmdUpdateGearDisplay(%gear)
{
	//updtGearDisp(%gear);
	PlayGui.updateGear(%gear);
//	echo("***:" @%gear);
	//showPlayDialog();
}

function updtGearDisp(%gear)
{
	PlayGui.updateGear(%gear);
//	echo("updategear:" @%gear);
}

//-----------------------------------------------------------------------------

function refreshBottomTextCtrl()
{
   BottomPrintText.position = "0 0";
}

function refreshCenterTextCtrl()
{
   CenterPrintText.position = "0 0";
}
