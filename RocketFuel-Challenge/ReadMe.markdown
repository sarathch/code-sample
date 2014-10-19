
TAIL COMMAND - UNIX Implementation
==================================

Inputs:
-------

    **tail -n [+k/k] [file_name1] [file_name2] [file_name3] ....**

    **tail -f [file_name1] [file_name2] [file_name3] ....**

>Note: You can give "quit" to quit application and "stop -f" to stop monitoring.</p>
 
### Design: 

   >This project contains 3 classes.</p>
   
   ###### Solution Class
   > This class is used to test TailCommand implementation . It Does tail command line processing and calls our Tail command implementation API.

   ######TailCommand Class
   > This Class contains APIS that implements tail command. USed RandomAccessFile API which handles large files and it does not bring the file into the memory as it randomly seeks to line number/Byte number in the File, So This handles big files as well. It implements both -n with +k/k  and -f options.

   ######PrintThread Class
   > This thread class will monitor the file for updates. It is used for -f option which prints appended lines.
