#!/usr/bin/env python

import os
import sys
import subprocess

if __name__ == "__main__":
    if (len(sys.argv) != 4):
        print(f"Wrong number of arguments")
        print(f"Usage: {sys.argv[0]} <udf_file> <function_name> <result_lib_name>")
        exit(1)

    # Parse args
    udf_file_name = sys.argv[1]
    function_name = sys.argv[2]
    result_lib_name = sys.argv[3]
    filename_wo_ext = os.path.splitext(os.path.basename(udf_file_name))[0]
    path = os.path.dirname(udf_file_name)

    # Create C file using Cython
    cmd = ["cython", "--embed", "-3", udf_file_name]
    p = subprocess.run(cmd)
    if p.returncode != 0:
        exit(2)

    # Get python compilation flags
    cmd = ["python3-config", "--includes"]
    p = subprocess.run(cmd, stdout=subprocess.PIPE)
    if p.returncode != 0:
        exit(3)
    py_includes = p.stdout.decode("utf-8").split("\n")[0].split(" ")
    cmd = ["python3-config", "--libs"]
    p = subprocess.run(cmd, stdout=subprocess.PIPE)
    if p.returncode != 0:
        exit(3)
    py_libs = p.stdout.decode("utf-8").split("\n")[0].split(" ")
    py_libs = [x for x in py_libs if x] # Remove empty strings

    # Compile the object file
    object_file_name = f"/tmp/{filename_wo_ext}.o"
    code_file_name = f"{path}/{filename_wo_ext}.c"
    header_file_name = f"{path}/{filename_wo_ext}.h"

    cmd = ["gcc", code_file_name, "-fPIC",\
       "-g", "-c", "-o", object_file_name] + py_includes

    p = subprocess.run(cmd)
    if p.returncode != 0:
        exit(4)

    # Link the shared object
    cmd = cmd = ["gcc", object_file_name, "-fPIC", "-pthread",\
       "-g", "-shared", "-o", result_lib_name] + py_libs

    p = subprocess.run(cmd)
    if p.returncode != 0:
        exit(5)

    os.remove(object_file_name)
    os.remove(code_file_name)
    os.remove(header_file_name)
    print(function_name, end='')
    exit(0)
