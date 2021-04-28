#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BF.h"
#include "HashFile.h"

unsigned int hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    
    return hash;
}

int HT_CreateIndex(char* fileName,char attrType,char* attrName,int attrLength,int buckets){
    int i = 0, file_descriptor, block_num;
    void* block;
    int flag_heap = 1;
    char* pointer;
    
    if (BF_CreateFile(fileName)<0){
        BF_PrintError("Cannot Create File");
        return -1;
    }
 
    if (file_descriptor = BF_OpenFile(fileName)<0){
        BF_PrintError("Cannot Open File");
        return -1;
    }
    
    for (i=0; i<=buckets; i++){
        if (BF_AllocateBlock(file_descriptor)<0){
            BF_PrintError("Cannot Allocate File");
            return -1;
        }
    }
        
    if (BF_ReadBlock(file_descriptor,0,&block)<0){
        BF_PrintError("Cannot Read File");
        return -1;
    }
        
    int record_num = 0;
    memcpy(block,&flag_heap,sizeof(int));
    memcpy(block+sizeof(int),&attrLength,sizeof(int));
    memcpy(block+2*sizeof(int),&file_descriptor,sizeof(int));
    memcpy(block+3*sizeof(int),&record_num,sizeof(int));
    memcpy(block+4*sizeof(int),attrName,sizeof(attrName));
    memcpy(block+4*sizeof(int)+sizeof(attrName),&attrType,sizeof(char));
    memcpy(block+4*sizeof(int)+sizeof(attrName)+sizeof(char),&buckets,sizeof(int));

    if(BF_WriteBlock(file_descriptor,0)<0){
        BF_PrintError("Cannot Write File");
        return -1;
    }
        
    if(BF_CloseFile(file_descriptor)<0){
        BF_PrintError("Cannot Close File");
        return -1;
    }
    return 0;
}

HT_info* HT_OpenIndex(char *filename){

  printf("I am going to open file\n");
  int flag;
  int fdd;
  HT_info* ht;

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
  
	if(flag!=1){
    printf("File is not hash !\n");
    return NULL;
  }else{
    printf("It is  hash! \n");
  }

  int fd;
	memcpy(&fd,(char*)block+2*sizeof(int),sizeof(int)); 
  printf("fd old %d fd new %d\n",fd,fdd);
  //memcpy((char*)block+2*sizeof(int),&file_descriptor,sizeof(int)); 

  ht = malloc(sizeof(HT_info));
  ht->attrName = malloc(sizeof(filename));  
  memcpy(&(ht->attrLength) , (char*)block+sizeof(int) ,sizeof(int));
  memcpy(&(ht->file_descriptor) , &fdd ,sizeof(int));
  memcpy(&(ht->record_num) , (char*)block+3*sizeof(int) ,sizeof(int));
  memcpy(ht->attrName , (char*)block+4*sizeof(int) ,sizeof(filename));
  memcpy(&(ht->attrType), (char*)block+4*sizeof(int)+sizeof(filename),sizeof(char));
  memcpy(&(ht->bucket_num),block+4*sizeof(int)+sizeof(filename)+sizeof(char),sizeof(int));
  printf("%s %d %d %c %d\n",ht->attrName,ht->attrLength,ht->record_num,ht->attrType,ht->bucket_num);

   // Get data
  if(BF_ReadBlock(fdd,0,&block)<0){
    printf("bad3\n");
    return NULL;
  }
  printf("ok %d \n",fdd);
  return ht;
}
 
int HT_CloseIndex(HT_info* header_info) {
  //Close the file with str_id file descriptor
  int file_descriptor = header_info->file_descriptor;
  if(BF_CloseFile(file_descriptor)< 0)
    return -1;
  else {
    free(header_info->attrName);
    free(header_info);
    return 0;
  }
}
 
