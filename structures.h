/*

 * functions.h
 * Student number: 18384439
 *
 *  Created on: 30 Sep 2014
 *      Author: Elan
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

typedef char* string;

typedef struct
{
	double latitude;
	double longitude;
}coord_t;

typedef struct
{
	time_t timestamp;
	coord_t coord;
	double speed;
}nr_trace_t;

typedef struct placeOfInterest_st {
	char name[20]; //name of place
	coord_t coord; //location in GPS coordinates
	double radius; // place’s area as radius in km
	time_t arrival; // arrival time in epoch format
	char arrivalStr[30]; //arr in human readable format
	time_t departure; //departure time in epoch format
	char departureStr[30]; //dep time in human readable format
}poi_t; //place of interest struct type

#endif /* STRUCTURES_H_ */
