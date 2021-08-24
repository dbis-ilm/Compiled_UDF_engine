def __udf_user(s):
    if (s is None):
        return None
    from time import time,ctime
    return(ctime(time()) + " " + str(s))

def __udf_main(a1):
    vectorsize = len(a1)
    return [__udf_user(a1[index]) for index in range(0, vectorsize)]