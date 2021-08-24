def __udf_user(a1):
    vectorsize = len(a1)
    ret = [None] * vectorsize
    for i, x in enumerate(a1):
        if (x is None):
            ret[i] = None
        else:
            ret[i] = x % 11
    return ret

def __udf_main(a1):
    return __udf_user(a1)
