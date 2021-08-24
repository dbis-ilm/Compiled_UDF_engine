#include "udf_wrappers.h"

typedef long (*ll)(long);
void func_5(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  *((long*)out) = ((ll)f)(*p0);
}

typedef double (*dl)(long);
void func_6(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  *((double*)out) = ((dl)f)(*p0);
}

typedef char* (*sl)(long);
void func_7(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  *((char**)out) = ((sl)f)(*p0);
}

typedef long (*ld)(double);
void func_9(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  *((long*)out) = ((ld)f)(*p0);
}

typedef double (*dd)(double);
void func_10(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  *((double*)out) = ((dd)f)(*p0);
}

typedef char* (*sd)(double);
void func_11(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  *((char**)out) = ((sd)f)(*p0);
}

typedef long (*ls)(char*);
void func_13(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  *((long*)out) = ((ls)f)(*p0);
}

typedef double (*ds)(char*);
void func_14(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  *((double*)out) = ((ds)f)(*p0);
}

typedef char* (*ss)(char*);
void func_15(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  *((char**)out) = ((ss)f)(*p0);
}

typedef long (*lll)(long, long);
void func_21(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  long* p1 = (long*) &params[1];
  *((long*)out) = ((lll)f)(*p0, *p1);
}

typedef double (*dll)(long, long);
void func_22(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  long* p1 = (long*) &params[1];
  *((double*)out) = ((dll)f)(*p0, *p1);
}

typedef char* (*sll)(long, long);
void func_23(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  long* p1 = (long*) &params[1];
  *((char**)out) = ((sll)f)(*p0, *p1);
}

typedef long (*lld)(long, double);
void func_37(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  double* p1 = (double*) &params[1];
  *((long*)out) = ((lld)f)(*p0, *p1);
}

typedef double (*dld)(long, double);
void func_38(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  double* p1 = (double*) &params[1];
  *((double*)out) = ((dld)f)(*p0, *p1);
}

typedef char* (*sld)(long, double);
void func_39(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  double* p1 = (double*) &params[1];
  *((char**)out) = ((sld)f)(*p0, *p1);
}

typedef long (*lls)(long, char*);
void func_53(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  char** p1 = (char**) &params[1];
  *((long*)out) = ((lls)f)(*p0, *p1);
}

typedef double (*dls)(long, char*);
void func_54(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  char** p1 = (char**) &params[1];
  *((double*)out) = ((dls)f)(*p0, *p1);
}

typedef char* (*sls)(long, char*);
void func_55(fplaceholder f, char **params, char **out) {
  long* p0 = (long*) &params[0];
  char** p1 = (char**) &params[1];
  *((char**)out) = ((sls)f)(*p0, *p1);
}

typedef long (*ldl)(double, long);
void func_25(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  long* p1 = (long*) &params[1];
  *((long*)out) = ((ldl)f)(*p0, *p1);
}

typedef double (*ddl)(double, long);
void func_26(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  long* p1 = (long*) &params[1];
  *((double*)out) = ((ddl)f)(*p0, *p1);
}

typedef char* (*sdl)(double, long);
void func_27(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  long* p1 = (long*) &params[1];
  *((char**)out) = ((sdl)f)(*p0, *p1);
}

typedef long (*ldd)(double, double);
void func_41(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  double* p1 = (double*) &params[1];
  *((long*)out) = ((ldd)f)(*p0, *p1);
}

typedef double (*ddd)(double, double);
void func_42(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  double* p1 = (double*) &params[1];
  *((double*)out) = ((ddd)f)(*p0, *p1);
}

typedef char* (*sdd)(double, double);
void func_43(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  double* p1 = (double*) &params[1];
  *((char**)out) = ((sdd)f)(*p0, *p1);
}

typedef long (*lds)(double, char*);
void func_57(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  char** p1 = (char**) &params[1];
  *((long*)out) = ((lds)f)(*p0, *p1);
}

typedef double (*dds)(double, char*);
void func_58(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  char** p1 = (char**) &params[1];
  *((double*)out) = ((dds)f)(*p0, *p1);
}

typedef char* (*sds)(double, char*);
void func_59(fplaceholder f, char **params, char **out) {
  double* p0 = (double*) &params[0];
  char** p1 = (char**) &params[1];
  *((char**)out) = ((sds)f)(*p0, *p1);
}

typedef long (*lsl)(char*, long);
void func_29(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  long* p1 = (long*) &params[1];
  *((long*)out) = ((lsl)f)(*p0, *p1);
}

typedef double (*dsl)(char*, long);
void func_30(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  long* p1 = (long*) &params[1];
  *((double*)out) = ((dsl)f)(*p0, *p1);
}

typedef char* (*ssl)(char*, long);
void func_31(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  long* p1 = (long*) &params[1];
  *((char**)out) = ((ssl)f)(*p0, *p1);
}

typedef long (*lsd)(char*, double);
void func_45(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  double* p1 = (double*) &params[1];
  *((long*)out) = ((lsd)f)(*p0, *p1);
}

typedef double (*dsd)(char*, double);
void func_46(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  double* p1 = (double*) &params[1];
  *((double*)out) = ((dsd)f)(*p0, *p1);
}

typedef char* (*ssd)(char*, double);
void func_47(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  double* p1 = (double*) &params[1];
  *((char**)out) = ((ssd)f)(*p0, *p1);
}

typedef long (*lss)(char*, char*);
void func_61(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  char** p1 = (char**) &params[1];
  *((long*)out) = ((lss)f)(*p0, *p1);
}

typedef double (*dss)(char*, char*);
void func_62(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  char** p1 = (char**) &params[1];
  *((double*)out) = ((dss)f)(*p0, *p1);
}

typedef char* (*sss)(char*, char*);
void func_63(fplaceholder f, char **params, char **out) {
  char** p0 = (char**) &params[0];
  char** p1 = (char**) &params[1];
  *((char**)out) = ((sss)f)(*p0, *p1);
}

WrapperFunc fmux_wrapper(FSigID id) {
   switch (id)
   {
      case 5:
          return func_5;
      case 6:
          return func_6;
      case 7:
          return func_7;
      case 9:
          return func_9;
      case 10:
          return func_10;
      case 11:
          return func_11;
      case 13:
          return func_13;
      case 14:
          return func_14;
      case 15:
          return func_15;
      case 21:
          return func_21;
      case 22:
          return func_22;
      case 23:
          return func_23;
      case 25:
          return func_25;
      case 26:
          return func_26;
      case 27:
          return func_27;
      case 29:
          return func_29;
      case 30:
          return func_30;
      case 31:
          return func_31;
      case 37:
          return func_37;
      case 38:
          return func_38;
      case 39:
          return func_39;
      case 41:
          return func_41;
      case 42:
          return func_42;
      case 43:
          return func_43;
      case 45:
          return func_45;
      case 46:
          return func_46;
      case 47:
          return func_47;
      case 53:
          return func_53;
      case 54:
          return func_54;
      case 55:
          return func_55;
      case 57:
          return func_57;
      case 58:
          return func_58;
      case 59:
          return func_59;
      case 61:
          return func_61;
      case 62:
          return func_62;
      case 63:
          return func_63;
      default:
           assert(0);
   }
}
