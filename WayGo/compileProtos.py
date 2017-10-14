#!/usr/bin/env python3
import glob
import subprocess

outType="--go_out"
def compileAllProto():    
    for file in glob.glob("**/*.proto",recursive=True):
        cmd="protoc {} . {}".format(outType,file)
        print(cmd)
        ret=subprocess.getoutput(cmd)
        print(ret)

if __name__ == '__main__':
    compileAllProto()