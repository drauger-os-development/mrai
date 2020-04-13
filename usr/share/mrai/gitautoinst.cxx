/*
 * gitautoinst.cxx
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
 * VERSION: 0.1.4-beta2
 */


#include <experimental/filesystem>
#include "mrai_lib.hpp"

using namespace std;

int main(int argc, char **argv)
{
	string HOME = argv[1];
	string USER = argv[2];
	string PASS = argv[3];
	string called_as = argv[0];
	chdir(ConvertToChar(gitautocache));
	try
	{
		if ((PASS.find("https://github.com/") != std::string::npos) or (PASS.find("http://github.com/") != std::string::npos) or (PASS.find("git://github.com/") != std::string::npos) or (PASS.find("https://gitlab.com/") != std::string::npos) or (PASS.find("http://gitlab.com/") != std::string::npos) or (PASS.find("git://gitlab.com/") != std::string::npos))
		{
			const char * COMMAND = ConvertToChar("/usr/bin/git clone " + PASS);
			system(COMMAND);
		}
		else if ((PASS.find("github.com/") != std::string::npos) or (PASS.find("gitlab.com/") != std::string::npos))
		{
			const char * COMMAND = ConvertToChar("/usr/bin/git clone https://" + PASS);
			system(COMMAND);
		}
		else
		{
			cout << Y << translate("url_infrencing_error", "", "") << NC << endl;
		return 1;
		}
	}
	catch (...)
	{
		error_report(2,called_as,translate("gitautoinst_clone_failure", "", ""));
		return 2;
	}
	string PATH = GetURLFilePath(PASS);
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
		cout << translate("gitautoinst_makefile_found", "", "") << endl;
		string ans;
		cout << translate("gitautoinst_preconfig_script_prompt", "", "") << " [y/N]: " << flush;
		cin >> ans;
		if (ans == "Y" or ans == "y")
		{
			system("/bin/ls --color=auto");
			string ans2;
			cout << translate("gitautoinst_preconfig_script_selection_prompt", "", "") << flush;
			if (ans2 == "exit" or ans2 == "EXIT")
			{
				cout << "\n" << R << translate("abort", "", "") << NC << endl;
				return 1;
			}
			else if (ans2 != "")
			{
				cout << translate("run_with_root_prompt", "", "") << " [y/N]: " << flush;
				cin >> ans;
				const char * COMMAND = ConvertToChar("/bin/chmod +x " + PATH + ans2);
				try
				{
					system(COMMAND);
				}
				catch (...)
				{
					error_report(2,called_as,translate("gitautoinst_mark_exec_fail", "", ""));
					return 2;
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
						cerr << R << ans2 << translate("failure", "", "") << NC << endl;
						error_report(2,called_as,translate("gitautoinst_preconfig_script_error", "", ""));
						return 2;
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
						cerr << R << ans2 << translate("failure", "", "") << NC << endl;
						error_report(2,called_as,translate("gitautoinst_preconfig_script_error", "", ""));
						return 2;
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
			error_report(1, called_as, translate("gitautoinst_make_depend_error", "", ""));
		}
		try
		{
			system("/usr/bin/make");
		}
		catch (...)
		{
			error_report(2, called_as, translate("gitautoinst_make_failure", "", ""));
			return 2;
		}
		try
		{
			system("/usr/bin/make install");
		}
		catch (...)
		{
			error_report(1, called_as, translate("gitautoinst_make_install_failure", "", ""));
		}
		ofstream flag;
		flag.open(gitautocache + PATH + "/auto.flag",std::ios_base::app);
		flag << "ADDRESS=" + PASS << endl;
	}
	else
	{
		cerr << R << translate("gitautoinst_no_makefile", "", "") << NC << endl;
		remove(ConvertToChar(gitautocache + PATH));
	}
}

