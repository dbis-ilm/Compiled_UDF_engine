#!/bin/python
import math
import itertools

# ----- PLEASE ADAPT IF NEEDED ---------
types = ["long", "double", "char*"]
abbrev = {
    "long": "l",
    "double": "d",
    "char*": "s"
}
type_idx = {
    "long": 1,
    "double": 2,
    "char*": 3
}
base = type_idx.get("char*") + 1
max_num_params = 2
out_name = "udf_wrappers"
# ----- END: PLEASE ADAPT. Keep remaining unchanged ----------

generated_sigIDs = []

def compute_sigID(in_types, out_type):
    id = type_idx.get(out_type)
    for i in range(len(in_types)):
        id += type_idx.get(in_types[i]) * math.pow(base, i+1)
    return int(id)

def generate_code(in_types, out_type):
    ab = [abbrev.get(out_type)] + [abbrev.get(x) for x in in_types]
    typedef_name = "".join(ab)
    param_str = ", ".join(in_types)
    sigID = compute_sigID(in_types, out_type)
    generated_sigIDs.append(sigID)
    var_defs = []
    var_names = []
    for i in range(len(in_types)):
        d = f"  {in_types[i]}* p{i} = ({in_types[i]}*) &params[{i}];"
        var_defs.append(d)
        var_names.append(f"*p{i}")
    var_defs_str = "\n".join(var_defs)
    var_names_str = ", ".join(var_names)

    code = \
        f"typedef {out_type} (*{typedef_name})({param_str});\n"\
        f"void func_{sigID}(fplaceholder f, char **params, char **out) {{\n"\
        f"{var_defs_str}\n"\
        f"  *(({out_type}*)out) = (({typedef_name})f)({var_names_str});\n"\
        f"}}\n\n"
    return code

def generate_switch_statement():
    generated_sigIDs.sort()
    header = \
        "typedef long FSigID;\n"\
        "WrapperFunc fmux_wrapper(FSigID id);\n\n"
    code = \
        "WrapperFunc fmux_wrapper(FSigID id) {\n"\
        "   switch (id)\n"\
        "   {\n"\

    for id in generated_sigIDs:
        code += \
            f"      case {id}:\n"\
            f"          return func_{id};\n"

    code += \
        "      default:\n"\
        "           assert(0);\n"\
        "   }\n"\
        "}\n"
    return (code, header)

if __name__ == "__main__":
    header = \
        "#include <assert.h>\n\n"\
        "typedef void (*fplaceholder)(void);\n"\
        "typedef void (*WrapperFunc)(fplaceholder, char**, char**);\n\n"

    code = \
        f"#include \"{out_name}.h\"\n\n"

    for i in range(1, max_num_params + 1):
        for params in list(itertools.product(types, repeat=i)):
            for out in types:
                code += generate_code(params, out)
    (c, h) = generate_switch_statement()
    code += c
    header += h

    f = open(out_name + ".c", "a")
    f.truncate(0)
    f.write(code)
    f.close()

    f = open(out_name + ".h", "a")
    f.truncate(0)
    f.write(header)
    f.close()