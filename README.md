# Polydeukes
Polydeukes is a 3d graphics renderer and test bed. It's goal is to create a flexible wrapper for an opengl backend, enabling rapid extension and exploration of the 3d graphics world.

At this time Polydeukes can only be built on MacOS. There are no plans to make this jawn cross platform. It's really just for fun and for sharing neat images and animations with friends. 

Polydeukes includes the following modules
1. A CPU based particle system and physics engine framework.
2. A .bvh interpreter (motion capture)
3. A CPU based spline curve system which can edit and interpret polynomial interpolation, hermite, and bezier spline curves.
4. A GPU based spline system which can edit and interpret bezier spline curves and surfaces. Also includes a .bpt interpreter
5. A chip-8 interpreter
6. A very basic obj file interpreter
7. A font engine (lol)

### Future
I'd like to beef up the obj file interpreter and make a rigging module for animations. Also, each of the existing modules can be improved by adding user facing tooling. For example, the chip8 interpret doesn't allow you to switch the game you're playing without restarting the app and hard coding it. Keeping the user "on the rails" doesn't exist and lots of activites cause crashing: if you try to render a quadric or higher interpolation curve, we crash. 

## Build Instructions

### Download glad
1. https://glad.dav1d.de/
2. Core mode
3. Generate a loader checked
4. Language C/C++
5. API gl version 4.1

### Download glfw
1. https://www.glfw.org/download
2. macOS pre-compiled binaries

### Download glm
1. https://github.com/g-truc/glm/releases/tag/1.0.1

### Create the xcode project
1. xcode -> open existing project -> double click the xcode project in the top level directory of this repo. 

### Add downloaded assets to library and header search paths
1. Replace glm folder to both debug and build header search paths with your location
2. Replace glfw include folder to both debug and build header search paths
3. Replace glad include folder to both debug and build header search paths
4. Replace glfw build/src/Debug folder to both debug and build library search paths

### Link libraries
1. Navigate to build phases tab of xcode project settings
2. Add IOKit.framework, OpenGL.framework, Cocoa.framework, and glfw.a in Link Binary with Libraries menu. 
