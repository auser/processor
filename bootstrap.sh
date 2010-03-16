#!/bin/bash
CURL=$(which curl)
PUSHD=$(which pushd)
POPD=$(which popd)

libtecla_version="1.6.1"
libtecla_tar="libtecla-${libtecla_version}.tar.gz"
libtecla_repos="http://www.astro.caltech.edu/~mcs/tecla/${libtecla_tar}"

# Build cmockery
cmockery_version="0.1.2"
cmockery_tar="cmockery-${cmockery_version}.tar.gz"
cmockery_url="http://cmockery.googlecode.com/files/${cmockery_tar}"

# Fix for OSX not finding malloc.h
if [[ `uname -s` == "Darwin" ]]; then
    export CFLAGS=-I/usr/include/malloc
fi

if [[ !(-d "./build") ]]; then
    mkdir ./build
fi

if [ -f "build/cmockery/lib/libcmockery.a" ]; then
    echo "libcmockery built"
else
  if [ -f "/usr/local/lib/libcmockery.a" ]; then
      mkdir -p `pwd`/build/cmockery/{lib,include}
      cp /usr/local/lib/libcmockery.a "$(pwd)/build/cmockery/lib"
      cp -r /usr/local/include/google "$(pwd)/build/cmockery/include"
  elif [ -f "/usr/local/lib/libcmockery.a" ]; then
      mkdir -p `pwd`/build/cmockery/{lib,include}
      cp /usr/lib/libcmockery.a "$(pwd)/build/cmockery/lib"
      cp -r /usr/include/google "$(pwd)/build/cmockery/include"
  else
      pushd build
      prefix=`pwd`/cmockery
      $CURL -o $cmockery_tar $cmockery_url
      tar -xzf cmockery-${cmockery_version}.tar.gz
      pushd cmockery-${cmockery_version}
      ./configure --prefix=$prefix && make && make install
      popd
      rm -rf cmockery-${cmockery_version}
      popd
  fi
fi

if [ -f "build/libtecla/lib/libtecla.a" ]; then
    echo "libtecla built"
else
  pushd build
  prefix=`pwd`/libtecla
  $CURL -o $libtecla_tar $libtecla_repos
  echo $libtecla_tar
  tar -xzf $libtecla_tar
  mv libtecla libtecla-${libtecla_version}
  pushd libtecla-${libtecla_version}
  ./configure --prefix=$prefix && make && make install
  popd
  rm -rf libtecla-${libtecla_version}
  popd
fi

# cleanup
rm -rf build/*.tar.gz