#include "type.h"

int sizeof_Type(Type t) {
    int ret;
    switch (t)
    {
    case INT:
        ret = sizeof(long);
        break;
    case DOUBLE:
        ret = sizeof(double);
        break;
    case STRING:
        ret = sizeof(char*);
        break;
    default:
        assert(0);
        break;
    }
    return ret;
}