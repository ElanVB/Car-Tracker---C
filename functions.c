/*
 * functions.c
 Student number: 18384439
 *
 *  Created on: 30 Sep 2014
 *      Author: Elan van Biljon
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "myData.h"
#include "functions.h"
#include "structures.h"
#include "StringHandling.h"

/*
 * gets the date passed to it by reference(or as an array/ pointer) and splits it and stores the values into 6 integers that are also passed by reference
 */

void extractDate(char *date, int *day, int *month, int *year, int *hour, int *min, int *sec)
{
	char sday[3] = {'\0'}, smonth[3] = {'\0'}, syear[5] = {'\0'}, shour[3] = {'\0'}, smin[3] = {'\0'}, ssec[3] = {'\0'};

	/*
	 * first splits the date intp 6 seperate strings
	 */

	strncpy (sday, date, 2);
	date = date + 3;

	strncpy (smonth, date, 2);
	date = date + 3;

	strncpy (syear, date, 4);
	date = date + 5;

	strncpy (shour, date, 2);
	date = date + 3;

	strncpy (smin, date, 2);
	date = date + 3;

	strncpy (ssec, date, 2);


	/*
	 * scans the 6 seperate strings into 6 seperate integers that were passed to the function by reference
	 */

	sscanf(sday, "%d", day);
	sscanf(smonth, "%d", month);
	sscanf(syear, "%d", year);
	sscanf(shour, "%d", hour);
	sscanf(smin, "%d", min);
	sscanf(ssec, "%d", sec);
}

time_t convertTimeToEpoch(char* datetime)
{
	time_t result;
	struct tm humanTime;
	int day, month, year, hour, min, sec;

	extractDate(datetime, &day, &month, &year, &hour, &min, &sec);


	/*
	 *take the values that the previous function has extracted and put them into the members of a time_t variable
	 */

	humanTime.tm_mday = day;
	humanTime.tm_mon = (month - 1); //month offset (jan is 0th month
	humanTime.tm_year = (year - 1900); //year offset, starts at 1900
	humanTime.tm_hour = hour;
	humanTime.tm_min = min;
	humanTime.tm_sec = sec;

	result = mktime(&humanTime); //give the member to the mktime function and it returns with epoch time

	return result;
}

/*
 * takes in a trace line type and converts it to a number trace type
 */

nr_trace_t convertTrace(str_trace_line_t trace_in)
{
	nr_trace_t result;
	char stime[20], slatitude[20], slongitude[20], sspeed[20];
	double latitude = 0, longitude = 0, speed = 0;


	/*
	 * parseCSV function splits the trace line into 4 strings
	 */

	parseCSV(trace_in.Line, stime, slatitude, slongitude, sspeed);


	/*
	 * the strings are then scaned into other variables as doubles
	 */

	sscanf(slatitude, "%lf", &latitude);
	sscanf(slongitude, "%lf", &longitude);
	sscanf(sspeed, "%lf", &speed);


	/*
	 * the new values are then stored in the members of a number trace type
	 */

	result.speed = speed * 1.6; //conversion to km/h from m/h
	result.coord.latitude = latitude;
	result.coord.longitude = longitude;
	result.timestamp = (long) convertTimeToEpoch(stime); //converts the string time into epochtime and strores it in the timestamp member

	if(DebugConvertTrace)
	{
		static int counter = 0;
		printf("%d, %ld, %lf, %lf, %lf\n", counter, (long) convertTimeToEpoch(stime), latitude, longitude, speed);
		printf("%d, %ld, %lf, %lf, %lf\n", counter++, (long) result.timestamp, result.coord.latitude, result.coord.longitude, result.speed);
	}
	return result;
}


/*
 * a function that loops the convertTrace function for an array and converts all elements to number traces
 * the function takes in an array of string trace lines and number traces, the string traces lines are then
 * converted and stored in the array that was (technically) passed by reference (as all arrays are
 */

void convertTraceArrayToNumbers(str_trace_line_t *strTrace, nr_trace_t *numTrace, int numOfTrace)
{
	int i;
	for(i = 0; i < numOfTrace; i++)
	{
		numTrace[i] = convertTrace(strTrace[i]);

		if(DebugconvertTraceArrayToNumbers)
		{
			printf("%d, %ld, %lf, %lf, %lf\n", i, (long) numTrace[i].timestamp, numTrace[i].coord.latitude, numTrace[i].coord.longitude, numTrace[i].speed);
		}
	}

	if(DebugPostconvertTraceArrayToNumbers)
	{
		i--;
		printf("%d, %ld, %lf, %lf, %lf\n", i, (long) numTrace[i].timestamp, numTrace[i].coord.latitude, numTrace[i].coord.longitude, numTrace[i].speed);
	}
}


/*
 * sorts the number traces in order from the earliest to the latest
 */

void sortNrTraces(nr_trace_t *traces, int numOfTraces)
{
	if(DebugPreSort)
	{
		int i = numOfTraces - 1;
		printf("%d, %ld, %lf, %lf, %lf\n", i, (long) traces[i].timestamp, traces[i].coord.latitude, traces[i].coord.longitude, traces[i].speed);
	}

	int x, y;


	/*
	 * this isnt bubble sort, it is slightly more efficient and
	 */

	for(x = 0; x < numOfTraces - 1; x++)
	{

		if(DebugSort)
		{
			printf("%d, %ld, %lf, %lf, %lf\n", x, (long) traces[numOfTraces - 1].timestamp, traces[numOfTraces - 1].coord.latitude, traces[numOfTraces - 1].coord.longitude, traces[numOfTraces - 1].speed);
		}

		for(y = x + 1; y < numOfTraces; y++)
		{
			if(traces[x].timestamp > traces[y].timestamp)
			{
				nr_trace_t temp = traces[x];
				traces[x] = traces[y];
				traces[y] = temp;
			}
		}
	}

	if(DebugPostSort)
	{
		int i = numOfTraces - 1;
		printf("%d, %ld, %lf, %lf, %lf\n", i, (long) traces[i].timestamp, traces[i].coord.latitude, traces[i].coord.longitude, traces[i].speed);
	}
}


/*
 * this function loops through the entire number trace array given to it and finds the min max and mean speed of the array
 * and since integers are parsed by reference to this function thier values are changed with in the function
 */

void calcSpeedStats(nr_trace_t traces[], int numOfItems, double *meanSpeed, double *maxSpeed, double *minSpeed)
{
	double totalSpeed = 0;
	*meanSpeed = 0;
	*maxSpeed = 0; //initialize max speed to something unrealisticly small
	*minSpeed = 999999999; //initialize min speed to something unrealisticly big

	int i;
	for(i = 0; i < numOfItems; i++)
	{
		totalSpeed = totalSpeed + traces[i].speed; //sum up speeds

		if(*maxSpeed < traces[i].speed) //check if current traces speed is higher than current max
		{
			*maxSpeed = traces[i].speed; //replaces current max with current traces soeed
		}

		if(*minSpeed > traces[i].speed)
		{
			*minSpeed = traces[i].speed; //checks if current traces speed is smaller than the current min and replaces if so
		}
	}

	*meanSpeed = (totalSpeed/numOfItems); //gets average speed
}


