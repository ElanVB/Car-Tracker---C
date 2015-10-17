/*
 * functions.h
Student number: 18384439
 *
 *  Created on: 30 Sep 2014
 *      Author: Elan
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "structures.h"

#define DebugConvertTrace 0
#define DebugCalcDuration 0
#define DebugconvertTraceArrayToNumbers 0
#define DebugPostconvertTraceArrayToNumbers 0
#define DebugPreSort 0
#define DebugSort 0
#define DebugPostSort 0
#define DebugCalcArrDepPOI 0

void extractDate(char *date, int *day, int *month, int *year, int *hour, int *min, int *sec);
time_t convertTimeToEpoch(char* datetime);
nr_trace_t convertTrace(str_trace_line_t trace_in);
void convertTraceArrayToNumbers(str_trace_line_t* strTrace, nr_trace_t* numTrace, int numOfTrace);
void sortNrTraces(nr_trace_t *numbers, int numOfTraces);
void calcSpeedStats(nr_trace_t traces[], int numOfItems, double * meanSpeed, double * maxSpeed, double * minSpeed);
long calcDuration(nr_trace_t *numTraces, int numOfTraces);
double convertSecToHour(long seconds);
double distanceHaversine(coord_t* coord1, coord_t* coord2);
double calcTotalHaversine(nr_trace_t *numTrace, int numOfTrace);
double calcRiemannDist(nr_trace_t *numTrace, int numOfTrace);
nr_trace_t * readCSVFile(int * numberLines, char * fileName);
void populateArrayWithFile(nr_trace_t *arrayOfTraces, const int numOfTraces, const char *fileName);
void calcArrDepPOI(poi_t * POI, nr_trace_t * tracesPtr, int numOfItems, int numPOI);
void printPOIToText(poi_t * POI, int numberOfPOI);
poi_t * countAndAllocateMemoryPOI(char * fileName, int *numOfPOI);
void ReadInPOI(poi_t * poiArray, char *fileName, int numOfPOI);
void WritePOIToBinary(poi_t * poiArray, int numOfPOI);
void ReadInUpdatedPOI(poi_t * poiArray, int numOfPOI);
void displayAllPOI(poi_t * poiArray, int numOfPOI);
nr_trace_t findVehicleLocationlinearSearch(nr_trace_t * traces, int numOfTraces);
int findTraceElementlinearSearch(time_t userTime, nr_trace_t * traces);
int isNumber(char character);
int isPunctuation(char character);
int checkFormatValidity(char * enteredDate);
int checkDateValidity(char *enteredDate, nr_trace_t *traces, int numOfTraces);
int checkValidity(char *enteredDate, nr_trace_t *traces, int numOfTraces);
void printNrTrace(nr_trace_t trace);
void printPOI(poi_t poi);
void sortPOI(poi_t *poi, int numOfPOI);
void displayPOIinfo(poi_t poi);
void displayAllPOIinfo(poi_t *poi, int numOfPOI);
int inPOI(nr_trace_t trace, poi_t *pois, int numOfPois);
void displayExtraSearchInfo(nr_trace_t trace, poi_t *pois, int numOfPois, nr_trace_t *allTraces, int numOfTraces);
void displayOptions();
void poiFileHandling(char *fileName);
void createOwnPOIFile();
void calcDistTable(poi_t *POI, int numPOI, nr_trace_t *tracesPtr, int numOfItems);
void plotTracesPathAndPOIs(nr_trace_t *traces, int numOfTraces, poi_t *pois, int numOfPois);
void randomFileFindAndDisplay();
void menu();

#endif /* FUNCTIONS_H_ */
