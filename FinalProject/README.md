# ECE6122 Final Project

Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Dec 5, 2023

## Description

It's a C++ application that uses a custom class(es) with OpenGL and a third-party library (i.e. ASSIMP) to load and display animated 3D objects in OBJ files. The program should load and render the objects in the 3D screen. The animated 3D objects are frozen in place until the user presses the “g” key and then the objects start moving around at random speeds and rotating randomly about an axis. We have four animated 3D objects and each object’s movements are calculated in a separate thread. The objects are able to collide and bounce off each other. The objects are confined to the space around the center of the scene and cannot just float off into space.

## Environment

Mac OS 14.0

## How to install

First, I strongly encourage you to inspect my project through my video, because I didn't test these codes on PACE-ICE as professor said that we don't need to ensure we can run the code on PACE-ICE. I don't know what will happen if you encounter some complex problem when you are setting the environment.

1. Set up the Required environment, including FreeImage, Assimp, OpenGL, GLAD. I didn't include the file in the submitted zip file. You need to set by yourself.

2. Create a build file folder in the directory

3. Running cmake file

   ```
   cmake.
   ```

4. ```
   ./tutorial09_AssImp
   ```

   