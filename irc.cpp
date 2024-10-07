/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:32:55 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/10/07 14:18:27 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>      // For input/output operations
#include <sys/types.h>   // For data types like sockaddr
#include <sys/socket.h>  // For socket APIs (socket, bind, listen, accept, etc.)
#include <netinet/in.h>  // For sockaddr_in structure and INADDR_ANY
#include <arpa/inet.h>   // For htons and other networking functions
#include <fcntl.h>       // For fcntl (to set the socket as non-blocking)
#include <unistd.h>      // For close (to close sockets)
#include <poll.h>        // For poll (monitoring multiple file descriptors)
#include <cstring>       // For memset (initialize data structures)

#define PORT 54000         // Define the port number on which the server will listen for connections.
#define MAX_CLIENTS 100    // The maximum number of clients the server can handle simultaneously.

int main() 
{
    // Step 1: Create a socket to listen for incoming connections.
    // AF_INET: Use IPv4 addressing.
    // SOCK_STREAM: Use a TCP socket (reliable, connection-based communication).
    // 0: Use the default protocol (TCP in this case).
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) // Error check to ensure socket creation succeeded.
    {
        std::cerr << "Error: failed to create the listening socket.\n";
        return -1; // Return error code if socket creation fails.
    }

    // Step 2: Set the socket to non-blocking mode.
    // By default, sockets are blocking, meaning calls like accept() or recv() will wait until an event occurs.
    // Non-blocking mode allows the server to continue running even when no events occur on the socket.
    int flags = fcntl(listenSocket, F_GETFL, 0); // Get the current socket flags.
    if (fcntl(listenSocket, F_SETFL, flags | O_NONBLOCK) == -1) // Add the O_NONBLOCK flag.
    { 
        std::cerr << "Error: failed to set socket to non-blocking mode.\n";
        close(listenSocket); // If setting non-blocking fails, close the socket and exit.
        return -1;
    }

    // Step 3: Define the server address structure.
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr)); // Zero out the structure to avoid undefined values.
    serverAddr.sin_family = AF_INET; // Use IPv4.
    serverAddr.sin_port = htons(PORT); // Convert the port number to network byte order (big-endian).
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to any available network interface (allows connections from any IP).

    // Step 4: Bind the socket to the server address and port.
    // This associates the socket with the specified IP address and port number so it can listen for incoming connections.
    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) 
    {
        std::cerr << "Error: can't bind socket to IP/port.\n";
        close(listenSocket); // Close the socket if the bind fails.
        return -1;
    }

    // Step 5: Set the socket to listen for incoming connections.
    // SOMAXCONN specifies the maximum length for the queue of pending connections.
    if (listen(listenSocket, SOMAXCONN) == -1) 
    {
        std::cerr << "Error: can't listen.\n";
        close(listenSocket); // Close the socket if listening fails.
        return -1;
    }

    // Step 6: Initialize the pollfd array to monitor file descriptors (sockets).
    struct pollfd fds[MAX_CLIENTS];
    memset(fds, 0, sizeof(fds)); // Zero out the array to ensure no undefined values.
    fds[0].fd = listenSocket; // Monitor the listening socket for incoming connections.
    fds[0].events = POLLIN; // Check for POLLIN events (indicating incoming data or a new connection).

    int clientCount = 1; // Start with only the listening socket being monitored.

    // Step 7: Main server loop.
    // This loop will run indefinitely, monitoring events on the sockets using the poll() function.
    while (1) 
    {
        // poll() waits for events on the file descriptors in the fds array.
        // It blocks until at least one file descriptor is ready for the operation (or an error occurs).
        int pollCount = poll(fds, clientCount, -1); // -1 means it waits indefinitely for an event. returns fd of event occured
        if (pollCount == -1) 
        {
            std::cerr << "Error: poll failed.\n";
            break; // Exit the loop if poll fails (likely a critical error).
        }

        // Step 8: Iterate over the file descriptors to check for events.
        for (int i = 0; i < clientCount; i++) 
        {
            // Check if the listening socket is ready (i.e., a new connection request has arrived).
            if (fds[i].fd == listenSocket && (fds[i].revents & POLLIN)) 
            {
                sockaddr_in client; // Structure to hold the client address information.
                socklen_t clientSize = sizeof(client);
                int clientSocket = accept(listenSocket, (sockaddr*)&client, &clientSize);
                if (clientSocket == -1) 
                {
                    std::cerr << "Error: failed to accept client.\n";
                } 
                else 
                {
                    // Set the newly accepted client socket to non-blocking mode.
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

                    // Check if the server has room to accept another client.
                    if (clientCount < MAX_CLIENTS) 
                    {
                        // Add the new client socket to the pollfd array to monitor it.
                        fds[clientCount].fd = clientSocket;
                        fds[clientCount].events = POLLIN; // Monitor for incoming data.
                        clientCount++; // Increase the count of monitored sockets.
                        std::cout << "New client connected (Client " << clientCount - 1 << ").\n";
                    } 
                    else 
                    {
                        // If the server is full, close the client socket to reject the connection.
                        std::cerr << "Error: maximum clients reached.\n";
                        close(clientSocket);
                    }
                }
            }
            // If an existing client socket is ready for reading (indicating incoming data).
            else if (fds[i].revents & POLLIN) 
            {
                char buffer[4096]; // Buffer to hold incoming data from the client.
                memset(buffer, 0, sizeof(buffer)); // Zero out the buffer to avoid undefined values.
                int bytesRecv = recv(fds[i].fd, buffer, sizeof(buffer), 0); // Read data into the buffer.
                // If recv returns 0 or negative, it means the client has disconnected or an error occurred.
                if (bytesRecv <= 0) 
                {
                    close(fds[i].fd); // Close the socket associated with the client.
                    fds[i] = fds[clientCount - 1]; // Replace the closed socket with the last socket in the array.
                    clientCount--; // Decrease the client count as one has disconnected.
                    std::cout << "Client " << i << " disconnected.\n";
                    i--; // Decrement i to recheck the swapped client socket in the next iteration.
                } 
                else 
                {
                    // Print the received message with the client number.
                    std::cout << "Client " << i << ": " << std::string(buffer, bytesRecv) << std::endl;
                    
                    // Echo the received message back to the client.
                    send(fds[i].fd, buffer, bytesRecv, 0);
                }
            }
        }
    }
    // Step 9: Clean up - close all active sockets before exiting the program.
    for (int i = 0; i < clientCount; i++) 
    {
        close(fds[i].fd); // Close each socket in the pollfd array.
    }
    return 0; // End of the program.
}
