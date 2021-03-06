/*
 * clean.cxx
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
 //VERSION: 0.1.7-beta2

#include <cstdlib>
#include <sys/types.h>
#include <experimental/filesystem>
#include "mrai_lib.hpp"


using namespace std;

int main(int argc, char **argv)
{
	string called_as = argv[0];
	int y;
	string check;
	if (argv[2])
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
		elif ( check == "")
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
	cout << "\n" << G << translate("clean_apt_clean", "", "") << NC << "\n" << endl;
	try
	{
		system("/usr/bin/apt clean");
	}
	catch (...)
	{
		error_report(2, called_as, translate("clean_apt_clean_failed", "", ""));
		return 2;
	}
	if ( y == 1)
	{
		cout << "\n" << G << translate("clean_autoremove", "", "") << NC << "\n" << endl;
		try
		{
			system("/usr/bin/apt -y autoremove --purge");
		}
		catch (...)
		{
			error_report(2, called_as, translate("clean_autoremove_error", "", ""));
			return 2;
		}
	}
	elif (y == 0)
	{
		cout << "\n" << G << translate("clean_autoremove", "", "") << NC << "\n" << endl;
		try
		{
			system("/usr/bin/apt autoremove --purge");
		}
		catch (...)
		{
			error_report(2, called_as, translate("clean_autoremove_error", "", ""));
			return 2;
		}
	}
	else
	{
		return 2;
	}
	cout << "\n" << G << translate("clean_git_clean", "", "") << NC << "\n" << endl;
	try
	{
		string DIR = "/home/" + user + "/.mrai";
		const char * WORKING_DIR = ConvertToChar(DIR);
		remove(WORKING_DIR);
		mkdir(WORKING_DIR, 01755);
		chown(WORKING_DIR,1000,1000);
	}
	catch (...)
	{
		error_report(2, called_as, translate("clean_git_error", "", ""));
		return 2;
	}
	cout << "\n" << G << translate("clean_done", "", "") << NC << "\n" << endl;
	return 0;
}

