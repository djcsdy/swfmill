# swfmill [![Build Status][5]][6]

swfmill is a tool to process Adobe Flash (SWF) files. It can convert SWF
from and to an XML dialect called “swfml”,  which is closely modeled after
the SWF file format.

It also provides a libxslt-based XSL transformation engine that supports an
extension (“swft”) that helps with generating IDs for SWF objects and can
import an SWF as XML using an XPath function (`swft:document()`).

As a simple application of such functionality, swfmill can pack together a
bunch of media files (jpeg and png images, other SWF movies, TrueType fonts)
into an SWF as “library  objects” for your `attachMovie()` pleasure.


## Warning

swfmill is alpha-quality software. It might well not work as it should.
Please report your experiences if that seems to be the case, send in (if
possible small) SWFs or patches.

You can report bugs or contribute patches via GitHub at
https://github.com/djcsdy/swfmill/issues


## Dependencies

swfmill requires, and the binary releases contain code from:

 * libxml2, © 1998–2003 Daniel Veillard.
 * libxslt/exslt, © 1998-2003 Daniel Veillard.
 * zlib, © 1995-2004 Jean-loup Gailly and Mark Adler.
 * freetype, © 1996-2000, 2002, 2004 David Turner, Robert Wilhelm, and
   Werner Lemberg.
 * libpng, © 2004 Glenn Randers-Pehrson.

To build, xsltproc is required in addition to the usual GCC build environment.

Some of the test cases require MTASC, and will be skipped if it is not found.
    
My references to the SWF format were:
    
 * [Alexis' SWF Reference]
   (http://sswf.sourceforge.net/SWFalexref.html)

 * [SSWF sources]
   (http://sswf.sourceforge.net/)
    
 * [MTASC sources]
   (http://mtasc.org/)
    
 * [SWF File Format Specification]
   (http://www.adobe.com/devnet/swf/)
    

## Compiling and Installing

On Linux, use the “GNU-standard”
    
    ./configure && make && make install
    
On Windows, if you have use for a tool like this, you know better than me
where to put swfmill.exe.


## Usage

see `swfmill -h` for general usage options.

convert an SWF (foo.swf) to XML:
    
    swfmill swf2xml foo.swf bar.xml
    
convert such XML to SWF:
    
    swfmill xml2swf bar.xml meep.swf
    


## Simple swfml Dialect

For library generation, and probably other fun SWF construction, swfmill
supports a simplified XML dialect. to create a library swf that includes
some assets, construct an XML file that looks like this:
    
    <?xml version="1.0" encoding="iso-8859-1"?>
    <movie width="320" height="240" framerate="12">
      <background color="#ffffff"/>
      
      <frame>
        <library>
          <clip id="jpg" import="library/testjpg.jpg"/>
          <clip id="png" import="library/testpng.png"/>
          <clip id="swf" import="library/shape.swf"/>
        </library>
        
        <font id="vera" import="library/vera.ttf"
            glyphs="abcdefghijklmnopqrstuvwxyz"/>
      </frame>
    </movie>
    
and run swfmill:
    
    swfmill simple <the-xml-above.xml> output.swf
    
output.swf should now contain the specified assets (the JPG, PNGs, other
SWF and font) specified.

The file paths in the simple XML are relative to where you start swfmill.
   
Any `<clip import=".."/>` that is placed within a <library/> tag will be
exported for attachMovie with the ID attribute as the linkage ID. For the
above example, you can attach library/testjpg.jpg with
       
    _root.attachMovie("jpg", "foo", 1);
       
The font is also available by its ID, and will contain only the characters
specified in its glyph attribute. Make sure you specify the correct
encoding in the XML declaration for characters outside the ASCII range.


## Shared Libraries

swfmill supports both generation and use of shared libaries. To generate a
shared libary, simply put your assets in a `<library/>` tag just as for
attachMovie (see above).

To use a shared library, the library swf has to be available both locally
and under its “public” URL. Assuming you have put your library on
http://foo.com/library.swf, and the SWF is also in the library/ subdirectory
of where you run swfmill, put this in your definition XML:
    
    <import file="library/library.swf"
        url="http://foo.com/library.swf"/>
    
That should import all symbols that are exported in library.swf, so they
should be available under their name with `attachMovie()`. Note: this only
works with proper assets, not with fonts. If you know how to use a font
from a shared library, please tell me.


## Copyright

Copyright © 2005–2007 Daniel Turing.
Copyright © 2005–2013 swfmill contributors (see AUTHORS).

swfmill is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License version 2, as published by the Free
Software Foundation. See COPYING for details.


## Getting Involved

Updates and stuff will be available at the [swfmill homepage][1].
    
  [1]: http://www.swfmill.org/
    
There is a [mailing list for swfmill][2] hosted by osflash.org.
    
  [2]: http://osflash.org/mailman/listinfo/swfmill_osflash.org
    
The [source code of swfmill][3] is hosted on GitHub.
    
  [3]: https://github.com/djcsdy/swfmill
    
[Report bugs or submit patches][4] via GitHub.
    
  [4]: https://github.com/djcsdy/swfmill/issues


  [5]: https://travis-ci.org/djcsdy/swfmill.png
  [6]: https://travis-ci.org/djcsdy/swfmill
