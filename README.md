# Linux command line chat application

## About

This is a simple chat application to be used on Linux machines. This chat has no gui and instead is compiled, run, and used on the command prompt. At the minimum, this application requires a client and a server. These can be on different machines or the same machine. The files for both the client  and the server implementation are in their respective folders. I made this program near the start of my interest in network programing, and I used a Linux virtual machine on my university's mainframe. Unfortunantly, since graduating, I no longer have access to the mainframe and thus am unable to make changes or improvements to the program. The program will run, but there are some amateurish issues with the design that I should address. So, since I cannot make adjustments, I will use this program as an example of how *not* to design a network oriented program.

## Usage

These files can be compiled using the GNU Compiler Collection (g++) and then run from a Linux machine. However, before compilation, the correct ports as well as IP addresses need to be configured. By defualt, the IP address of the server is set to the loopback address (127.0.0.1). Of course, this will only work if the clients and the host are all on the same machine. This can be changed to the address of a server on a different machine by setting the correct IP of the server on line 102 of client.cpp. Make sure to change the port number on the line directly above line 102 as well. The equivalent line that needs to be changed in server.cpp is line 300.

Once both these files are configured and run, an error message will display if the port could not be opened or a connection could not be made to the server. If neither error thrown, the connection will have been successful and on the client will display a list of possible commands that can be used. From there, it should be a fairly straightforward task to chat with another client.

## Implementation

This is where things get nasty. In order to examine the network coding practices used in the implementation of this project, I will divide the evaluation of the code into three sections: The Good, The Bad, and The Ugly. "The Good" will encompass all network coding practices used in the design of this project which I feel were good design choices. "The Bad" section will cover the inverse and examine all bad design choices regarding network coding practices. "The Ugly" will simply examine all the bad coding practices in general. This project was completed at the middle stage in both my networking and my programming education and as such can be used as an example of poor coding practices that don't necessarily relate to networking specifically.

### The Good

In my implementation of both the client and the server, I am using the sys library in which*sockets are treated as file descriptors*. This is an extremely important concept with examining the following code. Normally, file descriptors can be used to reference a file of course, but they can also be used to reference I/O devices which may have asynchronous behavior. This concept of asynchronous behavior is mirrored when implementing a networking project as packets can be sent and recieved at any given time. When a socket is ready to recieve data, the file descriptor (FD) will be set. When this is detected by either the server or client, the data can be read. For both the client and the server, data is sent using the **send** function which takes a string of characters, and a file descriptor of the appropriate reciever(s).

On the client-side of the implementation, The Good aspects of the design will only need a short discussion. The client begins in **main** by setting the file descriptor of the port used to communicate with the server. Skipping ahead, **main** then hands off functionality to **runserver** (which probably should be named something else), a function which provides the backbone of the client implementation. The **select** function is being used to monitor a list of file FD's. Now, the natural question is, what FD's should be monitored besides the connection port to the server? Well, recall that an FD is also used for I/O device. Therefore, the keyboard is also being monitored in the event that a command is typed or a message is sent. The list of FD's **listenlist** contains only the port FD and the keyboard FD and will set that either of these two are ready to be read from when appropriate. The **runserver** function, is therefore simply a loop monitoring activity from the keyboard or the port. 

The server side implementation functions very similarly to the client. The key difference is that the server makes use of something called a *listening socket*. The purpose of the listening socket is to listen for activity and, if activity is detected, delegate a file descriptor for the connection. The listening socket has its own file descriptor which is set using the **bind** and **listen** functions. The **accept** function, accepts a connection on the listening socket and delegates that connection's own file descriptor. The **doserver** function in the server is the equivalent of the **runserver** function tine the client. The **doserver** function uses a forloop to loop through all FD's and detect activity. If activity is detected from the listening socket, functionality is handed off to the **connection_accept** function. Else, the data is received and examined. 

The alternative to implementing socket connections as ports is to implement a non-blocking server port which is extremely difficult but can be done. 

### The Bad

The major flaw in the design of my implementation is that it parses text in order to execute commands. This makes evaluation of the text extremely cumbersome as there are many string comparisons that need to be done. For example, any username (or no username at all) could potentially appear after the "SEND" command, and therefore numerous string comparisons need to be done to see if there is username present following the command. Also, typos from the user can have disastrous results as well on account of the fact that the entire functionality of both the client and server depend on valid syntax. The alternative is to simply use an application structure. There is a reason why all networking protocols are implemented through encapsulation under a header: structures provide a simple a static way of examining metadata. Using a simple structure composed of say, the sender's username, the reciever's username, and any options would have made my life a lot easier. This structure could be sent as the header of message and then server operations would be a simple task of peeling off the header, examining it, and then proceeding with the given commands. 

Another minor issue that should be pointed out is do not store user information as plaintext. However, since this was just a toy project with no deployment goal in mind, this is less of a serious issue than the previous issue mentioned.

### The Ugly

This is where things get nasty. I'm going to simply list off several of my design choices which should be avoided by all programers. The first is *do not try to stuff your entire program into one file*. The reasoning behind this is easy to see. I can barely determine the functionality in my own program because the code is so bloated. Don't be afraid to split your program up into seperate files, just be sure to implement your header and source files correctly, which brings me to my second point. As a rule of thumb data structures should be defined in the header files, and operations on those data structures should be defined in the source files. As you can see, I have the user structure which includes the username, password, FD, and status, defined in a source file. If it is declared in a header file, then it is visible to the compiler at all times, and thus can be used in different source files as needed. The last major issue with my code is that I am frequently forward declaring variables within my functions. Non-static variables should always be declared an initialized together as this makes the code much more readable and does not waste space on the stack.
