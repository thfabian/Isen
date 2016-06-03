from __future__ import print_function, division

import numpy as np
import IsenPython

namelist = IsenPython.NameList()
namelist.iprtcfl = False

solver = IsenPython.Solver()
solver.init(namelist)

# solver.run()

namelist.nz = 500
output = solver.getOutput()
print(output.getNameList().nz)
print(np.shape(output.u()))

m = solver.getField("sold")
print(m[1:5,0])
print(np.shape(m))

v = solver.getField("topo")
print(v[1:5])
print(np.shape(v))

visualizer = IsenPython.Visualizer(output)
visualizer.plotVelocityContour(0)


# print(output.prec())
# print(solver.getField("unow")[1:5, 1:5])
# print(output.u()[6, 1:5, 1:5])

# print(solver.getNameList())
# solver.write(IsenPython.ArchiveType.Binary, "DownSlope2.bin")

# outputFile = IsenPython.Output("DownSlope2.bin")

# print(outputFile.t())
# print(outputFile.u()[0, 1:5, 1:5])
# print(outputFile.u()[6, 1:5, 1:5])
# print(outputFile.s()[0, 1:5, 1:5])

# print(outputFile.qv()[0, 1:5, 1:5])

