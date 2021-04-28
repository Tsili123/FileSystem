#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "HashFile.h"

#define RECORDS_NUM 1700

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
  FILE *fp;
  int lines;
  fp = fopen("records1K.txt","r");

  BF_Init();
  HT_info *ht;

  if( fp == NULL ) {
    return -1;
  }

  while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
        ++lines;

  printf("Lines : %d\n", lines);
  rewind(fp);

 int num_of_records = (BLOCK_SIZE-sizeof(int))/sizeof(Record);
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
    //printf("%d %s %s %s\n",record.id,record.name,record.surname,record.address);
    id++;
    if(HT_InsertEntry(*ht,record)>0){
          //printf("%s\n",record.name);
    }
  } 
  
   int num=976;
   HT_GetAllEntries(*ht,&num ); 

  Print_AllEntries(ht);

  printf("\n");
  HT_DeleteEntry(*ht,&num );
  num=945;
  HT_DeleteEntry(*ht,&num );

  Print_AllEntries(ht);


  HashStatistics("data.db");

  HT_GetAllEntries(*ht,&num ); 

  if(HT_CloseIndex(ht)==0)
    printf("index closed fine\n");
}

