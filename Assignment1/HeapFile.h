#ifndef HEAP_FILE_H
#define HEAP_FILE_H
#include "BF.h"

typedef struct Record {
	int id;
	char name[15];
	char surname[25];
	char address[50];
} Record;

typedef struct {
	int file_descriptor; 
	char attrType; 
	char* attrName;
	int attrLength; 
	int flag;
  // number of records that our heap file has
  int offset;
  int record_num;
} HP_info;	

int HP_CreateFile( char *filename, /* όνομα αρχείου */
char attrType, /* τύπος πεδίου-κλειδιού: 'c', 'i' */
char* attrName, /* όνομα πεδίου-κλειδιού */
int attrLength /* μήκος πεδίου-κλειδιού */
);

HP_info* HP_OpenFile( char *fileName /* όνομα αρχείου */ );

int HP_CloseFile( HP_info* header_info );

int HP_InsertEntry( HP_info header_info, /* επικεφαλίδα του αρχείου*/ Record record /* δομή που
//προσδιορίζει την εγγραφή */ );

void HP_GetAllEntries(HP_info header_info, void* value);

int HP_DeleteEntry( HP_info header_info, /* επικεφαλίδα του αρχείου*/ void *value /* τιμή του
πεδίου-κλειδιού προς διαγραφή */);

//unsigned int hash(char* str, unsigned int length);
#endif // HEAP_FILE_H