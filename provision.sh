#!/bin/bash

# Provisioning script for Vagrant virtual machine.

set -o nounset
set -o errexit
set -o pipefail

sudo apt-get update

sudo apt-get install -y build-essential mingw-w64 automake autoconf libtool \
	xsltproc pkg-config libxml2-dev libxslt-dev libfreetype6-dev \
	libpng-dev mtasc git
