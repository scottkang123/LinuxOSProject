execve():
executes the program referred to by pathname. This causes the program that is currently being run by the calling process to be re-placed with a new program, with newly initialized stack, heap, and (initialized and uninitialized) data segments. execve has three parameters, which are const char *pathname, char *const argv[], char *const and envp[]. Pathname must be either a binary executable or a script starting with a line of the form #!interpreter [optional-arg]. argv is an array of pointers to strings passed to the new program as its command-line arguments. The first item should contain the filename associated with the file being executed. envp is an array of pointers to strings, conventionally of the form key = value, which are passed as the environment of the new program. Both argv and envp must be terminated by a null pointer. The argument vector and environment can be accessed by the new program's main function defined as int main(int argc, char *argv[], char *envp[]). execve() does not return on success. Text, initialized dat, uninitialzied dat and stack of the calling process are overwritten according to the contents of the newly loaded program. 

mmap(): map files or devices into memory
It has 6 parameters, which are void *addr, size_t length, int prot, int flags, int fd, and off_t offset.
mmap() creates a new mapping in the virtual address space of the calling process. The starting address for the new mapping is specified in 'addr'. The 'length' argument specifies the length of the mapping (must be greater than 0). The contents of a file mapping are initialized using 'length' bytes starting at offset 'offset' in the file reffered to by the file descriptor 'fd'. 'prot' argument describes the desired memory protection of the mapping. The 'flags' argument determines wheter updates to the mapping are visible to other processes mapping the same region, and whether updates are carried through to the underlying file. On success, mmap() returns a pointer to the mapped area. On error, the value MAP_FAILED is returned, and errno is set to indicate the cause of the error. 

read(): read from a file descriptor.
It has 3 parameters, int fd, void *buf, and size_t count. read() attempts to read up to 'count' bytes from file descriptor 'fd' into the buffer starting at 'buf'. The read operation starts at the file offset, and the offset is incremented by the number of bytes read. If the file offset is at or past the end of file, no bytes are read and it returns zero. If 'count' is zero, the read() may detect the errors. If read() does not check for errors, a read() with a 'count' of zero will return zero. On success, the number of bytes read is returned, and the file position is advanced by this number. On error, -1 is returned. 


function f assembly code:
First, it decrements stack pointer by 32 bytes then stores x29 and x30 at location sp and sp+8 respectively. The return address is stored at x30. Then, it stores 'x' at sp+28 and store 'i' at sp+24.
It then goes to load sp+24 value (i) at w0 register and compare w0 with 9. (cmp w0, #0x9)
If w0 is less than or equal to 9, it branches back and loads sp+28 (x) to w0 and add 1 to it. Then it stores w0 to sp+28 (x). Then, it loads sp+24 (i) to w0 and add 1 to it. Then it stores w0 back to sp+24 (i). Then it loads sp+24 (i) back to w0 for next comparison. If w0 is greater than 9, it loads sp+28 (x) to w1. 
(the loop iteration works by using cmp to compare the value of i. When the value of i is less than or equal to 9, the assembly code increments x by 1 and i by 1. It repeats this procedure until i is greater than 9)

After calculating and shifting the page address, the assembly code goes to prinf function. 
Arguments are passed to the prinf() function by putting the relelvant address of the page to the register x0. The function can then access the address of relevant page and get the information necessary. In other word, the first argument is stored at x0 and printf function can access this to get its argument.



 

 

























