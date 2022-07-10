CC=`(cd ../config; ./setcc.sh)`
BUILD=`(cd ../config; ./pvmgetarch)`

simpkern: simpkern.c
	$(CC) simpkern.c -o simpkern -D$(BUILD)

