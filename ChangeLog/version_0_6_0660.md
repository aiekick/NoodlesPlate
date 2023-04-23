---
sidebar_position: -8
---

# NoodlesPlate 0.6.0660

[NoodlesPlate 0.6.0660](https://github.com/aiekick/NoodlesPlate/releases/tag/v0.6.660)

support of the VR (via OpenXR) :
  * a button in the left toolbar is available for enable/disable the vr 
  * display the left eye on the screen
  * support of controllers : 
    * Generic pads (tested => OK)
    * Occulus touch (tested => OK)
    * Valve index (not tested => but will work as expcted i guess)
  * camera can be controlled via VR controllers, if the camera vr control is checked and only when swizzle (side trigger) is pressed :
    * left controller thumbstick x can be used for roll (axis z) the camera
    * left controller trigger can be used for move the camera backward
    * right controller trigger can be used for move the camera forward
    * right controller thumbstick up/down can be used for pitch (axis x) the camera
    * right controller thumbstick left/right can be used for yaw (axis y) the camera
  * the triggers, thumbsticks, and swizzle are available as uniforms. 
    swizzle can be used if the camera vr control is unchecked
    check the help avaialble in elft toolabr for have the unifomrs syntax

```
[VR] : draw the VR FBO on screen
[VR] : add support of controller : kronos generic, oculus touch and valve index (no mesh, just control)
[VR] : add camera control view left and right thumbstick XY (can be enabled or disabled)
[VR] : add uniforms for trigger, squeese, thumbs 
       thumbs are not filled when the camera is used and 
	   when the camera via vr is enabled
[VR] : add camera factor for rotation and zoom when using vr controllers
[VR] : add factor for uniforms vr::thumb:accum
[ADD] : add rotation around Z for camera
[ADD] : add a lock for translation and rotation. by default translation XYZ and rotation Z are locked
[ADD] : add a Free Camera Mode
[FIX] : fix time widget speed between modes
[FIX] : fix and refactor of recent files system
```
