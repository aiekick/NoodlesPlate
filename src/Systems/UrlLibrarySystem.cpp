// NoodlesPlate Copyright (C) 2017-2024 Stephane Cuillerdier aka Aiekick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "UrlLibrarySystem.h"

#include <ctools/FileHelper.h>
#include <ImGuiPack.h>

#include <Gui/MainFrame.h>
#include <SoGLSL/src/Importer/NetCodeRetriever.h>

UrlLibrarySystem::UrlLibrarySystem()
{
	
}

UrlLibrarySystem::~UrlLibrarySystem()
{
	
}

void UrlLibrarySystem::AddUrl(std::string vPlatform, std::string vUser, const std::string& vName, std::string vUrl)
{
	puSampleUrls[vPlatform][vUser][vName] = vUrl;
}

void UrlLibrarySystem::Init()
{
	puSampleUrls["GlslSandBox"]["Unknow"]["e#37771.0"] = "http://glslsandbox.com/e#37771.0";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#38086.1"] = "http://glslsandbox.com/e#38086.1";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#38126.0"] = "http://glslsandbox.com/e#38126.0";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#38188.1"] = "http://glslsandbox.com/e#38188.1";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#38188.5"] = "http://glslsandbox.com/e#38188.5";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#39841.0"] = "http://glslsandbox.com/e#39841.0";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#42874.2"] = "http://glslsandbox.com/e#42874.2";
	puSampleUrls["GlslSandBox"]["Unknow"]["e#43376.0"] = "http://glslsandbox.com/e#43376.0";
	puSampleUrls["ShaderToy"]["Duke"]["Binary star"] = "https://www.shadertoy.com/view/ltjSWV";
	puSampleUrls["ShaderToy"]["Duke"]["Dusty nebula 1"] = "https://www.shadertoy.com/view/4lSXD1";
	puSampleUrls["ShaderToy"]["Duke"]["Dusty nebula 3"] = "https://www.shadertoy.com/view/lsVSRW";
	puSampleUrls["ShaderToy"]["Duke"]["Dusty nebula 4"] = "https://www.shadertoy.com/view/MsVXWW";
	puSampleUrls["ShaderToy"]["Duke"]["Supernova remnant"] = "https://www.shadertoy.com/view/MdKXzc";
	puSampleUrls["ShaderToy"]["Flexi"]["Rock-Paper-Scissor-4D"] = "https://www.shadertoy.com/view/4sK3RD";
	puSampleUrls["ShaderToy"]["Flexi"]["expansive reaction diffusion"] = "https://www.shadertoy.com/view/4dcGW2";
	puSampleUrls["ShaderToy"]["KylBlz"]["Type II-A Supernova"] = "https://www.shadertoy.com/view/XdVXWc";
	puSampleUrls["ShaderToy"]["P Malin"]["Multiple Transparency"] = "https://www.shadertoy.com/view/XtyGWD";
	puSampleUrls["ShaderToy"]["Reinder"]["[SH17C] Raymarching tutorial"] = "https://www.shadertoy.com/view/4dSfRc";
	puSampleUrls["ShaderToy"]["Shane"]["3D Cellular Tiling"] = "https://www.shadertoy.com/view/ld3Szs";
	puSampleUrls["ShaderToy"]["Shane"]["Abstract Corridor"] = "https://www.shadertoy.com/view/MlXSWX";
	puSampleUrls["ShaderToy"]["Shane"]["Abstract Glassy Field"] = "https://www.shadertoy.com/view/4ttGDH";
	puSampleUrls["ShaderToy"]["Shane"]["Abstract Plane"] = "https://www.shadertoy.com/view/4sGSRV";
	puSampleUrls["ShaderToy"]["Shane"]["Biomine"] = "https://www.shadertoy.com/view/4lyGzR";
	puSampleUrls["ShaderToy"]["Shane"]["Bioorganic Wall"] = "https://www.shadertoy.com/view/ldt3RN";
	puSampleUrls["ShaderToy"]["Shane"]["Bumped Sinusoidal Warp"] = "https://www.shadertoy.com/view/4l2XWK";
	puSampleUrls["ShaderToy"]["Shane"]["Cave Entrance"] = "https://www.shadertoy.com/view/ltjXzd";
	puSampleUrls["ShaderToy"]["Shane"]["Cellular Blocks"] = "https://www.shadertoy.com/view/ltySRt";
	puSampleUrls["ShaderToy"]["Shane"]["Cellular Tiled Tunnel"] = "https://www.shadertoy.com/view/MscSDB";
	puSampleUrls["ShaderToy"]["Shane"]["Dual 3D Truchet Tiles"] = "https://www.shadertoy.com/view/4l2cD3";
	puSampleUrls["ShaderToy"]["Shane"]["Endless Passageway"] = "https://www.shadertoy.com/view/4ttSRj";
	puSampleUrls["ShaderToy"]["Shane"]["Fractal Flythrough"] = "https://www.shadertoy.com/view/4s3SRN";
	puSampleUrls["ShaderToy"]["Shane"]["Full Scene Radial Blur"] = "https://www.shadertoy.com/view/XsKGRW";
	puSampleUrls["ShaderToy"]["Shane"]["Gray-Scott Pattern"] = "https://www.shadertoy.com/view/Ms3fRX";
	puSampleUrls["ShaderToy"]["Shane"]["Hexagonal Blocks"] = "https://www.shadertoy.com/view/XdjyWD";
	puSampleUrls["ShaderToy"]["Shane"]["Hexagonal Interlacing"] = "https://www.shadertoy.com/view/llfcWs";
	puSampleUrls["ShaderToy"]["Shane"]["Industrial Complex"] = "https://www.shadertoy.com/view/MtdSWS";
	puSampleUrls["ShaderToy"]["Shane"]["Industrial Tunnel"] = "https://www.shadertoy.com/view/4tKXzV";
	puSampleUrls["ShaderToy"]["Shane"]["Jigsaw"] = "https://www.shadertoy.com/view/XdGBDW";
	puSampleUrls["ShaderToy"]["Shane"]["Menger Variation 2"] = "https://www.shadertoy.com/view/ldScRt";
	puSampleUrls["ShaderToy"]["Shane"]["Mobius Object"] = "https://www.shadertoy.com/view/XldSDs";
	puSampleUrls["ShaderToy"]["Shane"]["Neon Lit Hexagons"] = "https://www.shadertoy.com/view/MsVfz1";
	puSampleUrls["ShaderToy"]["Shane"]["Perspex Web Lattice"] = "https://www.shadertoy.com/view/Mld3Rn";
	puSampleUrls["ShaderToy"]["Shane"]["Quasi Infinite Zoom Voronoi "] = "https://www.shadertoy.com/view/XlBXWw";
	puSampleUrls["ShaderToy"]["Shane"]["Raymarched Hexagonal Truchet"] = "https://www.shadertoy.com/view/4td3zj";
	puSampleUrls["ShaderToy"]["Shane"]["Reaction Diffusion - 2 Pass"] = "https://www.shadertoy.com/view/XsG3z1";
	puSampleUrls["ShaderToy"]["Shane"]["Rounded Voronoi Borders"] = "https://www.shadertoy.com/view/ll3GRM";
	puSampleUrls["ShaderToy"]["Shane"]["Terrain Lattice"] = "https://www.shadertoy.com/view/XslyRH";
	puSampleUrls["ShaderToy"]["Shane"]["Traced Minkowski Tube"] = "https://www.shadertoy.com/view/4lSXzh";
	puSampleUrls["ShaderToy"]["Shane"]["Twisted Geometry"] = "https://www.shadertoy.com/view/lt3XD8";
	puSampleUrls["ShaderToy"]["Shane"]["Weaved 3D Truchet"] = "https://www.shadertoy.com/view/4lSBzm";
	puSampleUrls["ShaderToy"]["Shane"]["Winding Menger Tunnel"] = "https://www.shadertoy.com/view/4scXzn";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Fractal Particles"] = "https://www.shadertoy.com/view/MlcSRX";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Gears Repeat 224c"] = "https://www.shadertoy.com/view/Mdy3Wz";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Hex Smooth RadClick"] = "https://www.shadertoy.com/view/XlK3W1";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Rad Rep  Particles 2 196c"] = "https://www.shadertoy.com/view/ldtSRl";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Radial Repeat  Hex GridClick"] = "https://www.shadertoy.com/view/XdKGRD";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Radial Repeat  Hex RadClick"] = "https://www.shadertoy.com/view/XdG3zm";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Radial Rotate Particles"] = "https://www.shadertoy.com/view/ltcGDM";
	puSampleUrls["ShaderToy"]["aiekick"]["2D Voro Flower"] = "https://www.shadertoy.com/view/MtGSRG";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstract Fragments Plane"] = "https://www.shadertoy.com/view/ltKXDt";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstract Log Lattice"] = "https://www.shadertoy.com/view/4lc3D2";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstract Log Lattice 2"] = "https://www.shadertoy.com/view/MlcGWj";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstract Noisy Plane"] = "https://www.shadertoy.com/view/MlXcW4";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstract Plane Beauty"] = "https://www.shadertoy.com/view/MlcSzn";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstract Plane Cloudy Beauty"] = "https://www.shadertoy.com/view/4ttXzr";
	puSampleUrls["ShaderToy"]["aiekick"]["Abstracts Curves"] = "https://www.shadertoy.com/view/ldlcD8";
	puSampleUrls["ShaderToy"]["aiekick"]["Alien Cavern"] = "https://www.shadertoy.com/view/XljGR3";
	puSampleUrls["ShaderToy"]["aiekick"]["Another 2D Pattern 141c"] = "https://www.shadertoy.com/view/4ly3Ry";
	puSampleUrls["ShaderToy"]["aiekick"]["Another 2D Pattern Variation "] = "https://www.shadertoy.com/view/XtV3zG";
	puSampleUrls["ShaderToy"]["aiekick"]["Another Cloudy Terrain"] = "https://www.shadertoy.com/view/XtSSRK";
	puSampleUrls["ShaderToy"]["aiekick"]["Another Cloudy Tunnel"] = "https://www.shadertoy.com/view/4lSXRK";
	puSampleUrls["ShaderToy"]["aiekick"]["Another Cloudy Tunnel 4"] = "https://www.shadertoy.com/view/Xdd3Ds";
	puSampleUrls["ShaderToy"]["aiekick"]["Another Maze"] = "https://www.shadertoy.com/view/Mlc3D7";
	puSampleUrls["ShaderToy"]["aiekick"]["Base Colors 131c"] = "https://www.shadertoy.com/view/MdG3RK";
	puSampleUrls["ShaderToy"]["aiekick"]["Bondage Ball"] = "https://www.shadertoy.com/view/MdGSRy";
	puSampleUrls["ShaderToy"]["aiekick"]["Claw Spiral"] = "https://www.shadertoy.com/view/lt3GW4";
	puSampleUrls["ShaderToy"]["aiekick"]["Cloudy_Julia_Variante"] = "https://www.shadertoy.com/view/wsSGzG";
	puSampleUrls["ShaderToy"]["aiekick"]["Cook-Torrance Light Model"] = "https://www.shadertoy.com/view/MdVXRW";
	puSampleUrls["ShaderToy"]["aiekick"]["Crumble Spiral (211c)"] = "https://www.shadertoy.com/view/4dd3Ds";
	puSampleUrls["ShaderToy"]["aiekick"]["Cubemap to EquiRectangular"] = "https://www.shadertoy.com/view/MdtfzX";
	puSampleUrls["ShaderToy"]["aiekick"]["Curve Revolute"] = "https://www.shadertoy.com/view/lsBBW1";
	puSampleUrls["ShaderToy"]["aiekick"]["Distance Field from Points Array"] = "https://www.shadertoy.com/view/MsjyWR";
	puSampleUrls["ShaderToy"]["aiekick"]["Disturbing Maze"] = "https://www.shadertoy.com/view/Xtc3DN";
	puSampleUrls["ShaderToy"]["aiekick"]["Disturbing Maze 2"] = "https://www.shadertoy.com/view/Mlt3D7";
	puSampleUrls["ShaderToy"]["aiekick"]["Donjon Tunnel (250c)"] = "https://www.shadertoy.com/view/Xslyzf";
	puSampleUrls["ShaderToy"]["aiekick"]["Donjon Tunnel 2"] = "https://www.shadertoy.com/view/4sfyWS";
	puSampleUrls["ShaderToy"]["aiekick"]["Donjon Tunnel 3"] = "https://www.shadertoy.com/view/XdXcDS";
	puSampleUrls["ShaderToy"]["aiekick"]["Double Spirale 3 (185c)"] = "https://www.shadertoy.com/view/MsyGRw";
	puSampleUrls["ShaderToy"]["aiekick"]["Fix of [SH17C] Raymarching tutorial of Reinder"] = "https://www.shadertoy.com/view/MdSfRd";
	puSampleUrls["ShaderToy"]["aiekick"]["Flower Lattice"] = "https://www.shadertoy.com/view/llsyDf";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 12  Morphing"] = "https://www.shadertoy.com/view/4s3XDH";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 13"] = "https://www.shadertoy.com/view/XdtSWN";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 15"] = "https://www.shadertoy.com/view/MstSD4";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 16"] = "https://www.shadertoy.com/view/XscSW7";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 17"] = "https://www.shadertoy.com/view/XsdSW7";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 18 Lattice"] = "https://www.shadertoy.com/view/4s3XRB";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 19"] = "https://www.shadertoy.com/view/Ms3SRS";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 22 Leather2"] = "https://www.shadertoy.com/view/Mtt3z4";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 23"] = "https://www.shadertoy.com/view/MlcXRl";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 24"] = "https://www.shadertoy.com/view/MlKSRy";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 25"] = "https://www.shadertoy.com/view/MtVSRy";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 27"] = "https://www.shadertoy.com/view/ldfBRH";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 28"] = "https://www.shadertoy.com/view/4slfzH";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal Experiment 9"] = "https://www.shadertoy.com/view/4stXR7";
	puSampleUrls["ShaderToy"]["aiekick"]["Fractal_Experiment_29__NDP_Test"] = "https://www.shadertoy.com/view/wdsSDj";
	puSampleUrls["ShaderToy"]["aiekick"]["Frozen Ground"] = "https://www.shadertoy.com/view/llScWw";
	puSampleUrls["ShaderToy"]["aiekick"]["Frozen Swamp"] = "https://www.shadertoy.com/view/4lScRy";
	puSampleUrls["ShaderToy"]["aiekick"]["Frozen Tunnel"] = "https://www.shadertoy.com/view/MtByDm";
	puSampleUrls["ShaderToy"]["aiekick"]["Gear Revolute"] = "https://www.shadertoy.com/view/4sXyzN";
	puSampleUrls["ShaderToy"]["aiekick"]["Gear Revolute 2"] = "https://www.shadertoy.com/view/XdlyRN";
	puSampleUrls["ShaderToy"]["aiekick"]["Go_little_firefly,_Go_!"] = "https://www.shadertoy.com/view/WdsSR7";
	puSampleUrls["ShaderToy"]["aiekick"]["Headache Experiment 1"] = "https://www.shadertoy.com/view/XtVSDc";
	puSampleUrls["ShaderToy"]["aiekick"]["Headache Experiment 2"] = "https://www.shadertoy.com/view/XlVSDc";
	puSampleUrls["ShaderToy"]["aiekick"]["Headache Experiment 5"] = "https://www.shadertoy.com/view/4dScRR";
	puSampleUrls["ShaderToy"]["aiekick"]["Heat Blob"] = "https://www.shadertoy.com/view/4dVXW1";
	puSampleUrls["ShaderToy"]["aiekick"]["Heat Tunnel"] = "https://www.shadertoy.com/view/llG3Dz";
	puSampleUrls["ShaderToy"]["aiekick"]["Hot Julia Revolute"] = "https://www.shadertoy.com/view/lljyWm";
	puSampleUrls["ShaderToy"]["aiekick"]["Julia Revolute 1"] = "https://www.shadertoy.com/view/4sXczN";
	puSampleUrls["ShaderToy"]["aiekick"]["Julia Revolute 2"] = "https://www.shadertoy.com/view/4sfczN";
	puSampleUrls["ShaderToy"]["aiekick"]["Julia Revolute Glossy"] = "https://www.shadertoy.com/view/MtjczG";
	puSampleUrls["ShaderToy"]["aiekick"]["Julia_Revolute_Sliced"] = "https://www.shadertoy.com/view/WslXWr";
	puSampleUrls["ShaderToy"]["aiekick"]["Krypto Planes"] = "https://www.shadertoy.com/view/MdVBRy";
	puSampleUrls["ShaderToy"]["aiekick"]["Kryptonite Tunnel"] = "https://www.shadertoy.com/view/lsffW7";
	puSampleUrls["ShaderToy"]["aiekick"]["Lace Tunnel"] = "https://www.shadertoy.com/view/4sGSzc";
	puSampleUrls["ShaderToy"]["aiekick"]["Maze Filling"] = "https://www.shadertoy.com/view/lttGWM";
	puSampleUrls["ShaderToy"]["aiekick"]["Maze Spiral"] = "https://www.shadertoy.com/view/4tt3W7";
	puSampleUrls["ShaderToy"]["aiekick"]["Maze Wallpaper 1"] = "https://www.shadertoy.com/view/4td3D7";
	puSampleUrls["ShaderToy"]["aiekick"]["MegaWave 4"] = "https://www.shadertoy.com/view/MljSRy";
	puSampleUrls["ShaderToy"]["aiekick"]["MegaWave 5"] = "https://www.shadertoy.com/view/Xl3SzH";
	puSampleUrls["ShaderToy"]["aiekick"]["Menger Sponge Var With Displace"] = "https://www.shadertoy.com/view/MsV3zG";
	puSampleUrls["ShaderToy"]["aiekick"]["Page Curl Effect Experiment 3"] = "https://www.shadertoy.com/view/XdVGRD";
	puSampleUrls["ShaderToy"]["aiekick"]["Page Curl Effect on Ball"] = "https://www.shadertoy.com/view/XtX3R2";
	puSampleUrls["ShaderToy"]["aiekick"]["Phantom Mode"] = "https://www.shadertoy.com/view/MtScWW";
	puSampleUrls["ShaderToy"]["aiekick"]["Polygon Voronoi"] = "https://www.shadertoy.com/view/ld2czw";
	puSampleUrls["ShaderToy"]["aiekick"]["Pseudo Invaders"] = "https://www.shadertoy.com/view/MdKGRz";
	puSampleUrls["ShaderToy"]["aiekick"]["RD_Waves"] = "https://www.shadertoy.com/view/3sf3WN";
	puSampleUrls["ShaderToy"]["aiekick"]["RM Study  Convergence S vs T"] = "https://www.shadertoy.com/view/XdcXDS";
	puSampleUrls["ShaderToy"]["aiekick"]["Rainbow Things"] = "https://www.shadertoy.com/view/ld3fzf";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 2"] = "https://www.shadertoy.com/view/4ll3Wn";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 23"] = "https://www.shadertoy.com/view/4lj3Ww";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 29"] = "https://www.shadertoy.com/view/XlfXDM";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 43"] = "https://www.shadertoy.com/view/lstXRl";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 44"] = "https://www.shadertoy.com/view/ls3XWB";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 46"] = "https://www.shadertoy.com/view/lsKSDm";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 47"] = "https://www.shadertoy.com/view/lsVSRz";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 50"] = "https://www.shadertoy.com/view/4syXRK";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 51"] = "https://www.shadertoy.com/view/lsySRV";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 52"] = "https://www.shadertoy.com/view/MdVXDV";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 55"] = "https://www.shadertoy.com/view/MdKSDV";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 56"] = "https://www.shadertoy.com/view/MlyGWz";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 59"] = "https://www.shadertoy.com/view/4tyGW3";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 60"] = "https://www.shadertoy.com/view/lttXDn";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 62"] = "https://www.shadertoy.com/view/4tyXDd";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 66"] = "https://www.shadertoy.com/view/4tK3Wz";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 67"] = "https://www.shadertoy.com/view/lsyXRG";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 69"] = "https://www.shadertoy.com/view/MlXyzS";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 70"] = "https://www.shadertoy.com/view/XllczX";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment 71"] = "https://www.shadertoy.com/view/4tXyDl";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment n�33bis"] = "https://www.shadertoy.com/view/MljSRD";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment n�37"] = "https://www.shadertoy.com/view/MsyGDz";
	puSampleUrls["ShaderToy"]["aiekick"]["Ray Marching Experiment n�76"] = "https://www.shadertoy.com/view/4stcRr";
	puSampleUrls["ShaderToy"]["aiekick"]["ReactDiff Experiment 1"] = "https://www.shadertoy.com/view/XtlcDl";
	puSampleUrls["ShaderToy"]["aiekick"]["Reaction Diffusion 6"] = "https://www.shadertoy.com/view/MdKyWW";
	puSampleUrls["ShaderToy"]["aiekick"]["ReactionDiffusion 3"] = "https://www.shadertoy.com/view/MlByzR";
	puSampleUrls["ShaderToy"]["aiekick"]["ReactionDiffusion 4 Field"] = "https://www.shadertoy.com/view/MtByRz";
	puSampleUrls["ShaderToy"]["aiekick"]["ReactionDiffusion 5"] = "https://www.shadertoy.com/view/XtBcR1";
	puSampleUrls["ShaderToy"]["aiekick"]["Rock Tunnel "] = "https://www.shadertoy.com/view/Mty3W3";
	puSampleUrls["ShaderToy"]["aiekick"]["Rocky Tunnel 2"] = "https://www.shadertoy.com/view/llGGWc";
	puSampleUrls["ShaderToy"]["aiekick"]["SSS Blob"] = "https://www.shadertoy.com/view/XdyXWG";
	puSampleUrls["ShaderToy"]["aiekick"]["SSS Bondage Ball"] = "https://www.shadertoy.com/view/MsVXzd";
	puSampleUrls["ShaderToy"]["aiekick"]["SSS Fractal"] = "https://www.shadertoy.com/view/4dyXWc";
	puSampleUrls["ShaderToy"]["aiekick"]["SSS Light Study 3"] = "https://www.shadertoy.com/view/XlcGzn";
	puSampleUrls["ShaderToy"]["aiekick"]["SSS Light Study 8"] = "https://www.shadertoy.com/view/ltfyDn";
	puSampleUrls["ShaderToy"]["aiekick"]["Shaded_Fluid"] = "https://www.shadertoy.com/view/4lyBW1";
	puSampleUrls["ShaderToy"]["aiekick"]["ShowerDoor Effect 118c"] = "https://www.shadertoy.com/view/XdGXDt";
	puSampleUrls["ShaderToy"]["aiekick"]["Skeletization Attempt"] = "https://www.shadertoy.com/view/MdByzW";
	puSampleUrls["ShaderToy"]["aiekick"]["Smooth RadClick Cristal Lattice"] = "https://www.shadertoy.com/view/MlyGDd";
	puSampleUrls["ShaderToy"]["aiekick"]["Smooth RadClick Cristal Tunnel"] = "https://www.shadertoy.com/view/XtyGWt";
	puSampleUrls["ShaderToy"]["aiekick"]["Sub Earth Tunnel"] = "https://www.shadertoy.com/view/lsyBR3";
	puSampleUrls["ShaderToy"]["aiekick"]["Subo Glacius 2"] = "https://www.shadertoy.com/view/MsGGWy";
	puSampleUrls["ShaderToy"]["aiekick"]["Tadpole"] = "https://www.shadertoy.com/view/MdXBRs";
	puSampleUrls["ShaderToy"]["aiekick"]["Tas d'poles"] = "https://www.shadertoy.com/view/Mslfzl";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty"] = "https://www.shadertoy.com/view/Mt3GW2";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 2"] = "https://www.shadertoy.com/view/MlVGRR";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 3"] = "https://www.shadertoy.com/view/llVGRR";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 4"] = "https://www.shadertoy.com/view/MtKGDD";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 5"] = "https://www.shadertoy.com/view/4t3XWn";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 6"] = "https://www.shadertoy.com/view/4l3SzH";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 7"] = "https://www.shadertoy.com/view/XltSDn";
	puSampleUrls["ShaderToy"]["aiekick"]["Tunnel Beauty 8"] = "https://www.shadertoy.com/view/lsfyW4";
	puSampleUrls["ShaderToy"]["aiekick"]["Turlututu Tryptich"] = "https://www.shadertoy.com/view/4tVXWt";
	puSampleUrls["ShaderToy"]["aiekick"]["Voro Ball Reduced"] = "https://www.shadertoy.com/view/lscXDf";
	puSampleUrls["ShaderToy"]["aiekick"]["Voro Tri Tunnel"] = "https://www.shadertoy.com/view/XtGGWy";
	puSampleUrls["ShaderToy"]["aiekick"]["Voronoi Experiment 10"] = "https://www.shadertoy.com/view/4lVSzK";
	puSampleUrls["ShaderToy"]["aiekick"]["Voronoi Experiment 7 381c"] = "https://www.shadertoy.com/view/llKSzy";
	puSampleUrls["ShaderToy"]["aiekick"]["Voronoi Experiment 8 Lighted"] = "https://www.shadertoy.com/view/XlyXRK";
	puSampleUrls["ShaderToy"]["aiekick"]["Voronoi Experiment 9 241c 249c"] = "https://www.shadertoy.com/view/MtyXRK";
	puSampleUrls["ShaderToy"]["aiekick"]["Warp My Britney"] = "https://www.shadertoy.com/view/MtjyWh";
	puSampleUrls["ShaderToy"]["aiekick"]["Warp My Britney 2"] = "https://www.shadertoy.com/view/ll2cWh";
	puSampleUrls["ShaderToy"]["aiekick"]["Wax Tunnel"] = "https://www.shadertoy.com/view/MsySWz";
	puSampleUrls["ShaderToy"]["aiekick"]["Weird Fractal 0"] = "https://www.shadertoy.com/view/Xts3RB";
	puSampleUrls["ShaderToy"]["aiekick"]["Weird Fractal 7"] = "https://www.shadertoy.com/view/MsK3Ry";
	puSampleUrls["ShaderToy"]["aiekick"]["Weird Lattice 2"] = "https://www.shadertoy.com/view/4l3SRr";
	puSampleUrls["ShaderToy"]["aiekick"]["Weird Shapes Grid"] = "https://www.shadertoy.com/view/MtKGWR";
	puSampleUrls["ShaderToy"]["aiekick"]["Whorl"] = "https://www.shadertoy.com/view/lsXcRH";
	puSampleUrls["ShaderToy"]["aiekick"]["Wire Ball"] = "https://www.shadertoy.com/view/XsVSRy";
	puSampleUrls["ShaderToy"]["aiekick"]["Yin vs Yang"] = "https://www.shadertoy.com/view/MlXBzf";
	puSampleUrls["ShaderToy"]["aiekick"]["[NV15] Tiny Cutting Planet"] = "https://www.shadertoy.com/view/4ljGRh";
	puSampleUrls["ShaderToy"]["aiekick"]["[SH17A] Particles"] = "https://www.shadertoy.com/view/XsBBWR";
	puSampleUrls["ShaderToy"]["aiekick"]["[SH17A] Particles 2"] = "https://www.shadertoy.com/view/XdBfDR";
	puSampleUrls["ShaderToy"]["aiekick"]["[SH17A] Particles 3"] = "https://www.shadertoy.com/view/Xs2BWR";
	puSampleUrls["ShaderToy"]["aiekick"]["[SH17A] Particles 4"] = "https://www.shadertoy.com/view/4djBDR";
	puSampleUrls["ShaderToy"]["aiekick"]["[SH17A] Reflected Ball"] = "https://www.shadertoy.com/view/4sjfWz";
	puSampleUrls["ShaderToy"]["aiekick"]["[SH17A] Rocky Planes"] = "https://www.shadertoy.com/view/XsjBWR";
	puSampleUrls["ShaderToy"]["aiekick"]["_Round_Voronoi_Border__Fork"] = "https://www.shadertoy.com/view/tslXWX";
	puSampleUrls["ShaderToy"]["aiekick"]["hexagonal truchet on GridClick"] = "https://www.shadertoy.com/view/Mst3D4";
	puSampleUrls["ShaderToy"]["kuvkar"]["BloodCells"] = "https://www.shadertoy.com/view/4ttXzj";
	puSampleUrls["ShaderToy"]["otaviogood"]["stochastic path tracer v1"] = "https://www.shadertoy.com/view/4ddcRn";
	puSampleUrls["ShaderToy"]["paniq"]["Light Propagation Volume"] = "https://www.shadertoy.com/view/XdtSRn";
	puSampleUrls["ShaderToy"]["tomkh"]["Random Delaunay Triangulation"] = "https://www.shadertoy.com/view/4sKyRD";
	puSampleUrls["VertexShaderArt"]["P_Malin"]["Planet Polygon"] = "https://www.vertexshaderart.com/art/X74uYKiDLZJmqhLnh";
	puSampleUrls["VertexShaderArt"]["aiekick"]["Pics Danse"] = "https://www.vertexshaderart.com/art/SAToMsqc7PybLLEc2";
	puSampleUrls["VertexShaderArt"]["aiekick"]["ShaderDough"] = "https://www.vertexshaderart.com/art/oJEAooRzirpb8qcPe";
	puSampleUrls["VertexShaderArt"]["aiekick"]["Torus Bulb"] = "https://www.vertexshaderart.com/art/b2AspWSvDsXib8Kkc";
	puSampleUrls["VertexShaderArt"]["aiekick"]["Triangular Voronoi"] = "https://www.vertexshaderart.com/art/iXKHFGqaAHrc6rZg2";
	puSampleUrls["VertexShaderArt"]["aiekick"]["Twisted Torus"] = "https://www.vertexshaderart.com/art/JscBDhcAFypHdaMCm";
	puSampleUrls["VertexShaderArt"]["gman"]["bwow"] = "https://www.vertexshaderart.com/art/6Yx2A7TQ6NnyHhFPQ";
	puSampleUrls["VertexShaderArt"]["kabuto"]["point cloud vs spheres"] = "https://www.vertexshaderart.com/art/nL6YpkW8YvGKNEKtj";
}

