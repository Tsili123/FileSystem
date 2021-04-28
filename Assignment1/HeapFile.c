#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BF.h"
#include "HeapFile.h"


int HP_CreateFile(char *filename,char attrType,char* attrName, int attrLength) {

  int file_descriptor;
  void* data;
  HP_info *hp;
  int flag_heap=5;
  //hp = malloc(sizeof(HP_info));

  void* block;
  //BF_Init();

  
  // Create a file 
  if(BF_CreateFile(filename)<0){
		return -1;
	}
  
  // Open the file 
  if((file_descriptor = BF_OpenFile(filename)) < 0){
		return -1;
	}
  
  // Create  metadata
  if(BF_AllocateBlock(file_descriptor)<0){
		return -1;
	}
  
  // Get data
  if(BF_ReadBlock(file_descriptor,0,&block)<0){
		return -1;
	}

  int record_num = 0;
  memcpy(block,&flag_heap,sizeof(int));
  memcpy(block+sizeof(int),&attrLength,sizeof(int));
  memcpy(block+2*sizeof(int),&file_descriptor,sizeof(int));
  memcpy(block+3*sizeof(int),&record_num,sizeof(int));
  memcpy(block+4*sizeof(int),attrName,sizeof(attrName));
  memcpy(block+4*sizeof(int)+sizeof(attrName),&attrType,sizeof(char));

  if(BF_WriteBlock(file_descriptor,0)<0){
		return -1;
	}

   //Close  file
  if(BF_CloseFile(file_descriptor)<0){
		return -1;
	} 
  
  //printf("ok\n");
  return 0;
}

HP_info* HP_OpenFile(char *filename){

  printf("I am going to open file\n");
  int flag;
  int fdd;
  HP_info* hp;

  void* block;
  //BF_Init();

  if((fdd = BF_OpenFile(filename)) <0) // Open again 
  {
    printf("%d\n",fdd );
    return NULL;
  }

  // Get data
  if(BF_ReadBlock(fdd,0,&block)<0){
    printf("bad3\n");
    return NULL;
  }

  memcpy(&flag,block,sizeof(int));
  
	if(flag!=5){
    printf("File is not heap !\n");
    return NULL;
  }else{
    printf("It is a heap ! \n");
  }

  int fd;
	memcpy(&fd,(char*)block+2*sizeof(int),sizeof(int)); 
  printf("fd old %d fd new %d\n",fd,fdd);
  //memcpy((char*)block+2*sizeof(int),&file_descriptor,sizeof(int)); 

  hp = malloc(sizeof(HP_info));
  hp->attrName = malloc(sizeof(filename));  
	memcpy(&(hp->attrLength) , (char*)block+sizeof(int) ,sizeof(int));
  memcpy(&(hp->file_descriptor) , &fdd ,sizeof(int));
  memcpy(&(hp->record_num) , (char*)block+3*sizeof(int) ,sizeof(int));
  memcpy(hp->attrName , (char*)block+4*sizeof(int) ,sizeof(filename));
  memcpy(&(hp->attrType), (char*)block+4*sizeof(int)+sizeof(filename),sizeof(char));
  int off=0;
  memcpy(&(hp->offset), &off,sizeof(int));
  printf("%s %d %d %c\n",hp->attrName,hp->attrLength,hp->record_num,hp->attrType);

   // Get data
  if(BF_ReadBlock(fdd,0,&block)<0){
    printf("bad3\n");
    return NULL;
  }
  printf("ok %d \n",fdd);
  return hp;
}

int HP_CloseFile(HP_info* header_info) {
  //Close the file with a file descriptor
  int file_descriptor = header_info->file_descriptor;
  if(BF_CloseFile(file_descriptor)< 0)
    return -1;
  else {
    free(header_info->attrName);
    free(header_info);
    return 0;
  }
}


