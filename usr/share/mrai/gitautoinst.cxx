/*
 * gitautoinst.cxx
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
 * VERSION: 0.0.9-beta1
 */


#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <experimental/filesystem>
#include <unistd.h>
#include "mrai_lib.h"

using namespace std;

int main(int argc, char **argv)
{
	string HOME = argv[1];
	string USER = argv[2];
	string PASS = argv[3];
	string gitautocache = "/etc/mrai/gitauto";
	string scripts = "/usr/share/mrai";
	string called_as = argv[0];
	string R = "\033[0;31m";
	string NC = "\033[0m";
	string DIR = "/home/" + USER + "/.mrai";
	if (! DoesPathExist(DIR))
	{
		const char * WORKING_DIR = ConvertToChar(DIR);
		mkdir(WORKING_DIR, 01755);
		chown(WORKING_DIR,1000,1000);
	}
	try
	{
		if ((PASS.find("https://github.com/") != std::string::npos) or (PASS.find("http://github.com/") != std::string::npos))
		{
			const char * COMMAND = ConvertToChar("/usr/bin/git clone " + PASS + " " + DIR);
			system(COMMAND);
		}
		else if ((PASS.find("github.com/") != std::string::npos))
		{
			const char * COMMAND = ConvertToChar("/usr/bin/git clone https://" + PASS + " " + DIR);
			system(COMMAND);
		}
		else
		{
			const char * COMMAND = ConvertToChar("/usr/bin/git clone https://github.com/" + PASS + " " + DIR);
			system(COMMAND);
		}
	}
	catch (...)
	{
		error_report("2",called_as,"git clone failed. Either because the provided URL is not GitHub, or because the cloning is being done as root, or there is no internet.");
	}
	string pass1 = GetURLFilePath(PASS);
	string PATH = "/home/" + USER + "/.mrai/" + pass1;
	string PATH2 = "/home/" + USER + "/.mrai";
	string mpath;
	if (DoesPathExist(PATH + "/makefile") or DoesPathExist(PATH + "/Makefile") or DoesPathExist(PATH + "/MAKEFILE"))
	{
		if (DoesPathExist(PATH + "/makefile"))
		{
			mpath =  PATH + "/makefile";
		}
		else if (DoesPathExist(PATH + "/Makefile"))
		{
			mpath = PATH + "/Makefile";
		}
		else if (DoesPathExist(PATH + "/MAKEFILE"))
		{
			mpath = PATH + "/MAKEFILE";
		}
		cout << "\nA Makefile has been detected. Some apps require you to run a seperate file before the Makefile." << endl;
		string ans;
		cout << "Would you like to run such a file? [y/N]: " << flush;
		cin >> ans;
		if (ans == "Y" or ans == "y")
		{
			system("/bin/ls --color=auto");
			string ans2;
			cout << "Which of the above files would you like to run? (Press enter with no input to cancel and continue with running the Makefile. Type 'exit' to exit.): " << flush;
			if (ans2 == "exit" or ans2 == "EXIT")
			{
				cout << "\n" << R << "Aborting . . .\n" << NC << endl;
				return 1;
			}
			else if (ans2 != "")
			{
				cout << "Would you like to run this file as root? [y/N]: " << flush;
				cin >> ans;
				const char * COMMAND = ConvertToChar("/bin/chmod +x " + PATH + ans2);
				try
				{
					system(COMMAND);
				}
				catch (...)
				{
					error_report("2",called_as,"Could not mark file as executable.");
				}
				if (ans == "Y" or ans == "y")
				{
					try
					{
						const char * COMMAND = ConvertToChar(". " + PATH + ans2);
						system(COMMAND);
					}
					catch (...)
					{
						cout << R << ans2 << "Failed" << endl;
						error_report("2",called_as,"Pre-Makefile script failed to run correctly.");
					}
				}
				else
				{
					try
					{
						const char * COMMAND = ConvertToChar("su -c " + USER + " '. " + PATH + ans2 + "'");
						system(COMMAND);
					}
					catch (...)
					{
						cout << R << ans2 << "Failed" << endl;
						error_report("2",called_as,"Pre-Makefile script failed to run correctly.");
					}
				}
			}
		}
		const char * WORKING_PATH = ConvertToChar(PATH);
		chdir(WORKING_PATH);
		try
		{
			system("/usr/bin/make depend");
		}
		catch (...)
		{
			error_report("1",called_as,"make depend not recognized inside Makefile. Continuing . . .");
		}
		try
		{
			system("/usr/bin/make");
		}
		catch (...)
		{
			error_report("2",called_as,"make has failed");
		}
		try
		{
			system("/usr/bin/make install");
		}
		catch (...)
		{
			error_report("1",called_as,"make install has failed");
		}
		try
		{
			string DIR = gitautocache + pass1;
			const char * WORKING_DIR = ConvertToChar(DIR);
			mkdir(WORKING_DIR, 01755);
			chown(WORKING_DIR,1000,1000);
			const char * makefile = ConvertToChar(mpath);
			string contents;
			ifstream ifs(makefile);
			contents.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
			ofstream output;
			output.open(DIR + "Makefile",std::ios_base::app);
			ofstream flag;
			flag.open(DIR + "/auto.flag",std::ios_base::app);
			flag << "ADDRESS=" + PASS << endl;
		}
		catch (...)
		{
			error_report("1",called_as,"Could not modify Makefile. Incorrect file system permissions.");
		}
	}
	else
	{
		const char * UPDIR = ConvertToChar(PATH2);
		chdir(UPDIR);
		try
		{
			const char * REMOVE = ConvertToChar(pass1);
			remove(REMOVE);
		}
		catch (...)
		{
			error_report("2",called_as,"Could not remove directory. Incorrect file system permissions.");
		}
	}
}

