/*
 * StringHandling.h
 *
 *  Created on: 30 Sep 2014
 *      Author: Elan
 */

#ifndef STRINGHANDLING_H_
#define STRINGHANDLING_H_

#include <time.h>
#include "myData.h"

int parseCSV(char * srcCSV, char* str1, char* str2, char* str3, char* str4);
void convertTimeFromEpoch(time_t epochTime, char * humanTime);


#endif /* STRINGHANDLING_H_ */
