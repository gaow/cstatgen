from cstatgen import cstatgen_py2 as cstatgen
from numpy import loadtxt
import os

gg = loadtxt("Z.txt", dtype=int, delimiter=",")
pc = loadtxt("YContinuous.txt", dtype=float, delimiter=",")
pb = loadtxt("YBinary.txt", dtype=int, delimiter=",")
cc = []
# Binary
tt = cstatgen.RVTester(gg.T, [pb], cc)
tests = ["CMATTest", "CMCFisherExactTest", "CMCTest", "CMCWaldTest", "MadsonBrowningTest", "FpTest", "SkatTest", "RareCoverTest",
         "KBACTest", "VariableThresholdPrice", "ZegginiWaldTest", "ZegginiTest"]
for test in tests:
    print test , "(Binary)"
    if tt.fit(test, is_binary = True) == 0:
        print tt.pvalue()
    else:
        print 'ERROR'
# Quantitative
tt = cstatgen.RVTester(gg.T, [pc], cc)
tests = ["CMCTest", "CMCWaldTest", "FpTest", "SkatTest", "VariableThresholdPrice", "ZegginiWaldTest", "ZegginiTest"]
for test in tests:
    print test, "(Continuous)"
    if tt.fit(test, is_binary = False) == 0:
        print tt.pvalue()
    else:
        print 'ERROR'
# SKAT benchmark
os.system('''
echo """
library(SKAT)
data(SKAT.example)
attach(SKAT.example)
obj<-SKAT_Null_Model(y.c ~ 1, out_type='C')
print(SKAT(Z, obj))
obj<-SKAT_Null_Model(y.b ~ 1, out_type='D')
print(SKAT(Z, obj))
""" | R --slave
''')
