/*
 * mrai_lib.h
 *
 * Copyright 2020 Thomas Castleman <contact@draugeros.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

//import libs
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <regex>

//define global vars
std::string R = "\033[0;31m";
std::string G = "\033[0;32m";
std::string Y = "\033[1;33m";
std::string NC = "\033[0m";
std::string cache = "/etc/mrai";
std::string scripts = "/usr/share/mrai";
std::string gitautocache = "/etc/mrai/gitauto";
std::string gitmancache = "/etc/mrai/gitman";

//define macros
#define elif else if
#define string_list std::vector<std::string>
#define int_list std::vector<int>
#define float_list std::vector<float>
#define bool_list std::vector<bool>
#define sleep(x) usleep(x)

int error_report(int error_code, std::string called_as, std::string error_message)
{
	if (error_code <= 1)
	{
		std::cerr << Y << "WARNING: " << NC << error_message << std::endl;
	}
	else
	{
		std::cerr << R << "ERROR: " << NC << error_message << std::endl;
	}
	std::string scripts = "/usr/share/mrai";
	const char *env_var = "PWD";
	std::string PWD = getenv(env_var);
	std::string COMMAND = scripts + "/log-out ";
	COMMAND += std::to_string(error_code);
	COMMAND = COMMAND + error_message + " mrai " + PWD + called_as;
	int len = COMMAND.length();
	char run[len + 1];
	strcpy(run, COMMAND.c_str());
	system(run);
	return 0;
}

bool DoesPathExist(const std::string &s)
{
  struct stat buffer;
  return (stat (s.c_str(), &buffer) == 0);
}

const char* ConvertToChar(const std::string &s)
{
	const char * WORKING_STRING = s.c_str();
	return WORKING_STRING;
}

std::string GetURLFilePath(const std::string& url) {
    auto last_slash_location = url.rfind('/');
    if(last_slash_location == std::string::npos || last_slash_location + 1 >= url.size()) {
        return "";
    }
    return url.substr(last_slash_location + 1);
}

int ConvertToInt(std::string &convert)
{
	std::stringstream sti(convert);
	int output;
	sti >> output;
	return output;
}

int base_spinner(std::string &message, std::string &PID)
{
	std::string sp[5] = {"/", "-", "\\", "|"};
	int i = 1;
	std::string DIR = "/proc/" + PID;
	while (DoesPathExist(DIR))
	{
		usleep(250000);
		std::cout << "\r\033[K";
		std::cout << "[" << sp[i] << "] " << message << std::flush;;
		if ( i == 3)
		{
			i = 0;
		}
		else
		{
			i++;
		}
	}
	std::cout << "\r\033[K";
	return 0;
}

std::string run(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result += buffer;
        }
    }
    catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int touch(std::string path)
{
	try
	{
		std::ofstream file;
		file.open(ConvertToChar(path), std::ios_base::app);
		file << "" << std::flush;
		file.close();
		return 0;
	}
	catch (...)
	{
		return 2;
	}
}

std::string VectorToString(string_list list)
{
	std::string exit_string;
	for (unsigned int i = 0; i < list.size(); i++)
	{
		if (i == 0)
		{
			exit_string = list[0];
		}
		else
		{
			exit_string = exit_string + " " + list[i];
		}
	}
	return(exit_string);
}

string_list StringToVector(std::string &string_to_convert)
{
	std::istringstream iss(string_to_convert);
	string_list results(std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>());
	return results;
}

std::string grep(std::string search_field, std::string search_pattern)
{
	std::regex rgx(search_pattern);
	std::smatch match;
	const std::string s = search_field;
	std::regex_search(s.begin(), s.end(), match, rgx);
	return match[0];
}

bool Vector_Contains(string_list &vector, std::string search)
{
	for (unsigned int i = 0; i < vector.size(); i++)
	{
		if (vector[i] == search)
		{
			return true;
		}
	}
	return false;
}
