#!/bin/sh

# swfmill release script
VERSION=$1
PACKAGE=swfmill
TMP=/tmp/$PACKAGE

function status() {
echo -ne "\033]0;$@\007"
echo --------------------------------------------------
echo $@
echo --------------------------------------------------
}

function mkdostext() {
	cat $PACKAGE-$VERSION/$1 | unix2dos > $PACKAGE-$VERSION-win32/$1.txt
#	cat $1 | unix2dos > $2
}

if [ ! -z $VERSION ]; then
	# update version in configure.ac
	sed -e "s/AC_INIT( $PACKAGE, [0-9\.]*,/AC_INIT( $PACKAGE, $VERSION,/" configure.ac > configure.ac2
	mv configure.ac2 configure.ac
	cvs commit -m "release $VERSION"
	# save version
	echo $VERSION > .version
else
	VERSION=$(cat .version)
fi



status autogen
./autogen.sh 
# not sure why this is needed
automake-1.6
./autogen.sh 
status configure
./configure
status make dist
make dist

if [ ! -e $PACKAGE-$VERSION.tar.gz ]; then
	echo make dist failed.
	exit -1
fi


status Building the Windoze binary...

mkdir -p $TMP
cp $PACKAGE-$VERSION.tar.gz $TMP
cd $TMP
tar xvfz $PACKAGE-$VERSION.tar.gz
cd $PACKAGE-$VERSION

export PATH=/opt/xmingw/bin:$PATH
export CC=i386-mingw32msvc-gcc
export CPP=i386-mingw32msvc-cpp
export CXX=i386-mingw32msvc-g++
export LD=i386-mingw32msvc-ld
export CFLAGS="-I/opt/xmingw/i386-mingw32msvc/include"
export CXXFLAGS="-I/opt/xmingw/i386-mingw32msvc/include"
export PKG_CONFIG_PATH=/opt/xmingw/i386-mingw32msvc/lib/pkgconfig/
./configure --prefix=/opt/xmingw/i386-mingw32msvc/ --build=i686-pc-linux-gnu --host=i386-mingw32msvc --target=i386-mingw32msvc 
make

status bundle win32 dist .zip
i386-mingw32msvc-strip src/swfmill.exe
cd ..
mkdir $PACKAGE-$VERSION-win32
cp $PACKAGE-$VERSION/src/swfmill.exe $PACKAGE-$VERSION-win32/
mkdostext README
mkdostext COPYING
mkdostext NEWS
zip -r $PACKAGE-$VERSION-win32.zip $PACKAGE-$VERSION-win32

status Copying release files to iterative.org/swfmill/pre/
scp $PACKAGE-$VERSION-win32.zip $PACKAGE-$VERSION.tar.gz iterative@iterative.org:iterative.org/swfmill/pre/

echo all done.
echo now just edit index.php and copy binaries from pre/