/*
 * calculates the duration of the trip by subtracting the last trace's epoch time from the first
 */

long calcDuration(nr_trace_t *numTraces, int numOfTraces)
{
	long result;

	result = numTraces[numOfTraces - 1].timestamp - numTraces[0].timestamp;

	if(DebugCalcDuration)
	{
		printf("num Traces: %d, Last time: %ld, First time: %ld\n", numOfTraces, (long) numTraces[numOfTraces - 1].timestamp, (long) numTraces[0].timestamp);
	}

	return result;
}


/*
 * converts seconds to hours
 */

double convertSecToHour(long seconds)
{
	double time;

	time = ((double) seconds/3600);

	return time;
}


/*
 * this function simply applies the haversine formula to two sets of coordinates to find the straight line distance between them
 * the two coordinates are read in by reference
 */

double distanceHaversine(coord_t* coord1, coord_t* coord2)
{
	double result, latitude1, latitude2, longitude1, longitude2, a, c;
	static const int R = 6371; //radius of the earth (average)

	latitude1 = coord1->latitude * (M_PI/180); //converts from degrees to radians
	latitude2 = coord2->latitude * (M_PI/180);
	longitude1 = coord1->longitude * (M_PI/180);
	longitude2 = coord2->longitude * (M_PI/180);

	a = pow(sin((latitude2 - latitude1)/2), 2) + cos(latitude1) * cos(latitude2) * pow(sin((longitude2 - longitude1)/2), 2);

	c = 2 * (atan2(sqrt(a), sqrt(1 - a)));

	result = R * c;

	return result;
}


/*
 * loops the haversine formula for all traces in the array and accumulates the distance and returns it
 */

double calcTotalHaversine(nr_trace_t *numTrace, int numOfTrace)
{
	double result = 0;

	int i;
	for(i = 0; i < numOfTrace - 1; i++)
	{
		result = result + distanceHaversine(&numTrace[i].coord, &numTrace[i + 1].coord);
	}

	return result;
}


/*
 * this takes the average speed between two points and the time change between two points and multiplies them together
 * it then loops this for the whole number trace array to find the accumulated distance
 */

double calcRiemannDist(nr_trace_t *numTrace, int numOfTrace)
{
	double result = 0;

	int i;
	for(i = 0; i < numOfTrace - 1; i++)
	{
		double averageSpeed = (numTrace[i].speed + numTrace[i + 1].speed)/2;
		double timeChange = ((double) numTrace[i + 1].timestamp - (double) numTrace[i].timestamp)/3600;

		result = result + (averageSpeed * timeChange);
	}

	return result;
}


/*
 * this function asks the user to specify a filename, it checks if it exists, and then it counts the number if lines in the file
 * the function gets two arguments by reference (an integer and a string) it alters these two values by reference
 */

nr_trace_t * readCSVFile(int * traceCounter, char * fileName)
{
	setbuf(stdout, 0);

	FILE *filePtr;
	*traceCounter = -1; //start counter at -1 to account for the headings

	printf("Enter the Text file's name to be read from to get the Traces(max 100 characters): \n");
	scanf("%s", fileName);

	filePtr = fopen(fileName, "r");

	while(filePtr == NULL) //checks if file exists
	{
		printf("Invalid file name \n");
		printf("Enter the Text file's name to be read from to get the Traces(max 100 characters): \n");
		scanf("%s", fileName);
		filePtr = fopen(fileName, "r");
	}

	do
	{
		char temp[100];
		fgets(temp, 100, filePtr);//moves cursor on in file
		(*traceCounter)++;//counts lines
	}while (!feof(filePtr));

	fclose(filePtr);

	return (nr_trace_t*) malloc((*traceCounter) * sizeof(nr_trace_t)); //returns a malloc of the size required to store all the elements in the file in a number trace array

}


/*
 * this function then reads all the elements from the file and converts them to number traces and stores them in the array given as the first argument
 */

void populateArrayWithFile(nr_trace_t *arrayOfTraces, const int numOfTraces, const char *fileName)
{
	FILE *filePtr;

	filePtr = fopen(fileName, "r");//do not need to check if file exists as this was already done

	int i;
	for(i = 0; i < numOfTraces; i++)
	{
		str_trace_line_t LineOfFile;

		if(i == 0)
		{
			fgets(LineOfFile.Line, 100, filePtr); //gets headings and ignores them
		}

		fgets(LineOfFile.Line, 100, filePtr);

		arrayOfTraces[i] = convertTrace(LineOfFile); //converts file line by line
	}
	fclose(filePtr);
}


/*
 * this function takes in an array of traces and POIs, it then loops through them and determines when (and if) the taxi arrived in and departed from a town
 */

