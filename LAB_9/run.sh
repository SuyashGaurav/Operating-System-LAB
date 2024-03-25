cp link.c /usr/src/minix/minix/servers/vfs/link.c
cp open.c /usr/src/minix/minix/servers/vfs/open.c
cp read.c /usr/src/minix/minix/servers/vfs/read.c
cd /usr/src/minix
make build MKUPDATE=yes