void UrlLibrarySystem::DrawImGui_Library()
{
	if (ImGui::BeginMenu("From Samples Url"))
	{
		for (auto it_platform = puSampleUrls.begin(); it_platform != puSampleUrls.end(); ++it_platform)
		{
			if (ImGui::BeginMenu(it_platform->first.c_str()))
			{
				for (auto it_user = it_platform->second.begin(); it_user != it_platform->second.end(); ++it_user)
				{
					if (ImGui::BeginMenu(it_user->first.c_str()))
					{
						for (auto it_url = it_user->second.begin(); it_url != it_user->second.end(); ++it_url)
						{
							if (ImGui::MenuItem(it_url->first.c_str()))
							{
								MainFrame::Instance()->OpenImportBar(it_url->second);
							}
						}

						ImGui::EndMenu();
					}
				}

				ImGui::EndMenu();
			}
		}

		ImGui::EndMenu();
	}
}

std::string UrlLibrarySystem::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

#ifdef _DEBUG
	std::string libraryCppFile = "";
#endif

	str += vOffset + "<UrlLibrarySystem>\n";
	const auto offset = vOffset + vOffset;

	for (auto it_platform = puSampleUrls.begin(); it_platform != puSampleUrls.end(); ++it_platform)
	{
		str += offset + "<platform value=\"" + it_platform->first + "\">\n";
		for (auto it_user = it_platform->second.begin(); it_user != it_platform->second.end(); ++it_user)
		{
			str += offset + "\t<user value=\"" + it_user->first + "\">\n";
			for (auto it_url = it_user->second.begin(); it_url != it_user->second.end(); ++it_url)
			{
				str += offset + "\t\t<url desc=\"" + it_url->first + "\" url=\"" + it_url->second + "\"/>\n";
#ifdef _DEBUG
				libraryCppFile += "puSampleUrls[\"" + it_platform->first + "\"][\"" + it_user->first + "\"][\"" + it_url->first + "\"] = \"" + it_url->second + "\";\n";
#endif
			}
			str += offset + "\t</user>\n";
		}
		str += offset + "</platform>\n";
	}

	str += vOffset + "</UrlLibrarySystem>\n";

