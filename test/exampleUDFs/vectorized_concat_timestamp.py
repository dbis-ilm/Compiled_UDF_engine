def __udf_user(s):
    from time import time,ctime
    ret = []
    vectorsize = len(s)
    ret = [None] * vectorsize
    for i, x in enumerate(s):
        if (x is None):
            ret[i] = None
        else:
            ret[i] = ctime(time()) + " " + str(x)
    return ret

def __udf_main(a1):
    return __udf_user(a1)
