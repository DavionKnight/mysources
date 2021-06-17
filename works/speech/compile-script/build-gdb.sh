set -e
export PATH=/usr/lib/jvm/java-8-openjdk-amd64/bin:/home/meteor/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/home/compiler/ecovacs/gcc-linaro-6.5.0-2018.12-x86_64_aarch64-linux-gnu/bin/
#CC=aarch64-linux-gnu-gcc
#AR=aarch64-linux-gnu-ar
#LD=aarch64-linux-gnu-ld
#RANLIB=aarch64-linux-gnu-ranlib
#LDSHARED=aarch64-linux-gnu-gcc -shared -Wl,-soname,libz.so.1,--version-script,zlib.map
./configure --prefix=/home/meteor/works/linux-speech-sdk-ecovacs/ssh/gdb-out --host=aarch64-linux-gnu --enable-shared CC=aarch64-linux-gnu-gcc
make
make install
