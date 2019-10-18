
# GeodesicViewer

GeodesicViewer was first developed by Thomas Müller and Frank Grave at the 
Visualization Research Center, University of Stuttgart, Germany (2009-2015).

Current contact:  
Dr. Thomas Müller  
Haus der Astronomie/Max Planck Institute for Astronomy  
69117 Heidelberg, Germany  
Email: tmueller [at] mpia.de  


## Before installation:

The GeodesicViewer is based on the Qt framework (version 5.9), the open graphics 
library (OpenGL), and the Motion4D-library. On standard Linux machines, OpenGL 
is already installed via the graphics board drivers. Qt 5.9 can be downloaded 
from the developer site as a (L)GPL-licensed version. The Motion4D-library also 
needs the GNU Scientific Library (GSL).

Note that you need a graphics card that supports the OpenGL Shading language 
(GLSL) version 2 or higher. If you start the GeodesicViewer from a console, 
a message will be print on the standard output whether your graphics card 
supports GLSL or not.


The following libraries have to be installed/downloaded before compiling the 
GeodesicViewer:

* GNU Scientific library  
  http://www.gnu.org/software/gsl/  
  For Windows, you should use https://github.com/ampl/gsl
  
* Motion4D library  
  https://github.com/tauzero7/Motion4D  
  (just download, compilation is done together with GeodesicViewer)
    
* Qt >= 5.9  
  The (L)GPL-licensed version of the Qt SDK can be downloaded from  
  http://download.qt.io/archive/qt/
    
* Freetype  
  https://sourceforge.net/projects/freetype/  
  Freetype is necessary to render text into the OpenGL window.  


## Installation:

1. Clone or download GeodesicViewer and Motion4D library.

2. Switch to GeodesicViewer folder and copy "local_template.pri" to "local.pri".   
    
    - Change "M4D_DIR" such that it points to the root folder of the Motion4D 
      folder; e.g. "/home/username/libMotion4D"
    
    - Change "GSL_DIR" such that it points to the root folder of the GSL library.

    - If you have freetype available, uncomment the lines below "freetype".
      Change the "FREETYPE_DIR" such that it points to the root folder of the
      freetype library. You might also have to adapt "FREETYPE_LIB_DIR" and the
      name of the library "-lfreetyped" or "-lfreetype".

3. Start QtCreator and open the file "gviewer_m4d.pro".
    
    - Switch to "Projects"-Tab and deactivate "Shadow-Build" within the
      "Build Settings".
      
    - Build GeodesicViewer via "Build -> Run qmake" and "Build -> Build All".


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

Construct a light ray in Schwarzschild spacetime that returns to its origin.

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
     

