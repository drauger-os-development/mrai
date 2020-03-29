/*
 * base-spinner.cxx
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


#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include "mrai_lib.hpp"

using namespace std;

int main(int argc, char **argv)
{
	string sp[5] = {"/", "-", "\\", "|"};
	int i = 1;
	string message = argv[1];
	string check_dir = argv[2];
	while (DoesPathExist("/proc/" + check_dir))
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

