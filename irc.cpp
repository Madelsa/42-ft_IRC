/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:32:55 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/09/30 14:52:20 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

int main()
{
    // Create a socket
    // AF_INET: Use IPv4 address family
    // SOCK_STREAM: Use TCP for reliable, connection-based communication
    // 0: Automatically choose the appropriate protocol (TCP in this case)
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket creation was successful. -1 indicates an error.
    if (listenSocket == - 1)
    {
        std::cout << "Error: failed to create the listening socket.\n";
        return -1;    
    }

    // Define a hint structure to hold the address and port information for binding
    sockaddr_in hint;
    hint.sin_family = AF_INET;           // Use IPv4
    hint.sin_port = htons(54000);        // Port number to bind the socket to (54000). range from 0 to 65535
                                         // htons() (Host TO Network Short) converts the port number to network byte order

    // Set the IP address for binding
    // inet_pton() converts a string IP address ("0.0.0.0") to binary form
    // INADDR_ANY (or "0.0.0.0") means the socket will bind to all available network interfaces on the machine. (Ethernet, WIFI, LocalHost, etc...)
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);  //inet_pton() (Pointer TO Number) Convert and store the IP address in the hint structure

    // Bind the socket to the specified IP and port
    // bind() associates the socket with the IP address and port number specified in the 'hint' structure
    if (bind(listenSocket, (sockaddr *)&hint, sizeof(hint)) == -1)
    {
        std::cout << "Error: can't bind socket to IP/port.\n";
        return -1;
    }

    // Listen for incoming connections on the socket
    // SOMAXCONN: Maximum queue of pending connections (system-defined maximum)
    if (listen(listenSocket, SOMAXCONN) == -1)
    {
        std::cout << "Error: can't listen.\n";
        return -1;
    }

    // Prepare to accept a connection
    sockaddr_in client;                     // Structure to store information about the connected client
    socklen_t clientSize = sizeof(client);  // Size of the client sockaddr_in structure

    // Buffers to store client information (host name and service)
    char host[NI_MAXHOST];  // Buffer for the client's hostname (NI_MAXHOST defines max host length)
    char svc[NI_MAXSERV];   // Buffer for the service (e.g., port number) the client is connected on

    // Accept an incoming connection
    // accept() blocks and waits for a connection. It returns a new socket for communication with the client.
    // listenSocket remains open and can accept more connections.
    int clientSocket = accept(listenSocket, (sockaddr *)&client, &clientSize);

    // Check if accept() succeeded. -1 indicates an error (e.g., no client connection).
    if (clientSocket == -1)
    {
        std::cout << "Error: client failed to connect.\n";
        return -1;
    }

    // Close the listening socket as we no longer need it (we're communicating with the client now).
    close(listenSocket);
    
    // Initialize host and service buffers to zero using memset
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);
    
    // Attempt to get the client's hostname and service information
    int result = getnameinfo((sockaddr *)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result)// If successful, print the client's hostname and service (port)
    {
        std::cout << host << " connected on " << svc << std::endl;
    }
    else // If getnameinfo fails, fallback to using IP address and port
    {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST); // Convert the IP address from binary to text
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl; // Print the IP and port  
    }

    // Buffer to store the received data from the client
    char buff[4096];

    // Enter an infinite loop to keep receiving data from the client
    while (1)
    {
        // Clear the buffer using memset to ensure it doesn't carry over any old data
        memset(buff, 0, 4096);

         // Receive data from the client and store it in the buffer
        int bytesRecv = recv(clientSocket, buff, 4096, 0);
        if (bytesRecv == -1) // Check if an error occurred during receiving
        {
            std::cout << "Error: An issue occured.\n";
            break ;
        }
        if (bytesRecv == 0) // Check if the client has disconnected (0 bytes received)
        {
            std::cout << "Client disconnected.\n";
            break ;
        }
        // Print the received message (convert to a std::string for better handling of the data)
        std::cout << "Received: " << std::string(buff, 0, bytesRecv) << std::endl;
        // Echo the received message back to the client
        send(clientSocket, buff, bytesRecv + 1, 0);  // Send back the received data plus the null terminator
    }
    close(clientSocket);
}
