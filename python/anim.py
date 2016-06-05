from __future__ import print_function, division

from IsenPython import Solver, NameList, Output
from IsenPython.Visualizer import Visualizer

namelist = NameList()
namelist.iprtcfl = False
namelist.iout = 9

solver = Solver()
solver.init(namelist)

solver.run()

output = solver.getOutput()
visualizer = Visualizer(output)

for i in xrange(len(output.t())):
    visualizer.plot('horizontal_velocity', i, ("anim-%03i.jpg" % (i)), show=False)
