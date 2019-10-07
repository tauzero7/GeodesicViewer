
# GeodesicViewer

GeodesicViewer was first developed by Thomas Müller and Frank Grave at the 
Visualization Research Center, University of Stuttgart, Germany (2009-2015).

Current contact:  
Dr. Thomas Müller  
Haus der Astronomie/Max Planck Institute for Astronomy  
69117 Heidelberg, Germany  
Email: tmueller@mpia.de  


## Before installation:

The GeodesicViewer is based on the Qt framework (version 5.9), the open graphics 
library (OpenGL), and the Motion4D-library. On standard Linux machines, OpenGL 
is already installed via the graphics board drivers. Qt 5.9 can be downloaded 
from the developer site as a LGPL-licensed version. The Motion4D-library also 
needs the GNU Scientific Library (GSL).

Note that you need a graphics card that supports the OpenGL Shading language 
(GLSL) version 2. If you start the GeodesicViewer from a console, a message 
will be print on the standard output whether your graphics card supports GLSL 
or not.


The following libraries have to be installed before compiling the GeodesicViewer:

* Qt >= 5.9  
  The LGPL-licensed version of the Qt SDK can be downloaded from 
  http://download.qt.io/archive/qt/
   
* GNU Scientific library  
  http://www.gnu.org/software/gsl/  
  For Windows, you should use https://github.com/ampl/gsl

* Motion4D library  
  A detailed discussion of the Motion4D library can be found here:
  DOI: 10.1016/j.cpc.2009.07.014
  The Motion4D library is delivered with the GeodesicViewer.
  
* Freetype  
  https://sourceforge.net/projects/freetype/  
  Freetype is necessary to render text into the OpenGL window. 


## Installation:

1. Start the QtCreator and open the file "gviewer_m4d.pro".
    
2. Edit the following variables in "local.pri" depending on your system:
    
    . M4D_DIR, M4D_SRC_DIR, M4D_LIB_DIR
      Root path to the Motion4D library source directory (src).
      In the "Projects"-Tab of the Edit window you should find all header
      and source files of the Motion4D library, otherwise the M4D_DIR 
      variable was not correctly set.
       
    . GSL_DIR, GSL_LIB_DIR  
      Path to the GNU Scientific library.

    . DEFINES += HAVE_FREETYPE,  
      FREETYPE_DIR, FREETYPE_LIB_DIR

    . unix:LIBS += -LGL -LGLU  
      win32:  
      macx:  
 
3. Switch to the "Project"-Tab and deactivate "Shadow-Build".
      
4. Build All.


## Examples:

The GeodesicViewer starts with an empty OpenGL-3D view. Now, you can either
load a specific setting with "File/Load Setting" or you can follow the examples
in the article 

T. Mueller and F. Grave, 
"Motion4D - A library for lightrays and timelike worldlines in the theory 
of relativity", 
Comput. Phys. Commun. 180, 2355-2360 (2009) 
(Sec. 4.1.1):


### Light ray in Schwarzschild spacetime:

1. Select "Schwarzschild" in the "Metric"-Tab of "Metric/Integrator/Constants".
 
2. Within the "Position"-Tab of "Local Tetrad" set the initial position to  
    [ t=0, r=10, theta=1.5707963, phi=0 ].
     
3. Set the local reference frame in the "Tetrad Directions"-Tab:  

            1  0  0  0  
            0 -1  0  0  
            0  0  0  1  
            0  0  1  0  
     
    Press "set" button.  
    (This transformation defines the local reference frame with respect to the
      natural local tetrad of the spacetime.)
    You could also use "inward" as tetrad default.
       
4. Set the angle xi in "Direction Angles"-Tab of "Geodesic" to 27.9935 (degree) 
   and the angle chi to 90.0 (degree). 
   The type of the Geodesic should be "lightlike".
     
5. Press "z" within the OpenGL 3D window to see the geodesic from top.
     

