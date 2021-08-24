cdef public __udf_user(s):
    from rake_nltk import Rake
    r = Rake()
    vectorsize = len(s)
    ret = [None] * vectorsize
    for i, x in enumerate(s):
        if (x is None):
            ret[i] = None
        else:
            s_cleaned = x.replace("< br />", "").replace("<br />", "")
            r.extract_keywords_from_text(s_cleaned)
            keywords = r.get_ranked_phrases()
            ret[i] = ", ".join(keywords[:3])
    return ret

cdef public __udf_main(a1):
    return __udf_user(a1)
