/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mabdelsa <mabdelsa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 21:06:48 by mahmoud           #+#    #+#             */
/*   Updated: 2024/10/21 13:55:07 by mabdelsa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "Server.hpp"

// Function to trim whitespace from the start and end of a string
std::string ft_trim(const std::string &str);

// Function to split a string by a given delimiter
std::vector<std::string> ft_split(std::string str, char delimiter);


#endif // UTILS_HPP
