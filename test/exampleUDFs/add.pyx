cdef public __udf_user(a1, a2):
    if (a1 is None or a2 is None): 
        return None
    return a1+a2

cdef public __udf_main(a1, a2):
    vectorsize = len(a1)
    return [__udf_user(a1[index], a2[index]) for index in range(0, vectorsize)]