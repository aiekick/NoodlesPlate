<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://raw.githubusercontent.com/aiekick/NoodlesPlate.help/main/images/NoodlesPlate-dark.svg">
    <source media="(prefers-color-scheme: light)" srcset="https://raw.githubusercontent.com/aiekick/NoodlesPlate.help/main/images/NoodlesPlate-light.svg">
    <img alt="Learn NoodlesPlate Logo" src="images/NoodlesPlate-dark.svg" width="200">
  </picture>

  <a href="https://github.com/aiekick/NoodlesPlate.help">NoodlesPlate.help</a> &nbsp;|&nbsp; <a href="https://github.com/aiekick/NoodlesPlate-Cpp">NoodlesPlate-C++</a> &nbsp;|&nbsp; <a href="https://github.com/aiekick/glfw3webgpu">glfw3webgpu</a> &nbsp;|&nbsp; <a href="https://github.com/aiekick/NoodlesPlate-binaries">NoodlesPlate-binaries</a>
</div>

Learn NoodlesPlate
============

This is the source files of the website [https://aiekick.github.io/NoodlesPlate.help](https://aiekick.github.io/NoodlesPlate.help).

Building
--------

Building the website requires Python.

1. It is recommended, but not mandatory, to set up a virtual Python environment:

```
$ virtualenv venv
$ venv/Scripts/activate
```

2. Then install Python packages

```
pip install -r requirements.txt
```

3. And finally build the website by running:

```
make html
```
