#ifndef HASH_INDEX_H
#define HASH_INDEX_H
#include "BF.h"

extern int primarydesc;

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
  // number of records that our hash index has
  char hash;
  int record_num;
  int bucket_num;
  
} HT_info;	

int HT_CreateIndex( char *indexname, /* όνομα αρχείου */
char attrType, /* τύπος πεδίου-κλειδιού: 'c', 'i' */
char* attrName, /* όνομα πεδίου-κλειδιού */
int attrLength, /* μήκος πεδίου-κλειδιού */
int bucketnum
);

HT_info* HT_OpenIndex( char *indexName /* όνομα αρχείου */ );

int HT_CloseIndex( HT_info* header_info );

int HT_InsertEntry( HT_info header_info, /* επικεφαλίδα του αρχείου*/ Record record /* δομή που
//προσδιορίζει την εγγραφή */ );

void HT_GetAllEntries(HT_info header_info, void* value);

unsigned int hash(unsigned char *str);

int HashStatistics( char* filename /* όνομα του αρχείου που ενδιαφέρει */ );

int HT_DeleteEntry( HT_info header_info,void *value);

void Print_AllEntries(HT_info *header_info);

#endif // HASH_INDEX_H