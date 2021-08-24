#include <assert.h>

typedef void (*fplaceholder)(void);
typedef void (*WrapperFunc)(fplaceholder, char**, char**);

typedef long FSigID;
WrapperFunc fmux_wrapper(FSigID id);

