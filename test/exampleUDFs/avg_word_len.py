def __udf_user(s):
    if (s is None):
        return None
    sum = 0
    num = 0
    x = s.split()
    for word in x:
        sum += len(word)
        num += 1
    return sum/num

def __udf_main(a1):
    vectorsize = len(a1)
    return [__udf_user(a1[index]) for index in range(0, vectorsize)]