/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 14:26:54 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/21 15:35:07 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

int main(int argc, char *argv[])
{
    // Check for correct number of command-line arguments
    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    // Convert the port argument to an integer
    int port = std::atoi(argv[1]);
    // Validate the port number
    if (port <= 0 || port > 65535)
    {
        std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535).\n";
        return 1;
    }

    // Get the server password from command-line arguments
    std::string serverPassword = argv[2];

    try
    {
        // Create an instance of the Server class
        Server server(port, serverPassword);
        // Start the server
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
