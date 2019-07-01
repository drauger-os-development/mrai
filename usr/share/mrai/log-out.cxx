/*
 * log-out.cxx
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
#include <sys/utsname.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
	if ( argc == 1 )
	{
		cout << "Nothing to log passed on stdin. Exiting . . ." << endl;
		return 127;
	}
	else
	{
		string ERROR_CODE = argv[1];
		string SCRIPT_PATH = argv[2];
		string ERROR_MESSAGE = argv[3];
		string CALL_METHOD = argv[6];
		string ERROR_PWD = argv[5];
		string name = argv[4];
		string LOG_LOCATION = "/tmp/" + name + "/log.txt";
		struct utsname sysinfo;
		uname(&sysinfo);
		string UNAME = sysinfo.release;
		UNAME = "KERNEL_VERSION: " + UNAME;
		auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
		string DATE = ctime(&timenow);
		const char *env_var = "XDG_SESSION_TYPE";
		string DISPLAY_SERVER = getenv(env_var);
		ifstream f("/usr/drauger/os-info.txt");
		string DRAUGER_OS_VERSION;
		if ( f.good() )
		{
			ifstream ifs("/usr/drauger/os-info.txt");
			DRAUGER_OS_VERSION.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
		}
		else
		{
			DRAUGER_OS_VERSION = "NOT A DRAUGER OS INSTALLATION";
		}
		string OUTPUT_LOG = "Date_Error_Occured: " + DATE + UNAME + "\nCurrent_Session: " + DISPLAY_SERVER + "\nDrauger_OS_Version: " + DRAUGER_OS_VERSION + "\nProgram: " + SCRIPT_PATH + "\nError_Code: " + ERROR_CODE + "\nError_Message: " + ERROR_MESSAGE +"\n\n";
		ofstream log;
		cout << OUTPUT_LOG;
		log.open (LOG_LOCATION);
		log << OUTPUT_LOG;
		log.close();
	}
}

