#ifndef INC_BASE64_H
#define INC_BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

extern long base64_encode(char *to, char *from, unsigned int len);
extern long base64_decode(char *to, char *from, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
