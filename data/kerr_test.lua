--[[
    Testfile for GeodesicViewer
]]

SetMetric("KerrBL");
SetMetricParams({mass = 1.0, angmom = 0.8})
--PrintMetric()

--PrintSolverDB()
SetGeodSolver("GSL_RK_Fehlberg")
SetGeodParams({stepctrl = "yes",
               stepsize = 1e-2,
               epsabs = 1e-12,
               constr = 1e-6})


SetLocalTetrad({
    pos = {0.0, 10.0, math.pi*0.5, 0.0},
    type = 'lnrf',
    e0 = {1.0, 0.0, 0.0, 0.0},
    e1 = {0.0,-1.0, 0.0, 0.0},
    e2 = {0.0, 0.0, 0.0, 1.0},
    e3 = {0.0, 0.0, 1.0, 0.0}})
    
chi = math.rad(-47.0)
CalculateGeodesic({
    dir = {math.cos(chi),math.sin(chi),0.0},
    vel = 0.35,
    type = 'timelike',
    timedir = 'backward',
    maxpoints = 3000})

--PrintObjectSettings()
