/*
 * StringHandling.c
 *
 *  Created on: 30 Sep 2014
 *      Author: Elan
 */


#include <stdio.h>
#include <stdlib.h>
#include "myData.h"
#include <string.h>
#include <time.h>


int parseCSV(char * srcCSV, char* str1, char* str2, char* str3, char* str4)
{
	 char c;
	 int i,j;
	 if (parseCSV_Debug)  printf("parseCSV>> The length of the string is %d :", strlen(srcCSV));

	 i = 0;
	 j = 0;
	 if (parseCSV_Debug) printf("parseCSV>> Input string: \"%s\"\n",  srcCSV);
	 c = srcCSV[i++];
	 while (c!=',')	 {
		 str1[j++] = c;
		 c = srcCSV[i++];
	 }
	 str1[j++] = '\0';
	 if (parseCSV_Debug) printf("parseCSV>> string1: \"%s\"\n",  str1);
	 c = srcCSV[i++];
	 j = 0;
	 while (c!=',')	 {
		 str2[j++] = c;
		 c = srcCSV[i++];
	 }
	 str2[j++] = '\0';
	 if (parseCSV_Debug) printf("parseCSV>> string2: \"%s\"\n",  str2);
	 c = srcCSV[i++];
	 j = 0;
	 while (c!=',' ) {
		 str3[j++] = c;
		 c = srcCSV[i++];
	 }
	 str3[j++] = '\0';
	 if (parseCSV_Debug) printf("parseCSV>> string3: \"%s\"\n",  str3);
	 c = srcCSV[i++];
	 j = 0;
	 while (c != '\0' && c!=10)	 {
		 str4[j++] = c;
		 c = srcCSV[i++];
	 }
	 str4[j++] = '\0';
	 if (parseCSV_Debug) printf("parseCSV>> string4: \"%s\"\n",  str4);
	 return 0;
}

void convertTimeFromEpoch(time_t epochTime, char * humanTime)
{
	struct tm ts;
	ts = *localtime(&epochTime);
	strftime(humanTime, 26, "%a %Y-%m-%d %H:%M:%S", &ts);
	if(convertTimeFromEpoch_Debug) printf("convertTimeFromEpoch>> %s\n", humanTime);
	return;
}

