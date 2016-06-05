from __future__ import print_function, division

import numpy as np
import IsenPython

namelist = IsenPython.NameList()
namelist.iprtcfl = False

solver = IsenPython.Solver()
solver.init(namelist)

# solver.run()

output = solver.getOutput()
visualizer = IsenPython.Visualizer(output)
visualizer.plotVelocityContour(0)