int HT_InsertEntry(HT_info header_info,Record record)
{
    int block_num, entries=0, temp;
    int * block_of;
    char* pointer;
    Record rec;
    Record* records;
    void* block;
 
 
    if(!strcmp(header_info.attrName,"id")){
        char str[10];
        sprintf(str, "%d", record.id );
        block_num = hash(str)%header_info.bucket_num + 1;
    }       
    else if(!strcmp(header_info.attrName,"name"))
        block_num = hash(record.name)%header_info.bucket_num + 1;
    
    else if(!strcmp(header_info.attrName, "surname"))   
        block_num = hash(record.surname)%header_info.bucket_num + 1;
    
    else if(!strcmp(header_info.attrName,"address"))
        block_num = hash(record.address)%header_info.bucket_num + 1;
    
    temp = block_num;
 
    
    while (temp)
    {
        if (BF_ReadBlock(header_info.file_descriptor, temp, &block) < 0){
            BF_PrintError("Cannot Read Block");
            return -1;
        }
 
        memcpy(&entries,block,sizeof(int));
        //case full block
        if ( (BLOCK_SIZE - entries*sizeof(Record) - 2*sizeof(int)) <= sizeof(Record) ) {
            block_num = temp;
            memcpy(&temp, block + BLOCK_SIZE - sizeof(int), sizeof(int));
            
            if (temp == 0) {     //overflow situation        
                if(BF_AllocateBlock(header_info.file_descriptor)<0){
                    BF_PrintError("Cannot Allocate File");
                    return -1;
                }
                
                temp = BF_GetBlockCounter(header_info.file_descriptor) - 1;
                memcpy(block + BLOCK_SIZE - sizeof(int), &temp, sizeof(int));
                if (BF_WriteBlock(header_info.file_descriptor, block_num)<0){ 
                    BF_PrintError("Cannot Write File");
                    return -1;
                }
            }
        }   
        //check if block has free space for the record
        else    
        {
            pointer = (void *)block + sizeof(int) + entries*sizeof(Record);
            memcpy(pointer, &record, sizeof(Record));
            entries++;
            memcpy(block, &entries, sizeof(int));
            block_num = temp;
            //printf("%d %s %s %s\n", record.id, record.name, record.surname, record.address);
            break;
        }
    }
    
    
    if (BF_WriteBlock(header_info.file_descriptor, block_num)<0) { 
        BF_PrintError("Cannot Write File");
        return -1;
    }

    return block_num;
}
 
 
void HT_GetAllEntries(HT_info header_info, void *value)
{
    int read_blocks = 0;
    int check;
    int temp_block;
    int entry_num, i;
    void* block;
    Record* record = malloc(sizeof(Record));
    char str_id[12];
    
    if(!strcmp(header_info.attrName,"id")){
        sprintf(str_id,"%d",*(int*)value);
        check = *(int*)value;
        value = str_id;
    }

    temp_block = hash(value)%header_info.bucket_num + 1;
    //printf("%s %d\n",(char*)value,temp_block);

    while(temp_block!=0) {
        if(BF_ReadBlock(header_info.file_descriptor, temp_block, &block)<0){
             printf("%s %d\n",(char*)value,temp_block);
             BF_PrintError("Cannot Read Block");
             break;
        }

        //number of read blocks
        read_blocks++;                                
        //number of records    
        memcpy(&entry_num, block, sizeof(int));       
        
        //traverse block records
        for(i=0;i<entry_num;i++)
        {
            memcpy(record, block+sizeof(int)+i*sizeof(Record), sizeof(Record));
            if(header_info.attrType == 'i'){
                //printf("inside %d \n",record->id);
                if(record->id == check )
                    printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);
            }   
            else if(header_info.attrType == 'c'){
                if(!strcmp(header_info.attrName,"name")){
                   if(!strcmp(record->name, value))
                      printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);
                }
                else if(!strcmp(header_info.attrName, "surname")){
                    if(!strcmp(record->surname, value))
                        printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);
                } 
                else if(!strcmp(header_info.attrName,"address")){
                        if(!strcmp(record->address, value))
                            printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);
                }
            }
        }

        memcpy(&temp_block,block + BLOCK_SIZE - sizeof(int),sizeof(int)); //next block
    }
    printf("%d Blocks read\n",read_blocks); 
}

