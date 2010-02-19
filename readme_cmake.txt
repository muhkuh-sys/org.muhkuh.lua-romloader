Creating the MSVS project files
-------------------------------

1) Move to the muhkuh source directory (that's the folder where this readme is).

2) Create a new folder there and change to the new folder.

 mkdir compile
 cd compile

3) Run cmake. If swig is not in your path, point cmake to the swig executable. The second example shows how to do this.

Run cmake with swig in path:
 cmake ..

Run cmake with swig not in the path. Here the swig.exe is in c:\tools\swigwin-1.3.40\ . Do not forget to set the filename of the swig executable as well:
 cmake -D SWIG_EXECUTABLE=c:\tools\swigwin-1.3.40\swig.exe ..


