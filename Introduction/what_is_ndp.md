---
sidebar_position: 0
---

# What is NoodlePlate

NoodlesPlate is a GLSL Shader Editor (almost an IDE) :
  * Offline : run only on Windows (for the moment), tested on win 7. Not tested on other version, but there is no reason for it not to work)
  * LightWeight : near 5 MO and no lib, just an exe, auto installed at first execution, in the directory where you put it.
  * Easy : Focused on the display and the tuning, leave editing to an external text editor of your choice (associated with .glsl file extention). i use [NotePad++](https://notepad-plus-plus.org/fr/)
  * Powerfull : you have many advanced features likes these ones :
    * Vertex, Geometry, and fragment Shader in one file
    * Can Import from url, ShaderToy (partially), VertexShaderArt (partially), GlslSandbox (Fully), ex : [here](https://twitter.com/aiekick/status/1097303744717438983)
    * a lot of widgets available for easy tuning of your uniforms, like, [sliders, checkbox, combobox](https://twitter.com/aiekick/status/1099803274180521984), [gizmo](https://twitter.com/aiekick/status/1095428778908680192), etc..
    * Support of Multi Pass Buffers
    * Can play with FrammeBuffer attachment (until 8) switchable in the ui.
    * Many primitive type : Quad (like on ShaderToy and GlslSanbox), Point (like on VertexShaderArt), and Mesh (Support only Obj File for the moment)
    * Include File Support (you can put in include file you tools, the include file can contain each Shader Sections, car ovveride parent section type, for complex imbrication if needed)
    * In-Code Config Systeme Selectable via ComboBox, let you switch between functionnality of your shader easily
    * Compute Shader Support, let you fill texture 2d on texture 3d by example (not very stable for the moment)
    * Support of mesh instancing
    * NodeGraph (experimental and not stable so pay attention when using it, i have also auto layout bug) you can see a quick vid of it [here](https://twitter.com/aiekick/status/1095138951181017088) 

I Discovered Shaders in 2014 for a game, and im in love with all the procedural stuff we can create with that since this moment.
I have designed this soft for have an easy way to tune shaders via widgets. but As a cad Designer, i not work in the GFX industry, 
so this soft, was a way i choosed for learn all the things around the gfx programming.
So maybe the way's i used here for all these features, are not the best. but i want learn, 
so if you have idea's, issue's, new feature's, please let me discuss with you about your request's on GitHub :)

I have designed this soft by passions for passionates, so i hope you will have a lot of pleasure to use it. 
Don't' hesitate to speak of it around you, its very pleasant to hear, that what we design is used and maybe enjoyed.

I have posted many video on twitter about NoodePlates, if you want to see how some features are working : [here](https://twitter.com/hashtag/noodlesplate?src=hash)

NoodlesPlate is designed with the help of many free libs, you can find the full list in the About Dialog.

Happys Codding. :)

AieKick.

