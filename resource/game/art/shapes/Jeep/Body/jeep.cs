
singleton TSShapeConstructor(JeepDae)
{
   baseShape = "./jeep.dae";
   neverImport = "Sketchup";
   adjustCenter = "1";
   adjustFloor = "1";
   loadLights = "0";
   unit = "0.028";
};

function JeepDae::onLoad(%this)
{
   %this.setNodeTransform("group_0", "0.967482 -1.98488 0.0302656 0.00239715 0.708106 0.706102 3.12931", "1");
   %this.addNode("Col-1", "", "0 0 0 0 0 1 0", "0");
   %this.addNode("ColBox-1", "Col-1", "0.00742962 -0.12786 0.590198 -0.999999 0 -0.00127579 1.53322", "1");
   %this.addCollisionDetail("-1", "Box", "Bounds", "4", "30", "30", "32", "30", "30", "30");
   %this.addNode("Hub0", "group_0", "-0.820273 0.99243 0.0250097 -0.999131 0.0400902 -0.0114249 1.57196", "1");
   %this.addNode("Hub1", "group_0", "0.6503 0.980824 0.0358766 0.00290373 0.71007 0.704125 3.12823", "1");
   %this.addNode("Hub2", "group_0", "-0.793761 -1.24183 0.152475 0.0113896 0.710182 0.703926 3.11123", "1");
   %this.addNode("Hub3", "group_0", "0.711122 -1.1901 0.100222 0.00302145 0.710225 0.703968 3.12816", "1");
   %this.addNode("EYE", "group_0", "-0.474816 -0.603249 1.19862 -0.995893 0.0241852 -0.0872446 0.0670205", "1");
   %this.addNode("CAM", "group_0", "-0.0470932 -1.74711 1.5463 0.0736091 0.996462 -0.0405735 3.13402", "1");
   %this.addNode("mount0", "group_0", "-0.48317 -0.217843 -0.152201 -0.620953 0.262518 -0.738581 0.153591", "1");
   %this.addNode("mount1", "group_0", "-0.322832 -0.89855 1.10415 0.943176 -0.0278479 -0.331124 0.0536948", "1");
   %this.setBounds("-0.878111 -1.75036 0.0392517 0.862212 1.6163 1.31899");
   %this.addNode("mount2", "group_0", "-0.839153 -1.82356 0.370847 -0.00502267 -0.666367 0.745607 3.13748", "1");
   %this.addNode("mount3", "group_0", "0.824373 -1.82032 0.392703 0.0048529 0.716548 -0.697521 3.14436", "1");
   %this.addNode("mount4", "group_0", "0.553854 1.53147 0.712019 0.000408859 0.99946 -0.0328575 3.07976", "1");
   %this.addNode("mount5", "group_0", "-0.621058 1.51635 0.71872 -0.112535 0.992733 -0.0426374 3.22318", "1");
   %this.addNode("mount7", "mount5", "-0.621058 1.48025 0.71872 -0.112517 0.992736 -0.0425983 3.22319", "1");
   %this.setNodeTransform("group_2", "0.0457733 -1.79164 0.302416 0.00147818 -0.00339485 0.999993 1.52022", "1");
   %this.setNodeTransform("group_1", "-0.47728 -1.74612 0.375357 -0.0030456 0.00546058 0.99998 3.13644", "1");
   %this.addNode("mount6", "mount4", "0.57972 1.50978 0.729045 0.000410468 0.99946 -0.0328557 3.07979", "1");
}
