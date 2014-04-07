                                  
/********************************************************************************************************************************* 
 * FileSharApp.cpp : Single file to handle the file sharing application
 *
 * Starts as a server or a client on a given port
 * If server, It acts as a moderator keeping track of clients joining and leaving the network.
 * If client, It get's itself registered with server and can upload and download files from registered clients in the network. 
 *
 * Created for CSE 589 Spring 2014 Programming Assignment 1
 * @author Sarath Chandra Y
 * @created 26 February 2014
 **********************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <list>
#include <sstream>
#include <map>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <iomanip>
#include <algorithm>

using namespace std;

#define STDIN 0
#define MAXDATASIZE 100
#define CHUNKSIZE 1000    /*change packet size here*/

/*
 * Method to get IPv4 || IPv6 sin_addr type respectively.
 * @arg struct sockaddr containing details of connected remote client. 
 * @return sin_addr || sin6_addr
 * References: copied from Beej guide: http://beej.us/guide/bgnet/examples/selectserver.c
 */ 
                           
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/* Data Structure to hold Id, IP Adress, Port Number Fields and link to next */

struct slink {
    int cID;
    string cIP;
    string cPort;
    slink* next;
};

/* Method Declarations */

slink* getnode();
slink* insert(slink* , string, string, int);
void display(slink*);
void displayConn(slink*);
slink* remove(slink*, int);
void sendList(int, int, slink*);
void sendListToClient(slink*, int);
void usrCommSession( char*, char*, string );
static void Initialize();
void userConnect( char*, char*, string, char*);
void recvList(int);
void destroy(slink*);
char* getHostNameFromIP(char*);
int getAddrFromHostName(char*, char*);
void sendFile(int, string, string, int);
void getFile(int, string, string);
string getRegClientPortNo(slink*, string);
int isValidConnection(slink* ,int);
int isExistingClient(slink* ,char*);
int listSize(slink*);
void removeSocket( int*, int);
vector<char*> inputStringSplit(string); 
void addFdToMaster(int);
string getHostNameFromConID(slink* ,int);
void getMyIPAddress(char*, char*);

/* Assigning numeric constants to strings */
static enum StringVal { ev_NotUsed,
                        ev_HELP,
                        ev_MYIP,
                        ev_MYPORT,
                        ev_REGISTER,
                        ev_CONNECT,
                        ev_LIST,
                        ev_TERMINATE,
                        ev_EXIT,
                        ev_UPLOAD,
                        ev_DOWNLOAD,
                        ev_CREATOR }stVal;

/* Map to associate strings with enum values */
static map<string, StringVal> map_StringVal;

char myip[INET_ADDRSTRLEN]; 		/* stores IP Address of the Host of this current process */		
string FILE_COMM_TOKEN;			/* stores Token to initiate File Transfer to inform Upload/Download Request*/
int OFFSET;				/* It captures the offset when generating socket file descriptors */ 
int servfd;				/* stores the server socket file descriptor */
fd_set master;				/* lists all file descriptors which select() is listening on */ 
int fdmax;				/* Max file descriptor */ 
slink *head =NULL;			/* Intializing head node of clients registered to server list */ 
slink* conn_head =NULL;			/* Intializing head node of client connections list */
char conn_IP[14],server_IP[13];		/* Connected client IP Address and Server IP Address */
int cSocks[10], size;			/* Sockets array listing socket file descriptors and size variable to store size */
struct timeval entry_tv[100];		/* stores entry times of download routine wrt connection */
double down_file_size[100];		/* Next 3 variables for storin file details.. */
double down_file_size_check[100];	                           
string down_file_name[100];		 					
	

/*
 * Main Method to start off .
 * @arg Argument 1: 'c'/'s' i.e Client/Server 
 * 	Argument 2: Listening Port Number.   
 * References: select() API part copied from Beej guide: http://beej.us/guide/bgnet/examples/selectserver.c
 */


