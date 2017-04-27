/**
 * Author: Gerardo Teruel
 * Id: A01018057
 * A collection of utility functions to check for errors and print such
 * errors
 */

#ifndef ERROR_UTIL_H
#define ERROR_UTIL_H

void printErrorAndExit(char* message);
void checkForError(int value, int errValue, char* errMessage);


#endif  // ERROR_UTIL_H
