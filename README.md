# FileSystem
Assignment for the course  Implementation and Design of Databases

# 1st assignment
My task was implement a HeapFile and HashFile in order to store records.For the first one, I filled each one block of memory with records(InsertEntry).At the start of the block I store a counter for my records at the specific block .Same work for (GetallEntries) ,where I traverse each block of memory record by record.For the delete function(DeleteEntry) ,I created an offset number , so as to reduce the block counter properly when a whole block is empty.My concept for the delete function was to find and erase the record from the block .Then I replace that empty place with the last record ,that is stored at the last block.In that way ,I use my memory effiecienty,beacuse there are no holes at my file system.