int main(int argc, char* argv[])
{

    int numbytes;                                       // number of bytes sent/received via send()/recv()
    struct addrinfo *ai, *p, hints;
    int sockfd;						// socket file descriptor 
    int listener;					// listening socket descriptor
    int newfd;						// newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; 		// client address
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];			// connected client IP address
    int yes=1;						
    int i, rv;
    char buf[256];					// buffer for client data	
    FILE* files[100];
    int count_t[100] = {0};	
    string procType;					// process type i.e client/server         
    fd_set read_fds;					// temporary list of generated file descriptors
    
    // Arguments validity check                                            
    if (argc != 3) {
        cout << " Invalid Arguments " << endl;
        cout << " Argument 1: Enter c/s i.e Client/Server\t Argument 2: Listening Port Number " << endl;
        exit(1);
    }else if (strcmp( argv[1],"c") != 0 && strcmp(argv[1],"s") != 0) {
        cout << " Invalid Argument 1 ! Enter 'c'/'s' to indicate Client/Server " << endl;
        exit(1);
    }else if(atoi(argv[2]) <= 1024 || atoi(argv[2]) > 65535) {
        cout << " Invalid Argument 2 ! Port Number should be >1024 and <65536 " << endl;
        exit(1);
    }
    
    // set client/server process  
    if (strcmp(argv[1],"c") == 0) {
	procType = "client";
	OFFSET = 4;
    }else {
	procType = "server";
	OFFSET = 3;
    }

	
    
    // get this host's IP address
    char comm_tok[] = "MYIP";
    getMyIPAddress(comm_tok,argv[2]);

    // clear the master and temp sets
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, argv[2], &hints, &ai)) != 0) {
	fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
	exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	if (listener < 0) {
	    continue;
	}
        // lose the pesky "address already in use" error message
	setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
	    close(listener);
	    continue;
	}
	break;
    }
    
    // if we got here, it means we didn't get bound
    if (p == NULL) {
	fprintf(stderr, "selectserver: failed to bind\n");
	exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
	perror("listen");
	exit(3);
    }
    // add the listener to the master set
    FD_SET(listener, &master);
    FD_SET(STDIN, &master);
    size = 1;
    cSocks[1] = listener;
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
    cout << " Enter 'HELP' to see available user interface commands ! Thank You !" << endl;
    // default 1st entry into registered client list ! Server will always be first entry in the list!
    head = insert(head, "128.205.36.8", argv[2], listener-OFFSET);

    // main loop
    for(;;) {
	read_fds = master; // copy it
	if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
	    perror("select");
	    exit(4);
	}
	// run through the existing connections looking for data to read
	for(i = 0; i <= fdmax; i++) {
	    if (FD_ISSET(i, &read_fds)) { // we got one!!
		if (i == listener) {
		    // handle new connections
		    addrlen = sizeof remoteaddr;
		    newfd = accept(listener,
		    	(struct sockaddr *)&remoteaddr,
				&addrlen);
		    if (newfd == -1) {
			perror("accept");
		    } else {
			string remoteAddr=inet_ntop(remoteaddr.ss_family,
                                        get_in_addr((struct sockaddr*)&remoteaddr),
                                                remoteIP, INET6_ADDRSTRLEN);
			
	 		string remIP;		// precise remote IP address
			// workaround for some workstations 'HighGate' sometimes ignore '::ffff:' before needed IP Address
			if ( strcmp(remoteAddr.substr(0,2).c_str() , "::") == 0)
			    remIP = remoteAddr.substr(7);
			else 
			    remIP = remoteAddr;	
			printf("Server:New connection from %s on socket %d\n",remoteAddr.c_str(),newfd);
                        int len;		// length of message		
			// receives the size of message token 
			if( numbytes = recv( newfd, &len, sizeof( len ), 0 ) == -1 )
        		    perror("recv");
			// receives message of size len in buffer 
			if ((numbytes = recv(newfd, buf, len, 0)) > 0)  
                            buf[len] = '\0';

			// Server/Client Functionality check
			if (strcmp(procType.c_str(),"server") == 0) {
                            string clientListnPort = buf;
			    // add this client to registered users list	
	      		    head = insert(head , remIP, clientListnPort, newfd-OFFSET );
			    // add this fd to master list to listen on	
			    addFdToMaster(newfd);    
			    // display and broadcast updated registered users list
    			    display(head);
			    for (int i=2; i <= size ; i++) {
			        sendList(cSocks[i], size, head);
			    }	
		        }else { // if client
			    // get client connections list size
			    int listsz = listSize(conn_head);				
			    cout << "*---*---*" <<buf << "*---*---*" << endl;		// display Success/Failure Query messages received 
	      		    // get port number of connected client from
			    string portNo = getRegClientPortNo(head, remIP);		 
			    string connToken;						// Success/Failure Token				
			    int tokSize;						// Token Size
			    // Max Connections Check	
			    if (listsz == 4) {
				cout << " YOU HAVE REACHED YOUR MAXIMUM CONNECTIONS LIMIT! "  << endl;
				connToken = "SERVER:NO";
                                tokSize = connToken.length();
                            }else {	 	
				// updating connections list
			    	conn_head = insert(conn_head , remIP, portNo, newfd-OFFSET );
			    	connToken = "SERVER : YEAH..!WE ARE CONNECTED!...!!";
			    	tokSize = connToken.length();
				addFdToMaster(newfd);
			    }
			    // Sending Success/Failure Token string 	
			    if (send (newfd, &tokSize, sizeof( tokSize ), 0) == -1)
			        perror("send ");
	         	    if (send(newfd, connToken.c_str(), tokSize, 0) == -1)
            			perror("send");
			}
			memset(&buf[0], 0, sizeof(buf));
  		    }    
		}else if (i == STDIN) {				// listening on STDIN FD
//		    cout << "BAZZINGA 2" << endl;	
		    char buff[100];				// buffer to hold input
		    //memset(&buff[0], 0, sizeof(buff));	
		    // reading bytes using read();	
  		    int n = read( 0, buff, 100);  		
		    string myPort = string(argv[2]);
		    // Functionality of program based on commands  
		    usrCommSession(buff, argv[1], myPort);
                    memset(&buff[0], 0, sizeof(buff));		// clearing buffer
		}else {							// listening on FD's in master list
		    // Server/Client Functionality check	
		    if (strcmp(argv[1],"s") == 0) {		  
			char ebuf[100];
			int len = 4;
			// server receiving message from clients
		        if ((numbytes = recv(i, ebuf, len, 0)) > 0) {
			    ebuf[len] = '\0';	
			    string str(ebuf);	
			    // Checking if received message is "EXIT" 
                            if (strcmp(str.c_str(),"EXIT")==0) {
				// closing connection 
				cout << "Connection closed - " << i << endl << "Updating Registered client list..." << endl;
			    	FD_CLR(i, &master);
				// updating registered users list
			    	head = remove(head, i-OFFSET);
				// remove socket from Connected Sockets array 
				removeSocket(cSocks,i);	
				//display and broadcasting updated list
                           	display(head);
				cout << "Brodcasting updated registered client list... " << endl;
				for (int i=size; i > 1; i--) {
	                            sendList(cSocks[i], size, head);
				}
		            }
		        }
		    }
		    else if (i == servfd) {			// client listening on socketfd linked with server
			// receives updated list from server
			recvList(i);
		    }else {					// client listening on socket fd's other than server linked socket
			char ebuf[BUFSIZ];			                
			// client receiving message from connected clients
                        if ((numbytes = recv(i, ebuf, CHUNKSIZE, 0)) > 0) {	
			    string str(ebuf);	
			    string chckBuf = str.substr(0,4);
			    // check if message is 'EXIT' 	
			    if (strcmp(chckBuf.c_str(),"EXIT")==0) {
		       	        cout << "Connection closed - " << i 
							<< endl << "Updating Connected client list..." << endl;
				// removing from master list
                                FD_CLR(i, &master);
				// removing from connected users list
                                conn_head = remove(conn_head, i-OFFSET);
				// removing from connected sockets
				removeSocket(cSocks,i);
  			    }
			    else if (strcmp(chckBuf.c_str(),"UPLO")==0) {			// check if message is 'UPLOAD'
			  	// getting Hostname of uploading peer from connections list using conn id = 'FD - OFFSET'                      
   				string source = getHostNameFromConID(conn_head, i-OFFSET);
				string file_name = str.substr(7);
				// receiving file from source
				getFile(i, file_name, source);
			    }else if (strcmp(chckBuf.c_str(),"DOWN")==0){ 			// message is 'DOWNLOAD'
				// getting Hostname of downloading peer from connections list using conn id = 'FD - OFFSET'
				string dest = getHostNameFromConID(conn_head, i-OFFSET);
				
			        string file_name = str.substr(9);
				vector<char*> file_tokens = inputStringSplit(file_name.c_str());
				for (long index=0; index<(long)file_tokens.size(); index += 1 ) {
                                    struct stat file_stat;
                                    int FD = open(file_tokens[index], O_RDONLY);
                                    if (FD == -1) {
                                    cout << "File Not found! Try Again!" << endl;
                                    }else if (fstat(FD, &file_stat) < 0) {
                                        cout << "File cannot be accessed! Try Again!" << endl;
                                    }else{
                                        // last minute work around to to receive at select()
                                        int fileSize = file_stat.st_size;
                                        ostringstream pp,qq;
                                        pp << fileSize;
                                        string fsize = pp.str();
				        string msg = "SIZE";
                                        string File_len = file_tokens[index];
					int tok_len = File_len.length();
                                        qq << tok_len;
                                        string tokLen = qq.str();
					string S_TOKEN = msg + ' ' + tokLen + ' ' + fsize + ' ' + file_tokens[index];
					// client receiving this token will have enough info to receive at select()
                                        if (send(i, S_TOKEN.c_str() , S_TOKEN.length(), 0) == -1)
                                            perror("send");
					//work around to avoid garbage values in the buffer 
					//this wont effect time delay..used to allow the code to reach memset(buffer)
					usleep(10000);	
					// send file to destination		
				        sendFile(i, file_tokens[index], dest, FD);	
				    }
				    if(file_tokens.size() > 1)
					usleep(10000);  // last minute workaround
			        }
			    }else if (strcmp(chckBuf.c_str(),"SIZE")==0) { 			// SIZE token..with necessary file details
				vector<char*> file_tokens = inputStringSplit(str.substr());
				int file_len = atoi(file_tokens[1]);				
				string filName = string(file_tokens[3]);
				down_file_size[i-OFFSET] = atoi(file_tokens[2]);		// file	sizes
				down_file_name[i-OFFSET] = file_tokens[3];			// file names..corresponding to conn id i.e i-OFFSET
                                down_file_size_check[i-OFFSET] = atoi(file_tokens[2]);
				string file_name = filName.substr(0,file_len);
				files[i-OFFSET] = fopen(file_name.c_str(), "ab+");		// fd's wrt file names	
				gettimeofday (&entry_tv[i-OFFSET], NULL);			// entry time values

			    }else
			    {
			        down_file_size[i-OFFSET] -= numbytes;
                                fwrite (ebuf, sizeof (char), numbytes, files[i-OFFSET]);
				if (down_file_size[i-OFFSET]==0) {
                                    //cout << "Download Complete::" << i << flush << endl;
				    fclose(files[i-OFFSET]);	
                                    double timeTaken;
                                    struct timeval tv2;
				    gettimeofday (&tv2, NULL);
                                    string source = getHostNameFromConID(conn_head,i-OFFSET);
                                    // compute and print the elapsed time in millisec
                                    timeTaken = (tv2.tv_sec - entry_tv[i-OFFSET].tv_sec) * 1000.0;      // sec to ms
                                    timeTaken += (tv2.tv_usec - entry_tv[i-OFFSET].tv_usec) / 1000.0;   // us to ms
                                    double rate = down_file_size_check[i-OFFSET]/(timeTaken/1000);
				    double rate_mbps = (rate * 8) / 1000000;	
                                    char* host = getHostNameFromIP(myip);
                                    string recv = string(host);
                                    cout << "File " << down_file_name[i-OFFSET] << " has been downloaded !" << flush << endl;
                                    cout << "Rx (" << recv << "): " << source << " -> " << recv << "File Size: "
                                                              << down_file_size_check[i-OFFSET] << " Bytes, Time Taken: "
								        << timeTaken << " milli seconds, Rx Rate: " << rate_mbps
                                                                                                        << "Mbits/sec." << flush << endl;

                                 }
			    }
			    memset(&ebuf[0], 0, sizeof(ebuf));
			}
			memset(&ebuf[0], 0, sizeof(ebuf)); 					// clear buffer
		    }
         
		} // END handle data from listening fd's  
	    } // END got new incoming connection
    	} // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}


