/* 
 * Christian Bazoian
 * CS315 Lab 3: C data types
 * compile with gcc -Wall Lab3.cabs in CygWin terminal
 * run with ./a song.mp3
 */

#include <stdio.h>
#include <stdlib.h>
#include "Lab3.h"


int main( int argc, char ** argv )
{
	const long int range = 10485760;
	const long int num = 1048576;
	
	FILE * fp = fopen(argv[1], "rb"); // open file specified in command line.
	
	int success = initialize(argc, argv);
	
	if(success == EXIT_FAILURE)
	{
		return(EXIT_FAILURE);
	}
	
	if( fp == NULL )
	{
		printf( "Can't open file %s\n", argv[1] );
		return(EXIT_FAILURE);
	}
	
	
	
	// We now have a pointer to the first byte of data in a copy of the file, have fun
	// unsigned char * data    <--- this is the pointer	
	unsigned char* data = readFile(fp, range, num);
	
	//find sync bits and return the pointer position after them to continue
	//checking file for rest of information.
	int pos = findSyncBits(fp, data);
	
	
	//pointer is now at start of bit rate and frequency byte, so check bit rate
	pos = findBitRate(data, pos);
	
	//pointer is still at start of bit rate and frequency, so check second half of byte for frequency bits
	pos = findFrequency(data, pos);
	
	//pointer is now incremented to the start of the fourth and last byte of MP3 frame
	pos = findCopyrightInfo(data, pos);
	
	
	
	// Clean up
	free(data);
	fclose(fp);				// close and free the file
	exit(EXIT_SUCCESS);		// or return 0;
	
} // end main()



// ************* Function Implementations  *********************************************

int initialize(int argc, char** argv)
{
	// Open the file given on the command line
	if( argc != 2 )
	{
		printf( "Usage: %s filename.mp3\n", argv[0] );
		return(EXIT_FAILURE);
	}
	
	return(EXIT_SUCCESS);
}


unsigned char* readFile(FILE * fp, long int range, long int num)
{
	// Loads file into memory and returns a pointer to the beginning of it.
	long size = findFileSize(fp);	// go back to the beginning
	
	if( size < 1 || size > range )
	{
		printf("File size is not within the allowed range\n"); 
		// Clean up
		fclose(fp);				// close and free the file
		exit(EXIT_SUCCESS);		// or return 0;
		
	}
	
	printf( "File size: %ld MB\n", size/num );
	
	// Allocate memory on the heap for a copy of the file
	unsigned char * data = (unsigned char *)malloc(size);
	
	// Read it into our block of memory
	size_t bytesRead = fread( data, sizeof(unsigned char), size, fp );
	
	if( bytesRead != size )
	{
		printf( "Error reading file. Unexpected number of bytes read: %zd\n",bytesRead );
		// Clean up
		fclose(fp);				// close and free the file
		free(data);
		exit(EXIT_SUCCESS);		// or return 0;
		
	}
	
	return(data);
}

int findSyncBits(FILE * fp, unsigned char * data)
{
	long size = findFileSize(fp);
	
	int i = 0;
	
	//search for sync bits 0xFFF
	while(i < size)
	{
		//printf("%x \n",data[i]);
		
		//found first 8 bits
		if(data[i] == 0xff)
		{
			//printf("Found 0xff! \n");
			//check next four bits to confirm these are indeed sync bits
			if(data[i++] & 00001111) // masks out the last four bits and checks half of the byte stored at this location
			{
				//printf("Sync bits found. \n");
				
				//if next bit is 1, then the file is MPEG, otherwise exit program
				//if(data[i++] & 00011111) //mask out all but first five bits alternate way of doing it
				
				if(data[i++] >> 4 & 00000001) //shift bits right 4 spots
				{
					printf("File is MPEG. \n");
				}
				else
				{
					printf("File is not MPEG. Exiting program.");
					exit(EXIT_SUCCESS);
				}
	
				//find if file is MPEG Layer 3
				//shift bits right five spaces and account for whether error checking third bit is set or not
				if( (data[i++] >> 5 & 00000110) || (data[i++] >> 5 & 00000010) )
				{
					printf("File is Layer 3.");
				}
				else
				{
					printf("File is not Layer 3. \n");
					//exit(EXIT_SUCCESS);
				}
				
				
				break; //break out of while loop and leave pointer where it is to access rest of mp3 file
			}
		}
		i++; //i increments in bytes, not bits
	}
	
	
	
	//shift to next byte
	i++;
	
	
	return(i); //returns position of pointer after the first two bytes
	
}

int findBitRate(unsigned char* data, int pos)
{
	if(((5 << data[pos] >> 7)  &&  (7 << data[pos] >> 7))  & 00000001)
	{
		printf("Bit rate: 160 \n");
	}
	else
	{
		printf("Bit rate not standard 160. \n");
	}

	return(pos);
}

int findFrequency(unsigned char* data, int pos)
{
	//shift bits to clear out everything but the two frequency bits
	if(!((2 << data[pos] >> 6)  & 00000011))
	{
		printf("Frequency is 44.1 \n");
	}
	else
	{
		printf("Frequency not 44.1 \n");
	}
	
	pos++;
	
	return(pos);
}

int findCopyrightInfo(unsigned char* data, int pos)
{	 
	//shift bits right four spaces to isolate the bit telling whether file is copyrighted or not
	if(!( 3 << data[pos] >> 7 & 00000001)) //if copyright bit is set to zero, file is not copyrighted
	{
		printf("MP3 File is not copyrighted. \n");
	}
	else
	{
		printf("File is copyrighted. \n");
	}
	
	if(!( 2 << data[pos] >> 7 ) & 00000001) //if original media bit set to 0, it is a copy and not original file
	{
		printf("File is a copy of original media. \n");
	}
	else
	{
		printf("File is original media. \n");
	}
	
	return(pos);
}

long findFileSize(FILE * fp)
{
	// How many bytes are there in the file?  If you know the OS you're
	// on you can use a system API call to find out.  Here we use ANSI standard
	// function calls.
	long size = 0;
	fseek( fp, 0, SEEK_END );		// go to 0 bytes from the end
	size = ftell(fp);				// how far from the beginning?
	rewind(fp);	
	return(size);
}