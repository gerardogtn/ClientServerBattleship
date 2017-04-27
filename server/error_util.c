/**
 * Author: Gerardo Teruel
 * Id: A01018057
 * A collection of utility functions to check for errors and print such
 * errors
 */

#include <stdio.h>
#include <stdlib.h>

 // TODO: Add descriptive error messages.
 void printErrorAndExit(char* message) {
   fprintf(stderr, "%s\n", message);
   exit(-1);
 }

 void checkForError(int value, int errValue, char* errMessage) {
   if (value == errValue) {
     printErrorAndExit(errMessage);
   }
 }
