cdef public __udf_user(a1, a2):
    vectorsize = len(a1)
    ret = [None] * vectorsize
    for i, (x, y) in enumerate(zip(a1, a2)):
        if (x is None or y is None):
            ret[i] = None
        else:
            ret[i] = x+y
    return ret

cdef public __udf_main(a1, a2):
    return __udf_user(a1, a2)
