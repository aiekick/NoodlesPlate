---
sidebar_position: -3
---

# NoodlesPlate 0.1.3075

[NoodlesPlate 0.1.3075](https://github.com/aiekick/NoodlesPlate/releases/tag/v0.1.3075)

```
[ADD] Mac version added (x64 only)
[ADD] Linux version added (x64 only)
[ADD] Add a timeline widget :
  * inspired by the timeline of Blender3D
  * bezier curve based : three control points mode : free, aligned, symmetric
  * add key by clinking the dot on the left of uniforms in uniforms pane
  * move keys by dragging with mouse
  * you can add whole the current shader config to the timeline and clean for keep only changed uniforms after
  * use middle mouse on timebar, valuebar or view for scale the view
  * can rescale keys along time by values (Re Scale Menu)
  * you can disable gpu upload for frame whith a button
  * you can show all keys or edit channel(s) for show corrresponding curves (little checkbox on the right of names in timeline) 
  * can export current frames to pictures (video export is planned but not supported for the moment)

[ADD] Opengl chooser at start, can support :
  * 3.2 mini : Geometry Stage 
  * 4.0 : Tesselation Stage
  * 4.3 : Compute Stage

[ADD] New cross platform File Tracker system :
  * need the .glsl extention associated with your favorite Editor
  * on Linux, a bug appear with some editors, like gedit ( a file called .goutputstream-xxx is generated after file save, NDP not detect the file changes due to that)
  * Succesfully tested on Linux and Mac with atom and vscode. no problem seen on windows with many editor

[FIX] Many Bug Fixs
```
