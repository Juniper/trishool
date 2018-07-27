Trishool is a Malware Reversal and Instrumentation Toolkit.

* Development
** Checking out the source code

   While holding the code/source/repo on the filesystem, make sure the word
   trishool is the name of the root folder only, and the word isn't
   there anywhere else in the path.

   For example,

   Correct:
   /home/user/development/repos/trishool
   /home/user/development/trishool

   Wrong:
   /home/user/trishool/repos/trishool - trishool is seen twice here

** Downloading 3rdparty dependencies

   Intel PIN isn't currently shipped along with Trishool.  To build the
   tool along with Intel PIN support, you will have to download Intel PIN
   from https://software.intel.com into ROOT/3rdparty-dependencies folder.
   To accomodate the download you will have to update the variable
   PIN_VERSION in ROOT/3rdparty-dependencies/pin/Makefile.

   DynamoRio isn't currently shipped along with Trishool.  To build the
   tool along with DynamoRio support, you will have to download DynamoRIO
   from http://www.dynamorio.org/ into ROOT/3rdparty-dependencies folder.
   To accomodate the download you will have to update the variable
   DRIO_VERSION in ROOT/3rdparty-dependencies/drio/Makefile.

** Build environment Setup

   For development and building the tool you need Cygwin installed, with
   MS Visual Studio environment loaded into cygwin's environment/bash.

   Set the environment variable TRISHOOL_BUILD_ROOT to a linux style
   path inside cygwin bash so that all the compiled and built binaries
   go to this directory.

** Build Command

   From the root directory run "make".

   If you want Trishool to use Intel PIN, use "make BT_PLATFORM=pin"
   If you want Trishool to use DynamoRIO, use "make BT_PLATFORM=drio"
