#ifndef SHT_H_
#define SHT_H_

typedef struct SHT_info {
	int fileDesc;
	char *attrName;
	int attrLength;
	int numBuckets;
	char *fileName;
	int record_num;
} SHT_info;

typedef struct SecondaryRecord {
	int blockId;
	Record record;
} SecondaryRecord;

int SHT_CreateSecondaryIndex( char *sfileName,char* attrName, int attrLength, int buckets , char* fileName);
SHT_info* SHT_OpenSecondaryIndex( char *sfileName);
int SHT_CloseSecondaryIndex( SHT_info* header_info);
int SHT_SecondaryInsertEntry( SHT_info header_info,SecondaryRecord record);
int SHT_SecondaryGetAllEntries(SHT_info header_info_sht,HT_info header_info_ht,void *value); 
int SecondaryHashStatistics(char* filename);

#endif 