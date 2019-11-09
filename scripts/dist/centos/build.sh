#/usr/bin/env bash

SOURCEDIR=/storage/git
VERSION=`date +"%Y-%m-%d"`
RPM_VERSION=`date +"%Y.%m.%d"`
VERSION_H=`date +"%B %d, %Y"`
PROGRAM=naken_asm
FULLNAME=${PROGRAM}-${VERSION}

cd ${SOURCEDIR}
#git pull
cd /storage/git && git clone https://github.com/mikeakohn/naken_asm.git
cd

rm -rf rpmbuild
rm -rf ${FULLNAME} ${FULLNAME}.tar.gz
cp -r ${SOURCEDIR}/${PROGRAM} ${FULLNAME}

cat naken_asm.spec.templ | sed "s/#RPM_VERSION#/${RPM_VERSION}/" | sed "s/#VERSION#/${VERSION}/" > naken_asm.spec

rpmdev-setuptree

cd ${FULLNAME}
make clean

cat <<EOF > common/version.h
#ifndef _VERSION_H
#define _VERSION_H

#define VERSION "$VERSION_H"

#endif
EOF

rm -rf .git .gitignore

cd ..
tar cvzf ${FULLNAME}.tar.gz ${FULLNAME}

mv ${FULLNAME}.tar.gz rpmbuild/SOURCES

rpmbuild -ba naken_asm.spec

if [ -d /dist ]
then
  cp rpmbuild/SOURCES/${FULLNAME}.tar.gz /dist/
  cp rpmbuild/RPMS/x86_64/naken_asm-${RPM_VERSION}-1.el7.x86_64.rpm /dist/
fi

