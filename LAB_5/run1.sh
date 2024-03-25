cp system.c /usr/src/minix/minix/kernel/system.c
cd /usr/src/minix
make build MKUPDATE=yes >log.txt 2>log.txt
exit 0