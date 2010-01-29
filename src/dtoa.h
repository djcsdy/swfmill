#ifndef NETLIB_DTOA_H
#define NETLIB_DTOA_H

#ifdef __cplusplus
extern "C" {
#endif

double strtod(const char *s00, char **se);
char *dtoa(double d, int mode, int ndigits,
        int *decpt, int *sign, char **rve);
void freedtoa(char *s);
void g_fmt(char *buf, double d);

#ifdef __cplusplus
}
#endif

#endif
