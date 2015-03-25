// ----------------------------------------------------------
//  first_example.js
// ----------------------------------------------------------

gv.reset()

var waitSeconds = 0.1

// Set the Schwarzschild metric
gv.setMetric("SchwarzschildIsotropic")

// Set the initial position
lt.setPosition(0.0, 10.0, 6.0, 0.0)

gv.wait(waitSeconds)

// Set the initial direction
ksi.set(180.0)

gv.wait(waitSeconds)

// Set max num points
gv.setSolverParam("max points",6000)

gv.wait(waitSeconds)

// add objects
obj.addObject("sphere3d 0.0 0.0 0.0  2.0  40.0 40.0  0.5 0.0 0.0")
gv.wait(waitSeconds)
obj.addObject("box3d    0.0 0.0 0.0 20.0 12.0 10.0  1.0  1.0 1.0 1.0")

/*
var y
for(y=6.0; y>4.0; y-=0.01) {
  lt.setPosition(0.0, 10.0, y, 0.0)
  gv.wait(0.11)
}
*/