from __future__ import print_function, division

import numpy as np
import IsenPython

visualiter = IsenPython.Visualizer()

namelist = IsenPython.NameList()
namelist.iprtcfl = False
namelist.nz = 61

solver = IsenPython.Solver()
solver.init(namelist)

output = solver.getOutput()
print(output.getNameList().nz)

m = solver.getField("sold")
print(m[1:5,0])
print(np.shape(m))

v = solver.getField("topo")
print(v[1:5])
print(np.shape(v))

# solver.run()

# print(solver.getNameList())
#solver.write(IsenPython.ArchiveType.Unknown)
