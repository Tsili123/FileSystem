#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BF.h"
#include "HashFile.h"
#include "sht.h"

unsigned int sdbm_hash(unsigned char *str){
    unsigned long sdbm_hash = 5381;
    int c;
    while (c = *str++)
        sdbm_hash = ((sdbm_hash << 5) + sdbm_hash) + c;  /* sdbm_hash * 33 + c */
    
    return sdbm_hash;
}

int SHT_CreateSecondaryIndex(char* sfileName,char* attrName,int attrLength,int buckets, char* fileName){
    int i = 0, file_descriptor, block_num;
    void* block;
    int flag_ht = 3;
    char* pointer;
    
    if (BF_CreateFile(sfileName)<0){
        BF_PrintError("Cannot Create File");
        return -1;
    }
 
    if ((file_descriptor = BF_OpenFile(sfileName))<0){
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
    memcpy(block,&flag_ht,sizeof(int));
    memcpy(block+sizeof(int),&attrLength,sizeof(int));
    memcpy(block+2*sizeof(int),&file_descriptor,sizeof(int));
    memcpy(block+3*sizeof(int),&record_num,sizeof(int));
    memcpy(block+4*sizeof(int),attrName,sizeof(attrName));
    memcpy(block+4*sizeof(int)+sizeof(attrName),&buckets,sizeof(int));
        
    printf("SHT_CreateIndex: File first block created. It contains: typeOfFile=%s, attrName=%s, attrLength=%d, buckets=%d\n","secondary sdbm_hash",attrName,attrLength,buckets);
    
    if(BF_WriteBlock(file_descriptor,0)<0){
        BF_PrintError("Cannot Write File");
        return -1;
    }
        
    if(BF_CloseFile(file_descriptor)<0){
        BF_PrintError("Cannot Close File");
        return -1;
    }
    //Same procedure as Hashfile.c until there
    //Open the file of secondary hash table
    SHT_info* shi = SHT_OpenSecondaryIndex(sfileName);

    int blockId=1,nor=0,nxt=-1,j;
    //printf("%d",BF_GetBlockCounter(primarydesc));
    //First block is for metadata
    for (i = 1; i < BF_GetBlockCounter(primarydesc); i++)
    {
        //Get primary file descriptor(that hashfile.c provides)
        //printf("ok\n");
        //read first block of file,and then traverse all of it
        j=BF_ReadBlock(primarydesc , i , &block);
        //number of records inside block
        memcpy(&nor,block,sizeof(int));
        //get id of next block
        memcpy(&nxt,block+ BLOCK_SIZE - sizeof(int), sizeof(int));
        //copy records of block
        Record rectable[nor];
        for (j = 0; j < nor; j++){  
            memcpy(&rectable[j],block+sizeof(int)+j*sizeof(Record),sizeof(Record));
        }

        for (j = 0; j < nor; j++){
            //printf("ok\n");
            //printf("%d\n",nor);
            //put them inside secondary hashtable, with block id as index
            SecondaryRecord sRecord;
            sRecord.record=rectable[j];
            printf("Record found:  %d %s %s %s\n",rectable[j].id,rectable[j].name,rectable[j].surname,rectable[j].address );
            sRecord.blockId=i;
            int sInsertError=SHT_SecondaryInsertEntry(*shi,sRecord);
        }
    } 


    printf("\tSHT_CreateIndex: All records that existed on the primary index are now inserted to this secondary index also \n");
    
    return 0;

}

SHT_info* SHT_OpenSecondaryIndex(char *filename){

  printf("I am going to open file\n");
  int flag;
  int fdd;
  SHT_info* sht;

  void* block;

  if((fdd = BF_OpenFile(filename)) <0) // Open again 
  {
    printf("%d\n",fdd );
    return NULL;
  }

  // Get data
  if(BF_ReadBlock(fdd,0,&block)<0){
      BF_PrintError("Cannot Read File in Open");
      return NULL;
  }

  memcpy(&flag,block,sizeof(int));
  
	if(flag!=3){//flag of secondary hashtable is 3 (number)
    printf("File is not secondary sdbm_hash !\n");
    return NULL;
  }else{
    printf("It is socondary sdbm_hash! \n");
  }

  int fd;
  memcpy(&fd,(char*)block+2*sizeof(int),sizeof(int)); 
  printf("fd old %d fd new %d\n",fd,fdd);
  //memcpy((char*)block+2*sizeof(int),&file_descriptor,sizeof(int)); 

 //copy metadata to return them through struct
  sht = malloc(sizeof(SHT_info));
  sht->attrName = malloc(sizeof(filename));  
  memcpy(&(sht->attrLength) , (char*)block+sizeof(int) ,sizeof(int));
  memcpy(&(sht->fileDesc) , &fdd ,sizeof(int));
  memcpy(&(sht->record_num) , (char*)block+3*sizeof(int) ,sizeof(int));
  memcpy(sht->attrName , (char*)block+4*sizeof(int) ,sizeof(filename));
  memcpy(&(sht->numBuckets),block+4*sizeof(int)+sizeof(filename),sizeof(int));
  printf("%s %d %d %d\n",sht->attrName,sht->attrLength,sht->record_num,sht->numBuckets);

   // Get data
  if(BF_ReadBlock(fdd,0,&block)<0){
    printf("bad3\n");
    return NULL;
  }
  printf("ok %d \n",fdd);
  return sht;
}

int SHT_CloseSecondaryIndex(SHT_info* header_info) {
  //Close the file with str_id file descriptor
  int file_descriptor = header_info->fileDesc;
  if(BF_CloseFile(file_descriptor)< 0)
    return -1;
  else {
    free(header_info->attrName);
    free(header_info);
    return 0;
  }
}


int SHT_SecondaryInsertEntry(SHT_info header_info,SecondaryRecord record)
{
    int block_num, entries=0, temp;
    char* pointer;
    SecondaryRecord rec;
    SecondaryRecord* records;
    void* block;
 
    //printf("ok\n");
    //attrType will for sure be 'c'
    if(!strcmp(header_info.attrName,"name"))
        block_num = sdbm_hash(record.record.name)%header_info.numBuckets + 1;
    
    else if(!strcmp(header_info.attrName, "surname"))   
        block_num = sdbm_hash(record.record.surname)%header_info.numBuckets + 1;
    
    else if(!strcmp(header_info.attrName,"address"))
        block_num = sdbm_hash(record.record.address)%header_info.numBuckets + 1;
    
    temp = block_num;
    
    while (temp)
    {
        if (BF_ReadBlock(header_info.fileDesc, temp, &block) < 0){
            BF_PrintError("Cannot Read Block");
            return -1;
        }
 
        memcpy(&entries,block,sizeof(int));
        //case full block
        if ( (BLOCK_SIZE - entries*sizeof(SecondaryRecord) - 2*sizeof(int)) <= sizeof(SecondaryRecord) ) {
            block_num = temp;
            memcpy(&temp, block + BLOCK_SIZE - sizeof(int), sizeof(int));
            
            if (temp == 0) {     //overflow situation        
                if(BF_AllocateBlock(header_info.fileDesc)<0){
                    BF_PrintError("Cannot Allocate File");
                    return -1;
                }
                
                temp = BF_GetBlockCounter(header_info.fileDesc) -1 ;
                memcpy(block + BLOCK_SIZE - sizeof(int), &temp, sizeof(int));
                if (BF_WriteBlock(header_info.fileDesc, block_num)<0){ 
                    BF_PrintError("Cannot Write File");
                    return -1;
                }
            }
        }   
        //check if block has free space for the record
        else    
        {
            pointer = (void *)block + sizeof(int) + entries*sizeof(SecondaryRecord);
            memcpy(pointer, &record, sizeof(SecondaryRecord));
            entries++;
            memcpy(block, &entries, sizeof(int));
            block_num = temp;
            //printf("%d %s %s %s\n", record.record.id, record.record.name, record.record.surname, record.record.address);
            //printf("%d\n",entries);
            break;
        }
    }
    
    
    if (BF_WriteBlock(header_info.fileDesc, block_num)<0) { 
        BF_PrintError("Cannot Write File");
        return -1;
    }

    return block_num;
}


int SHT_SecondaryGetAllEntries(SHT_info header_info_sht,HT_info header_info_ht, void *value)
{
    int read_blocks = 0;
    int check;
    int temp_block;
    int entry_num, i;
    void* block, *pblock;
    SecondaryRecord* record = malloc(sizeof(SecondaryRecord));
    
    temp_block = sdbm_hash(value)%header_info_sht.numBuckets + 1;
    //printf("%s %d\n",(char*)value,temp_block);

    while(temp_block!=0) {
        if(BF_ReadBlock(header_info_sht.fileDesc, temp_block, &block)<0){
             printf("%s %d\n",(char*)value,temp_block);
             BF_PrintError("Cannot Read Block");
             return -1;
        }

        //number of read blocks
        read_blocks++;                                
        //number of records    
        memcpy(&entry_num, block, sizeof(int));       
        //printf("%d\n",entry_num);

        //traverse block records in secondary hashtable
        for(i=0;i<entry_num;i++)
        {
            memcpy(record, block+sizeof(int)+i*sizeof(SecondaryRecord), sizeof(SecondaryRecord));
           
            if(!strcmp(header_info_sht.attrName,"name")){
               if(!strcmp(record->record.name, value)){
                    printf("will look for '%s' in primary's block %d ... ",record->record.name, record->blockId);
                    BF_ReadBlock(header_info_ht.file_descriptor,record->blockId,&pblock);
                    int nor;
                    memcpy(&nor, pblock, sizeof(int));
                    Record* precord;
                    precord = malloc(sizeof(Record));
                    for (int j = 0; j <= nor; j++)
                    {
                        memcpy(precord, block+sizeof(int)+j*sizeof(Record), sizeof(Record));
                        if (strcmp(precord->name,record->record.name)==0)
                        {
                            printf("Record found:  %d %s %s %s\n",record->record.id,record->record.name,record->record.surname,record->record.address );
                            return read_blocks;
                        }
                    }
               }
            }
            else if(!strcmp(header_info_sht.attrName, "surname")){
                if(!strcmp(record->record.surname, value)){
                    printf("will look for '%s' in primary's block %d ... ",record->record.surname, record->blockId);
                    BF_ReadBlock(header_info_ht.file_descriptor,record->blockId,&pblock);
                    int nor;
                    memcpy(&nor, pblock, sizeof(int));
                    Record* precord;
                    precord = malloc(sizeof(Record));
                    //printf("%d\n",nor);
                    int j;
                    for (j = 0; j < nor; j++)
                    {  
                        memcpy(precord, pblock+sizeof(int)+j*sizeof(Record), sizeof(Record));
                        printf("Record %s\n",precord->name);
                        if (strcmp(precord->surname,record->record.surname)==0)
                        {
                            printf("Record found:  %d %s %s %s\n",record->record.id,record->record.name,record->record.surname,record->record.address );
                            return read_blocks;
                        }
                    }
               }
            } 
            else if(!strcmp(header_info_sht.attrName,"address")){
                    if(!strcmp(record->record.address, value)){
                    printf("will look for '%s' in primary's block %d ... ",record->record.address, record->blockId);
                    BF_ReadBlock(header_info_ht.file_descriptor,record->blockId,&pblock);
                    int nor;
                    memcpy(&nor, pblock, sizeof(int));
                    Record* precord;
                    precord = malloc(sizeof(Record));
                    for (int j = 0; j <= nor; j++)
                    {
                        memcpy(precord, block+sizeof(int)+j*sizeof(Record), sizeof(Record));
                        if (strcmp(precord->address,record->record.address)==0)
                        {
                            printf("Record found:  %d %s %s %s\n",record->record.id,record->record.name,record->record.surname,record->record.address );
                            return read_blocks;
                        }
                    }
               }
            }
        }

        memcpy(&temp_block,block + BLOCK_SIZE - sizeof(int),sizeof(int)); //next block
    }
    printf("%d Blocks read\n",read_blocks); 
}

int SecondaryHashStatistics(char* filename){
    SHT_info *sht;
    void *block;
    int blocks_num,temp_block_num=1,entry_num,curr;
    int i,var=0;
    //The same applies 
    if((sht = SHT_OpenSecondaryIndex(filename))!= NULL){
        blocks_num =  BF_GetBlockCounter(sht->fileDesc) ;
        printf("The hash file has currently %d number of blocks\n",blocks_num);
    }else{
        printf("there is not such file \n");
        var=-1;
    }

    int min_rec=100000,max_rec=-1,mean_rec=0,count=0,count2=0,count4=0;
    for(i=1;i<=sht->numBuckets;i++){
        temp_block_num = i;
        while(temp_block_num!=0) {
            
            if(BF_ReadBlock(sht->fileDesc, temp_block_num, &block) < 0){
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

    int count3 = count2 / (sht->numBuckets);
    printf("mean blocks for every bucket %d\n",count3);
    printf("number of overflow buckets %d\n",count4);
    // counters explain

    // count  is for counting  blocks of each bucket
    // count2 is for total overflow buckets
    // count3 is for mean blocks of bucket
    // count4 is for how many buckets have overflowed
    
    return var;
}
