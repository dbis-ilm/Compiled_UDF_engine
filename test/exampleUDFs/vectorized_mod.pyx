cdef public __udf_user(a1):
    vectorsize = len(a1)
    ret = [None] * vectorsize
    for i, x in enumerate(a1):
        if (x is None):
            ret[i] = None
        else:
            ret[i] = x % 11
    return ret

cdef public __udf_main(a1):
    return __udf_user(a1)
