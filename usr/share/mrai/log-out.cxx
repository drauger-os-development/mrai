/*
 * log-out.cxx
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

#include <sys/utsname.h>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <sys/types.h>
#include "mrai_lib.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if ( argc == 1 )
	{
		cout << "Nothing to log passed on stdin. Exiting . . ." << endl;
		return 2;
	}
	string ERROR_CODE = argv[1];
	if ( (ERROR_CODE == "--help") || (ERROR_CODE == "-h") )
	{
		cout << "log-out is used to log important info to files in /tmp. This way they don't build up on a user's system.\nlog-out MUST be passed 6 options:\n\tOption 1: Error Code\n\tOption 2: Path to program\n\tOption 3: Error message (in quotations)\n\tOption 4: Name of package\n\tOption 5: Working directory error occured in.\n\tOption 6: How program was called" << endl;
		return 1;
	}
	else
	{
		string SCRIPT_PATH = "UNKNOWN"; //argv[2];
		string ERROR_MESSAGE = argv[3];
		string CALL_METHOD = argv[6];
		string ERROR_PWD = argv[5];
		string name = argv[4];
		string DIR = "/tmp/" + name;
		string LOG_LOCATION = DIR + "/log.txt";
		const char * LOG_DIR = ConvertToChar(DIR);
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
		string OUTPUT_LOG = "Date_Error_Occured: " + DATE + UNAME + "\nCurrent_Session: " + DISPLAY_SERVER + "\nDrauger_OS_Version: " + DRAUGER_OS_VERSION + "\nProgram: " + SCRIPT_PATH + "\nError_Code: " + ERROR_CODE + "\nError_Message: " + ERROR_MESSAGE +"\n";
		ofstream log;
		log.open (LOG_LOCATION, std::ios_base::app);
		if (mkdir(LOG_DIR, 01777) == -1)
		{
			log << "ERROR: " << strerror(errno) << endl;
		}
		else
		{
			log << "Directory created" << strerror(errno) << endl;
		}
		log << OUTPUT_LOG << endl;
		log.close();
	}
}

