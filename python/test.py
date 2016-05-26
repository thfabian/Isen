from __future__ import print_function, division

import numpy as np
import IsenPython

solver = IsenPython.Solver()
solver.init()
solver.run()
#solver.write(IsenPython.ArchiveType.Unknown)
