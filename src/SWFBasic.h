#ifndef SWF_BASIC_H
#define SWF_BASIC_H

#define SWF_WARNING(msg) fprintf(stderr,msg,...);
#define SWF_ERROR(msg) { fprintf(stderr,msg,...); abort(); }

#endif
