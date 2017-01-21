# BranchToolVR

A tool designed to map vascular systems present in medical images in virtual reality. The previous non-vr iteration of the tool can be found here: https://github.com/microdr/Branch-Tool

## TODO list:
* continue work on vein tracing using controller position
* recreate old ui in imgui
	* <sup>isovalue sliders</sup>
	* <sup>orthoslice slider</sup>
	* <sup>how to use instructions text</sup>
* transparent object depth sorting
* auto detect sensible windowing values
* data set testing
* clamp box within bounds
* add isovalue coloring to static mesh
* refactor shaders
* combine proj and view matrices on cpu
* refactor screen ui
* sort points by isovalue for faster surface lookup
* add object manager class
* refactor code from other sources
* re-enable shadows
* point cloud culling
* refactor class/struct destructors
* refactor class privelages
* identify performance bottlenecks
* add visual feedback to actions
* repository features
	* <sup>add compile instructions</sup>
	* <sup>create libraries zip in download section</sup>
	* <sup>uml documentation</sup>
* triangulate vein structures
* export triangulation

## Libraries used:
* OpenGL
* OpenVR
* GLFW
* GLM
* imgui
* imebra
