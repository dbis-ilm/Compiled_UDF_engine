def __udf_user(a1):
    if (a1 is None): 
        return None
    return a1 % 11

def __udf_main(a1):
    vectorsize = len(a1)
    return [__udf_user(a1[index]) for index in range(0, vectorsize)]