// ----------------------------------------------------------
//  second_example.js
// ----------------------------------------------------------

gv.reset()

var waitSeconds = 0.1

// Set the Morris-Thorne metric
gv.setMetric("MorrisThorne")

// Set local tetrad
lt.setPredefTetrad("inward")

dh.setType(1)
dh.setPosition(-9,-22,8)
dh.setPOI(0,0,0)
dh.setFoV(50)

dh.setScale(1.0,1.0,1.3)
dh.setFog(1,0.03)
//dh.showNumPoints(547)
