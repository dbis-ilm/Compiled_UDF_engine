#!/usr/bin/env python

import os
import sys
import subprocess
import shutil

def generate_code_file(udf_file_name, function_name, numba_signature):
    udf_file = open(udf_file_name, 'r')
    UDFUtils = os.getenv("UDFUTILS")
    template_file = open(f"{UDFUtils}/numba_template.py", 'r')
    numba_code_file_name = "/tmp/numba_code.py"
    numba_code_file = open(numba_code_file_name, 'w')

    udf_code = udf_file.read()
    template_code = template_file.read()
    template_code = template_code.replace("$$UDF$$", udf_code)
    template_code = template_code.replace("$$FNAME$$", function_name)
    template_code = template_code.replace("$$MODULE_NAME$$", function_name)
    template_code = template_code.replace("$$NUMBA_SIGNATURE$$", numba_signature)
    numba_code_file.write(template_code)

    udf_file.close()
    template_file.close()
    numba_code_file.close()
    return numba_code_file_name

def change_func_visibility(object_file, prefix, function_name):
    import lief
    lib = lief.parse(object_file)
    function_symbol = None
    for sym in lib.symbols:
        name = sym.demangled_name
        if name.startswith(prefix) and function_name in name:
            sym.visibility = lief.ELF.SYMBOL_VISIBILITY(0)
            function_symbol = name
    lib.write(object_file)
    return function_symbol

if __name__ == "__main__":
    if (len(sys.argv) != 6):
        print(f"Wrong number of arguments")
        print(f"Usage: {sys.argv[0]} <udf_file> <function_name> <result_lib_name> <numba_signature> <NRT_necessary>")
        exit(1)

    # Parse args
    udf_file_name = sys.argv[1]
    function_name = sys.argv[2]
    result_lib_name = sys.argv[3]
    numba_signature = sys.argv[4]
    NRT_necessary = int(sys.argv[5])

    # Generate the code file
    numba_code_file = generate_code_file(udf_file_name, function_name, numba_signature)

    # Compile the code
    cmd = ["numba", numba_code_file]
    p = subprocess.run(cmd, stdout=subprocess.PIPE)
    if p.returncode != 0:
        exit(2)

    out = p.stdout.decode("utf-8").split("\n")
    object_file = out[0].split(" ")[-1]
    compile_cmd = out[-2]
    build_dir = os.path.dirname(object_file)

    # Change the UDF module's cfunc visibility
    prefix = "._pycc_method" if NRT_necessary == 1 else "cfunc"
    function_symbol = change_func_visibility(object_file, prefix, function_name)
    if function_symbol is None:
        exit(3)

    # Compile again
    cmd = compile_cmd.split(" ")
    cmd[-1] = result_lib_name
    p = subprocess.run(cmd)
    if p.returncode != 0:
        exit(4)

    os.remove(numba_code_file)
    shutil.rmtree(build_dir)
    print(function_symbol, end='')
    exit(0)