int HP_InsertEntry(HP_info header_info, Record record) {

//try to find last block

int blocks_num;
 if((blocks_num = BF_GetBlockCounter(header_info.file_descriptor))<0){
  //printf("entered5\n");
  //printf("%d\n",blocks_num);
  return -1;
}
blocks_num = blocks_num - header_info.offset;
  // printf("aaaaaaaaaaaaaa %d %d\n",header_info.offset,blocks_num);
void* block;
char * pointer;
int a;
Record * records;
int num_of_records,entry_num;

//printf("%d\n",header_info.file_descriptor);
if(( a = BF_ReadBlock(header_info.file_descriptor,blocks_num-1,&block))<0){
  BF_PrintError("Cannot Read File");
  //printf("aaa %d %d %d \n",a,blocks_num,i);
  printf("entered6\n");
  return -1;
}

//record num
if(blocks_num-1 == 0){
    //memcpy(&entry_num , (char*)block+3*sizeof(int) ,sizeof(int));
    //num_of_records = (BLOCK_SIZE-4*sizeof(int)-sizeof(header_info.attrName)-1)/sizeof(Record);
}else{
    memcpy(&entry_num , (char*)block ,sizeof(int));
    num_of_records = (BLOCK_SIZE-sizeof(int))/sizeof(Record);
}

//not enough space , we need a new block
if (entry_num == num_of_records || blocks_num - 1 == 0)  {
  //printf("entered2\n");
  void* block2;

  if(BF_AllocateBlock(header_info.file_descriptor)<0) {
    BF_PrintError("Cannot Allocate File");
    return -1;
  }

  if (BF_ReadBlock(header_info.file_descriptor, blocks_num, &block2)<0){
      BF_PrintError("Cannot Read File");
      return -1;
  }

  entry_num=1;
  memcpy((char*)block2,&entry_num,sizeof(int));
  memcpy((char*)block2+sizeof(int),&record,sizeof(record));

}else{
    memcpy((char*)block+sizeof(int)+ entry_num * sizeof(Record),&record,sizeof(record));
    entry_num++;
    memcpy(	(char *)block,&entry_num,sizeof(int));
}

  if(BF_WriteBlock(header_info.file_descriptor,blocks_num-1)< 0) {
    BF_PrintError("Cannot Write File");
		return -1;
	}

  if (entry_num == num_of_records || blocks_num - 1 == 0)
    return blocks_num;
  else  
    return blocks_num-1;
}

void HP_GetAllEntries(HP_info header_info, void* value){

    int i, j, entry_num,have_read=0;
    Record record;
    void* block;
    int blocks; 
    int off_set = sizeof(int); 
    int file_descriptor = header_info.file_descriptor;
    
    if ((blocks = BF_GetBlockCounter(file_descriptor)) < 0){
        BF_PrintError("Cannot Count");
    }

    blocks = blocks - header_info.offset;
    //printf("aaaaaaaaaaaaaa %d %d\n",header_info.offset,blocks);

    //searh block one by one starting from second block
    for(i = 1; i < blocks; i++)                   
    {  
        off_set = sizeof(int); //reset

        if (BF_ReadBlock(file_descriptor, i, &block)<0){
            BF_PrintError("Cannot Read Block");
            break;                                   //error case
        } 
           
        memcpy(&entry_num, block, sizeof(int)); 
        
        have_read++;                                                                                              
           
        for(j = 0; j < entry_num; j++){
            memcpy(&record, block+off_set, sizeof(Record));
            
            if(!strcmp(header_info.attrName,"id")) {                //check key
                if(record.id == *((int *)value)|| *((int *)value) == -2)
                    printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
            }   
            else if(!strcmp(header_info.attrName,"name")) {
                if(!strcmp(record.name, value)|| *((int *)value) == -2)
                    printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
            }
            else if(!strcmp(header_info.attrName, "surname"))  {
                if(!strcmp(record.surname, value)|| *((int *)value) == -2)
                    printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
            }   
            else if(!strcmp(header_info.attrName,"address" )){
                if(!strcmp(record.address, value) || *((int *)value) == -2)
                    printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
            }
            //next record                
            off_set += sizeof(Record);                   
        }
                            
    }
    printf("%d Blocks read\n", have_read);
}

