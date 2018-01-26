Sojern Code Challenge
=====================
Author: Sarath


Note: For some reason I couldn't push this project into BitBucket via Eclipse. Please don't mind the gruelling setup instructions. 

Requirements
1. Tomcat Server v7
2. Jersey Framework v1.18


Eclipse Project Setup Steps
-----------------------------

1. Create Dynamic Web Project with
	->   project name  - 'rest.chandu.jersey.web'
	->   Target runtime - 'Apache Tomcat v7.0'
	->   Dynamic web module version - 2.5

2. create Server class
	-> package - rest.chandu.jersey.web
	-> class name - Server
3. Copy Server.java in sojern_challenge repo to here

4. Download Jersey 1.18 jars and copy them into WEB-INF->lib folder

5. create Test class
	-> package - rest.chandu.jersey.web.client
	-> class name - Test

6. copy client/test.java code in sojern_challenge repo to this class

7. create SingletonStore class
	-> package - rest.chandu.jersey.web
	-> class name - SingletonStore

8. copy SingletonStore.java code in sojern_challenge repo to this class

9. copy conf/web.xml code in sojern_challenge repo into WEB-INF->web.xml


Notes for SingletonStore.java
-----------------
1. To add some state and persistence to the previously existing records I choose to insert the records on to a TEXT file. 
2. When this application starts, it checks if there are records on the backup text file and loads them
3. Simple Hash Table is implemented to do comparisions, retreivals of key-value pairs. 
  	


Execution Instructions:
---------------------

1. Select Server.java and Run-As Server and choose Tomcat v-7.
2. Select Test.java and Run-As Java Application.
3. Test values are inserted using the format, "kx-vy", where x and y are random numbers, when using the set endpoint
    described below.
4. Listed below are the commands to be issued on Test.java console,
	* /get/<single-key> 		// Get single value.
	* /multiget/k1/k2/k3	    // Get multiple keys and values as Json format.
	* /set						// Post a json dictionary.
	* /exit 					// Quit.

