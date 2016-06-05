from __future__ import print_function, division

from IsenPython import Solver, NameList, Output
from IsenPython.Visualizer import Visualizer

namelist = NameList()
namelist.iprtcfl = False

solver = Solver()
solver.init(namelist)

solver.run()

output = solver.getOutput()
visualizer = Visualizer(output)

i = 6
visualizer.plot('horizontal_velocity', i, "DownSlope-{0}.pdf".format(i))