/*
 * Method to add file descriptors to master list.
 * @arg File Descriptor                                                
 * @return none               
 */

void addFdToMaster(int fd)
{
    FD_SET(fd, &master); // add to master set
    if (fd > fdmax) {
    // keep track of the max
        fdmax = fd;
        size++;
        cSocks[size] = fd;
        cSocks[size+1] = '\0';
    }
}

                           
/*
 * Method to implement program functionalities based on STDIN commands.
 * @arg Arg1: Command issued
 *	Arg2: Process type i.e c/s
 *	Arg3: Listening port number                                                 
 * @return none               
 */

void usrCommSession(char* inp_Option, char* proc, string my_port) 
{
    // initiallizing mapper	
    Initialize();
    string inp = string (inp_Option);
    transform( inp.begin(), inp.end(), inp.begin(), ::toupper);
    // Tokenizing input command at spaces              
    vector<char*> command = inputStringSplit(inp.c_str());	
    string exit_msg = "EXIT";	
    string upload_msg = "UPLOAD";
    string download_msg = "DOWNLOAD";
//    string comm = string(command[0]);
//    transform( comm.begin(), comm.end(), comm.begin(), ::toupper);	
    cout << "Command:: " << command[0] << endl;	
    // switch with mapped string values 
    switch( map_StringVal[command[0]] ){
	 
        case ev_HELP:							// List available commands
            cout << "/**-------------------------------Enter user interface commands listed below:----------------------------------**/"<< endl;
	    cout 															<< endl;
	    cout << "MYIP			   	     " << " ::Command to display host IP Adress" 			 	<< endl;
	    cout << "MYPORT			   	     " << " ::Command to display port number on your host which is "
											<<"listening for connections " 		 	<< endl;
	    cout << "REGISTER <ServerIP> <PortNo>  	     " << " ::Command to register with Server " 			 	<< endl;
	    cout << "CONNECT <Destination> <PortNo>	     " << " ::Command to connect with registered clients " 		 	<< endl;
	    cout << "LIST			   	     " << " ::Command to list all connections this host is part of " 	 	<< endl;
	    cout << "TERMINATE <Connection ID>     	     " << " ::Command to make a client terminate connection with " 
											<<"one of its connected clients " 	 	<< endl;
            cout << "EXIT				     " << " ::Command to close all connections and exit the application" 	<< endl;
	    cout << "UPLOAD <Conn ID> <FileName>   	     " << " ::Command to client host for uploading a file to a "
											<<"client in connection id" 		 	<< endl; 	
 	    cout << "DOWNLOAD <ConID1> <FN1> <ConID2> <FN2>.." << " ::Command to download files FN1,FN2 from connections ConID1,ConID2" << endl;
	    cout << "CREATOR				     " << " ::Command to know about the developer"				<< endl;
	    cout 															<< endl;
	    cout << "/**---------------------------------------------Happy Sharing!-------------------------------------------------**/"<< endl;
            break;
        case ev_MYIP:							// Display IP Address
	{
            cout << "HOST IP Address:: " << myip << endl;
            break;
	}
        case ev_MYPORT:							// Display Port No
            cout << "Host's Listening PORT:: " << my_port << endl;
            break;
        case ev_REGISTER:							// Register with Server
	    cout << "command size " << command.size() << "command::" << inp_Option << endl;
	    // flag wrong commands 
	    if (command.size()!=3) {
                cout << "Invalid command syntax " << endl;
                break;
            }		
	    // flag self connections	
	    if (strcmp(command[1],myip)==0) {
		cout << "You are trying to connect yourself!" << endl;                                   	
			break;
	    }	
	    // Check if client/server
	    if( strcmp( proc,"s" ) == 0 )
                cout << " Wrong command issued to a server process " << endl;
            else{
                char port_No[5];
		//ector<char*> inp = inputStringSplit(inp_Option);
		strcpy(server_IP,command[1]); 
		strcpy(port_No, command[2]);
		//check for duplicate connections 
		if (!isExistingClient(head,myip)) {
                    char* serv_name = getHostNameFromIP(server_IP);
  	            // registering with server 
                    userConnect(serv_name, port_No, my_port, command[0]);
		}else
		    cout << " You are already Registered! Thanks!" << endl;
            }
            break;
        case ev_CONNECT:
	    if (command.size()!=3) {
                cout << "Invalid command syntax " << endl;
                break;
            }
	    // Check if client/Server	
	    if( strcmp( proc,"s" ) == 0 )
                cout << " Server dont connect to clients in this app !" << endl;
            else{			        		
		char port_No[5];
		char IP[100];
		int INVALID_FLAG = 0;
		// flagging invalid IP
		if(strcmp(getHostNameFromIP(command[1]),"bad ip") == 0) {
		    if(getAddrFromHostName(command[1], IP))
			strcpy(conn_IP, IP);
		    else {
			cout << " Enter valid IP Address / Host Name to Connect " << endl;
			INVALID_FLAG = 1;
		    }
		}else 
		    strcpy(conn_IP, command[1]); 
		if (INVALID_FLAG != 1) {
		    // flagging self connections   
		    if (strcmp(conn_IP,myip)==0)
			cout << "Your trying to connect to your own system! Please try again!" << endl;   
		    else {
		        strcpy(port_No, command[2]);
			// flagging unregistered client connections
		    	if (isExistingClient(head,conn_IP)) {
			    // flagging duplicate connections	
			    if(!isExistingClient(conn_head,conn_IP)) {
				int listsz = listSize(conn_head);
				// flagging Extra Connections
				if(listsz < 4) {	
		    	    	    char* conn_name = getHostNameFromIP(conn_IP);
                    	    	    userConnect(conn_name, port_No, my_port, command[0]);
				}else
				    cout << " YOU CAN CONNECT TO MAXIMUM 3 PEERS! SORRY! " << conn_IP << endl;
			    }else
			    	cout << " YOU ARE ALREADY CONNECTED TO " << conn_IP << endl;
		    	}else
	           	cout << "WANT TO CONNECT WITH " << command[1] << " ?! PLEASE REGISTER WITH SERVER '128.205.36.8' :) " << endl;
                    }
	    	}
	    }
            break;
        case ev_LIST:						// display connections of this process
		if( strcmp( proc,"s" ) == 0 )			// display registered users
		    displayConn(head);				
		else 
		    displayConn(conn_head);			// display connected users	
            break;
        case ev_TERMINATE:					// Terminate client connections
	    if( strcmp( proc,"s" ) == 0 )			 
                cout << " Wrong command to a server process " << endl;
            else{
		const char conn_id = (const char)inp_Option[10];
		int conn_no = conn_id - '0';
		// check if valid connection                                                                  
		if(isValidConnection(conn_head, conn_no)) {
		    // send exit token	
		    if (send(conn_no +OFFSET, exit_msg.c_str(), exit_msg.length(), 0) == -1)
                        perror("send");
		    FD_CLR(conn_no+OFFSET, &master); 		// removing fd from master list
                    conn_head = remove(conn_head, conn_no);	// remove from connections list
		    removeSocket (cSocks, conn_no+OFFSET);      // remove socket from connected sockets    
		}else
		    cout << " Invalid Connection ID" << endl;
	    }	         	
            break;
        case ev_EXIT:						// exit network
	    if( strcmp( proc,"s" ) != 0 ) { 			// client closing its connections
		// checking for existing connections
		if (size > 2) {					
                    cout << "Closing Connections...." << endl; 
		    for (int i=size; i > 2; i--) {
			// send exit token
		        if (send(cSocks[i], exit_msg.c_str(), exit_msg.length(), 0) == -1) 
  	                    perror("send");
		    }
		  
		}						// client unregistering from server
		cout << "Notifying Server...." << endl;		
		// exit token
		if (send(servfd, exit_msg.c_str(), exit_msg.length(), 0) == -1)
                        perror("send");
		}
	    exit(0);						
            break;
        case ev_UPLOAD:						// client uploads a file to connections
 	    if (command.size()!=3) {
                cout << "Invalid command syntax " << endl;
                break;
            }
	    if( strcmp( proc,"s" ) == 0 )				
                cout << " Wrong command to a server process " << endl;
            else{						
		int EXIT_FLAG;  
		string myHostName = getHostNameFromIP (myip);                           
                int conn_no = atoi (command[1]); 		// connection no to upload
		string file_name = string(command[2]);		// file name to upload	
		FILE_COMM_TOKEN = upload_msg + ' '  + file_name;

		struct stat file_stat;
                int fd = open(command[2], O_RDONLY);
                if (fd == -1) {
                    cout << "File Not found! Try Again!" << endl;
                    EXIT_FLAG = 1;
                }

                /* Get file stats */
                if (fstat(fd, &file_stat) < 0) {
                    cout << "File cannot be accessed! Try Again!" << endl;
                    EXIT_FLAG = 1;
                }
		if (EXIT_FLAG != 1) {
		    // flag invalid connections
                    if(isValidConnection(conn_head, conn_no)) {
			if (send(conn_no + OFFSET, FILE_COMM_TOKEN.c_str(), FILE_COMM_TOKEN.length(), 0) == -1)
                            perror("send");
			FILE_COMM_TOKEN.clear();
                        int fileSize = file_stat.st_size;
                                             
                        /* Sending file size */
                        if (send(conn_no + OFFSET, &fileSize , sizeof fileSize, 0) == -1)
                            perror("send");

		        string dest = getHostNameFromConID(conn_head,conn_no);	
		        // sending the file 	
                        sendFile(conn_no +OFFSET, file_name, dest, fd);
		    }else
		        cout << "Not a valid Connection" << endl;	
		}
	    }
	    break;
        case ev_DOWNLOAD:
	    if( strcmp( proc,"s" ) == 0 )
                cout << " Wrong command to a server process " << endl;
            else{
		if (command.size() % 2 != 1) {
		    cout << "Invalid command syntax" << endl;
		    break;
		}
		int conn_no_list[20] = {0}; 	 
		int req_files_count =0;
		string files_list[200];

 		// iterating over command input looking for multiple requests from one connection 			          
	        for (long index=1; index<(long)command.size(); index += 2 ) {
		    int DUPLICATE = 0;
		    char* conn = command[index];
		    string file_name = string(command[index+1]);                // file name to get
		    int conn_no = atoi(conn);	
		    for (int p = req_files_count; p>=0; p--) {
			if (conn_no_list[p] == conn_no) {
			    DUPLICATE = 1;		
			}
		    }
		    if(DUPLICATE == 0){ 
			conn_no_list[req_files_count] = conn_no;		
			req_files_count++;
		    }
		    files_list[conn_no] += file_name + ' ';
		}
		// sending download requests to connections
		for (int p=0 ; p<req_files_count; p++) {
		    int conn_no = conn_no_list[p];				// connection to download from
		    string file_name = files_list[conn_no];
                    FILE_COMM_TOKEN = download_msg + ' ' + file_name;	
		    // flagging invalid connections	
		    if(isValidConnection(conn_head, conn_no)) {
			if (send(conn_no+OFFSET, FILE_COMM_TOKEN.c_str(), FILE_COMM_TOKEN.length(), 0) == -1)
                            perror("send");
                        cout << " Receiving Files .... " << endl;
                    }else
                            cout << "Not a valid Connection" << endl;
		}
            }

            break;
        case ev_CREATOR:							
	    cout << " SARATH CHANDRA Y " << " sarathch@buffalo.edu " << endl; 
            break;
        default:
            cout << " Enter Valid Command " << endl;
            break;
    }
}

