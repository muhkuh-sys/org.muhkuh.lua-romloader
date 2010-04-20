iCreating the MSVS project files
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


Select the build type
---------------------

The project can be build in "Release" or "Debug" mode. Release is optimized code without debug information. "Debug" is not optimized but with debug information.
The MSVS project files offer both configurations. Unix Makefiles can only offer one build type. You have to select the build type when generating the Makefile or change it by regenerating the Makefile.

Generating a Makefile with Debug build type:

 cmake -D CMAKE_BUILD_TYPE=Debug <other_options> ..

Regenerate the Makefiles with another build type:
Just run cmake with the path to the source root (that's the "..") and the build type. All other options are set to the values from the last execution of cmake.

 cmake -D CMAKE_BUILD_TYPE=Debug ..


Verbose Output
--------------

Execute the Makefile with verbose output to see the complete commands instead of the summary:
  make VERBOSE=1

