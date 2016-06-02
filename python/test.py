from __future__ import print_function, division

import numpy as np
import IsenPython

visualiter = IsenPython.Visualizer()

solver = IsenPython.Solver()
solver.init()

m = solver.getField("sold")
print(m[1:5,0])
print(np.shape(m))

v = solver.getField("topo")
print(v[1:5])
print(np.shape(v))

#solver.run()

#solver.write(IsenPython.ArchiveType.Unknown)
