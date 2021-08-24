def __udf_user(s):
    vectorsize = len(s)
    ret = [None] * vectorsize
    for i, x in enumerate(s):
        if (x is None):
            ret[i] = None
        else:
            sum = 0
            num = 0
            y = x.split()
            for word in y:
                sum += len(word)
                num += 1
            ret[i] = sum/num
    return ret

def __udf_main(a1):
    return __udf_user(a1)
