/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 13:36:54 by mabdelsa          #+#    #+#             */
/*   Updated: 2024/10/21 15:34:37 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Utils.hpp"
#include "Server.hpp"

// Sends the Message of the Day (MOTD) to the client after they register.
// This function checks if the client is registered and then proceeds to read 
// the MOTD from a file. It constructs various welcome messages and sends them 
// to the client, providing information about the server and its status.
//
// The function starts by creating a string stream for building messages and 
// initializing an input file stream to read the MOTD from a text file. If the 
// client is not registered, the function exits early. It opens the MOTD file 
// and sends a series of welcome messages to the client's server replies. 
// If the MOTD file is successfully opened, it reads each line and sends it as 
// part of the MOTD to the client. If the file cannot be opened, an error message 
// is sent instead. Finally, the end of the MOTD is indicated, and a flag is set 
// to mark that the welcome message has been sent.
void Client::MOTD(Client *client, std::string serverStartTime) {
    std::ostringstream oss;  // String stream for building messages
    std::string line;        // Line buffer for reading from the MOTD file
    std::ifstream infile;    // Input file stream for reading the MOTD file

    // Check if the client is registered; if not, exit the function
    if (!client->getIsRegistered())
        return;

    // Open the MOTD file for reading
    infile.open("./MOTD.txt", std::ios::in);
    
    // Add welcome messages to the server replies for the client
    // RPL_WELCOME: Sent when the client connects successfully, greeting them.
    client->getServerReplies().push_back(RPL_WELCOME(idFormat(client->getNickname(), client->getUsername()), client->getNickname()));

    // RPL_YOURHOST: Provides the client with the server's name and version.
    client->getServerReplies().push_back(RPL_YOURHOST(client->getUsername(), "ircserv", "1"));

    // RPL_CREATED: Indicates when the server was created, includes the server start time.
    client->getServerReplies().push_back(RPL_CREATED(client->getUsername(), serverStartTime));

    // RPL_MYINFO: Provides information about the server, including capabilities.
    client->getServerReplies().push_back(RPL_MYINFO(client->getUsername(), "irssi", "1", "", "", ""));

    // RPL_ISUPPORT: Informs the client of specific server capabilities and features.
    client->getServerReplies().push_back(RPL_ISUPPORT(client->getUsername(), "No features "));

    // RPL_MOTDSTART: Indicates the start of the MOTD, signaling upcoming messages.
    client->getServerReplies().push_back(RPL_MOTDSTART(client->getUsername(), std::string("ircserv")));

    // Check if the MOTD file is open and readable
    if (infile.is_open()) {
        // Read the MOTD file line by line
        while (std::getline(infile, line)) {
            // Add each line of the MOTD to the server replies
            client->getServerReplies().push_back(RPL_MOTD(std::string("ircserv"), line));
        }
        infile.close(); // Close the file after reading
    } else {
        // If the MOTD file cannot be opened, send an error message
        client->getServerReplies().push_back(ERR_NOMOTD(std::string("ircserv")));
    }
    
    // Add the end of MOTD message to the server replies
    client->getServerReplies().push_back(RPL_ENDOFMOTD(std::string("ircserv")));
    client->setHasSentWelcomeMessage(true); // Mark that the welcome message has been sent
    return; // Exit the function
}


// Handles the NICK command sent by the client to change their nickname.
// This function checks the parameters provided for the nickname command 
// and validates the new nickname according to IRC rules. It ensures that 
// a nickname is provided, does not start with a digit, and is within the 
// maximum length of 9 characters. It also checks for invalid characters 
// and verifies that the nickname is not already in use.
//
// If the client already has a nickname, it updates the nickname and 
// sends a notification to the client about the change. If the client 
// does not have a nickname yet, it sets the new nickname and adds it 
// to the list of active nicknames. The function interacts with the 
// server's list of nicknames to ensure uniqueness and validity.
void Client::handleNickCommand(Client *client, const std::vector<std::string> &params, std::vector<std::string>& nicknames) {
    // Check if there are parameters for the nickname command
    if (params.size() < 1) {
        client->getServerReplies().push_back(ERR_NONICKNAMEGIVEN(std::string("ircserv"))); // No nickname given
        return; // Exit if no nickname is provided
    }

    std::string nickName = params[0]; // Get the provided nickname

    // Check if the nickname starts with a digit
    if (!nickName.empty() && isdigit(nickName[0])) {
        client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserv"), nickName)); // Invalid nickname
        return; // Exit if the nickname is invalid
    }

    // Check if the nickname exceeds the maximum length of 9 characters
    if (nickName.length() > 9) {
        client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserv"), nickName)); // Invalid nickname
        return; // Exit if the nickname is too long
    }

    // Check for invalid characters in the nickname
    for (std::string::iterator it = nickName.begin(); it != nickName.end(); ++it) {
        if (!isalnum(*it) && *it != '[' && *it != ']' && *it != '{' && *it != '}'
            && *it != '|' && *it != '-' && *it != '_') {
            client->getServerReplies().push_back(ERR_ERRONEUSNICKNAME(std::string("ircserv"), nickName)); // Invalid nickname
            return; // Exit if the nickname contains invalid characters
        }
    }

    // Check if the nickname is already in use
    if (!nicknames.empty() && std::find(nicknames.begin(), nicknames.end(), nickName) != nicknames.end()) {
        client->getServerReplies().push_back(ERR_NICKNAMEINUSE(std::string("ircserv"), nickName)); // Nickname already in use
        return; // Exit if the nickname is taken
    }

    // If the client already has a nickname, update it
    if (!client->getNickname().empty()) {
        // Remove the old nickname from the list of nicknames
        nicknames.erase(std::remove(nicknames.begin(), nicknames.end(), client->getNickname()), nicknames.end());
        client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName)); // Notify about the nickname change
        client->setNickname(nickName); // Set the new nickname
        nicknames.push_back(nickName); // Add the new nickname to the list
        return; // Exit the function
    }

    // If the client does not have a nickname yet, set it
    client->setNickname(nickName); // Set the new nickname
    nicknames.push_back(nickName); // Add the new nickname to the list
    client->getServerReplies().push_back(RPL_NICK(client->getNickname(), client->getUsername(), nickName)); // Notify about the new nickname
}


// Processes the USER command sent by the client to set their username and real name.
// This function checks if the client has provided enough parameters for the USER command. 
// If the parameters are insufficient, an error message is sent back to the client. 
// If there are enough parameters, the function sets the client's username and real name 
// using the provided parameters. It then checks if the password has been successfully 
// sent, and if so, sends a success message indicating that the client has been registered 
// successfully, and marks the client as registered in the server.
void Client::handleUserCommand(Client *client, const std::vector<std::string> &params) {
    // Check if there are enough parameters for the USER command
    if (params.size() < 3) {
        client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserv"), "USER")); // Not enough parameters
        return; // Exit if parameters are insufficient
    }

    // Set the client's username and real name from the provided parameters
    client->setUsername(params[0]);
    client->setRealName(params[2]);

    // Check if the password has been successfully sent
    if (client->getHasSentPass()) {
        // Prepare a success message indicating successful registration
        std::string successMessage = "USER " + client->getUsername() + " registered successfully!\r\n";
        client->getServerReplies().push_back(successMessage); // Add the success message to the server replies
        client->setIsRegistered(true); // Mark the client as registered
    }
}


// Handles the CAP command for capabilities negotiation with the client.
// This function checks if the client is sending the CAP LS command to list the 
// available capabilities. If so, it sets the appropriate registration step for 
// capabilities and sends the capability list to the client. If the client is in 
// the right registration step, it handles additional capability requests, 
// acknowledging or denying capabilities as necessary based on the parameters 
// provided. If no valid parameters are given, an error message is sent.
void Client::handleCapCommand(Client *client, const std::vector<std::string> &params) {
    // Check if the client is sending the CAP LS command
    if (params.size() > 0 && params[0] == "LS") {

        client->setRegisterSteps(0, true); // Set the registration step for capabilities
        client->getServerReplies().push_back(":ircserv CAP * LS : \r\n"); // Send the capability list to the client
    } 
    // Check if the client is in the appropriate registration step for capabilities
    else if (client->getRegisterSteps(0)) {
        // Handle capability requests
        if (params.size() == 2 && params[0] == "REQ") {
            // Example: Handle requested capabilities
            // Store the requested capabilities, assuming you have a vector or set in the Client class
            client->addRequestedCapability(params[1]); // Store requested capability
            client->getServerReplies().push_back(":ircserv CAP * REQ : " + params[1] + "\r\n"); // Acknowledge capability request
        } 
        else if (params.size() == 2 && params[0] == "NAK") {
            client->getServerReplies().push_back(":ircserv CAP * NAK : " + params[1] + "\r\n"); // Notify about capability denial
        } 
        else if (params.size() == 2 && params[0] == "ACK") {
            client->getServerReplies().push_back(":ircserv CAP * ACK : " + params[1] + "\r\n"); // Acknowledge received capabilities
        } 
    }
}

void Client::handlePassCommand(Client *client, const std::vector<std::string> &params, const std::string &expectedPassword) {   
    // Check if the client sent exactly one parameter for the PASS command
    if (params.size() != 1) {
        client->getServerReplies().push_back(ERR_NEEDMOREPARAMS(std::string("ircserv"), "PART")); // Insufficient parameters
        return; // Exit if the parameters are not correct
    }

    const std::string &passwordAttempt = params[0]; // Get the password attempt from parameters
    // Check if the provided password matches the expected password
    if (passwordAttempt == expectedPassword) {
        // Prepare a success message indicating the password was accepted
        std::string successMessage = "Password accepted\r\n";
        client->getServerReplies().push_back(successMessage); // Add the success message to the server replies
        client->setHasSentPass(true); // Mark that the password has been sent successfully
    }
}