# -*- coding: utf-8 -*-
#                        _________ _______   __
#                       /  _/ ___// ____/ | / /
#                       / / \__ \/ __/ /  |/ / 
#                     _/ / ___/ / /___/ /|  /  
#                    /___//____/_____/_/ |_/   
#            
#  Isentropic model - ETH Zürich
#  Copyright (C) 2016  Fabian Thüring (thfabian@student.ethz.ch)
# 
#  This file is distributed under the MIT Open Source License. See LICENSE.TXT for details.
#

from __future__ import print_function, division

import unittest
import os
import tempfile
import numpy as np

import IsenPython

ScriptPath = os.path.dirname(os.path.realpath(__file__))

# Possibly namelist files
files = [os.path.join(ScriptPath, "..", "data", "namelist.m"), 
         os.path.join(ScriptPath, "..", "test", "data", "namelist.m")]
files = filter(lambda f: os.path.exists(f) == True, files)

## Solver
class TestSolver(unittest.TestCase):
    """Test PySolver"""
    
    def setUp(self):
        self.solver = IsenPython.Solver()
        
    def test_constructor(self):
        """Test constructor"""
        namelist = IsenPython.NameList()
        try:
            solver = IsenPython.Solver("ref")
            solver.init(namelist)
        except RuntimeError as e:
            self.fail("IsenException caught: {0}".format(e.message))
        
        with self.assertRaises(RuntimeError):
            solver = IsenPython.Solver("qwfawsdqwadqwf")
            solver.init(namelist)
        
    def test_init_namelist(self):
        """Test initalization with a namelist"""
        namelist = IsenPython.NameList()
        
        try:
            self.solver.init(namelist)
        except RuntimeError as e:
            self.fail("IsenException caught: {0}".format(e.message))
     
    @unittest.skipIf(len(files) == 0, "no namelist file available")      
    def test_init_file(self):
        """Test initalization with a file"""
        try:
            self.solver.init(files[0])
        except RuntimeError as e:
            self.fail("IsenException caught: \"{0}\"".format(e.message))
            
    def test_get_field(self):
        """Test querying fields"""
        namelist = IsenPython.NameList()
        
        try:
            self.solver.init(namelist)
            u = self.solver.getField("unow")
            self.assertTrue(np.shape(u) == (namelist.nx + 1 + 2*namelist.nb, namelist.nz))
        except RuntimeError as e:
            self.fail("IsenException caught: \"{0}\"".format(e.message))
            
        with self.assertRaises(RuntimeError):
            self.solver.getField("not-a-field")
            
    def test_get_namelist(self):
        """Test querying namelist"""
        namelist = IsenPython.NameList()
        namelist.nz = 100
        
        self.solver.init(namelist)
        self.assertTrue(self.solver.getNameList().nz == namelist.nz)
        
    def test_get_output(self):
        """Test querying output"""
        namelist = IsenPython.NameList()
        self.solver.init(namelist)
        
        self.assertTrue(self.solver.getOutput().getNameList().nz == namelist.nz)
        
    def test_write(self):
        """Test writing output files"""
        namelist = IsenPython.NameList()
        namelist.nx = 5
        namelist.nz = 5
        
        self.solver.init(namelist)

        tfile = "__temporary_output_file__.txt"
        try:
            self.solver.write(IsenPython.ArchiveType.Text, tfile)
        except RuntimeError as e:
            self.fail("IsenException caught: \"{0}\"".format(e.message))
        finally:
            os.remove(tfile)  

