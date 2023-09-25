import pyift as ift
import numpy as np
import sys

if len(sys.argv) != 3:
    print("python "+sys.argv[0]+" <OPF dataset path> <number of splits>\n")
    sys.exit(0)

file_name = sys.argv[1]
n_splits  = int(sys.argv[2])

ift.RandomSeed(42)

Z = ift.ReadDataSet(file_name)

sampler = ift.KFold(Z.nsamples, n_splits)

kappas = []
for i in range(0, n_splits):
    
    ift.DataSetSampling(Z, sampler, i)
    
    Z_train = ift.ExtractSamples(Z,  ift.TRAIN) # TRAIN = 0x04
    Z_test = ift.ExtractSamples(Z, ift.TEST) # TEST = 0x02
    
    graph = ift.CreateCplGraph(Z_train)

    ift.SupTrain(graph)

    ift.Classify(graph, Z_test)
    
    score = ift.CohenKappaScore(Z_test)
    kappas.append(score)
    print("Kappa Score %d: %f" % (i+1, score))

kp = np.asarray(kappas)

print("Score Mean: %f" % kp.mean())
print("Score Std Dev: %f" % kp.std())

