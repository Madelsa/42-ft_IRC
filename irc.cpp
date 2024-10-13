/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:32:55 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/10/13 13:17:30 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <iostream>      // For input/output operations
// #include <sys/types.h>   // For data types like sockaddr
// #include <sys/socket.h>  // For socket APIs (socket, bind, listen, accept, etc.)
// #include <netinet/in.h>  // For sockaddr_in structure and INADDR_ANY
// #include <arpa/inet.h>   // For htons and other networking functions
// #include <fcntl.h>       // For fcntl (to set the socket as non-blocking)
// #include <unistd.h>      // For close (to close sockets)
// #include <poll.h>        // For poll (monitoring multiple file descriptors)
// #include <cstring>       // For memset (initialize data structures)

// #define PORT 54000         // Define the port number on which the server will listen for connections.
// #define MAX_CLIENTS 100    // The maximum number of clients the server can handle simultaneously.

// int main() 
// {
//     // Step 1: Create a socket to listen for incoming connections.
//     // AF_INET: Use IPv4 addressing.
//     // SOCK_STREAM: Use a TCP socket (reliable, connection-based communication).
//     // 0: Use the default protocol (TCP in this case).
//     int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (listenSocket == -1) // Error check to ensure socket creation succeeded.
//     {
//         std::cerr << "Error: failed to create the listening socket.\n";
//         return -1; // Return error code if socket creation fails.
//     }

//     // Step 2: Set the socket to non-blocking mode.
//     // By default, sockets are blocking, meaning calls like accept() or recv() will wait until an event occurs.
//     // Non-blocking mode allows the server to continue running even when no events occur on the socket.
//     int flags = fcntl(listenSocket, F_GETFL, 0); // Get the current socket flags.
//     if (fcntl(listenSocket, F_SETFL, flags | O_NONBLOCK) == -1) // Add the O_NONBLOCK flag.
//     { 
//         std::cerr << "Error: failed to set socket to non-blocking mode.\n";
//         close(listenSocket); // If setting non-blocking fails, close the socket and exit.
//         return -1;
//     }

//     // Step 3: Define the server address structure.
//     sockaddr_in serverAddr;
//     memset(&serverAddr, 0, sizeof(serverAddr)); // Zero out the structure to avoid undefined values.
//     serverAddr.sin_family = AF_INET; // Use IPv4.
//     serverAddr.sin_port = htons(PORT); // Convert the port number to network byte order (big-endian).
//     serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to any available network interface (allows connections from any IP).

//     // Step 4: Bind the socket to the server address and port.
//     // This associates the socket with the specified IP address and port number so it can listen for incoming connections.
//     if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
//     {
//         std::cerr << "Error: can't bind socket to IP/port.\n";
//         close(listenSocket); // Close the socket if the bind fails.
//         return -1;
//     }

//     // Step 5: Set the socket to listen for incoming connections.
//     // SOMAXCONN specifies the maximum length for the queue of pending connections.
//     if (listen(listenSocket, SOMAXCONN) == -1) 
//     {
//         std::cerr << "Error: can't listen.\n";
//         close(listenSocket); // Close the socket if listening fails.
//         return -1;
//     }

//     // Step 6: Initialize the pollfd array to monitor file descriptors (sockets).
//     struct pollfd fds[MAX_CLIENTS];
//     memset(fds, 0, sizeof(fds)); // Zero out the array to ensure no undefined values.
//     fds[0].fd = listenSocket; // Monitor the listening socket for incoming connections.
//     fds[0].events = POLLIN; // Check for POLLIN events (indicating incoming data or a new connection).

//     int clientCount = 1; // Start with only the listening socket being monitored.

//     // Step 7: Main server loop.
//     // This loop will run indefinitely, monitoring events on the sockets using the poll() function.
//     while (1) 
//     {
//         // poll() waits for events on the file descriptors in the fds array.
//         // It blocks until at least one file descriptor is ready for the operation (or an error occurs).
//         int pollCount = poll(fds, clientCount, -1); // -1 means it waits indefinitely for an event. returns fd of event occured
//         if (pollCount == -1) 
//         {
//             std::cerr << "Error: poll failed.\n";
//             break; // Exit the loop if poll fails (likely a critical error).
//         }

//         // Step 8: Iterate over the file descriptors to check for events.
//         for (int i = 0; i < clientCount; i++) 
//         {
//             // Check if the listening socket is ready (i.e., a new connection request has arrived).
//             if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN)) 
//             {
//                 sockaddr_in client; // Structure to hold the client address information.
//                 socklen_t clientSize = sizeof(client);
//                 int clientSocket = accept(listenSocket, (sockaddr*)&client, &clientSize);
//                 if (clientSocket == -1) 
//                 {
//                     std::cerr << "Error: failed to accept client.\n";
//                 } 
//                 else 
//                 {
//                     // Set the newly accepted client socket to non-blocking mode.
//                     fcntl(clientSocket, F_SETFL, O_NONBLOCK);

