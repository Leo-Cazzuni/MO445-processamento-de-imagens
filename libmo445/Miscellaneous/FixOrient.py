#!/usr/bin/python

from Tkinter import *
import os
import sys


#
# Main function
#

if __name__ == "__main__":

    if len(sys.argv) != 3:
        print 'Usage: FixOrient <input directory> <output directory>'
        sys.exit()

    command = 'ls -v '+sys.argv[1]+' > temp.txt'
    os.system(command)

    file = open('temp.txt','r')
    filename = file.readline()
    filelist = [] 
    while (filename):
        filelist = filelist + [filename.split('\n')[0]]
        filename = file.readline()

    file.close()

    for filename in filelist:
        print 'Processing ' + filename
        command = 'iftChangeAxes '+sys.argv[1]+'/'+filename+' temp.scn 5'
        os.system(command)
        command = 'mv temp.scn '+sys.argv[2]+'/'+filename
        os.system(command)
        
    sys.exit()