## Output
class TestOutput(unittest.TestCase):
    """Test PyOutput"""
    
    def setUp(self):
        self.namelist = IsenPython.NameList()
        self.namelist.nx = 5
        self.namelist.nz = 5
        self.imoist = False
        self.imicrophys = 0
        self.idthdt = False
        self.solver = IsenPython.Solver()
        self.solver.init(self.namelist)
        
    def test_init_from_solver(self):
        """Test querying fields"""
        try:
            output = self.solver.getOutput()
            self.assertTrue(output.getNameList().nx == self.namelist.nx) 
        except RuntimeError as e:
            self.fail("IsenException caught: \"{0}\"".format(e.message))
            
    def test_get_field(self):
        """Test initalization from solver"""
        try:
            output = self.solver.getOutput()
            z = output.z()
            u = output.u()
            s = output.s()
            t = output.t()
        except RuntimeError as e:
            self.fail("IsenException caught: \"{0}\"".format(e.message))   
    
        with self.assertRaises(RuntimeError):
            output = self.solver.getOutput()
            output.prec()
            
    def test_read_write(self):
        """Test serialization/deserialization"""
        for archive in [IsenPython.ArchiveType.Text, 
                        IsenPython.ArchiveType.Xml, 
                        IsenPython.ArchiveType.Binary]:
            tfile = "__temporary_output_file__"
            try:
                if archive == IsenPython.ArchiveType.Text:
                    tfile += ".txt"
                elif archive == IsenPython.ArchiveType.Xml:
                    tfile += ".xml"
                else:
                    tfile += ".bin"        
            
                self.solver.write(archive, tfile)
                output = IsenPython.Output()
                output.read(tfile)
            except RuntimeError as e:
                self.fail("IsenException caught: \"{0}\"".format(e.message))
            finally:
                os.remove(tfile)   

## NameList
class TestNameList(unittest.TestCase):
    """Test PyNameList"""

    def test_init_default(self):
        """Test default initalization"""
        namelist = IsenPython.NameList()
        self.assertIsNotNone(namelist)
        
    @unittest.skipIf(len(files) == 0, "no namelist file available")      
    def test_init_file(self):
        """Test initalization with a file"""
        try:
            namelist = IsenPython.NameList(files[0])
        except RuntimeError as e:
            self.fail("IsenException caught: {0}".format(e.message))
          
        with self.assertRaises(RuntimeError):
            namelist = IsenPython.NameList("not-a-file")
            
    def test_print(self):
        """Test printing"""
        namelist = IsenPython.NameList()
        self.assertIsNotNone(namelist.__str__())
            
    def test_property(self):
        """Test properties"""
        namelist = IsenPython.NameList()
        self.assertTrue(hasattr(namelist, 'run_name'))
        self.assertTrue(hasattr(namelist, 'iout'))
        self.assertTrue(hasattr(namelist, 'iiniout'))
        self.assertTrue(hasattr(namelist, 'xl'))
        self.assertTrue(hasattr(namelist, 'nx'))
        self.assertTrue(hasattr(namelist, 'thl'))
        self.assertTrue(hasattr(namelist, 'nz'))
        self.assertTrue(hasattr(namelist, 'time'))
        self.assertTrue(hasattr(namelist, 'dt'))
        self.assertTrue(hasattr(namelist, 'diff'))
        self.assertTrue(hasattr(namelist, 'topomx'))
        self.assertTrue(hasattr(namelist, 'topowd'))
        self.assertTrue(hasattr(namelist, 'topotim'))
        self.assertTrue(hasattr(namelist, 'u00'))
        self.assertTrue(hasattr(namelist, 'bv00'))
        self.assertTrue(hasattr(namelist, 'th00'))
        self.assertTrue(hasattr(namelist, 'ishear'))
        self.assertTrue(hasattr(namelist, 'k_shl'))
        self.assertTrue(hasattr(namelist, 'k_sht'))
        self.assertTrue(hasattr(namelist, 'u00_sh'))
        self.assertTrue(hasattr(namelist, 'nab'))
        self.assertTrue(hasattr(namelist, 'diffabs'))
        self.assertTrue(hasattr(namelist, 'irelax'))
        self.assertTrue(hasattr(namelist, 'nb'))
        self.assertTrue(hasattr(namelist, 'iprtcfl'))
        self.assertTrue(hasattr(namelist, 'itime'))
        self.assertTrue(hasattr(namelist, 'imoist'))
        self.assertTrue(hasattr(namelist, 'imoist_diff'))
        self.assertTrue(hasattr(namelist, 'imicrophys'))
        self.assertTrue(hasattr(namelist, 'idthdt'))
        self.assertTrue(hasattr(namelist, 'iern'))
        self.assertTrue(hasattr(namelist, 'vt_mult'))
        self.assertTrue(hasattr(namelist, 'autoconv_th'))
        self.assertTrue(hasattr(namelist, 'autoconv_mult'))
        self.assertTrue(hasattr(namelist, 'sediment_on'))

if __name__ == "__main__":
    IsenPython.Logger().disable()
    unittest.main()

