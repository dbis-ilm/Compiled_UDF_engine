#!/usr/bin/env python

import os
import sys
import subprocess
import shutil
import tempfile

def change_func_binding(object_file, prefix, function_name):
    import lief
    lib = lief.parse(object_file)
    function_symbol = None
    for sym in lib.symbols:
        name = sym.demangled_name
        if name.startswith(prefix) and name.endswith(function_name):
            sym.binding = lief.ELF.SYMBOL_BINDINGS(1)
            function_symbol = name
    lib.write(object_file)
    return function_symbol

if __name__ == "__main__":
    if (len(sys.argv) != 4):
        print(f"Wrong number of arguments")
        print(f"Usage: {sys.argv[0]} <udf_file> <function_name> <result_lib_name>")
        exit(1)

    # Parse args
    udf_file_name = sys.argv[1]
    function_name = sys.argv[2]
    result_lib_name = sys.argv[3]
    tmp_dir = tempfile.mkdtemp(prefix = "nuitka_")
    filename, file_extension = os.path.splitext(os.path.basename(udf_file_name))

    # Compile the code
    os.chdir(tmp_dir)
    cmd = ["python3.6", "-m", "nuitka", "--module", "--unstripped", "--show-scons", udf_file_name]
    p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if p.returncode != 0:
        print(p.stdout)
        exit(2)

    build_dir = [s for s in os.listdir(tmp_dir) if ".build" in s][0]
    build_dir = tmp_dir + "/" + build_dir
    os.chdir(build_dir)

    out = p.stdout.decode("utf-8").split("\n")
    compile_cmd = out[-5].split(" ")
    object_file = [s for s in compile_cmd if "module" in s and filename in s][0]

    # Change the UDF module's func binding
    prefix = "impl_"
    function_symbol = change_func_binding(build_dir + "/" + object_file, prefix, function_name)
    if function_symbol is None:
        exit(3)

    # Compile again
    compile_cmd[2] = result_lib_name
    p = subprocess.run(compile_cmd)
    if p.returncode != 0:
        exit(4)

    shutil.rmtree(tmp_dir)
    print(function_symbol, end='')
    exit(0)