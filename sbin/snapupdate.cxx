/*
 * snapupdate.cxx
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
 *
 * VERSION: 0.1.7-beta2
 */


#include <iostream>
#include <string>
#include "../usr/share/mrai/mrai_lib.h"
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
	string version = "";
	string scripts = "/usr/share/mrai";
	string called_as = argv[0];
	string R = "\033[0;31m";
	string G = "\033[0;32m";
	string NC = "\033[0m";
	string arg1;
	if (argc == 2)
	{
		arg1 = argv[1];
		if (arg1 == "--version" or arg1 == "-v")
		{
			cout << "\n" << version << "\n" << endl;
			return 0;
		}
		else
		{
			cout << "\n" << R << "Option not recognized.\n" << endl;
			return 1;
		}
	}
	if (is_snapd_installed())
	{
		cout << "\n" << G << "Force Updating Snaps. Please Wait . . . " << NC << "\n" << endl;
		try
		{
			/*
			 * We are curently having a code regression due to a segmentation fault the
			 * folling block is causing. So, we will not have a progress spinner this time arround.
			 * Issue should be fixed later. Seems the issue is 'child_pid'
			 * refuses to be tacked onto the end of 'COMMAND' with either append() or just
			 * what we do below
			pid_t child_pid = fork();
			if (getpid() == child_pid)
			{
				system("/usr/bin/snap refresh");
				return 0;
			}
			string COMMAND = "/usr/share/mrai/base-spinner 'Updating Snaps . . . ' " + child_pid;
			//COMMAND.append(child_pid);
			const char * run = ConvertToChar(COMMAND);
			cout << run << endl;
			system(run);
			*
			* Do the following instead so that the damn thing works:
			*/
			system("/usr/bin/snap refresh");
			cout << endl;
		}
		catch (...)
		{
			error_report(2,called_as,"Snap has had an error. Most likely network issues.");
			return 2;
		}
	}
	else
	{
		cout << "\n" << R << "We're sorry. It appears snapd is not installed. Please run { mrai -ia snapd } to install it.\n" << NC << endl;
		return 1;
	}
	return 0;
}