//                     // Check if the server has room to accept another client.
//                     if (clientCount < MAX_CLIENTS) 
//                     {
//                         // Add the new client socket to the pollfd array to monitor it.
//                         fds[clientCount].fd = clientSocket;
//                         fds[clientCount].events = POLLIN; // Monitor for incoming data.
//                         clientCount++; // Increase the count of monitored sockets.
//                         std::cout << "New client connected (Client " << clientCount - 1 << ").\n";
//                     } 
//                     else 
//                     {
//                         // If the server is full, close the client socket to reject the connection.
//                         std::cerr << "Error: maximum clients reached.\n";
//                         close(clientSocket);
//                     }
//                 }
//             }
//             // If an existing client socket is ready for reading (indicating incoming data).
//             else if (fds[i].revents & POLLIN) 
//             {
//                 char buffer[4096]; // Buffer to hold incoming data from the client.
//                 memset(buffer, 0, sizeof(buffer)); // Zero out the buffer to avoid undefined values.
//                 int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0); // Read data into the buffer.
//                 // If recv returns 0 or negative, it means the client has disconnected or an error occurred.
//                 if (bytesRecv <= 0) 
//                 {
//                     close(fds[i].fd); // Close the socket associated with the client.
//                     fds[i] = fds[clientCount - 1]; // Replace the closed socket with the last socket in the array.
//                     clientCount--; // Decrease the client count as one has disconnected.
//                     std::cout << "Client " << i << " disconnected.\n";
//                     i--; // Decrement i to recheck the swapped client socket in the next iteration.
//                 } 
//                 else 
//                 {
//                     // Print the received message with the client number.
//                     std::cout << "Client " << i << ": " << std::string(buffer, bytesRecv) << std::endl;
                    
//                     // Echo the received message back to the client.
//                     send(fds[i].fd, buffer, bytesRecv, 0);
//                 }
//             }
//         }
//     }
//     // Step 9: Clean up - close all active sockets before exiting the program.
//     for (int i = 0; i < clientCount; i++) 
//     {
//         close(fds[i].fd); // Close each socket in the pollfd array.
//     }
//     return 0; // End of the program.
// }


#include <iostream>
#include <cstring>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <map>

// Maximum number of clients the server can handle simultaneously.
const int MAX_CLIENTS = 100;
// Default IRC port (6667).
const int PORT = 6667;

int main() {
    // Step 1: Create a socket to listen for incoming connections.
    //     AF_INET: Use IPv4 addressing.
    //     SOCK_STREAM: Use a TCP socket (reliable, connection-based communication).
    //     0: Use the default protocol (TCP in this case).
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        std::cerr << "Error: could not create socket.\n";
        return 1;
    }

    // Step 2: Define server address structure (IPv4 and port).
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address
    serverAddr.sin_port = htons(PORT); // Convert port number to network byte order

    // Step 3: Bind the socket to the specified IP and port.
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error: could not bind socket.\n";
        return 1;
    }

    // Step 4: Listen for incoming connections (up to 5 pending connections).
    listen(listenSocket, 5);
    std::cout << "Server listening on port " << PORT << "\n";

    // Step 5: Initialize the pollfd structure to monitor the listen socket and clients.
    struct pollfd fds[MAX_CLIENTS];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = listenSocket; // Add the listen socket to the poll array
    fds[0].events = POLLIN; // Monitor for incoming connections

    // Initialize client count and a map to track client numbers.
    int clientCount = 1; // Start with only the listening socket
    std::map<int, int> clientIndex; // Maps file descriptors to client numbers

    while (true) {
        // Step 6: Poll to monitor multiple file descriptors for events.
        int pollCount = poll(fds, clientCount, -1);
        if (pollCount == -1) {
            std::cerr << "Error: poll failed.\n";
            break;
        }

        // Step 7: Iterate through the file descriptors to handle events.
        for (int i = 0; i < clientCount; i++) {
            // Check if there is a new incoming connection on the listen socket.
            if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN)) {
                sockaddr_in client;
                socklen_t clientSize = sizeof(client);
                int clientSocket = accept(listenSocket, (sockaddr*)&client, &clientSize);
                if (clientSocket == -1) {
                    std::cerr << "Error: failed to accept client.\n";
                } else {
                    // Set the client socket to non-blocking mode.
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
                    if (clientCount < MAX_CLIENTS) {
                        fds[clientCount].fd = clientSocket; // Add the client socket to the poll array
                        fds[clientCount].events = POLLIN; // Monitor for incoming messages
                        clientIndex[clientSocket] = clientCount; // Map the socket to a client number
                        std::cout << "Client " << clientCount << " connected.\n";
                        clientCount++;
                    } else {
                        std::cerr << "Error: maximum clients reached.\n";
                        close(clientSocket);
                    }
                }
            } 
            // Handle incoming data from existing clients.
            else if (fds[i].revents & POLLIN) {
                char buffer[4096];
                memset(buffer, 0, sizeof(buffer));
                int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesRecv <= 0) {
                    // If no data is received or an error occurs, disconnect the client.
                    std::cout << "Client " << clientIndex[fds[i].fd] << " disconnected.\n";
                    close(fds[i].fd);
                    fds[i] = fds[clientCount - 1]; // Replace with the last client
                    clientIndex[fds[i].fd] = clientIndex[fds[clientCount - 1].fd]; // Update the map
                    clientIndex.erase(fds[clientCount - 1].fd); // Remove the disconnected client
                    clientCount--;
                    i--; // Adjust the loop index
                } else {
                    // Convert the received data to a string for easier processing.
                    std::string message(buffer, bytesRecv);
                    int clientNum = clientIndex[fds[i].fd];
                    std::cout << "Client " << clientNum << ": " << message << std::endl;

                    // Step 8: Respond to PING messages to keep the connection alive.
                    if (message.substr(0, 4) == "PING") {
                        std::string pongReply = "PONG " + message.substr(5) + "\r\n";
                        send(fds[i].fd, pongReply.c_str(), pongReply.size(), 0);
                    } 
                    // Step 9: Handle CAP LS request from the client.
                    else if (message.substr(0, 6) == "CAP LS") 
                    {
                        // Respond to CAP LS with a basic message indicating no server capabilities.
                        std::string capReply = ":server 005 * :No server capabilities\r\n";
                        send(fds[i].fd, capReply.c_str(), capReply.size(), 0);
                    }
                }
            }
        }
    }

    // Step 10: Clean up and close the listening socket before exiting.
    close(listenSocket);
    return 0;
}
