import glob
import os
import sys


if __name__ == "__main__":
  if len(sys.argv) != 4:
    print('Usage:')
    print('python '+sys.argv[0]+' <images directory> <file extension (e.g., ppm, png)> <fileout.csv>')
  else:
    os.system('ls -v '+sys.argv[1]+'/*.'+sys.argv[2]+' > '+sys.argv[3])

  

