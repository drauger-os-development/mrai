/*
 * mrai.cxx
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


#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>

using namespace std;

int error_report(string error_code, string called_as, string error_message)
{
	string R = "\033[0;31m";
	string NC = "\033[0m";
	cout << R + "\bERROR:" + NC << error_message << endl;
	string scripts = "/usr/share/mrai";
	const char *env_var = "PWD";
	string PWD = getenv(env_var);
	string COMMAND = scripts + "/log-out " + error_code + " /usr/share/mrai/clean " + error_message + " mrai " + PWD + called_as;
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
