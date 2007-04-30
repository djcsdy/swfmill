/*---------------------------------------------------------------------------

   rpng - simple PNG display program                              readpng.h

  ---------------------------------------------------------------------------

      Copyright (c) 1998-2000 Greg Roelofs.  All rights reserved.

      This software is provided "as is," without warranty of any kind,
      express or implied.  In no event shall the author or contributors
      be held liable for any damages arising in any way from the use of
      this software.

      Permission is granted to anyone to use this software for any purpose,
      including commercial applications, and to alter it and redistribute
      it freely, subject to the following restrictions:

      1. Redistributions of source code must retain the above copyright
         notice, disclaimer, and this list of conditions.
      2. Redistributions in binary form must reproduce the above copyright
         notice, disclaimer, and this list of conditions in the documenta-
         tion and/or other materials provided with the distribution.
      3. All advertising materials mentioning features or use of this
         software must display the following acknowledgment:

            This product includes software developed by Greg Roelofs
            and contributors for the book, "PNG: The Definitive Guide,"
            published by O'Reilly and Associates.


  ---------------------------------------------------------------------------

	NOTE: Greg Roelofs gave explicit permission to use this code under the 
	terms of GPLv2:
	
	Greg Roelofs <email stripped>, on Mon, 30 Apr 2007 09:01:24 -0700:
	> I have no interest in being
	> petty about this--and I've already been leaning toward copyleft
	> licensing in my other projects over the last few years--so I've
	> decided to go ahead and dual-license the pngbook code (readpng{,2},
	> writepng, rpng{,2}-{x,win}, wpng) under the GNU GPL v2 or later.  You
	> may take this message as explicit permission insofar as it's likely
	> to be a while before I actually get around to releasing a new version
	> of the code--I'm working on XV right now and have pngcheck and a
	> couple others queued up next.  (I'll do my best to do so be- fore the
	> next version of libpng goes out, however, so the contrib code can be
	> updated at the same time.)

  ---------------------------------------------------------------------------*/

#ifndef TRUE
#  define TRUE 1
#  define FALSE 0
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) > (b)? (a) : (b))
#  define MIN(a,b)  ((a) < (b)? (a) : (b))
#endif

#ifdef DEBUG
#  define Trace(x)  {fprintf x ; fflush(stderr); fflush(stdout);}
#else
#  define Trace(x)  ;
#endif

#include <png.h>

typedef unsigned char   uch;
typedef unsigned short  ush;
typedef unsigned long   ulg;

#ifdef __cplusplus
extern "C" {
#endif

/* prototypes for public functions in readpng.c */

void readpng_version_info(void);

int readpng_init(FILE *infile, ulg *pWidth, ulg *pHeight);

int readpng_get_bgcolor(uch *bg_red, uch *bg_green, uch *bg_blue);

uch *readpng_get_image(double display_exponent, int *pChannels,
                       ulg *pRowbytes, png_colorp *palette, int *n_pal );

void readpng_cleanup(int free_image_data);

#ifdef __cplusplus
}
#endif
