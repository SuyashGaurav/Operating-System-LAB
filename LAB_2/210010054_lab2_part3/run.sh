cp forkexit.c /usr/src/minix/minix/servers/pm/forkexit.c
cd /usr/src/minix
make build MKUPDATE=yes >log.txt 2>log.txt
exit 0