
<h1> TAIL COMMAND - UNIX Implementation </h1>

<h2> Inputs: </h2>

<p>tail -n [+k/k] [file_name1] [file_name2] [file_name3] .... 
tail -f [file_name1] [file_name2] [file_name3] .... </p>

<p>Note: You can give "quit" to quit application and "stop -f" to stop monitoring.</p>
 
<h2> Design: </h2>

   <p>This project contains 3 classes.</p>
   
   <h3> Solution Class </h3>
       <p>This class is used to test TailCommand implementation . It Does tail command line processing and calls our Tail command implementation API.</p>

   <h3>TailCommand Class </h3> 
   	<p>This Class contains APIS that implements tail command. USed RandomAccessFile API which handles large files and it does not bring the file into the memory as it randomly seeks to line number/Byte number in the File, So This handles big files as well. It implements both -n with +k/k  and -f options.</p>

   <h3>PrintThread Class </h3> 
   	<p>This thread class will monitor the file for updates. It is used for -f option which prints appended lines.</p>
