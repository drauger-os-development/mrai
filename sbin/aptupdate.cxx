/*
 * aptupdate.cxx
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
 * VERSION: 0.2.1-beta2
 */

#include "../usr/share/mrai/mrai_lib.hpp"

using namespace std;

int main(int argc, char **argv)
{
	string called_as = argv[0];
	string arg1;
	int s = 0;
	int y = 0;
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
			if (arg1 == "-sy" or arg1 == "-ys" or arg1 == "-s")
			{
				s = 1;
			}
			if (arg1 == "-sy" or arg1 == "-ys" or arg1 == "-y")
			{
				y = 1;
			}
		}
	}
	else if (argc > 2)
	{
		cerr << "\n" << R << "aptupdate does not support setting flags seperately. Please use -sy or -ys.\n" << NC << endl;
		return 1;
	}
	cout << "\n" << G << "Updating apt cache . . ." << NC << "\n" << endl;
	try
	{
		system("/usr/bin/apt update");
	}
	catch (...)
	{
		cerr << "\n" << R << "An error has occured. Most likely your computer cannot communicate with one or more servers.\nPlease make sure you are connected to the internet and/or try again later.\n" << NC << endl;
		return 2;
	}
	try
	{
		if (is_aptfast_installed())
		{
			if (y == 0)
			{
				system("/usr/sbin/apt-fast dist-upgrade");
				system("/usr/bin/apt autoremove");
				system("/usr/bin/apt purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')");
			}
			else
			{
				system("/usr/sbin/apt-fast -y dist-upgrade");
				system("/usr/bin/apt -y autoremove");
				system("/usr/bin/apt -y purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')");
			}
		}
		else
		{
			if (y == 0)
			{
				system("/usr/bin/apt dist-upgrade");
				system("/usr/bin/apt autoremove");
				system("/usr/bin/apt purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')");
			}
			else
			{
				system("/usr/bin/apt -y dist-upgrade");
				system("/usr/bin/apt -y autoremove");
				system("/usr/bin/apt -y purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')");
			}
		}
	}
	catch (...)
	{
		system("/usr/bin/dpkg --configure -a");
		error_report(2,called_as,"An error has occured mid-update.\nRecovery has been attempted. Shutting down mrai for safey.");
		return 2;
	}
	if (s == 1)
	{
		system("/usr/bin/pkexec /sbin/snapupdate");
	}
	return 0;
}

