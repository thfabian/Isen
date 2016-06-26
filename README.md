# Isen - 2D Isentropic model [![Build Status]( https://travis-ci.org/thfabian/Isen.svg?branch=master)](https://travis-ci.org/thfabian/Isen) [![Build status](https://ci.appveyor.com/api/projects/status/wcuxkwaxtt3vskqu/branch/master?svg=true)](https://ci.appveyor.com/project/thfabian/isen/branch/master) [![license](https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000)](https://github.com/thfabian/Isen/blob/master/LICENSE.TXT)

*Isen* is a two dimensional isentropic vertical coordinate model derived from the existing Matlab implementation of the course *Numerical Modelling of Weather and Climate* (701-1216-00L) at ETH Zürich. The newly developed model, written in C++11, achieves a speedup factor of 49x with repsect to the Matlab code while retaining the ease of use due to a powerful Python frontend. The code is carefully optimized for modern shared-memory multiprocessors.

<p align="center">
  	<img src="https://github.com/thfabian/Isen/blob/master/doc/resources/animation.gif" alt="Horizontal velocity distribution"/>
</p>
<p align="center">
   Figure 1 - Horizontal velocity distribution of an adiabatic flow over a mountain ridge.
</p>

#### Table of Contents
  * [Build](#build)
     * [Linux](#build-linux)
     * [Mac OSX](#build-mac)
     * [Windows](#build-windows)
  * [Running Isen](#running-isen)
     * [Binary driver](#run-isen)
     * [Python Interface](#run-isenpython)

## Build <a id="build"></a>

Isen buils on all major platforms and relies on the [CMake](https://cmake.org/) build system. In addition to a working C++ toolchain (GNU, LLVM or MSVC), you need to install the following external libraries:
 
* [Boost C++ Libraries](http://www.boost.org/)
* [Eigen 3](http://eigen.tuxfamily.org/index.php?title=Main_Page)

For the Python bindings you further need Python (2.7) with the corresponding Boost.Python library.

### Linux (Ubuntu 14.04) <a id="build-linux"></a>

This section will walk you through the building process for Ubuntu 14.04. The steps can easily be adapted for other flavors of Linux.

1. Make sure you have a working C++ compiler which is capable of compiling C++11 code as well as CMake installed.

   ```bash
   sudo apt-get update
   sudo apt-get install build-essential cmake
   ``` 

2. Install the dependencies (Boost and Eigen3)

   ```bash
   sudo apt-get libboost-serialization-dev libboost-program-options-dev libboost-filesystem-dev libboost-system-dev libboost-regex-dev libboost-python-dev libeigen3-dev
   ``` 

   for the Python bindings you additionally need [Python](https://www.python.org/) with [NumPy](http://www.numpy.org/) and [matplotlib](http://matplotlib.org/)

   ```bash
   sudo apt-get install python-dev python-numpy python-matplotlib
   ``` 

3. Obtain the source code and setup build directories

   ```bash
   cd where-you-want-to-build-Isen
   git clone https://github.com/thfabian/Isen.git
   cd Isen
   mkdir build && cd build
   ```


4. Build the library

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ../
   make
   make install
   ```

   to also build the Python bindings add `-DISEN_PYTHON=ON` to the `cmake` invocation. The `make install` will only install it locally in the `Isen/bin/Linux` folder.

   After  successful compilation you will have two binaries:
   * `isen`  the binary driver of the library
   * `isen_test` a collection of the unittests
   
   You should run the unittests to assert everything is working correctly. See [Running Isen](#running-isen) for further instructions on how to run the program.


### Mac OSX (Homebrew) <a id="build-mac"></a>

This section will walk you through the building process for Mac OSX using [Homebrew](http://brew.sh/). 

1. Make sure you have the [Xcode Command Line Tools](http://railsapps.github.io/xcode-command-line-tools.html) installed.

   ```bash
   xcode-select --install
   ``` 

2. Install the dependencies (Boost, Eigen3 and CMake) via Homebrew

   ```bash
   brew update
   brew install eigen python 
   brew install boost --with-python
   brew install boost-python
   ``` 

   for the Python bindings you additionally need to install [NumPy](http://www.numpy.org/) and [matplotlib](http://matplotlib.org/) via pip

   ```bash
   export PATH=/usr/local/bin:$PATH
   pip install numpy matplotlib
   ``` 

3. Obtain the source code and setup build directories

   ```bash
   cd where-you-want-to-build-Isen
   git clone https://github.com/thfabian/Isen.git
   cd Isen
   mkdir build && cd build
   ```

4. Build the library

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ../
   make
   make install
   ```
   Building the Python bindings can be slightly tricky: You have to make sure to link against the *same* Python libraries Boost.Python was built with (otherwise you may end up with linker errors). This is usually requires the usage of the Python provided by Homebrew and *not* the one from Apple. If you haven't done so, it is advisable to add the Homebrew Python to the system path (atleast temporarily).

   ```bash
   export PATH=/usr/local/bin:$PATH
   export PYTHON_EXECUTABLE=/usr/local/bin/python
   export PYTHON_INCLUDE_DIR=$(python-config --prefix)/include/python2.7
   export PYTHON_LIBRARY=$(python-config --prefix)/lib/libpython2.7.dylib
   cmake -DCMAKE_BUILD_TYPE=Release ../ -DISEN_PYTHON=ON -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE} -DPYTHON_INCLUDE_DIR=${PYTHON_INCLUDE_DIR} -DPYTHON_LIBRARY=${PYTHON_LIBRARY}
   make
   make install
   ```

   The `make install` will only install it locally in the `Isen/bin/Darwin` folder

   After  successful compilation you will have two binaries:
   * `isen`  the binary driver of the library
   * `isen_test` a collection of the unittests
   
   You should run the unittests to assert everything is working correctly. See [Running Isen](#running-isen) for further instructions on how to run the program.

### Windows <a id="build-windows"></a>

This section will walk you through the building process on Windows using [Microsoft Visual Studio 2015](https://www.visualstudio.com/). Note that earlier versions do not fully support C++11 and are thus incompatible. In case you just want to run the code, there are precompiled binaries in `Isen/bin/Windows`.

*Word of caution: Building on Windows is signifcantly more difficult than on the other platforms and this guide might be incomplete at some points (especially on how to obtain the dependencies).*

1. Make sure you have installed the C++ compiler from Microsoft Visual Studio 2015. This has to be done manully once you downloaded and installed Visual Studio e.g File -> New Project -> Visual C++ and donwload the toolchain.   

2. Install the dependencies (Boost, Eigen3 and CMake)
   * CMake can be downloaded and installed from [here](https://cmake.org/).
   * Eigen is a header only library and can be downloaded from [here](http://eigen.tuxfamily.org/) and installed in `C:\Program Files\Eigen` for example.
   * Building the [Boost C++ Libraries](http://www.boost.org/) can be somewhat painful. In case you want to build Isen with the Python bindings, make sure you have installed Python 2.7 (e.g in `C:\Python27`).

      1. Download the newest sources of the [Boost C++ Libraries](http://www.boost.org/) and unzip them in e.g `C:\Boost`.
      
      2. Bootstrap the boost build-engine
         
         ```bash
         cd C:\Boost
         bootstrap.bat
         ```

      3. If you do *not* want to build the Python bindings, you can skip this step. Create a project specific config file `user-config64.jam` in `C:\Boost` to tell Boost about your Python installation.

         ```bash
         using python 
           : 2.7 
           : C:\\Python27\\include
           : C:\\Python27\\libs
           : <address-model>64
           ;
         ```

      4. Build & install the libraries (64 bit using Visual Studio 2015)

         ```bash
         b2.exe -j4 --toolset=msvc-14.0 address-model=64 --without-mpi --build-type=complete
         ```

         or with Boost.Python

         ```bash
         b2.exe -j4 --user-config="C:\Boost\boost_1_60_0\user-config64.jam" --toolset=msvc-14.0 address-model=64 --without-mpi --build-type=complete
         ```


         This will most likley install the libraries in `C:\Boost\lib` and the headers in `C:\Boost\boost`.

   * If you want to build the Python bindings, you also have to install [NumPy](http://www.numpy.org/) and [matplotlib](http://matplotlib.org/) via pip.


      ```bash
      C:\Python27\Scripts\pip.exe install numpy matplotlib
      ```


3. Obtain the source code via [git](https://git-scm.com/download/win) (you may have to install it) and setup build directories.

   ```bash
   cd where-you-want-to-build-Isen
   git clone https://github.com/thfabian/Isen.git
   cd Isen
   mkdir build && cd build
   ```


4. Build the library. You may have to give CMake several hints where your Boost libraries or Eigen are installed (omit or adjust them for your needs)

   ```bash
   cd Isen
   mkdir build && cd build
   cmake ../ -G "Visual Studio 14 2015 Win64" -BOOST_ROOT="C:\Boost" -DBOOST_LIBRARYDIR="C:\Boost\lib" -DEIGEN3_INCLUDE_DIR="C:\Program Files\Eigen"
   ```
   or 

   ```bash
   cd Isen
   mkdir build && cd build
   set "PYTHON_EXECUTABLE=C:\Python27\python.exe" 
   cmake ../ -G "Visual Studio 14 2015 Win64" -DISEN_PYTHON=ON -DPYTHON_EXECUTABLE="%PYTHON_EXECUTABLE%" -DBOOST_LIBRARYDIR="C:\Boost\lib" -DEIGEN3_INCLUDE_DIR="C:\Program Files\Eigen"
   ```
   if you want to build the Python bindings (assuming you have installed Python in `C:\Python27`). This will create an `Isen.sln` solution file which can be used to build the project with Visual Studio. 


## Running Isen <a id="running-isen"></a>

Isen can either be used direclty from the command-line with the binary driver `isen` (`isen.exe`) or called via the IsenPython module from Python.

### Binary driver <a id="run-ise"></a>

To run isen, you need to pass a `namelist.m` (or `namelist.py`) file containing the simulation parameters (a sample file is contained in `Isen/test/namelist.m`). Further information can be retrieved via `isen --help`.

### Python Interface <a id="run-isenpython"></a>

To use the IsenPython module you have to compile Isen with Python enabled (`-DISEN_PYTHON=ON` in `cmake`).

To import the library you should run your scripts from `Isen/python` (the building process will copy the library into the srouce tree) or if you have installed Isen, then you can run also from `Isen/bin/Linux/python` or `Isen/bin/Darwin/python`, respectively.


The following snippet shows a sample execution (see `Isen/python/example.py`).

   ```python
from IsenPython import Solver, NameList, Output
from IsenPython.Visualizer import Visualizer

# Setup Namelist
namelist = NameList()
namelist.nx = 100

# Setup & run Solver
solver = Solver()
solver.init(namelist)

solver.run()

# Plot horizontal velocity distribution
output = solver.getOutput()
visualizer = Visualizer(output)
visualizer.plot('horizontal_velocity', 6)
   ```
