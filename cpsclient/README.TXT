#########################################################

       cps client application installation

       (c) 2018, zhusizhi Development Systems 

#########################################################

------------------
     SUMMARY
------------------

 1 - REQUIREMENTS
 2 - INSTALLED FILE STRUCTURE
 3 - BUILD cpsclient
 4 - SUPPORT

1 - REQUIREMENTS
----------------

SOFTWARE REQUIREMENTS

 - libc/glibc, the standard C Library (min. v2)
 - POSIX threads library (POSIX threads enabled libc)
 - Qt4 library, at least Qt 4.5.2
 - Qt4 build tools (e.g. qmake)

Versions prior to Qt 4.5.2 might also work, but they are untested. We suggest to
download the latest Qt4 version from http://www.qtsoftware.com/downloads


2 - INSTALLED FILE STRUCTURE
----------------------------

For compatibility reasons, the cps client application is delivered as source code 
package and must be compiled on the target host. The source code is installed together
with the GigE cps Linux software (driver and SDK) and can be found at 
/usr/src/cps/cpsclient.

The directory contains the following files:

images/*                    Bitmaps and icons for the application's GUI
src/*                       The application's C++ sources
mainview.qrc                Qt ressource definitions
mainview.ui                 Qt main window UI definition
properties.ui               Qt properties dialog UI definition
cpsclient.doxyfile          Doxygen control file for documentation
cpsclient.pro               Qt profile definition
README.TXT                  This readme file


3 - BUILD cpsclient
----------------------

We recommend to create a copy of the source files in a working directory, so you won't have
to be root to build cpsclient.

a - Locate the proper version of qmake in case you have installed multiple Qt4 versions
b - Copy the source files to a working directory, for example ~/cpsclient:
        $> cp -R /usr/src/cps/cpsclient ~/cpsclient
c - Go to the build directory
        $> cd ~/cpsclient
d - Run qmake (use explicit binary path if the proper qmake version is not in your $PATH)
        $> qmake
e - Build the application
        $> make

The cpsclient should compile without errors. After successful build, the binary can be found
at the top of the previously choosed working directory. To install, simply copy the binary to
/usr/bin or adjust your $PATH environment variable.

4 - SUPPORT
-----------

The cpsclient is a demonstration application intended to demonstrate the cps product 
features. It is released under a free license and is not officially supported. If you encounter
problems that might come from the cps library, please contact your distributor or 
the cps Imaging Development Systems GmbH support.
Please read the support instructions that come with your driver package.
