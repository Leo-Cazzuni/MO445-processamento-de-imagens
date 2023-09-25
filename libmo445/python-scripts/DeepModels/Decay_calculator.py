import numpy as np
import sys


if __name__ == '__main__':
  if len(sys.argv)!=6:
     print("Usage: python decay_calculator.py <initial lr> <final lr> <number of train samples> <batch size> <epochs>")
  else:
     initial_lr = float(sys.argv[1])
     final_lr   = float(sys.argv[2])
     N_train    = int(sys.argv[3])
     batch_size = int(sys.argv[4])
     epochs     = int(sys.argv[5])
     updates = np.ceil(N_train/batch_size)
     final_lr*=1.001
     decay = (initial_lr - final_lr)/(updates*epochs*final_lr)
     decay = float("{0:.5f}".format(decay))
     lr = initial_lr/(1+decay*updates*epochs)
     print("Final decay = " , decay)
     print("Expected final lr = ", lr)