/*
 * Method mapping string values to enum values.
 * @arg none                                                 
 * @return none               
 */

void Initialize()
{
    map_StringVal["HELP"] = ev_HELP;
    map_StringVal["MYIP"] = ev_MYIP;
    map_StringVal["MYPORT"] = ev_MYPORT;
    map_StringVal["REGISTER"] = ev_REGISTER;
    map_StringVal["CONNECT"] = ev_CONNECT;
    map_StringVal["LIST"] = ev_LIST;
    map_StringVal["TERMINATE"] = ev_TERMINATE;
    map_StringVal["EXIT"] = ev_EXIT;
    map_StringVal["UPLOAD"] = ev_UPLOAD;
    map_StringVal["DOWNLOAD"] = ev_DOWNLOAD;
    map_StringVal["CREATOR"] = ev_CREATOR;
}

/*
 * Method to get this host's IP Address by connecting to google DNS 
 * @arg none                                                 
 * @return none               
 */

void getMyIPAddress (char* command, char* port) {
    struct hostent *he;
    struct in_addr ipv4addr;
    char dnsIP[] = "8.8.8.8";                                   // Google DNS IP
    char dnsPort[] = "53" ;                                     // Google DNS TCP Port
    // Converting IP to struct in_addr type     
    inet_pton(AF_INET, dnsIP, &ipv4addr);
    // getting host details from IP address
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    // Connecting to google DNS to get IP address of this process
    userConnect (he->h_name, dnsPort, port, command);          
}

