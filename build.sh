#!/bin/bash

autoconf
autoheader
hg archive -X configure.ac ${1}
rm ${1}/.hg*
cp configure config.h.in ${1}
