CC = gcc

CFLAGS = -g -Wall

frameRateTracker: frameRateTracker.c frameRateTracker.h
	$(CC) frameRateTracker.c $(CFLAGS) -c

frameRateTracker_tests: generateListCode frameRateTracker.c frameRateTracker.h frameRateTracker_tests.c
	$(CC) frameRateTracker.c frameRateTracker_tests.c $(CFLAGS) -o frameRateTracker_tests

generateListCode: ../../dataStructures/templates/listTemplate.h ../../dataStructures/templates/listTemplate.inc
	../../dataStructures/generateList.py listHeaders listCode ptrTable:ptrTableNode