/*
 * Method to connect to the given server IP and Port Number.
 * @arg Arg1: IP address you are trying to connect 
 *      Arg2: Port Number @Arg1 is listening on
 *      Arg3: Your Listening port number
 *	Arg4: Input Command                                                  
 * @return none               
 */

void userConnect ( char* sIP, char* sPort, string my_port, char* command )
{
    int numbytes;
    char buf[MAXDATASIZE];
    char rbuf[MAXDATASIZE];
    int rv;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo *servinfo,*p,hints;
    int sockfd;
    int SETUP_ERROR = 0;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    // Getting address related information 
    // flagging invalid ip addresses 
    if ((rv = getaddrinfo(sIP, sPort, &hints, &servinfo)) != 0) {
        cout << "getaddrinfo: " << endl << gai_strerror(rv) << endl;
	cout << " Invalid IP Address! " << endl;
        SETUP_ERROR = 1;
    }else
    {
        int count;
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            	perror("client: socket");
            	continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            	close(sockfd);
            	perror("client: connect");
            	continue;
            }

            break;
        }
	// flagging invalid port numbers
        if (p == NULL) {
            cout << stderr << "client: failed to connect" << endl;
	    cout  << "Invalid Port Number! " << endl;
            SETUP_ERROR = 1;
        }
    }
    // connection success check	                       
    if (SETUP_ERROR != 1) {

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);	// gettin printable ip
        freeaddrinfo(servinfo);									// don't need anymore
        int len;
	// Functionalities based on input commands
        if (strcmp(command,"REGISTER")==0) {							
            // client adding server as default first connection
            conn_head = insert(conn_head , server_IP, sPort, sockfd-OFFSET );
            servfd = sockfd;
            len = my_port.length(); 
            // sending client's listening port number to server 
            if (send (sockfd, &len, sizeof( len ), 0) == -1)
                perror("send packet length");
            if (send(sockfd, my_port.c_str(), len, 0) == -1)
            	perror("send");
	    // adding socket fd to master list 
	    addFdToMaster(sockfd);
        }else if (strcmp(command,"CONNECT")==0) {
 	    // updating connections list
	    conn_head = insert(conn_head , conn_IP, sPort, sockfd-OFFSET );
	    // sending query token
            string connToken = "CLIENT : CONNECTION SUCCESS.....?";
	    len = connToken.length();
	    if (send(sockfd, &len , sizeof len, 0) == -1)
            	perror("send");
            if (send(sockfd, connToken.c_str() , len, 0) == -1)
            	perror("send");
	    // receiving reply from connection
	    if (numbytes = recv (sockfd, &len , sizeof len, 0) == -1)
            	perror("send");
	    if ((numbytes = recv(sockfd, buf, len, 0)) > 0)
            	buf[len] = '\0';
	    cout << "*---*---*" <<buf << "*---*---*" << endl;
	    // if length of reply from connection is >10 connection success else connection fail
	    if (len > 10)
	    	addFdToMaster(sockfd);				// adding fd to master 				
	    else						
	    	cout << " Client you reached is not accepting anymore connections! " << endl;  	
        }else if(strcmp(command, "MYIP")==0) {			
	    /* Next 10 line code snippet is taken from a blog online! Cant find it anymore to post reference ! :) */	 
 	    //get local socket info::
            struct sockaddr_in local_addr;
            	socklen_t addr_len = sizeof(local_addr);
            if (getsockname(sockfd, (struct sockaddr*)&local_addr, &addr_len) < 0) {
            	perror("getsockname");
            }
            /* get peer ip addr */
            char my_ip[INET_ADDRSTRLEN];
            if (inet_ntop(local_addr.sin_family, &(local_addr.sin_addr), myip, sizeof(myip)) == NULL) {
            	perror("inet_ntop");
            }
        }
    }
}

