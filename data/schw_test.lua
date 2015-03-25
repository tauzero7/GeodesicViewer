--[[
    Testfile for GeodesicViewer
]]

SetMetric("Schwarzschild");
SetMetricParams({mass = 0.5})
--PrintMetric()

--PrintSolverDB()
SetGeodSolver("GSL_RK_Fehlberg")
SetGeodParams({stepctrl = "yes",
               stepsize = 1e-2,
               epsabs = 1e-12,
               constr = 1e-6})


SetLocalTetrad({
    pos = {0.0, 10.0, math.pi*0.5, 0.0},
    e0 = {1.0, 0.0, 0.0, 0.0},
    e1 = {0.0,-1.0, 0.0, 0.0},
    e2 = {0.0, 0.0, 0.0, 1.0},
    e3 = {0.0, 0.0, 1.0, 0.0}})
    
CalculateGeodesic({
    dir = {1.0,1.0,0.0},
    vel = 0.35,
    type = 'timelike',
    maxpoints = 3000})

--PrintObjectSettings()

ClearAllGVObjects()

AddGVObject({
    type = "sphere2d", 
    pos = {1.0, 0.0},
    color = {1.0, 0.5, 0.0},
    width = 2
})

AddGVObject({
    type = "sphere3d",
    pos = {0.0, 0.0, 1.0},
    color = {0.5, 0.3, 0.8}
})

SetGVCamera({
	pos = {30.0, 10.0, 30.0},
	poi = {0.0, 0.0, 3.0},
	vup = {0.0, 0.0, 1.0}
})
