#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "HashFile.h"
#include "sht.h"

const char* names[] = {
  "Yannis",
  "Christofos",
  "Sofia",
  "Marianna",
  "Vagelis",
  "Maria",
  "Iosif",
  "Dionisis",
  "Konstantina",
  "Theofilos",
  "Giorgos",
  "Dimitris"
};

const char* surnames[] = {
  "Ioannidis",
  "Svingos",
  "Karvounari",
  "Rezkalla",
  "Nikolopoulos",
  "Berreta",
  "Koronis",
  "Gaitanis",
  "Oikonomou",
  "Mailis",
  "Michas",
  "Halatsis"
};

const char* addresses[] = {
  "Athens",
  "San Francisco",
  "Los Angeles",
  "Amsterdam",
  "London",
  "New York",
  "Tokyo",
  "Hong Kong",
  "Munich",
  "Miami"
};


int main() {
  FILE *fp,*fs;
  int lines;
  fp = fopen("records1K.txt","r");
  fs = fopen("records5K.txt","r");

  BF_Init();
  HT_info *ht;
  SHT_info *sht;

  if( fp == NULL || fs == NULL ) {
    return -1;
  }

  while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
        ++lines;

  printf("Primary file Lines : %d\n", lines);
  rewind(fp);
  lines = 0;
  
  while (EOF != (fscanf(fs, "%*[^\n]"), fscanf(fs,"%*c")))
        ++lines;

  printf("Secondary file Lines : %d\n", lines);
  rewind(fs);

printf("ok\n");

 int num_of_records = (BLOCK_SIZE-2*sizeof(int))/sizeof(SecondaryRecord);
 printf("%d\n",num_of_records);

  int fd;
  if(HT_CreateIndex("data.db",'i',"id",4,20) == 0){
      printf("index created successfully\n");
  }

ht = HT_OpenIndex("data.db");
//printf("%s\n",ht->attrName);
printf("ok2\n");


 Record record;
  srand(125874);
  int r;
  printf("Insert Entries\n");
 char *line = NULL;
 size_t len = 0;
 ssize_t nread;
 int id = 0;
 while ((nread = getline(&line, &len, fp)) != -1) {
    sscanf( line , "{%d,\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"}", &record.id, record.name,record.surname, record.address );
    printf("%d %s %s %s\n",record.id,record.name,record.surname,record.address);
    id++;
    if(HT_InsertEntry(*ht,record)>0){
          printf("Error for %s\n",record.name);
    }
  } 
  
   int num=976;
   HT_GetAllEntries(*ht,&num ); 
   HashStatistics("data.db");

//////////////
   char* sfileName="sec.db";
   char sAttrType='c';
   char* sAttrName="surname";
   int sAttrLength=15;
   int sBuckets=5;
   printf("\n\nWe are finished with primary hashtable \n");
   printf("Now create a secondary index based on %s\n",sAttrName );
   sBuckets=10;
   int createErrorCode=SHT_CreateSecondaryIndex(sfileName,sAttrName,sAttrLength,sBuckets,"data.db");
   if(createErrorCode<0){
		printf("SecondaryIndex creation FAILED\n");
		return -1;
	}	

	SHT_info* sh=SHT_OpenSecondaryIndex(sfileName);
	if(sh==NULL){
		printf("Open SecondaryIndex failure\n");
	} 
    
  printf("Insert Entries for secondary\n");
  line = NULL;
  len = 0;
  int id2 = 0;
  while ((nread = getline(&line, &len, fs)) != -1) {
    sscanf( line , "{%d,\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"}", &record.id, record.name,record.surname, record.address );
    //printf("%d %s %s %s\n",record.id,record.name,record.surname,record.address);
    id2++;
    int blk;
    if((blk=HT_InsertEntry(*ht,record))>0){
          //printf("%s\n",record.name);
    }

	SecondaryRecord srecord;
	srecord.record=record;
	srecord.blockId=blk;
	int sbl=SHT_SecondaryInsertEntry(*sh,srecord); 
  
  } 
  //  num_of_records = (BLOCK_SIZE-2*sizeof(int))/sizeof(SecondaryRecord);
  //  printf("%d\n",num_of_records);
  
  Record record2;
  strcpy(record2.surname,"surname_1125");
	int err=SHT_SecondaryGetAllEntries(*sh,*ht,(void*)record2.surname);

    num=1123;
    HT_GetAllEntries(*ht,&num ); 
  SecondaryHashStatistics("sec.db");

  if(HT_CloseIndex(ht)==0)
    printf("index closed fine\n"); 

  SHT_CloseSecondaryIndex(sh); 

}