void calcArrDepPOI(poi_t * POI, nr_trace_t * tracesPtr, int numOfItems, int numPOI)
{
	int inCity = 0;

	int **poiArrivalDeparture = (int**) malloc(numPOI * sizeof(int)); //this will become a 2D array that will store a true or false value for the taxi arriving at and departing from a POI
	int traceCounter;
	int poiCounter;

	for(poiCounter = 0; poiCounter < numPOI; poiCounter++)
	{
		poiArrivalDeparture[poiCounter] = (int*) malloc(2 * sizeof(int)); //gives the array its second dimension
	}

	for(poiCounter = 0; poiCounter < numPOI; poiCounter++) //initializes all values to false
	{
		poiArrivalDeparture[poiCounter][0] = 0;
		poiArrivalDeparture[poiCounter][1] = 0;
	}

	for(poiCounter = 0; poiCounter < numPOI; poiCounter++) //loop through all pois
	{
		for(traceCounter = 0; traceCounter < numOfItems; traceCounter++) //then loop through all traces
		{

			/*
			 * if the straight line distance between two points is smaller than the raidus of the POI, and the taxi isnt already in a POI, and if the taxi hasnt already arrived in this specific POI
			 * this excludes the first point as the -1 factor for getting the timestamp would cause errors
			 */
			if(distanceHaversine(&tracesPtr[traceCounter].coord, &POI[poiCounter].coord) < POI[poiCounter].radius && !inCity && poiArrivalDeparture[poiCounter][0] == 0 && traceCounter != 0)
			{
				poiArrivalDeparture[poiCounter][0] = 1; //set that the taxi now has arrived in this specific POI
				POI[poiCounter].arrival = tracesPtr[traceCounter - 1].timestamp; //the taxi is now already in the POI so set the time it arrived to be the previous traces timestamp
				convertTimeFromEpoch(tracesPtr[traceCounter - 1].timestamp, POI[poiCounter].arrivalStr);
				inCity = 1;

				if(DebugCalcArrDepPOI)
				{
					printf("Arrival:   ");
					printPOI(POI[poiCounter]);
				}
			}
			else
			{
				if(inCity && distanceHaversine(&tracesPtr[traceCounter].coord, &POI[poiCounter].coord) > POI[poiCounter].radius && poiArrivalDeparture[poiCounter][0] == 1)
				{
					poiArrivalDeparture[poiCounter][1] = 1; //set that the taxi now has deoparted from this specific POI
					POI[poiCounter].departure = tracesPtr[traceCounter - 1].timestamp; //the taxi is now already outside the POI so set the time it departed to be the previous traces timestamp
					convertTimeFromEpoch(tracesPtr[traceCounter - 1].timestamp, POI[poiCounter].departureStr);
					inCity = 0;

					if(DebugCalcArrDepPOI)
					{
						printf("Departure: ");
						printPOI(POI[poiCounter]);
					}
				}
			}
		}
	}

	for(poiCounter = 0; poiCounter < numPOI; poiCounter++) //this then checks the first trace and if the taxi is in a POI it sets the arrival to false as the data set stared with the taxi at that location
	{
		if(distanceHaversine(&tracesPtr[0].coord, &POI[poiCounter].coord) < POI[poiCounter].radius)
		{
			poiArrivalDeparture[poiCounter][0] = 0;
		}
	}

	for(poiCounter = 0; poiCounter < numPOI; poiCounter++) //checks which POIs where never arrived to
	{
		if(poiArrivalDeparture[poiCounter][0] == 0)
		{
			POI[poiCounter].arrival = 0;

			strncpy(POI[poiCounter].arrivalStr, "N/A", 3); //set arrival time to zero and arrival string to N/A
			POI[poiCounter].arrivalStr[3] = '\0';
		}

		if(poiArrivalDeparture[poiCounter][1] == 0)//checks which POIs where never departed from
		{
			POI[poiCounter].departure = 0;

			strncpy(POI[poiCounter].departureStr, "N/A", 3);//set departure time to zero and departure string to N/A
			POI[poiCounter].departureStr[3] = '\0';
		}
	}

	for(poiCounter = 0; poiCounter < numPOI; poiCounter++) //frees the 2D array
	{
		free(poiArrivalDeparture[poiCounter]);
	}
	free(poiArrivalDeparture);
}


/*
 *takes in an array of POIs and prints the details to a txt file
 */

void printPOIToText(poi_t * POI, int numberOfPOI)
{
	FILE *filePtr = fopen("myPOI.txt", "w");
	int timeSpent = 0;
	int i;

	for(i = 0; i < numberOfPOI; i++)
	{
		if(i == 0) //prints the heading to the file before it prints the data
		{
			fputs("\t\tPOI \t\t\t\t\tArrival \t\t\t\t\tDeparture \t\t\t\t\t\tSpent in\n--------------------------------------------------------------------------------------------------------------------\n", filePtr);
		}

		if(POI[i].departure > 0 && POI[i].arrival > 0) //calculates time spent in the POI
		{
			timeSpent = (POI[i].departure - POI[i].arrival)/60;
		}
		else
		{
			timeSpent = 0;
		}
		fprintf(filePtr, "%20s %30s %30s %10d min\n", POI[i].name, POI[i].arrivalStr, POI[i].departureStr, timeSpent); //prints the data intp the txt file
	}
	fclose(filePtr);
}


/*
 * this funtion counts how many poi type variables are stored in the binary file and allocates enough memory to store all the POIs to an array
 */

poi_t * countAndAllocateMemoryPOI(char * fileName, int *numOfPOI)
{
	setbuf(stdout, 0);

	FILE *filePtr;
	*numOfPOI = -1;

	poiFileHandling(fileName);

	filePtr = fopen(fileName, "rb");

	while(filePtr == NULL)
	{
		printf("Invalid file name \n");
		poiFileHandling(fileName);
		filePtr = fopen(fileName, "rb");
	}

	do
	{
		char temp[sizeof(poi_t)];
		fread(temp, sizeof(poi_t), 1, filePtr);//counts lines
		(*numOfPOI)++;
	}while (!feof(filePtr));

	fclose(filePtr);

	return (poi_t*) malloc((*numOfPOI) * sizeof(poi_t)); //returns a malloc of sufficient size to store data in an array
}


/*
 * this function then reads the pois in from a binary file and stores them into the array that is given as the first argument
 */

void ReadInPOI(poi_t * poiArray, char *fileName, int numOfPOI)
{
	FILE *filePtr = fopen(fileName, "r");

	int i;
	for(i = 0; i < numOfPOI; i++)
	{
		fread(&poiArray[i], sizeof(poi_t), 1, filePtr);
	}
	fclose(filePtr);
}


/*
 * this function creates a new binary file and stores the poi array given to it in this file
 */

void WritePOIToBinary(poi_t * poiArray, int numOfPOI)
{
	FILE *filePtr = fopen("UpdatedPOI.bin", "wb");

	int i;
	for(i = 0; i < numOfPOI; i++)
	{
		fwrite(&poiArray[i], sizeof(poi_t), 1, filePtr);
	}
	fclose(filePtr);
}


/*
 * this function then reads in the pois from your binaryfile that the previous function created and stores them in an array
 */

void ReadInUpdatedPOI(poi_t * poiArray, int numOfPOI)
{
	FILE *filePtr = fopen("UpdatedPOI.bin", "r");

	int i;
	for(i = 0; i < numOfPOI; i++)
	{
		fread(&poiArray[i], sizeof(poi_t), 1, filePtr);
	}
	fclose(filePtr);
}


/*
 * displays all the poi data of the pois passed to it
 */

void displayAllPOI(poi_t * poiArray, int numOfPOI)
{
	int i;
	for(i = 0; i < numOfPOI; i++)
	{
		printPOI(poiArray[i]);
	}
}


/*
 * this function allows the user to enter a date and time and the function will find the trace at that time
 * or if it doesnt exist it will find the next trace (as stated in the PDF project description)
 */

nr_trace_t findVehicleLocationlinearSearch(nr_trace_t * traces, int numOfTraces)
{
	nr_trace_t result;
	char userTime[80] = {'\0'}, date[40] = {'\0'}, time[40] = {'\0'};
	time_t tempUserEpoch;
	int counter = 0;

	printf("Enter the date and time please: (DD/MM/YYYY HH:MM:SS)\n");
	scanf("%s %s", date, time); //scans in the date and time seperated by a space into two variables (strings)
	sprintf(userTime, "%s %s", date, time); //uses the sprintf function to combine these two strings and stores them in one string

	while(!checkValidity(userTime, traces, numOfTraces)) //check if the date they entered is valid and if not ask them to enter a different one
	{
		printf("Invalid entry.\nEnter the date and time please: (DD/MM/YYYY HH:MM:SS)\n");
		scanf("%s %s", date, time);
		sprintf(userTime, "%s %s", date, time);
	}

	tempUserEpoch = convertTimeToEpoch(userTime); //converts the entered date into epoch time

	while(tempUserEpoch > traces[counter].timestamp) //compares the epoch time to the timestamp of every trace, as soon as the epoch time is equal to or bigger than the trace timestam you have found the correct trace
	{
		counter++;
	}

	result = traces[counter];

	return result;
}


