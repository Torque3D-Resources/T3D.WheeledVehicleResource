
singleton TSShapeConstructor(Jeep_gunDAE)
{
   baseShape = "./jeep_gun.DAE";
   unit = "1";
   loadLights = "0";
   adjustCenter = "1";
};

function Jeep_gunDAE::onLoad(%this)
{
   %this.setNodeTransform("muzzlepoint", "0.227816 0.790292 0.48625 1 0 0 0.247039", "1");
}
