
/*

https://tools.ietf.org/html/rfc4122

Produce RFC4122 version 4 compliant UUIDs.  Quoting...

   The version 4 UUID is meant for generating UUIDs from truly-random or
   pseudo-random numbers.

   The algorithm is as follows:

   o  Set the two most significant bits (bits 6 and 7) of the
      clock_seq_hi_and_reserved to zero and one, respectively.

   o  Set the four most significant bits (bits 12 through 15) of the
      time_hi_and_version field to the 4-bit version number from
      Section 4.1.3.

   o  Set all the other bits to randomly (or pseudo-randomly) chosen
      values.

...so according to section 4.1.2 that means the 2 most significant bits
(that would be bits 6 and 7) of byte 8 need to be set to 0 and 1, respectively, 
and the most significant 4 bits (that would be bits 4, 5, 6, and 7) of byte 6
need to be set to 0, 1, 0, and 0, respectively, according to section 4.1.3.  

The time_hi_and_version field is a 2-byte field and we only need bother with 
the most significant byte.  Bits 4, 5, 6, and 7 of byte 6 are bits 12 through
15 of the 16 bit time_hi_and_version field in bytes 6 and 7.

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define NB_BYTES 16

int main( int argc, char **argv )
{
  FILE *prngFile;
  unsigned char randoBytes[NB_BYTES];
  unsigned long i = 0;
  unsigned long iterations = 0;

  if ( 1 < argc ) {
    /* set errno to 0 to detect bad input */
    errno = 0;
    iterations = strtoul( argv[1], NULL, 10 );
  } 

  if ( 0 == iterations || errno ) {
    /* if a bad # iterations (or none) is supplied, just do 1 */
    iterations = 1;
  }

  /* this is the neat freak in me */
  memset( randoBytes, '\0', NB_BYTES );

  /*
    https://www.google.com/search?&q=just+use+%2Fdev%2Furandom
  */

  prngFile = fopen( "/dev/urandom", "rb" );

  if ( NULL == prngFile ) {
    /* 
      if this happens it's likely the machine is on fire.  Or
      you're running Windows.  Or DOS.
    */
    perror( "fopen(\"/dev/urandom\", \"rb\") failed" );
    return 16;
  }
 
  for( i = 0; i < iterations; i++ ) {
    /*
      Here's where you need to go back to the top of the file and read
      about the algorithm whose implementation you're looking at.  I know
      you said "TL;DR" a minute ago and just skipped down to the code but
      that's doing you no good now is it?
    */
    fread( randoBytes, 1, NB_BYTES, prngFile );
    randoBytes[8] = randoBytes[8] & 0x3F; /* 0b00111111 turn off two high order bits */
    randoBytes[8] = randoBytes[8] | 0x40; /* 0b01000000 turn on bit 6 */
    randoBytes[7] = randoBytes[6] & 0x0F; /* 0b00001111 turn off four high order bits */
    randoBytes[7] = randoBytes[6] | 0x40; /* 0b01000000 turn on bit 6 */
    printf( "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
          , randoBytes[0]
          , randoBytes[1]
          , randoBytes[2]
          , randoBytes[3]
          , randoBytes[4]
          , randoBytes[5]
          , randoBytes[6]
          , randoBytes[7]
          , randoBytes[8]
          , randoBytes[9]
          , randoBytes[10]
          , randoBytes[11]
          , randoBytes[12]
          , randoBytes[13]
          , randoBytes[14]
          , randoBytes[15]
          );
  }

  fclose( prngFile );
  return 0;

}

