from __future__ import print_function, division

from IsenPython import Solver, NameList, Output
from IsenPython.Visualizer import Visualizer

namelist = NameList()
namelist.iprtcfl = False

solver = Solver('ref')
solver.init(namelist)

solver.run()

output = solver.getOutput()
visualizer = Visualizer(output)
visualizer.plot('horizontal_velocity', 6)