/*
 * Method to split a string based on spaces ' '.
 * @arg Arg1: Input String                                                  
 * @return char array vector of split tokens               
 */

vector<char*> inputStringSplit( string inputLine) 
{
    string str(inputLine);
    string buf; 		// Buffer to hold string
    stringstream ss(str); 	// Insert the string into a stream
    vector<char*> tokens;       // vector to hold our tokens

    while (ss >> buf){
	char *a=new char[buf.size()+1];
	a[buf.size()]=0;
	memcpy(a,buf.c_str(),buf.size());
        tokens.push_back(a);
    }
    return tokens;
}

/*
 * Method to get IP Address from Host Name.
 * @arg Arg1: Host name
 *      Arg2: empty char array to copy IP                                                 
 * @return 1/0 'success/failure'                
 * References: copied from Beej guide: http://beej.us/guide/bgnet/examples/
 */

int getAddrFromHostName(char* hostname, char* ip)
{
    struct hostent *he;
    struct in_addr **ipv4addr;
    // flag invalid host names	
    if((he = gethostbyname(hostname))==NULL)
	return 0;			
    ipv4addr = (struct in_addr **) he->h_addr_list;
    for (int i = 0; ipv4addr[i] != NULL; i++)
    {
        //Return the first one;           
        strcpy(ip , inet_ntoa(*ipv4addr[i]) );
        
    }
    return 1;
}

/*
 * Method to get Host Name from IP address.
 * @arg Arg1: IP Address                                                
 * @return Host Name               
 */

char* getHostNameFromIP(char* servIP)
{
    struct hostent *he;
    struct in_addr ipv4addr;
    // convert string ip address to in_addr type
    int result = inet_pton(AF_INET, servIP, &ipv4addr);
    // conversion failed return "bad ip"
    if (result == 0) {
	string b = "bad ip";	
	char* p = const_cast<char*>(b.c_str());
	return p;				
    }	
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    return he->h_name;
}

/**************************************************-- List Operations--************************************************************/
/* Should have used STL! Thought I should not !..Too much code :/ */

/*
 * Method to insert into list.
 * Inserting to back of the list
 * @arg Arg1: head node of the list
 *	Arg2: IP address                                                
 *	Arg3: Port Number
 * 	Arg4: Entry ID	
 * @return head node               
 */


slink* insert(slink *head ,string IP, string Port, int id)
{
    slink *makenode;
    slink *prev = NULL , *curr = NULL ;
    curr = head ;
    if(head==NULL) {
        makenode = getnode();
        makenode->cIP = IP;
	makenode->cPort = Port;
        makenode->cID = id;
        makenode->next = NULL ; 
        head = makenode ;
        return head ;
    }
    while(curr != NULL) {
        prev = curr ;
        curr = curr->next ;
    }
    makenode = getnode();
    makenode->cID = id;
    makenode->cIP = IP; 	
    makenode->cPort = Port ;
    makenode->next = NULL ; 
    prev->next = makenode ;
    return head;
}

/*
 * Method to display the Registered users list.
 * @arg Arg1: head node                                                
 * @return node               
 */

