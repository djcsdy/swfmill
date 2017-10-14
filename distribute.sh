#!/bin/bash

# Packages swfmill for distribution

set -o nounset
set -o errexit
set -o pipefail

PROJECT_DIR="/vagrant/"

# Check this script is running inside the Vagrant virtual machine.
# If not, run it inside the Vagrant virtual machine and exit.
if [[ ! -e "${PROJECT_DIR}/distribute.sh" ]]; then
	vagrant up
	vagrant ssh -c "bash '${PROJECT_DIR}/distribute.sh'" &&
		exit_code=0 ||
		exit_code=$?
	
	echo
	
	if [[ ${exit_code} -eq 0 ]]; then
		echo "Successfully built distributables."
	else
		echo "Failed to build distributables."
	fi
	echo
	echo "The Vagrant VM is still running, but is no longer"
	echo "required."
	echo
	echo "To stop it: vagrant suspend"
	echo "To destroy it and free up disk space: vagrant destroy"
	echo
	
	exit ${exit_code}
fi

WORK_DIR="${HOME}/distribute-work"
DISTCHECK_WORK_DIR="${WORK_DIR}/distcheck"
WINDOWS_32_WORK_DIR="${WORK_DIR}/win32"
OUTPUT_DIR="${PROJECT_DIR}/distribution"


# Update Autotools

cd "${PROJECT_DIR}"
./autogen.sh


# Work around low timestamp precision of some host systems.

sleep 2


# Wipe any existing builds.

rm -rf "${WORK_DIR}" &>/dev/null || true


# Build source package and check it works properly.

mkdir -p "${OUTPUT_DIR}"

mkdir -p "${DISTCHECK_WORK_DIR}"
cd "${DISTCHECK_WORK_DIR}"
"${PROJECT_DIR}/configure"
make distcheck

# Get the name of the source package.
# This is fairly hacky, but I don't know of a better way of doing this.
package_name=$(make -pn |
	grep '^PACKAGE = ' |
	head -1 |
	sed -re 's/^PACKAGE = //')
version=$(make -pn |
	grep '^VERSION = ' |
	head -1 |
	sed -re 's/^VERSION = //')
source_package="${DISTCHECK_WORK_DIR}/${package_name}-${version}.tar.gz"

# Copy source package to the output directory.
cp "${source_package}" "${OUTPUT_DIR}"


# Build 32-bit Windows binaries.

mkdir -p "${WINDOWS_32_WORK_DIR}"
cd "${WINDOWS_32_WORK_DIR}"
/vagrant/configure --host=i686-w64-mingw32 \
	--prefix=/usr/local/i686-w64-mingw32 \
	--enable-static --disable-shared \
	--with-libiconv-prefix=/usr/local/i686-w64-mingw32 \
	XML_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libxml2 \
	XML_LIBS="-lxml2" \
	XSLT_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libxml2 \
	XSLT_LIBS="-lxml2 -lxslt" \
	EXSLT_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libxml2 \
	EXSLT_LIBS="-lxml2 -lxslt -lexslt" \
	PNG_CFLAGS=-I/usr/local/i686-w64-mingw32/include/libpng12 \
	PNG_LIBS=-lpng16 \
	CPPFLAGS='-I/usr/local/i686-w64-mingw32/include' \
	LDFLAGS='-L/usr/local/i686-w64-mingw32/lib'
make

mkdir -p dist
cp src/swfmill.exe dist/swfmill.exe
for file in AUTHORS COPYING NEWS README.md; do
	cp "${PROJECT_DIR}/${file}" "dist/${file%.*}.txt"
	todos "dist/${file%.*}.txt"
done

win32_zip_file="${WORK_DIR}/${package_name}-${version}-win32.zip"
cd dist
zip -9 "${win32_zip_file}" \
	swfmill.exe AUTHORS.txt COPYING.txt NEWS.txt README.txt
cp "${win32_zip_file}" "${OUTPUT_DIR}"
