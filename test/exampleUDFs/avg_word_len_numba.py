def avg_word_len(s):
    sum = 0
    num = 0
    x = s.split()
    for word in x:
        sum += len(word)
        num += 1
    return sum/num