void display(slink *head)
{
    slink *q;
    q = head;
    if(q == NULL) {
        printf(" No Connections Found \n");
    }
    cout << setw(10) << "Conn ID" << setw(25) << "IP Address" << setw(15) << "Port" << endl;
    while(q != NULL) {
        if(q->next == NULL)
        {
            cout << setw(9) << q->cID << ":" << setw(25) << q->cIP << setw(15) << q->cPort << endl;
            break;
        }else {
            cout << setw(9) << q->cID << ":" << setw(25) << q->cIP << setw(15) << q->cPort << endl;
            q = q->next ;
        }
    }
}

/*
 * Method to Connected users list.
 * @arg Arg1: head node                                                
 * @return none               
 */


void displayConn(slink *head)
{
    slink *q;
    q = head;
    if(q == NULL) {
        printf(" No Connections Found \n");
    }
    cout << setw(5) << "Conn ID:"  << setw(35) << " HostName" << setw(25) << "IP Address" << setw(15) << "Port" << endl;
    while(q != NULL) {
        if(q->next == NULL)
        {   
            cout << setw(9) << q->cID << ":" << setw(35) << getHostNameFromIP(const_cast<char*>(q->cIP.c_str())) 
									<< setw(25) << q->cIP << setw(15) << q->cPort << endl;
            break;
        }else {
            cout << setw(9) << q->cID << ":" << setw(35) << getHostNameFromIP(const_cast<char*>(q->cIP.c_str())) 
									<< setw(25) << q->cIP << setw(15) << q->cPort << endl;
            q = q->next ;
        }
    }
}

/*
 * Method to display list size.
 * @arg Arg1: head                                                
 * @return size of the list               
 */


int listSize(slink *head)
{
    slink *q;
    q = head;
    int count;
    if(q == NULL) {
        printf(" No Connections Found \n");
	return 0;
    }
    while(q != NULL) {
	count++;
	q=q->next;
    }	
    return count;
}

/*
 * Method to create new node.
 * @arg Arg1: node                                                
 * @return created node               
 */


slink* getnode()
{
    slink *create ;
    create = new slink;
    create->next = NULL ;
    return create;
}

/*
 * Method to remove node with given element.
 * @arg Arg1: head node
 * 	Arg2: element to remove                                                
 * @return head node               
 */


slink* remove(slink *head , int element)
{
    slink *curr = head ;
    slink *adv = curr->next;
    cout << "ELEMENT IS:: " << element << endl;
    while(curr != NULL) {
	if(adv->cID != element) {
	    curr = adv;	
            adv = curr->next;
	}else {
	    curr->next = adv->next;
	    free(adv);	
	    break;	
        }
    }
    
    return head ;
}

/*
 * Method to delete entire list.
 * @arg Arg1: head node                                                
 * @return none               
 */


void destroy(slink* head)
{
    slink *temp = getnode();

    if(head)
    {
        temp = head;
        head = temp->next;
        delete(temp);
        destroy(head);
    }
}

/****************************************************---Linked List Operations End---*******************************************************/

/*
 * Method to get Host Name from IP address.
 * @arg Arg1: IP Address                                                
 * @return Host Name               
 */


string getRegClientPortNo(slink *head , string rIP)
{
    slink *curr = head ;
    while(curr != NULL) {
        if(strcmp(curr->cIP.c_str(), rIP.c_str()) != 0) {
            curr = curr->next;
        }else {
            return curr->cPort;
        }
    }

    return "" ;
}

/*
 * Method to get Host Name from IP address.
 * @arg Arg1: IP Address                                                
 * @return Host Name               
 */


int isExistingClient(slink* head, char* ip)
{
    slink *curr = head;
    curr = curr->next;
    while(curr != NULL) {
        if(strcmp(curr->cIP.c_str(),ip)==0)
            return 1;
        else
            curr = curr->next;
    }
    return 0;
}

/*
 * Method to check valid Connections.
 * @arg Arg1: head
 * 	Arg2: connection id                                                
 * @return 1/0 i.e yes/No               
 */

int isValidConnection (slink* head, int id) 
{
    slink *curr = head ;
    curr = curr->next; 
    while(curr != NULL) {
        if(curr->cID == id) {
	    return 1;
	}
	else
            curr = curr->next;
    }
    return 0;	
}

/*
 * Method to get Host Name from Connection ID.
 * @arg Arg1: head                                                
 * @return hostname               
 */


string getHostNameFromConID (slink* head, int id)
{
    slink *curr = head;
    char* connClient;	
    string sender;	
    while(curr != NULL) {
        if(curr->cID == id) {
            connClient = getHostNameFromIP(const_cast<char*>(curr->cIP.c_str()));
            string sender = string(connClient);
            return sender;
        }
        else
            curr = curr->next;
    }
    return NULL;
}

/*
 * Method to get remove socket from connected sockets array.
 * @arg Arg1: sockets array
 *	Arg2: socket to remove	                                                
 * @return none               
 */

void removeSocket(int* socs, int e) 
{
    for (int p = size; p > 1; p--) {
        if (cSocks[p] == e) {
            for ( int q=p; q <= size; q++)
                cSocks[q] = cSocks[q+1];
	    size--;
            cSocks[size+1] = '\0';
        }
    }

}

/*
 * Method to send list.
 * @arg Arg1: socket fd
 * 	Arg2: list size
 * 	Arg3: head node                                                
 * @return hostname               
 */

