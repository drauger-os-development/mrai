/*
 * clean.cxx
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
 //VERSION: 0.1.1-beta2

#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <experimental/filesystem>
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

int main(int argc, char **argv)
{
	string scripts = "/usr/share/mrai";
	string called_as = argv[0];
	string R = "\033[0;31m";
	string G = "\033[0;32m";
	string NC = "\033[0m";
	int y;
	string check;
	if ( argv[2])
	{
		check = argv[1];
	}
	else
	{
		check = "";
	}
	string user;
	int * y_location = &y;
	try
	{
		if ( check == "y" )
		{
			*y_location = 1;
		}
		else if ( check == "")
		{
			*y_location = 0;
		}
		else
		{
			return 1;
		}
	}
	catch (...)
	{
		*y_location = 0;
	}
	if ( y == 1 )
	{
		user = argv[2];
	}
	else
	{
		user = argv[1];
	}
	cout << "\n" << G << "Deleting old *.deb packages . . . " << NC << "\n" << endl;
	try
	{
		system("/usr/bin/apt clean");
	}
	catch (...)
	{
		error_report("2",called_as,"apt clean has failed. Most likly due to file system permission issues.");
		return 2;
	}
	if ( y == 1)
	{
		cout << "\n" << G << "Removing old and out dated dependencies . . . " << NC << "\n" << endl;
		try
		{
			system("/usr/bin/apt -y autoremove");
		}
		catch (...)
		{
			error_report("2",called_as,"apt autoremove has failed. Most likly due to app configuration issues.");
		}
		cout << "\n" << G << "Deleting old config files . . . " << NC << "\n" << endl;
		try
		{
			system("/usr/bin/apt -y purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')");
		}
		catch (...)
		{
			error_report("2",called_as,"Config file clean up has failed. Most likely due to app config issues.");
		}
	}
	else if (y == 0)
	{
		cout << "\n" << G << "Removing old and out dated dependencies . . . " << NC << "\n" << endl;
		try
		{
			system("/usr/bin/apt autoremove");
		}
		catch (...)
		{
			error_report("2",called_as,"apt autoremove has failed. Most likly due to app configuration issues.");
		}
		cout << "\n" << G << "Deleting old config files . . . " << NC << "\n" << endl;
		try
		{
			system("/usr/bin/apt purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')");
		}
		catch (...)
		{
			error_report("2",called_as,"Config file clean up has failed. Most likely due to app config issues.");
		}
	}
	else
	{
		return 2;
	}
	cout << "\n" << G << "Cleaning up old GitHub files . . ." << NC << "\n" << endl;
	try
	{
		string DIR = "/home/" + user + "/.mrai";
		int len = DIR.length();
		char WORKING_DIR[len + 1];
		strcpy(WORKING_DIR, DIR.c_str());
		remove(WORKING_DIR);
		mkdir(WORKING_DIR, 01755);
		chown(WORKING_DIR,1000,1000);
	}
	catch (...)
	{
		error_report("2",called_as,"GitHub clean up failed. Most likly due to incorrect file system permissions");
	}
	cout << "\n" << G << "Clean up complete." << NC << "\n" << endl;
	return 0;
}

