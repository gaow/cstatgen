from cstatgen import cstatgen_py2 as cstatgen

gg = [[1,0,0],[1,1,0],[0,0,0]]
pp = [[1.2,3.4,5.4]]
cc = []

tt = cstatgen.RVTester(gg,pp,cc)
tests = ["CMATTest", "CMCFisherExactTest", "CMCTest", "CMCWaldTest", "MadsonBrowningTest", "FpTest"]
tests = ["SkatTest", "VTCMC", "VariableThresholdPrice", "ZegginiWaldTest"]
tests = ["ZegginiTest", "CMCTest"]
for test in tests:
    print tt.fit(test), tt.pvalue()