void sendList( int soc, int size, slink *lhead) {
    //tringstream convert;
    //nvert << size;
    //ring lsize = convert.str();
    if (send  (soc, &size, sizeof (size), 0) == -1)
        perror("send list size");
    //cout << "SENT SIZE OF LIST:: " << lsize << endl;
    //sleep(1);
    slink* q;
    q = lhead;
    string msg[3];
    if(q == NULL) 
        printf("\n List Empty \n");
     
    while(q != NULL) {
        if(q->next == NULL) {
    	    ostringstream conv1;
            conv1 << q->cID;
            msg[0] = conv1.str();
            msg[1] = q->cIP;
            msg[2] = q->cPort;
            string packet = msg[0] + msg[2] + msg[1];
            int len = packet.length();
            if (send (soc, &len, sizeof( int ), 0) == -1)
		perror("send last packet length");
            if (send (soc, packet.c_str(), len, 0) == -1)
                perror("send last packet");
            break;
        }else {
            ostringstream conv2;
            conv2 << q->cID;
            msg[0] = conv2.str();
            msg[1] = q->cIP;
            msg[2] = q->cPort;
            string packet = msg[0] + msg[2] + msg[1];
            int len = packet.length();
            if (send (soc, &len, sizeof( int ), 0) == -1)
                perror("send packet length");
            if (send (soc, packet.c_str(), len, 0) == -1)
                perror("send packet");
            q = q->next ;
        }
    }

}

/*
 * Method to receive list from server socket.
 * @arg Arg1: socket fd                                                
 * @return none               
 */

void recvList ( int sockfd) 
{
    int numbytes;
    //char buf[MAXDATASIZE]
    char rbuf[MAXDATASIZE];
    int lsize;	
    if ((numbytes = recv(sockfd, &lsize, sizeof (lsize), 0)) == -1) 
        perror("recv");
  
    //int lsize = atoi(buf);
    int recvdBytes=0;   
    int BUFFERSIZE = 24;
    int id;     
    string ip,port;     
    string cpBuf[lsize]; 
    if (head != NULL) {
	destroy(head);
	head = NULL;
    }  
 
    int p=0;
    cout << " Receiving Updated Registered Clients List" << endl;
    cout << " Processing.... " << endl;
 
    while(p<lsize) {
        int len;
        if( numbytes = recv( sockfd, &len, sizeof(int), 0 ) == -1 )
	    perror("recv");
        if( numbytes = recv( sockfd, rbuf, len, 0 ) > 0 ) {
            rbuf[len] = '\0';
	    cpBuf[p] = rbuf;
        }else
            perror("recv list");
	cout.flush();
	p++;
    }
    for (int i =0;i<lsize;i++) {
	string str(cpBuf[i]);
	int regcId = atoi(str.substr (0,1).c_str());
	string regcPort = str.substr (1,4);
	string regcIP = str.substr (5);
	head = insert(head , regcIP,  regcPort, regcId );
    }
    display(head);	
}

/*
 * Method to send file.
 * @arg Arg1: socket fd
 *      Arg2: file name
 *      Arg3: destination host name                                                
 * @return none               
 */

void sendFile(int sock, string fName, string dest, int fd)
{
    int numbytes;
    int EXIT_FLAG;
//    int CHUNK_SIZE=1000;	
    struct stat file_stat;
    struct timeval tv1,tv2;
    double timeTaken;


    /* Get file stats */
    if (fstat(fd, &file_stat) < 0) {
        cout << "File cannot be accessed! Try Again!" << endl;
    }
    int fileSize = file_stat.st_size;
    int fsize = fileSize;
    off_t offset = 0;
    cout << "Sending File..." << endl;
    /* Sending file data */
    gettimeofday (&tv1, NULL);
    while (fileSize > 0) {
        if((numbytes = sendfile(sock, fd, &offset, CHUNKSIZE)) == -1)
	    perror("sendfile");	
        fileSize -= numbytes;
    }
    gettimeofday (&tv2, NULL);
    timeTaken = (tv2.tv_sec - tv1.tv_sec) * 1000.0;      // sec to ms
    timeTaken += (tv2.tv_usec - tv1.tv_usec) / 1000.0;   // us to ms
    double rate = fsize/(timeTaken/1000);
    double rate_mbps = (rate * 8)/1000000;
    cout << "File Sent !" << endl;
    char* host = getHostNameFromIP(myip);
    string sender = string(host);
    cout << "Tx (" << sender << "): " << sender << " -> " << dest << " File Size: " << fsize << " Bytes, Time Taken: " << timeTaken 
							<< " milli seconds, Tx Rate: " << rate_mbps << " Mbits/sec." << flush << endl;
    close(fd);    
}

/*
 * Method to recv file.
 * @arg Arg1: socket fd
 *      Arg2: file name
 *      Arg3: source host name                                                
 * @return none               
 */

void getFile(int sock, string fname, string source) 
{
    int fileSize;
    //int CHUNK_SIZE = 1000;
    char buffer[CHUNKSIZE];	
    int numbytes;
    double timeTaken;
    struct timeval tv1,tv2;
    if (send(sock, FILE_COMM_TOKEN.c_str(), FILE_COMM_TOKEN.length(), 0) == -1)
                        perror("send");	
    /* Receiving file size */
    if( numbytes = recv( sock, &fileSize, sizeof fileSize, 0 ) == -1 )
        perror("recv");
    int fsize = fileSize;	
    FILE *fp = fopen(fname.c_str(), "ab+");
    gettimeofday (&tv1, NULL);
    cout << " Receiving... " << endl;
    while (fileSize > 0) {
	if ((numbytes = recv(sock, buffer, CHUNKSIZE, 0)) == -1)
	    perror("recv");
	fwrite (buffer, sizeof (char), numbytes, fp);
	fileSize -= numbytes;
	//cout << "Receiving... " << numbytes << endl << "Waiting for..." << fileSize << endl;
    }
    gettimeofday (&tv2, NULL);
    // compute and print the elapsed time in millisec
    timeTaken = (tv2.tv_sec - tv1.tv_sec) * 1000.0;      // sec to ms
    timeTaken += (tv2.tv_usec - tv1.tv_usec) / 1000.0;   // us to ms
    double rate = fsize/(timeTaken/1000);	
    double rate_mbps = (rate * 8)/1000000;
    char* host = getHostNameFromIP(myip);
    string recv = string(host);
    cout << "File " << fname << " has been downloaded !" << endl; 
    cout << "Rx (" << recv << "): " << source << " -> " << recv << "File Size: " << fsize << " Bytes, Time Taken: " << timeTaken        
                                                                      << " milli seconds, Rx Rate: " << rate_mbps << " Mbits/sec." << endl;
    fflush(stdout);
    FILE_COMM_TOKEN.clear();
    fclose(fp);
}
	 





