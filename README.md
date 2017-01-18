# BranchToolVR

A tool designed to map vascular systems present in medical images in virtual reality. The previous non-vr iteration of the tool can be found here: https://github.com/microdr/Branch-Tool

## TODO list:
* continue work on vein tracing using controller position
* recreate old ui in imgui
	* <sup>isovalue sliders</sup>
	* <sup>orthoslice slider</sup>
	* <sup>isovalue sliders</sup>
	* <sup>isovalue sliders</sup>
* transparent object depth sorting
* auto detect sensible windowing values
* data set testing
* triangulate vein structures
* export triangulation
* add isovalue coloring to static mesh
* refactor shaders
* combine proj and view matrices on cpu
* refactor screen ui
* sort points by isovalue for faster surface lookup
* refactor code from other sources
* identify performance bottlenecks
* repository features
	* <sup>add compile instructions</sup>
	* <sup>create libraries zip in download section</sup>
	* <sup>uml documentation</sup>


## Libraries used:
* OpenGL
* OpenVR
* GLFW
* GLM
* imgui
* imebra

