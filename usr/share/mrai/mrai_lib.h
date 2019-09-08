/*
 * mrai_lib.h
 *
 * Copyright 2019 Thomas Castleman <contact@draugeros.org>
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
#define string_list vector<string>
#define integer_list vector<int>
#define floats_list vector<float>


using namespace std;

int error_report(string error_code, string called_as, string error_message)
{
	string R = "\033[0;31m";
	string NC = "\033[0m";
	cout << R + "\bERROR:" + NC << error_message << endl;
	string scripts = "/usr/share/mrai";
	const char *env_var = "PWD";
	string PWD = getenv(env_var);
	string COMMAND = scripts + "/log-out " + error_code + error_message + " mrai " + PWD + called_as;
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

int ConvertToInt(string &convert)
{
	stringstream sti(convert);
	int output;
	sti >> output;
	return output;
}

int base_spinner(string &message, string &PID)
{
	string sp[5] = {"/", "-", "\\", "|"};
	int i = 1;
	string DIR = "/proc/" + PID;
	while (DoesPathExist(DIR))
	{
		usleep(250000);
		cout << "\r\033[K";
		cout << "[" << sp[i] << "] " << message << flush;;
		if ( i == 3)
		{
			i = 0;
		}
		else
		{
			i++;
		}
	}
	cout << "\r\033[K";
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

int touch(string path)
{
	try
	{
		ofstream file;
		file.open(ConvertToChar(path), std::ios_base::app);
		file << "" << flush;
		file.close();
		return 0;
	}
	catch (...)
	{
		return 2;
	}
}
