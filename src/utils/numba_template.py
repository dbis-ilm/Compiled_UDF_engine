#!/usr/bin/env python

from numba.pycc import CC

cc = CC('$$MODULE_NAME$$')
cc.verbose=True

@cc.export('$$FNAME$$', '$$NUMBA_SIGNATURE$$')
$$UDF$$

if __name__ == "__main__":
    cc.compile()
