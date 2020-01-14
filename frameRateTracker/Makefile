CC = gcc

CFLAGS = -g -Wall

frameRateTracker: frameRateTracker.c frameRateTracker.h
	$(CC) frameRateTracker.c $(CFLAGS) -c

frameRateTracker_tests: frameRateTracker.c frameRateTracker.h frameRateTracker_tests.c
	$(CC) -include ../../studiousBroccoli/utils/testMallocTracker/malloc_redefinition.h -L../../studiousBroccoli/utils/testMallocTracker -Wl,-rpath=$(shell pwd)/../../studiousBroccoli/utils/testMallocTracker -ltestMallocTracker -I../../studiousBroccoli/utils/testMallocTracker -I../../jTypes frameRateTracker.c frameRateTracker_tests.c $(CFLAGS) -o frameRateTracker_tests

generateListCode: ../../dataStructures/templates/listTemplate.h ../../dataStructures/templates/listTemplate.inc
	../../dataStructures/generateList.py listHeaders listCode ptrTable:ptrTableNode
