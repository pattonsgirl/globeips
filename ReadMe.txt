Use VTK class vtkGeoAssignCoordinates to visualize geo locations of IP addresses on a globe.

To Run:
****I hard coded my VTK library in CMakeLists.txt
****May need to modify on other machines
mkdir build
In build directory:
cd build
cmake ..
make
./GlobeIPs

Notes on files:

location.dat - cleaned version of locations-orig.dat (cleaned by Kijowski)
dayton-orient.dat - file of northerly points starting from Dayton, OH (created by Kijowski)
03222019-A-GlobeIPs.cxx - non-rotated version of data overlaying globe
GeoAssignCoordingates.cxx - reference file for class usage
ReadOBJ.cxx - refernce file for class usage
Also referenced: Mace.cxx for how to use glyphs


Examples referenced:

https://vtk.org/gitweb?p=VTK.git;a=blob;f=Geovis/Core/Testing/Cxx/TestGeoAssignCoordinates.cxx

ReadOBJ.cxx - taken from VTK examples.  Pass file name to read from
	Don't forget if you're in build you need to reference the correct directory ;)
