#!/bin/sh

mtasc -cp /usr/local/share/mtasc/std -swf classes.swf -separate -header 320:240:25 Main.as org/swfmill/Foo.as
swfmill simple test.swfml output.swf
#&& mozilla -chrome chrome://fludge/content/fludge.xul?$(pwd)/output.swf 
