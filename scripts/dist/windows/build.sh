#!/usr/bin/env sh

FULLNAME=naken_asm-$1

git clone https://github.com/mikeakohn/naken_asm.git
cd naken_asm

cat <<EOF > common/version.h
#ifndef _VERSION_H
#define _VERSION_H

#define VERSION "$VERSION_H"

#endif
EOF

make clean
./configure --compiler-prefix=i686-w64-mingw32-
make
rm -rf build/asm build/common build/disasm build/fileio build/prog build/simulate build/table .git ${FULLNAME}.zip
cd ..
mv naken_asm ${FULLNAME}
zip -r ${FULLNAME}.zip ${FULLNAME}

cp ${FULLNAME}.zip /dist/

