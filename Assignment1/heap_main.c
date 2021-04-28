#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "HeapFile.h"

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
  HP_info *hp;

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
  if(HP_CreateFile("data.db",'i',"id",4) == 0){
      printf("file created successfully\n");
  }

hp = HP_OpenFile("data.db");
//printf("%s\n",hp->attrName);
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
    if(HP_InsertEntry(*hp,record)>0){
          //printf("%s\n",record.name);
    }
  }

  
  /* for (int id = 0; id < RECORDS_NUM; ++id) {
    record.id = id;
    r = rand() % 12;
    memcpy(record.name, names[r], strlen(names[r]) + 1);
    r = rand() % 12;
    memcpy(record.surname, surnames[r], strlen(surnames[r]) + 1);
    r = rand() % 10;
    memcpy(record.address, addresses[r], strlen(addresses[r]) + 1);

     //printf("%s\n",record.name);
    if(HP_InsertEntry(hp, record,id)==0){
      //printf("%s\n",record.name);
    }
  } */
  
  int num=997;
  HP_GetAllEntries(*hp,&num );

  printf("\n");
  HP_DeleteEntry(*hp,&num );
  num=942;
  HP_DeleteEntry(*hp,&num );
   num=999;
  HP_DeleteEntry(*hp,&num );
   num=996;
  HP_DeleteEntry(*hp,&num );
   num=995;
  HP_DeleteEntry(*hp,&num );
   num=994;
   HP_DeleteEntry(*hp,&num );
   num=963;
   HP_DeleteEntry(*hp,&num );
   num=993;
   HP_DeleteEntry(*hp,&num );
   num=992;
   HP_DeleteEntry(*hp,&num );
   num=991;
   HP_DeleteEntry(*hp,&num );
   num=990;

  record.id = 555;
  HP_InsertEntry(*hp,record);
  //num=982;
  //HP_DeleteEntry(*hp,&num ); */
  num= -2;
  HP_GetAllEntries(*hp,&num );

  if(HP_CloseFile(hp)==0)
    printf("file closed fine\n");
  
  fclose(fp);
  }