#ifdef _DEBUG
#ifdef USE_NETWORK
	if (!libraryCppFile.empty())
	{
		FileHelper::Instance()->SaveToFile(libraryCppFile, "libraryCppFile.h", (int)FILE_LOCATION_Enum::FILE_LOCATION_APP);
	}
#endif // #ifdef USE_NETWORK
#endif

	return str;
}

bool UrlLibrarySystem::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	// The value of this child identifies the name of this element
	std::string strName = "";
	std::string strValue = "";
	std::string strParentName = "";

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strName == "UrlLibrarySystem")
	{
		for (auto child = vElem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
		{
			RecursParsingConfig(child->ToElement(), vElem);
		}
	}
	else
	{
		if (strName == "platform" && strParentName == "UrlLibrarySystem")
		{
			for (auto attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				std::string attName = attr->Name();
				std::string attValue = attr->Value();

				if (attName == "value")
					puUrlLibrarySystemPlatform = attValue;
			}
		}

		if (strName == "user" && strParentName == "platform")
		{
			for (auto attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				std::string attName = attr->Name();
				std::string attValue = attr->Value();

				if (attName == "value")
					puUrlLibrarySystemUser = attValue;
			}
		}

		if (strName == "url" && strParentName == "user")
		{
			for (auto attr = vElem->FirstAttribute(); attr != nullptr; attr = attr->Next())
			{
				std::string attName = attr->Name();
				std::string attValue = attr->Value();

				if (attName == "desc")
					puUrlLibrarySystemDesc = attValue;
				if (attName == "url")
					puUrlLibrarySystemUrl = attValue;
			}

			if (!puUrlLibrarySystemUrl.empty())
			{
				puSampleUrls[puUrlLibrarySystemPlatform][puUrlLibrarySystemUser][puUrlLibrarySystemDesc] = puUrlLibrarySystemUrl;
			}
		}
	}

	return false;
}