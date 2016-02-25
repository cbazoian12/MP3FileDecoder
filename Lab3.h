/*
 * Written by Christian Bazoian
 *
 */

#ifndef LAB3_H
#define LAB3_H

/* Function to open the file given on the command line. */
int initialize(int argc, char** argv);

/* Reads a file into memory and stores it as an array of bytes. Returns a pointer to 
   the beginning of the file.*/
unsigned char* readFile(FILE * fp, long int range, long int num); 

/* Function to find the sync bits of an MP3 File. Walks a pointer through the binary MP3 file.
   Looks for 0xFFF and returns the position of the pointer in memory. */
int findSyncBits(FILE* fp, unsigned char* data);

/* Function to find the bit rate of an MP3 file assuming the sync bits assuming the pointer 
   position passed in is set to the correct byte to search. */
int findBitRate(unsigned char* data, int pos);

/* Function to find the Frequency of an MP3 file assuming pointer position passed in is set 
   to correct byte in the file to search.*/
int findFrequency(unsigned char* data, int pos);

/* Function to find the copyright info for an MP3 file assuming pointer position passed in 
   is set to the correct byte in the file to search. */
int findCopyrightInfo(unsigned char * data, int pos);

/* Function to find the size of the file passed in. */
long findFileSize(FILE * fp);


#endif