add_library(Ops scan.c pythonUDF.c cythonUDF.c numbaUDF.c numbaNRTUDF.c nuitkaUDF.c)
target_link_libraries(Ops Src)