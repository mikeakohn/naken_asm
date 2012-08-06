#!/usr/bin/env bash

BUILDDIR=/storage/builds
SOURCEDIR=/storage/git
VERSION=`date +"%Y-%m-%d"`
VERSION_H=`date +"%B %d, %Y"`
OLDPATH=${PATH}
PROGRAM=naken_asm
FULLNAME=${PROGRAM}-${VERSION}

if [ -d ${BUILDDIR}/${FULLNAME} ]
then
  rm -rf ${BUILDDIR}/${FULLNAME}
fi

cp -r ${SOURCEDIR}/${PROGRAM} ${BUILDDIR}/${FULLNAME}

cd ${BUILDDIR}/${FULLNAME}
make distclean

cat <<EOF > version.h
#ifndef _VERSION_H
#define _VERSION_H

#define VERSION "$VERSION_H"

#endif
EOF

pwd

a=`find . -type d -name "\.svn"`

for b in $a
do
  rm -rf $b
done

a=`find . -type d -name "\.git"`

for b in $a
do
  rm -rf $b
done

rm -f TODO
rm -f out.*
#rm -f asm.s43
rm -f .*.swp
#mkdir tests
#mv *.asm tests/
#mv *.inc tests/

debug=`cat Makefile | grep '^DEBUG='`
if [ "${debug}" != "" ]
then
  echo "debug set"
  exit 1
fi

cd ..
tar cvzf ${FULLNAME}.tar.gz ${FULLNAME}

export PATH=$PATH:/home/mike/mingw32/bin
cd ${BUILDDIR}/${FULLNAME}
make clean
./configure --compiler-prefix=i386-mingw32-
make
rm -f *.o ${FULLNAME}.zip
cd ..
zip -r ${FULLNAME}.zip ${FULLNAME}




