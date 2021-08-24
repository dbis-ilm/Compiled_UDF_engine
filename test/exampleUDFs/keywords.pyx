cdef public __udf_user(s):
    if (s is None):
        return None
    from rake_nltk import Rake
    r = Rake()
    s_cleaned = s.replace("< br />", "").replace("<br />", "")
    r.extract_keywords_from_text(s_cleaned)
    keywords = r.get_ranked_phrases()
    return(", ".join(keywords[:3]))

cdef public __udf_main(a1):
    vectorsize = len(a1)
    return [__udf_user(a1[index]) for index in range(0, vectorsize)]