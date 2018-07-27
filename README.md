Trishool is a Malware Reversal and Instrumentation Toolkit.

* Development
** Checking out the source code

   While holding the code/source/repo on the filesystem, make sure the word
   trishool is the name of the last folder in the path, and the word isn't
   there anywhere else in the path other than the last folder.

   For example,

   Correct:
   /home/user/development/repos/trishool
   /home/user/development/trishool

   Wrong:
   /home/user/development/trishool/repos/trishool - trishool is seen twice here

** Build environment Setup

   For development and building the tool you need Cygwin installed, with
   MS Visual Studio environment loaded into cygwin's environment/bash.

   To build set the environment variable TRISHOOL_BUILD_ROOT to a linux style
   path inside cygwin bash

** Build Command

   From the root directory run make.
