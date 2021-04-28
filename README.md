# FileSystem
Assignment for the course  Implementation and Design of Databases

# 1st assignment

My task was to implement a [```HeapFile```](https://github.com/aristsili/FileSystem/blob/main/Assignment1/HeapFile.c) and  [```HashFile```](https://github.com/aristsili/FileSystem/blob/main/Assignment1/HashFile.c)  in order to store records.For the first one, I filled each one block of memory with records using [```InsertEntry```](https://github.com/aristsili/FileSystem/blob/main/Assignment1/HeapFile.c#L130) .At the start of the block I store a counter for my records at the specific block .Same work for [```GetAllEntries```](https://github.com/aristsili/FileSystem/blob/main/Assignment1/HeapFile.c#L201) ,where I traverse each block of memory record by record.For the function [```DeleteEntry```](https://github.com/aristsili/FileSystem/blob/main/Assignment1/HeapFile.c#L258) ,I created an offset number , so as to reduce the block counter properly when a whole block is empty.My concept for the delete function was to find and erase the record from the block .Then I replace that empty place with the last record ,that is stored at the last block.In that way ,I use my memory effiecienty,beacuse there are no holes at my file system.

The hash functions manipulates memory in a little bit different way.In the hashfile ,block zero was purposed for storing metadata.I just added another integer variable at the end of every block.If one block is not yet filled, that integer stays zero.But when it is filled with records, I allocate another block and I store the current block counter value at that integer in ```InsertEntry```.Also when we have one bucket overflowed, that one is extended with another block , so at to store more records.The difference from the previous implementation is that now we have grouped blocks to buckets.Depending on the hash of its record, it is getting inserted at the proper bucket .If the bucket is overflowed, we traverse the blocks of the bucket , to place the record at the correct position.Same situation for the```GetallEntries``` ,where we traverse every bucket accordingly.The ```DeleteEntry``` here works fine .The record gets deleted but the memory isn't used efficiently.(I don't fill the deleted space with another record).

Finally , I created a function ```HashStatistics``` to get statistics from the whole procedure.It concerns the follow:(Only for HashFile)

1. the number of blocks in current file
2. the minimum and the maximum number of records that a bucket has
3. the mean number of records in buckets
4. the mean number of blocks that a bucket has
5. the number of buckets that have overflowed blocks.
