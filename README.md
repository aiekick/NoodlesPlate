# NoodlesPlate

NoodlesPlate is a offline Shader Editor. 

His particularity is to offer to the user many type of widgets, let you easilty tune in real time your shader.

all the system is designed for let the user choose his text editor. NoodlespLate update all ui after each code change (file save)

you can check the wiki if you want more infos

<ins>Some features availables :</ins>
* many uniforms widgets :
  * sliders
  * buttons
  * checkbox
  * combobox
  * radio buttons
  * time / date / deltatime
  * frame
  * gizmo (translate / rotate / scale) via [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
  * gamepad (go in settings dialog for tune and affect your gamepad buttons / sticks)
  * mouse
  * color picker
  * midi devices
* Time Line for easy animation crration (based on the behavior of blender3D)
  * export all frames in pictures
* uniforms collapsable / expandable by section or hidden (defined in code)
* Vertex / Fragment / Geometry / tesselation control / tesselation eval shaders => all in one file
* primitive instancing
* in code config (selectable in ui)
* include files
* multipass :
  * in many files
  * or in one file
* compute shader => not finalized, support only 2d/3d texture. no mesh generation
* import of ShaderToy / GlslSandbox / VertexShaderArt
  * import fo shadertoy now possible in one file only
* multi attachments buffers
* uniforms config switcher => lets you save/load uniforms conf file
* Primitve Culling System via Gizmo
* sdf merge with mesh
* wold camera (perspective / orthographic) in your shaders
* fullscreen windowed or full
* transparant window (controled with the alpha value of your shader)
* save picture for each code modification (let you create video of your shader design)
* pixel debugging, offer rgba value inspection (with mouse, under line on screen)
* inspect gpu metrics (temperature / load / memory / etc.. only support nvidia for the moment)
* gpu live profiler (flamegraph type)
* support of transform feedback for extrac mesh from vertex/geometry/tesselation stage
* Advanced UI system for extract panes to windows (thanks to Dear Imgui Docking/Viewport system)
* Advanced theming system (thanks to Dear Imgui)
* support vr (not pads for the moment, but pose and view dir is working perfectly)
<ins>Features planned :</ins>
* Node Graph (wip, not stable)
* video / audio (playback / recording)
* bvh player for uniforms

Binaries => https://github.com/aiekick/NoodlesPlate/releases

<ins>Some screenshots :</ins>

NoodlesPlate v0.5.897

![0.5.897.1](NoodlesPlate_Msvc_x64_N69aImL27C.png)
![0.5.897.2](NoodlesPlate_Msvc_x64_UXpK9TMq4S.png)
![0.5.897.3](NoodlesPlate_Msvc_x64_wPe36WwlZX.png)
![0.5.897.4](NoodlesPlate_Msvc_x64_ldzPsKLqlK.png)

NoodlesPlate v0.1.3075

![0.1.3075.1](NoodlesPlate_MSVC_x64_Release_2019-07-14_05-54-36.png)
![0.1.3075.2](NoodlesPlate_MSVC_x64_Release_2019-07-14_05-55-13.png)