int HT_DeleteEntry( HT_info header_info,void *value){

    int read_blocks = 0;
    int check;
    int temp_block;
    int entry_num, i;
    void* block;
    Record* record = malloc(sizeof(Record));
    char str_id[12];
    
    if(!strcmp(header_info.attrName,"id")){
        sprintf(str_id,"%d",*(int*)value);
        check = *(int*)value;
        value = str_id;
    }

    temp_block = hash(value)%header_info.bucket_num + 1;
    //printf("%s %d\n",(char*)value,temp_block);

    while(temp_block!=0) {
        if(BF_ReadBlock(header_info.file_descriptor, temp_block, &block)<0){
             printf("%s %d\n",(char*)value,temp_block);
             BF_PrintError("Cannot Read Block");
             break;
        }

        //number of read blocks
        read_blocks++;                                
        //number of records    
        memcpy(&entry_num, block, sizeof(int));       
        
        //traverse block records
        for(i=0;i<entry_num;i++)
        {
            memcpy(record, block+sizeof(int)+i*sizeof(Record), sizeof(Record));
            if(header_info.attrType == 'i'){
                //printf("inside %d \n",record->id);
                if(record->id == check ) {
                     printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);

                     //set memory to zeros
                     if(BF_WriteBlock(header_info.file_descriptor,temp_block)< 0) {
                          BF_PrintError("Cannot Write File");
                          return -1;
                      }

                    memset(block+sizeof(int)+i*sizeof(Record),0,sizeof(Record)); 
                }
            }   
            else if(header_info.attrType == 'c'){
                if(!strcmp(header_info.attrName,"name")){
                   if(!strcmp(record->name, value)){
                       printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);

                       //set memory to zeros
                     if(BF_WriteBlock(header_info.file_descriptor,temp_block)< 0) {
                          BF_PrintError("Cannot Write File");
                          return -1;
                      }

                      memset(block+sizeof(int)+i*sizeof(Record),0,sizeof(Record)); 
                   }
                }
                else if(!strcmp(header_info.attrName, "surname")){
                    if(!strcmp(record->surname, value)) {
                        printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);

                        //set memory to zeros
                       if(BF_WriteBlock(header_info.file_descriptor,temp_block)< 0) {
                          BF_PrintError("Cannot Write File");
                          return -1;
                      }

                        memset(block+sizeof(int)+i*sizeof(Record),0,sizeof(Record)); 
                    }
                } 
                else if(!strcmp(header_info.attrName,"address")){
                        if(!strcmp(record->address, value)) {
                             printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);

                             //set memory to zeros
                            if(BF_WriteBlock(header_info.file_descriptor,temp_block)< 0) {
                                BF_PrintError("Cannot Write File");
                                return -1;
                            }

                            memset(block+sizeof(int)+i*sizeof(Record),0,sizeof(Record)); 
                        }
                }
            }
        }

        memcpy(&temp_block,block + BLOCK_SIZE - sizeof(int),sizeof(int)); //next block
    }
    printf("%d Blocks read\n",read_blocks); 

}

void Print_AllEntries(HT_info *ht){
    int i,j,temp_block_num,curr,entry_num;
    void *block;
    Record *record = malloc(sizeof(Record));
    for(i=1;i<=ht->bucket_num;i++){
        temp_block_num = i;
        while(temp_block_num!=0) {
            //printf("ok\n");
            if(BF_ReadBlock(ht->file_descriptor, temp_block_num, &block) < 0){
                BF_PrintError("Cannot Read ");
                break;
            }

            memcpy(&entry_num,block,sizeof(int));
            // printf("%d\n\n",entry_num);
            for(j=0;j<entry_num;j++){   
                memcpy(record, block+sizeof(int)+j*sizeof(Record), sizeof(Record));
                
                printf("%d %s %s %s\n", record->id, record->name, record->surname, record->address);
            }

            memcpy(&curr,block + BLOCK_SIZE - sizeof(int), sizeof(int));

            if(curr==0){
                break;
            }
            else{
                temp_block_num = curr;
            }
        }
    }
}

int HashStatistics(char* filename){
    HT_info *ht;
    void *block;
    int blocks_num,temp_block_num=1,entry_num,curr;
    int i,var=0;
    //The same applies 
    if((ht = HT_OpenIndex(filename))!= NULL){
        blocks_num =  BF_GetBlockCounter(ht->file_descriptor) ;
        printf("The hash file has currently %d number of blocks\n",blocks_num);
    }else{
        printf("there is not such file \n");
        var=-1;
    }

    int min_rec=100000,max_rec=-1,mean_rec=0,count=0,count2=0,count4=0;
    for(i=1;i<=ht->bucket_num;i++){
        temp_block_num = i;
        while(temp_block_num!=0) {
            
            if(BF_ReadBlock(ht->file_descriptor, temp_block_num, &block) < 0){
                BF_PrintError("Cannot Read ");
                var =  -1;
            }

            memcpy(&entry_num,block,sizeof(int));
            //printf("%d\n",entry_num);
            if(entry_num < min_rec){
                min_rec = entry_num; 
            }

            if(entry_num > max_rec){
                max_rec = entry_num; 
            }

            mean_rec +=  entry_num;
            count++;
            count2++;
            memcpy(&curr,block + BLOCK_SIZE - sizeof(int), sizeof(int));

            if(curr==0){
                break;
            }
            else{
                temp_block_num = curr;
            }
        }
        mean_rec = mean_rec / count; //blocks inside current bucket
        if(count>1)
            count4++;//count overflow buckets

        printf("For bucket %d: min %d max %d mean %d overflow blocks %d\n",i,min_rec,max_rec,mean_rec,count-1);
        min_rec=100000,max_rec=-1,mean_rec=0,count=0;
    }

    int count3 = count2 / (ht->bucket_num-1);
    printf("mean blocks for every bucket %d\n",count3);
    printf("number of overflow buckets %d\n",count4);
    // counters explain

    // count  is for counting  blocks of each bucket
    // count2 is for total overflow buckets
    // count3 is for mean blocks of bucket
    // count4 is for how many buckets have overflowed
    
    return var;
}