int HP_DeleteEntry( HP_info header_info,void *value){
    
    int i, j, entry_num,have_read=0;
    Record record;
    void* block;
    int blocks; 
    int off_set = sizeof(int); 
    int file_descriptor = header_info.file_descriptor;
    
    if ((blocks = BF_GetBlockCounter(file_descriptor)) < 0){
        BF_PrintError("Cannot Count");
    }

    blocks -= header_info.offset;

    //searh block one by one starting from second block
    for(i = 1; i < blocks; i++)                   
    {  
        off_set = sizeof(int); //reset

        if (BF_ReadBlock(file_descriptor, i, &block)<0){
            BF_PrintError("Cannot Read Block");
            break;                                   //error case
        } 
           
        memcpy(&entry_num, block, sizeof(int)); 
        
        have_read++;                                                                                              
           
        for(j = 0; j < entry_num; j++){
            memcpy(&record, block+off_set, sizeof(Record));
            //printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);

            if(!strcmp(header_info.attrName,"id")) {                //check key
                if(record.id == *((int *)value)){
                    printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);

                    if(BF_WriteBlock(header_info.file_descriptor,i)< 0) {
                        BF_PrintError("Cannot Write File");
                          return -1;
                      }

                    memset(block+off_set,0,sizeof(Record));
                    ///////
                    int blocks_numb = BF_GetBlockCounter(header_info.file_descriptor);
                    blocks_numb = blocks_numb - header_info.offset;
                    blocks_numb--;
                    void *block2;
                    int entry_num2;
                    if (BF_ReadBlock(file_descriptor, blocks_numb, &block2)<0){
                        BF_PrintError("Cannot Read Block");
                        break;                                   //error case
                    } 

                     if(BF_WriteBlock(header_info.file_descriptor,blocks_numb)< 0) {
                        BF_PrintError("Cannot Write File");
                          return -1;
                      } 

                    memcpy(&entry_num2, block2, sizeof(int)); 
                    memcpy(block+off_set,block2+sizeof(int)+(entry_num2-1)*sizeof(record),sizeof(Record));//swap last record of block with hole
                    memset(block2+sizeof(int)+(entry_num2-1)*sizeof(record),0,sizeof(Record)); 
                    entry_num2--;
                    if(entry_num2 == 0){
                      header_info.offset++;
                      printf("hdd %d\n",header_info.offset);
                    }
                    memcpy(block2,&entry_num2, sizeof(int)); 

                    ////alternatively
                    //memcpy(block+off_set,block+sizeof(int)+(entry_num-1)*sizeof(record),sizeof(Record));//swap last record of block with hole
                    //memset(block+sizeof(int)+(entry_num-1)*sizeof(record),0,sizeof(Record));
                }
            }   
            else if(!strcmp(header_info.attrName,"name")) {
                if(!strcmp(record.name, value)){
                  printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
                  //set memory to zeros
                   if(BF_WriteBlock(header_info.file_descriptor,i)< 0) {
                        BF_PrintError("Cannot Write File");
                          return -1;
                      }

                    memset(block+off_set,0,sizeof(Record)); 
                    memcpy(block+off_set,block+sizeof(int)+(entry_num-1)*sizeof(record),sizeof(Record));
                    memset(block+sizeof(int)+(entry_num-1)*sizeof(record),0,sizeof(Record));  
                    entry_num--;
                    memcpy(block,&entry_num, sizeof(int)); 
                }
            }
            else if(!strcmp(header_info.attrName, "surname"))  {
                if(!strcmp(record.surname, value)){

                    printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address); 
                    //set memory to zeros
                   if(BF_WriteBlock(header_info.file_descriptor,i)< 0) {
                        BF_PrintError("Cannot Write File");
                          return -1;
                      }

                    memset(block+off_set,0,sizeof(Record)); 
                    memcpy(block+off_set,block+sizeof(int)+(entry_num-1)*sizeof(record),sizeof(Record));
                    memset(block+sizeof(int)+(entry_num-1)*sizeof(record),0,sizeof(Record));  
                    entry_num--;
                    memcpy(block,&entry_num, sizeof(int)); 
                }
            }   
            else if(!strcmp(header_info.attrName,"address")){
                if(!strcmp(record.address, value)){
                   printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
                    //set memory to zeros
                   if(BF_WriteBlock(header_info.file_descriptor,i)< 0) {
                        BF_PrintError("Cannot Write File");
                          return -1;
                      }

                    memset(block+off_set,0,sizeof(Record));
                    memcpy(block+off_set,block+sizeof(int)+(entry_num-1)*sizeof(record),sizeof(Record));
                    memset(block+sizeof(int)+(entry_num-1)*sizeof(record),0,sizeof(Record));  
                    entry_num--;
                    memcpy(block,&entry_num, sizeof(int));  
                }
            }
            //next record                
            off_set += sizeof(Record);                   
        }
                            
    }
    printf("%d Blocks read\n", have_read);
}

		