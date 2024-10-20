/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mahmoud <mahmoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 21:02:31 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/20 21:03:39 by mahmoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"



// Function to trim whitespace
std::string ft_trim(const std::string &str)
{
    std::size_t start = str.find_first_not_of(" \n\r\t");
    std::size_t end = str.find_last_not_of(" \n\r\t");
    if (start == std::string::npos || end == std::string::npos)
    {
        return "";
    }
    return str.substr(start, end - start + 1);
}

// Function to split strings by a delimiter
std::vector<std::string> ft_split(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::string word;

    for (std::size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] != delimiter)
        {
            word += str[i];
        }
        else
        {
            if (!word.empty())
            {
                result.push_back(word);
                word.clear();
            }
        }
    }
    if (!word.empty())
    {
        result.push_back(word);
    }
    return result;
}

std::string getCurrentDateTime()
{
    // Get the current time
    std::time_t rawTime;
    std::time(&rawTime);

    // Convert to local time
    struct tm *localTime = std::localtime(&rawTime);

    // Buffer to hold the formatted date and time
    char buffer[80];

    // Format: YYYY-MM-DD HH:MM:SS
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localTime);

    return std::string(buffer);
}