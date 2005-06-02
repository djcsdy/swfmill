#!/bin/sh

swfmill simple test.swfml output.swf \
#&& mozilla -chrome chrome://flaunch/content/flaunch.xul?$(pwd)/output.swf \
#&& mtasc -cp /usr/local/share/mtasc/std -swf output.swf -main Main.as \