/*
 * this function takes in an epoch time and checks what element number it is in the array of number traces and returns that number
 */

int findTraceElementlinearSearch(time_t userTime, nr_trace_t * traces)
{
	int counter = 0;

	while(userTime > traces[counter].timestamp)
	{
		counter++;
	}

	return counter;
}


/*
 * checks if the argument is a number and returns true or false
 */

int isNumber(char character)
{
	int result = 0;

	if(character >= '0' && character <= '9')
	{
		result++;
	}

	return result;
}


/*
 * checks if the argument is a '/' or ' ' or ':' and returns true or false
 */

int isPunctuation(char character)
{
	int result = 0;

	if(character == '/' || character == ':' || character == ' ')
	{
		result++;
	}

	return result;
}


/*
 * checks if the argument (which is the entered date) is in the correct format so that the it can be converted to epoch time and returns true or false
 */

int checkFormatValidity(char * enteredDate)
{
	int result = 0;

	if(isNumber(enteredDate[0]))
	{
		if(isNumber(enteredDate[1]))
		{
			if(isPunctuation(enteredDate[2]))
			{
				if(isNumber(enteredDate[3]))
				{
					if(isNumber(enteredDate[4]))
					{
						if(isPunctuation(enteredDate[5]))
						{
							if(isNumber(enteredDate[6]))
							{
								if(isNumber(enteredDate[7]))
								{
									if(isNumber(enteredDate[8]))
									{
										if(isNumber(enteredDate[9]))
										{
											if(isPunctuation(enteredDate[10]))
											{
												if(isNumber(enteredDate[11]))
												{
													if(isNumber(enteredDate[12]))
													{
														if(isPunctuation(enteredDate[13]))
														{
															if(isNumber(enteredDate[14]))
															{
																if(isNumber(enteredDate[15]))
																{
																	if(isPunctuation(enteredDate[16]))
																	{
																		if(isNumber(enteredDate[17]))
																		{
																			if(isNumber(enteredDate[18]))
																			{
																				result++;
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return result;
}


/*
 * checks if the entered date falls in the range of the trip and returns true or false
 */

int checkDateValidity(char *enteredDate, nr_trace_t *traces, int numOfTraces)
{
	int result = 0;
	time_t epochTime = convertTimeToEpoch(enteredDate);

	if(epochTime <= traces[numOfTraces - 1].timestamp && epochTime >= traces[0].timestamp)
	{
		result++;
	}
	return result;
}


/*
 * uses the above two funtions to definitivley decide whether the entered date is valid and can be worked with and returns true or false
 */

int checkValidity(char *enteredDate, nr_trace_t *traces, int numOfTraces)
{
	int result = 0;

	if(checkFormatValidity(enteredDate) && checkDateValidity(enteredDate, traces, numOfTraces))
	{
		result++;
	}

	return result;
}


/*
 * prints out the details of one number trace
 */

void printNrTrace(nr_trace_t trace)
{
	char humanTime[40];
	convertTimeFromEpoch(trace.timestamp, humanTime);
	printf("Human Time: %s\nEpoch Time: %ld\nLatitude: %lf\nLongitude: %lf\nSpeed: %.2lf (km/h)\n", humanTime, (long) trace.timestamp, trace.coord.latitude, trace.coord.longitude, trace.speed);
}


/*
 * prints out the details of one poi
 */

void printPOI(poi_t poi)
{
	printf("%20s", poi.name);
	printf("%20lf", poi.coord.latitude);
	printf("%20lf", poi.coord.longitude);
	printf("%20lf", poi.radius);
	printf("%20ld", (long) poi.arrival);
	printf("%30s", poi.arrivalStr);
	printf("%20ld", (long) poi.departure);
	printf("%30s\n", poi.departureStr);
}


/*
 * sorts the poi array by the date and time the taxi arrived to the poi and then by the departure time
 */

void sortPOI(poi_t *poi, int numOfPOI)
{
	int x, y;

	for(x = 0; x < numOfPOI - 1; x++)
	{
		for(y = x + 1; y < numOfPOI; y++)
		{
			if(poi[x].arrival > poi[y].arrival || poi[x].departure > poi[y].departure)
			{
				poi_t temp = poi[x];
				poi[x] = poi[y];
				poi[y] = temp;
			}
		}
	}
}


/*
 * this function displays the arrival and departure dates and times as well as the time spent in for a certain poi
 */

void displayPOIinfo(poi_t poi)
{
	int timeSpent;
	if(poi.departure > 0 && poi.arrival > 0)
	{
		timeSpent = (poi.departure - poi.arrival)/60;
	}
	else
	{
		timeSpent = 0;
	}
	printf("%20s %30s %30s %10d min\n", poi.name, poi.arrivalStr, poi.departureStr, timeSpent);
}


/*
 * uses a loop and the previous function to print all the important information for all the pois
 */

void displayAllPOIinfo(poi_t *poi, int numOfPOI)
{
	int i;

	printf("\t\tPOI \t\t\tArrival \t\tDeparture \t\tSpent in\n----------------------------------------------------------------------------------------------------------\n");
	for(i = 0; i < numOfPOI; i++)
	{
		displayPOIinfo(poi[i]);
	}
}


/*
 *
 */

void poiFileHandling(char *fileName)
{
	setbuf(stdout, 0);
	int option;

	if(sizeof(poi_t) == 120)
	{
		printf("\nYou should use the 32 bit POI Binary file.\nWould you like to: \n1. Use the default 32 bit POI Binary File \n2. Specify your own Binary file");
		scanf("%d", &option);

		do
		{
			switch(option)
			{
			case 1:
				sprintf(fileName, "%s", "POIs_32bit.bin");
				break;

			case 2:
				printf("Enter your own Binary file name: \n");
				scanf("%s", fileName);
				break;

			default:
				printf("Invalid Option.\n");
				printf("\nYou should use the 32 bit POI Binary file.\nWould you like to: \n1. Use the default 32 bit POI Binary File \n2. Specify your own Binary file");
				scanf("%d", &option);
				break;
			}
		}while(option != 1 && option != 2);
	}
	else
	{
		if(sizeof(poi_t) == 128)
		{
			printf("\nYou should use the 64 bit POI Binary file.\nWould you like to: \n1. Use the default 64 bit POI Binary File \n2. Specify your own Binary file");
			scanf("%d", &option);

			do
			{
				switch(option)
				{
				case 1:
					sprintf(fileName, "%s", "POIs_64bit.bin");
					break;

				case 2:
					printf("Enter your own Binary file name: \n");
					scanf("%s", fileName);
					break;

				default:
					printf("Invalid Option.\n");
					printf("\nYou should use the 64 bit POI Binary file.\nWould you like to: \n1. Use the default 64 bit POI Binary File \n2. Specify your own Binary file");
					scanf("%d", &option);
					break;
				}
			}while(option != 1 && option != 2);
		}
		else
		{
			printf("\nYou should create your own POI Binary file.\nWould you like the program to do it for you now? (y/n)");
			scanf("%d", &option);

			do
			{
				switch(option)
				{
				case 121:
				case 89:
					createOwnPOIFile();
					sprintf(fileName, "%s", "myOwnPOIFile.bin");
					break;

				case 110:
				case 78:
					printf("Enter your own Binary file name: \n");
					scanf("%s", fileName);
					break;

				default:
					printf("Invalid Option.\n");
					printf("\nYou should create your own POI Binary file.\nWould you like the program to do it for you now? (y/n)");
					scanf("%d", &option);
					break;
				}
			}while(option != 121 && option != 89 && option != 78 && option != 110);
		}
	}
}

void createOwnPOIFile()
{
	/*
	 ============================================================================
	 Name        : MakeBinFile.c
	 Author      : mjbooysen
	 Version     :
	 Copyright   : Feel free to copy this as many times as you need or want
	 Description : Use this to create your own POI binary file.
	 ============================================================================
	 */

	poi_t POIs[] = {
			{"Beaufort West", {-32.353988, 22.583382}, 5},
			{"Laingsburg", {-33.196538, 20.858824}, 5},
			{"Graaff Reinett", {-32.251934, 24.540489}, 5},
			{"Stellenbosch", {-33.918969, 18.851545},5},
			{"Tunnel", {-33.744986, 19.056267}, 4},
			{"Worcester", {-33.628914, 19.437704},10},
			{"Aberdeen", { -32.483339, 24.066446},3},
			{"Cradock",{-32.167475, 25.615196},5},
			{"Tarkastad",{-32.006402, 26.260142},5},
			{"Queenstown",{-31.900295, 26.881814},5},
			{"Cofimvaba", {-32.012006, 27.586055}, 4},
			{"Ncobo", {-31.676174, 28.000317}, 5},
			{"Mthatha", {-31.588406,28.790088},10},
	};

	FILE* fPtr;
	fPtr = fopen("myOwnPOIFile.bin", "w");
	fwrite(POIs, sizeof(poi_t), 13, fPtr);
	fclose(fPtr);
}

void calcDistTable(poi_t *POI, int numPOI, nr_trace_t *tracesPtr, int numOfItems)
{
	int startDistCount = 0, firstPoi = 1;
	double tempDistTotal = 0;

	int traceCounter = 0, poiCounter = 0, prevPoi = -1, currPoi = 0, innerPoiCounter = 0;

	double *distancesBetweenPois = malloc((numPOI - 1) * sizeof(double));
	double **accumulatedDistances = (double**) malloc((numPOI) * sizeof(double));

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		accumulatedDistances[poiCounter] = (double*) malloc((numPOI) * sizeof(double));
	}

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		distancesBetweenPois[poiCounter] = 0;
	}

	while((long) POI[currPoi].arrival <= 0 && (long) POI[currPoi].departure <= 0)
	{
		currPoi++;
		prevPoi++;
	}

	while(traceCounter < numOfItems)
	{
		if(startDistCount)
		{
			tempDistTotal = tempDistTotal + distanceHaversine(&tracesPtr[traceCounter - 1].coord, &tracesPtr[traceCounter].coord);
		}

		if(distanceHaversine(&tracesPtr[traceCounter].coord, &POI[currPoi].coord) < POI[currPoi].radius)
		{
			if(!firstPoi)
			{
				distancesBetweenPois[prevPoi] = tempDistTotal;
				tempDistTotal = 0;
			}

			firstPoi = 0;
			startDistCount = 1;
			prevPoi++;
			currPoi++;
		}

		traceCounter++;
	}

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		if(distancesBetweenPois[poiCounter] != 0)
		{
			distancesBetweenPois[poiCounter] = distancesBetweenPois[poiCounter] - POI[poiCounter].radius + POI[poiCounter + 1].radius;
		}
	}

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		if(distancesBetweenPois[poiCounter] == 0)
		{
			printf("Distance Between %s and %s is: N/A\n", POI[poiCounter].name, POI[poiCounter + 1].name);
		}
		else
		{
			printf("Distance Between %s and %s is: %lf\n", POI[poiCounter].name, POI[poiCounter + 1].name, distancesBetweenPois[poiCounter]);
		}
	}

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		for(innerPoiCounter = poiCounter + 1; innerPoiCounter < numPOI; innerPoiCounter++)
		{
			if(POI[poiCounter].departure != 0 && POI[innerPoiCounter].departure != 0)
			{
				if(innerPoiCounter == poiCounter + 1)
				{
					accumulatedDistances[poiCounter][innerPoiCounter] = distancesBetweenPois[innerPoiCounter - 1];
				}
				else
				{
					accumulatedDistances[poiCounter][innerPoiCounter] = accumulatedDistances[poiCounter][innerPoiCounter - 1] + distancesBetweenPois[innerPoiCounter - 1];
				}
			}
			else
			{
				accumulatedDistances[poiCounter][innerPoiCounter] = 0;
			}
		}
	}

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		printf("\n");
		for(innerPoiCounter = poiCounter + 1; innerPoiCounter < numPOI; innerPoiCounter++)
		{
			if(accumulatedDistances[poiCounter][innerPoiCounter] == 0)
			{
				printf("Distance Between %s and %s is: N/A\n", POI[poiCounter].name, POI[innerPoiCounter].name);
			}
			else
			{
				printf("Distance Between %s and %s is: %lf\n", POI[poiCounter].name, POI[innerPoiCounter].name, accumulatedDistances[poiCounter][innerPoiCounter]);
			}
		}
	}

	for(poiCounter = 0; poiCounter < numPOI - 1; poiCounter++)
	{
		printf("\n");

		int i;
		for(i = 0; i < poiCounter; i++)
		{
			printf("%5s", " ");
		}

		printf("%20s", POI[poiCounter].name);

		for(innerPoiCounter = poiCounter + 1; innerPoiCounter < numPOI; innerPoiCounter++)
		{
			if(accumulatedDistances[poiCounter][innerPoiCounter] == 0)
			{
				printf("%5s", "N/A");
			}
			else
			{
				printf("%5.0lf", accumulatedDistances[poiCounter][innerPoiCounter]);
			}
		}
	}


	for(poiCounter = 0; poiCounter < numPOI; poiCounter++)
	{
		free(accumulatedDistances[poiCounter]);
	}
	free(accumulatedDistances);

	free(distancesBetweenPois);
}

int inPOI(nr_trace_t trace, poi_t *pois, int numOfPois)
{
	int result = -1, poiCounter;

	for(poiCounter = 0; poiCounter < numOfPois; poiCounter++)
	{
		if(distanceHaversine(&trace.coord, &pois[poiCounter].coord) <= pois[poiCounter].radius)
		{
			result = poiCounter;
		}
	}

	return result;
}

void displayExtraSearchInfo(nr_trace_t trace, poi_t *pois, int numOfPois, nr_trace_t *allTraces, int numOfTraces)
{
	int poiNumber = inPOI(trace, pois, numOfPois);
	int prevPoiElement = 0, nextPoiElement = 0, traceElement, poiCounter, counter;
	int *poiElementNums = malloc(numOfPois * sizeof(int));
	double prevPoiDist = 0, nextPoiDist = 0;
	poi_t prevPoi, nextPoi;
	time_t timeSpent = 0;

	prevPoi.arrival = 0;
	prevPoi.departure = 0;
	strncpy(prevPoi.name, "Begining of Trip", 16);

	nextPoi.arrival = 0;
	nextPoi.departure = 0;
	strncpy(nextPoi.name, "End of Trip", 11);


	for(counter = 0; counter < numOfPois; counter++)
	{
		poiElementNums[counter] = 0;
	}

	if(poiNumber != -1 && pois[poiNumber].arrival != 0)
	{
		timeSpent = (trace.timestamp - pois[poiNumber].arrival);
		printf("The time spent in %s so far (from the found trace) is: %d seconds\n", pois[poiNumber].name, timeSpent);
	}
	else
	{
		for(poiCounter = 0; poiCounter < numOfPois; poiCounter++)
		{
			if(pois[poiCounter].arrival != 0 && pois[poiCounter].departure != 0)
			{
				poiElementNums[poiCounter] = findTraceElementlinearSearch(((time_t)(pois[poiCounter].departure + pois[poiCounter].arrival)/2), allTraces);
			}
			else
			{
				if(pois[poiCounter].arrival == 0)
				{
					poiElementNums[poiCounter] = 0;
				}
				else
				{
					poiElementNums[poiCounter] = numOfTraces;
				}
			}
		}

		traceElement = findTraceElementlinearSearch(trace.timestamp, allTraces);

		for(poiCounter = 0; poiCounter < numOfPois - 1; poiCounter++)
		{
			if(traceElement >= poiElementNums[poiCounter] && traceElement <= poiElementNums[poiCounter + 1])
			{
				prevPoi = pois[poiCounter];
				nextPoi = pois[poiCounter + 1];
				prevPoiElement = findTraceElementlinearSearch(prevPoi.departure, allTraces);
				nextPoiElement = findTraceElementlinearSearch(nextPoi.arrival, allTraces);
			}
			else
			{
				if(traceElement > poiElementNums[poiCounter + 1])
				{
					prevPoi = pois[numOfPois - 1];
					prevPoiElement = findTraceElementlinearSearch(prevPoi.departure, allTraces);
					nextPoiElement = numOfTraces;
				}
			}
		}

		for(counter = prevPoiElement; counter < nextPoiElement - 1; counter++)
		{
			if(counter < traceElement)
			{
				prevPoiDist = prevPoiDist + distanceHaversine(&allTraces[counter].coord, &allTraces[counter + 1].coord);
			}

			if(counter > traceElement)
			{
				nextPoiDist = nextPoiDist + distanceHaversine(&allTraces[counter - 1].coord, &allTraces[counter].coord);
			}
		}

		prevPoiDist = prevPoiDist + prevPoi.radius;
		nextPoiDist = nextPoiDist + nextPoi.radius;

		printf("The distance from the last POI (%s) is: %lf km\n", prevPoi.name, prevPoiDist);
		printf("The distance to the next POI (%s) is: %lf km\n", nextPoi.name, nextPoiDist);

		free(poiElementNums);
	}
}

void plotTracesPathAndPOIs(nr_trace_t *traces, int numOfTraces, poi_t *pois, int numOfPois) //some credit for this must go to Emile Nel as he and I discussed this and he helped a great deal
{
	char longstring[2048] = {}, label = '\0';
	int traceCounter, poiCounter;

	sprintf(longstring,"start chrome \"http://maps.googleapis.com/maps/api/staticmap?center=South+Africa&zoom=6&size=800x500&maptype=hybrid&path=color:0x0000ff|weight:4");

	for(traceCounter = 0; traceCounter < numOfTraces; traceCounter += 25)
	{
		sprintf(longstring,"%s|%.2lf,%.2lf", longstring, traces[traceCounter].coord.latitude, traces[traceCounter].coord.longitude);
	}

	sprintf(longstring,"%s&markers=", longstring);

	for(poiCounter = 0; poiCounter < numOfPois; poiCounter++)
	{
		if(pois[poiCounter].arrival > 0 || pois[poiCounter].departure > 0)
		{
			label = pois[poiCounter].name[0];
			sprintf(longstring,"%s|label:%c", longstring, label);
			sprintf(longstring,"%s|%.2lf,%.2lf", longstring, pois[poiCounter].coord.latitude, pois[poiCounter].coord.longitude);
		}
	}
	sprintf(longstring,"%s\"", longstring);
	system(longstring);
}

void randomFileFindAndDisplay()
{
	int randomNumber = 0, currTime = 0;
	char fileName[40] = {'\0'};
	char line[200] = {'\0'};
	FILE *headingFile;
	srand(time(NULL));

	randomNumber = rand() % 20 + 1;

	sprintf(fileName, "%d.txt", randomNumber);

	headingFile = fopen(fileName, "r");

	while(!feof(headingFile))
	{
		fgets(line, 200, headingFile);
		printf("%s", line);

		currTime = time(NULL);
		while(time(NULL) < currTime + 1)
		{

		}
	}

	currTime = time(NULL);
	while(time(NULL) < currTime + 1)
	{

	}
	printf("\n");
}

void display42()
{
	int currTime = 0;
	char line[300] = {'\0'};
	FILE *headingFile = fopen("42.txt", "r");

	while(!feof(headingFile))
	{
		fgets(line, 200, headingFile);
		printf("%s", line);

		currTime = time(NULL);
		while(time(NULL) < currTime + 1)
		{

		}
	}

	currTime = time(NULL);
	while(time(NULL) < currTime + 1)
	{

	}
	printf("\n");
}

void displayOptions()
{
	printf("\n----------------------------------------------------\n");
	printf("What would you like to see?(0 to stop)\n");
	printf("----------------------------------------------------\n");
	printf("1. CalcStats and estimated Distance\n");
	printf("2. Haversine Distance\n");
	printf("3. Riemann Distance\n");
	printf("4. Use file for all of above\n");
	printf("5. See all POI data\n");
	printf("6. Search for a specific trace by time\n");
	printf("7. Display distances between each Poi\n");
	printf("8. Plot POI's on Google Maps\n");
	printf("9. Demo Requested Info\n");
	printf("----------------------------------------------------\n");
}

void menu()
{
	str_trace_line_t traces[] = all_the_traces;
	const int amountOfTraces = sizeof(traces)/sizeof(traces[0]);
	nr_trace_t arrayOfNumberTraces[amountOfTraces];

	double meanSpeed = 0, maxSpeed = 0, minSpeed = 999999, estimatedDistance = 0, durationHour = 0, distHaversine = 0, distRiemann = 0;
	long duration = 0;

	nr_trace_t *arrayOfNumberTracesFile = NULL;
	int amountOfTracesInFile = 0, amountOfPOI = 0, option = 0;
	char fileName[100];

	poi_t myPOI = {"Beaufort West", {-32.353988, 22.583382}, 5};
	poi_t *myReadInPOI = NULL;
	nr_trace_t foundValue;

	setbuf(stdout, 0);

	randomFileFindAndDisplay();
	displayOptions();
	scanf("%d", &option);

	while(option != 0)
	{
		switch(option)
		{
		case 1:
			convertTraceArrayToNumbers(traces, arrayOfNumberTraces, amountOfTraces);
			sortNrTraces(arrayOfNumberTraces, amountOfTraces);

			calcSpeedStats(arrayOfNumberTraces, amountOfTraces, &meanSpeed, &maxSpeed, &minSpeed);
			printf("ACTUAL DATA\n");
			printf("Mean speed: %f (km/h); ",meanSpeed);
			printf("Max speed: %f (km/h); ",maxSpeed);
			printf("Min speed is %f (km/h); \n",minSpeed);

			duration = calcDuration(arrayOfNumberTraces, amountOfTraces);
			printf("Duration of Trip: %ld seconds\n", duration);

			durationHour = convertSecToHour(duration);
			printf("Duration of Trip: %lf Hours\n", durationHour);

			estimatedDistance = durationHour * meanSpeed;
			printf("Estimated Distance: %lf km\n", estimatedDistance);

			displayOptions();
			scanf("%d", &option);
			break;

		case 2:
			convertTraceArrayToNumbers(traces, arrayOfNumberTraces, amountOfTraces);
			sortNrTraces(arrayOfNumberTraces, amountOfTraces);

			distHaversine = calcTotalHaversine(arrayOfNumberTraces, amountOfTraces);
			printf("Haversine Distance: %lf km\n", distHaversine);

			displayOptions();
			scanf("%d", &option);
			break;

		case 3:
			convertTraceArrayToNumbers(traces, arrayOfNumberTraces, amountOfTraces);
			sortNrTraces(arrayOfNumberTraces, amountOfTraces);

			distRiemann = calcRiemannDist(arrayOfNumberTraces, amountOfTraces);
			printf("Riemann Distance: %lf km\n", distRiemann);

			displayOptions();
			scanf("%d", &option);
			break;

		case 4:
			arrayOfNumberTracesFile = readCSVFile(&amountOfTracesInFile, fileName);
			populateArrayWithFile(arrayOfNumberTracesFile, amountOfTracesInFile, fileName);

			sortNrTraces(arrayOfNumberTracesFile, amountOfTracesInFile);
			calcSpeedStats(arrayOfNumberTracesFile, amountOfTracesInFile, &meanSpeed, &maxSpeed, &minSpeed);
			printf("ACTUAL FILE DATA\n");
			printf("Mean speed: %f (km/h); ",meanSpeed);
			printf("Max speed: %f (km/h); ",maxSpeed);
			printf("Min speed is %f (km/h); \n",minSpeed);

			duration = calcDuration(arrayOfNumberTracesFile, amountOfTracesInFile);
			printf("Duration of Trip: %ld seconds\n", duration);

			durationHour = convertSecToHour(duration);
			printf("Duration of Trip: %lf Hours\n", durationHour);

			estimatedDistance = durationHour * meanSpeed;
			printf("Estimated Distance: %lf\n", estimatedDistance);

			distHaversine = calcTotalHaversine(arrayOfNumberTracesFile, amountOfTracesInFile);
			printf("Haversine Distance: %lf\n", distHaversine);

			distRiemann = calcRiemannDist(arrayOfNumberTracesFile, amountOfTracesInFile);
			printf("Riemann Distance: %lf km\n", distRiemann);

			displayOptions();
			scanf("%d", &option);
			break;

		case 5:
			arrayOfNumberTracesFile = readCSVFile(&amountOfTracesInFile, fileName);
			populateArrayWithFile(arrayOfNumberTracesFile, amountOfTracesInFile, fileName);

			sortNrTraces(arrayOfNumberTracesFile, amountOfTracesInFile);

			myReadInPOI = countAndAllocateMemoryPOI(fileName, &amountOfPOI);
			ReadInPOI(myReadInPOI, fileName, amountOfPOI);

			calcArrDepPOI(myReadInPOI, arrayOfNumberTracesFile, amountOfTracesInFile, amountOfPOI);
			sortPOI(myReadInPOI, amountOfPOI);
			WritePOIToBinary(myReadInPOI, amountOfPOI);
			ReadInUpdatedPOI(myReadInPOI, amountOfPOI);
			displayAllPOIinfo(myReadInPOI, amountOfPOI);
			printPOIToText(myReadInPOI, amountOfPOI);

			displayOptions();
			scanf("%d", &option);
			break;

		case 6:
			arrayOfNumberTracesFile = readCSVFile(&amountOfTracesInFile, fileName);
			populateArrayWithFile(arrayOfNumberTracesFile, amountOfTracesInFile, fileName);

			sortNrTraces(arrayOfNumberTracesFile, amountOfTracesInFile);

			myReadInPOI = countAndAllocateMemoryPOI(fileName, &amountOfPOI);
			ReadInPOI(myReadInPOI, fileName, amountOfPOI);

			calcArrDepPOI(myReadInPOI, arrayOfNumberTracesFile, amountOfTracesInFile, amountOfPOI);
			sortPOI(myReadInPOI, amountOfPOI);

			foundValue = findVehicleLocationlinearSearch(arrayOfNumberTracesFile, amountOfTracesInFile);
			printNrTrace(foundValue);

			displayExtraSearchInfo(foundValue, myReadInPOI, amountOfPOI, arrayOfNumberTracesFile, amountOfTracesInFile);

			displayOptions();
			scanf("%d", &option);
			break;

		case 7:
			arrayOfNumberTracesFile = readCSVFile(&amountOfTracesInFile, fileName);
			populateArrayWithFile(arrayOfNumberTracesFile, amountOfTracesInFile, fileName);

			sortNrTraces(arrayOfNumberTracesFile, amountOfTracesInFile);

			myReadInPOI = countAndAllocateMemoryPOI(fileName, &amountOfPOI);
			ReadInPOI(myReadInPOI, fileName, amountOfPOI);

			calcArrDepPOI(myReadInPOI, arrayOfNumberTracesFile, amountOfTracesInFile, amountOfPOI);
			sortPOI(myReadInPOI, amountOfPOI);

			calcDistTable(myReadInPOI, amountOfPOI, arrayOfNumberTracesFile, amountOfTracesInFile);

			displayOptions();
			scanf("%d", &option);
			break;

		case 8:
			arrayOfNumberTracesFile = readCSVFile(&amountOfTracesInFile, fileName);
			populateArrayWithFile(arrayOfNumberTracesFile, amountOfTracesInFile, fileName);

			sortNrTraces(arrayOfNumberTracesFile, amountOfTracesInFile);

			myReadInPOI = countAndAllocateMemoryPOI(fileName, &amountOfPOI);
			ReadInPOI(myReadInPOI, fileName, amountOfPOI);

			calcArrDepPOI(myReadInPOI, arrayOfNumberTracesFile, amountOfTracesInFile, amountOfPOI);
			sortPOI(myReadInPOI, amountOfPOI);

			//plotPois(myReadInPOI, amountOfPOI);
			plotTracesPathAndPOIs(arrayOfNumberTracesFile, amountOfTracesInFile, myReadInPOI, amountOfPOI);

			displayOptions();
			scanf("%d", &option);
			break;

		case 9:

			convertTraceArrayToNumbers(traces, arrayOfNumberTraces, amountOfTraces);
			sortNrTraces(arrayOfNumberTraces, amountOfTraces);
			calcSpeedStats(arrayOfNumberTraces, amountOfTraces, &meanSpeed, &maxSpeed, &minSpeed);
			duration = calcDuration(arrayOfNumberTraces, amountOfTraces);
			durationHour = convertSecToHour(duration);
			estimatedDistance = durationHour * meanSpeed;
			distHaversine = calcTotalHaversine(arrayOfNumberTraces, amountOfTraces);
			distRiemann = calcRiemannDist(arrayOfNumberTraces, amountOfTraces);

			printf("Project Part 1\n");
			printf("--------------------\n");
			printf("Results with data from myData.h\n");
			printf("--------------------\n");
			printf("The size of the array is %d bytes and each trace is %d bytes.\n", sizeof(traces), sizeof(traces[0]));
			printf("There area %d traces.\n", sizeof(traces)/sizeof(traces[0]));
			printf("Mean speed: %.2lf (km/h); Max speed: %.2lf (km/h); Min speed is %.2lf (km/h)\n", meanSpeed, maxSpeed, minSpeed);
			printf("The duration is %ld (sec) or %.2lf (h)\n", duration, durationHour);
			printf("The approximated (aveSpeed * duration) distance is %.2lf (km)\n", estimatedDistance);
			printf("The approximated (haversine) distance is %.2lf (km)\n", distHaversine);
			printf("The approximated distance (Riemann) is %.2lf (km)\n", distRiemann);


			printf("Project Part 2\n");
			printf("--------------------\n");
			printf("Reading from a CSV file\n");
			printf("--------------------\n");

			arrayOfNumberTracesFile = readCSVFile(&amountOfTracesInFile, fileName);
			populateArrayWithFile(arrayOfNumberTracesFile, amountOfTracesInFile, fileName);
			sortNrTraces(arrayOfNumberTracesFile, amountOfTracesInFile);
			calcSpeedStats(arrayOfNumberTracesFile, amountOfTracesInFile, &meanSpeed, &maxSpeed, &minSpeed);
			duration = calcDuration(arrayOfNumberTracesFile, amountOfTracesInFile);
			durationHour = convertSecToHour(duration);
			estimatedDistance = durationHour * meanSpeed;
			distHaversine = calcTotalHaversine(arrayOfNumberTracesFile, amountOfTracesInFile);
			distRiemann = calcRiemannDist(arrayOfNumberTracesFile, amountOfTracesInFile);

			printf("%d samples read from CSV file\n", amountOfTracesInFile);
			printf("Mean speed: %.2lf (km/h); Max speed: %.2lf (km/h); Min speed is %.2lf (km/h)\n", meanSpeed, maxSpeed, minSpeed);
			printf("The duration is %ld (sec) or %.2lf (h)\n", duration, durationHour);
			printf("The approximated (aveSpeed * duration) distance is %.2lf (km)\n", estimatedDistance);
			printf("The approximated (haversine) distance is %.2lf (km)\n", distHaversine);
			printf("The approximated distance (Riemann) is %.2lf (km)\n", distRiemann);

			printf("Calculating POI information\n");
			printf("--------------------\n");
			calcArrDepPOI(&myPOI, arrayOfNumberTracesFile, amountOfTracesInFile, 1);
			printf("%s:\t Arrival at %s\n", myPOI.name, myPOI.arrivalStr);
			printf("\t\t Departure at %s\n", myPOI.departureStr);
			printf("\t\t Time spent in, %d (min).\n", (myPOI.departure - myPOI.arrival)/60 );

			myReadInPOI = countAndAllocateMemoryPOI(fileName, &amountOfPOI);
			ReadInPOI(myReadInPOI, fileName, amountOfPOI);
			calcArrDepPOI(myReadInPOI, arrayOfNumberTracesFile, amountOfTracesInFile, amountOfPOI);
			sortPOI(myReadInPOI, amountOfPOI);
			WritePOIToBinary(myReadInPOI, amountOfPOI);
			ReadInUpdatedPOI(myReadInPOI, amountOfPOI);
			displayAllPOIinfo(myReadInPOI, amountOfPOI);
			printPOIToText(myReadInPOI, amountOfPOI);

			printf("Project Part 3\n");
			printf("--------------------\n");
			foundValue = findVehicleLocationlinearSearch(arrayOfNumberTracesFile, amountOfTracesInFile);
			printNrTrace(foundValue);
			displayExtraSearchInfo(foundValue, myReadInPOI, amountOfPOI, arrayOfNumberTracesFile, amountOfTracesInFile);

			calcDistTable(myReadInPOI, amountOfPOI, arrayOfNumberTracesFile, amountOfTracesInFile);
			plotTracesPathAndPOIs(arrayOfNumberTracesFile, amountOfTracesInFile, myReadInPOI, amountOfPOI);

			displayOptions();
			scanf("%d", &option);
			break;

		case 42:
			display42();

			displayOptions();
			scanf("%d", &option);
			break;

		default:
			printf("Invalid Option!");

			displayOptions();
			scanf("%d", &option);
			break;
		}
	}
	free(arrayOfNumberTraces);
	free(arrayOfNumberTracesFile);
	free(myReadInPOI);
}
