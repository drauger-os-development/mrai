/*
 * aptremove.cxx
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
 * VERSION: 0.1.4-beta3
 */


#include <iostream>
#include <string>
#include "mrai_lib.h"
#include <unistd.h>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <experimental/filesystem>

using namespace std;

int main(int argc, char **argv)
{
	string called_as = argv[0];
	string pass = argv[1];
	bool assume_yes = false;
	if (pass == "1")
	{
		pass = argv[2];
		assume_yes = true;
	}
	vector<string> passv;
	vector<string> * passv_address = &passv;
	if (getuid() != 0)
	{
		error_report(2,called_as,"aptremove.sh has failed with fatal error: Not running with correct permission set.\n");
		return 2;
	}
	string cache = "/etc/mrai";
	try
	{
		if (DoesPathExist(cache + "apt-fast.flag"))
		{
			if (assume_yes)
			{
				split(*passv_address, pass, boost::is_any_of(","));
				system(ConvertToChar("/usr/sbin/apt-fast -y purge " + pass));
			}
			else
			{
				split(*passv_address, pass, boost::is_any_of(","));
				system(ConvertToChar("/usr/sbin/apt-fast purge " + pass));
			}
		}
		else
		{
			if (assume_yes)
			{
				split(*passv_address, pass, boost::is_any_of(","));
				system(ConvertToChar("/usr/bin/apt -y purge " + pass));
			}
			else
			{
				split(*passv_address, pass, boost::is_any_of(","));
				system(ConvertToChar("/usr/bin/apt purge " + pass));
			}
		}
	}
	catch (...)
	{
		error_report(2,called_as,"apt purge has failed. Most likely a configuration bug with an app.");
		return 2;
	}
	try
	{
		string file;
		for (auto & each : passv)
		{
			if (each == "apt-fast")
			{
				file = cache + "/apt-fast.flag";
				const char * DELETE_FILE = ConvertToChar(file);
				remove(DELETE_FILE);
			}
			else if (each == "snapd")
			{
				file = cache + "/snapd.flag";
				const char * DELETE_FILE = ConvertToChar(file);
				remove(DELETE_FILE);
			}
			else if (each == "flatpak")
			{
				file = cache + "/flatpak.flap";
				const char * DELETE_FILE = ConvertToChar(file);
				remove(DELETE_FILE);
			}
			else if (each == "mrai")
			{
				return 0;
			}
		}
	}
	catch (...)
	{
		error_report(1,called_as,"Cannot modify files in " + cache);
	}
	try
	{
		if (assume_yes)
		{
			string convert = "/usr/share/mrai/clean -y ";
			convert.append(argv[3]);
			const char * COMMAND = ConvertToChar(convert);
			system(COMMAND);
		}
		else
		{
			string ans;
			cout << "Would you like mrai to clean up left-over, unused dependencies? [y/N]: " << flush;
			cin >> ans;
			if (ans == "Y" or ans == "y")
			{
				string convert = "/usr/share/mrai/clean ";
				convert.append(argv[2]);
				const char * COMMAND = ConvertToChar(convert);
				system(COMMAND);
			}
		}
	}
	catch (...)
	{
		error_report(2,called_as,"clean.sh has failed");
	}
	return 0;
}

