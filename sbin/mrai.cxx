/*
 * mrai.cxx
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
 * first off, mrai should NOT be run as root. So, check for that and kill mrai if it is running as root, before it can do any damage
 * we want todo this first, in case there are any bugs in the code below
 */


#include <iostream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <experimental/filesystem>
#include <unistd.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <regex>
#include "../usr/share/mrai/mrai_lib.h"

using namespace std;

//start defining functions

bool is_snapd_installed()
{
	return DoesPathExist(cache + "/snapd.flag");
}

bool is_aptfast_installed()
{
	return DoesPathExist(cache + "/apt-fast.flag");
}

bool is_flatpak_installed()
{
	return DoesPathExist(cache + "/flatpak.flag");
}

void unlock()
{
	remove("/tmp/.mrai.lock");
}

string_list gitautolist(string &called_as)
{
	try
	{
		string list = run(ConvertToChar("/bin/ls --group-directories-first \"" + gitautocache + "\""));
		return StringToVector(list);
	}
	catch (...)
 	{
		error_report(2,called_as,"Function gitautolist failed. File system permissions are messed up in " + gitautocache);
		string null = "NULL not able to get list";
		return StringToVector(null);
	}
}

string_list gitmanlist(string &called_as)
{
	try
	{
		string list = run(ConvertToChar("/bin/ls --group-directories-first \"" + gitmancache + "\""));
		return StringToVector(list);
	}
	catch (...)
 	{
		error_report(2,called_as,"Function gitautolist failed. File system permissions are messed up in " + gitmancache);
		string null = "NULL not able to get list";
		return StringToVector(null);
	}
}

int snapupdate()
{
		try
		{
			system("/sbin/snapupdate");
			return 0;
		}
		catch (...)
		{
			cerr << "\n" << R << "snapupdate failed. " << "\033[0m" << "Please fill out an issue report on our GitHub at\nhttps://github.com/drauger-os-development/mrai/issues\n" << endl;
			return(2);
		}
}

int aptinstall (string_list passed, bool assume_yes)
{
	string_list install_vector;
	string search_string;
	for (unsigned int i = 0; i < passed.size(); i++)
	{
		search_string = run(ConvertToChar("/usr/bin/dpkg -l " + passed[i] + " 2>/dev/null"));
		if (search_string.find("ii	" + passed[i])  != std::string::npos)
		{
			continue;
		}
		else
		{
			install_vector.insert(install_vector.end(),passed[i]);
		}
	}
	if ( ! (install_vector.size() >= 0) )
	{
		cout << "\n" << Y << "All passed packages are already installed.\n" << NC << endl;
		return(1);
	}
	else
	{
		string install_string = VectorToString(install_vector);
		if (is_aptfast_installed())
		{
			try
			{
				if (assume_yes)
				{
					system(ConvertToChar("/usr/bin/pkexec /usr/sbin/apt-fast -y install " + install_string));
				}
				else
				{
					system(ConvertToChar("/usr/bin/pkexec /usr/sbin/apt-fast install " + install_string));
				}
				for (unsigned int i = 0; i < install_vector.size(); i++)
				{
					if (install_vector[i] == "flatpak")
					{
						touch(cache + "/flatpak.flag");
						system("/usr/bin/flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo");
					}
					elif (install_vector[i] == "snapd")
					{
						touch(cache + "/snapd.flag");
					}
				}
			}
			catch (...)
			{
				return 2;
			}
		}
		else
		{
			try
			{
				if (assume_yes)
				{
					system(ConvertToChar("/usr/bin/pkexec /usr/bin/apt -y install " + install_string));
				}
				else
				{
					system(ConvertToChar("/usr/bin/pkexec /usr/bin/apt install " + install_string));
				}
				for (unsigned int i = 0; i < install_vector.size(); i++)
				{
					if (install_vector[i] == "flatpak")
					{
						touch(cache + "/flatpak.flag");
						system("/usr/bin/flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo");
					}
					elif (install_vector[i] == "snapd")
					{
						touch(cache + "/snapd.flag");
					}
					elif (install_vector[i] == "apt-fast")
					{
						touch(cache + "/apt-fast.flag");
					}
				}
			}
			catch (...)
			{
				return 2;
			}
		}
	}
	return 0;
}

int gitautoinst(string &user, string &pass)
{
	try
	{
		system(ConvertToChar("/usr/bin/pkexec " + scripts + "/gitautoinst " + getenv("HOME") + " " + user + " " + pass));
		return 0;
	}
	catch (...)
	{
		cerr << "\n" << R << "ERROR: " << NC << "gitautoinst has failed. Please fill out an issue report\non our GitHub at https://github.com/drauger-os-development/mrai/issues\n" << endl;
		return 1;
	}
}

int flatinstall(string &arg1, string &arg2, string_list &package_vector, bool assume_yes, string called_as)
{
	if (is_flatpak_installed())
	{
		string package_string = VectorToString(package_vector);
		string remotes;
		for (unsigned int i = 0; i < package_vector.size(); i++)
		{
			try
			{
				while (true)
				{
					string package = run(ConvertToChar("/usr/bin/flatpak search \"" + package_vector[i] + "\""));
					if ((package.find("No matches found")  != std::string::npos) or (package == "") or (package == " "))
					{
						remotes = run(ConvertToChar("/usr/bin/flatpak remotes"));
						if (package.find("flathub") != std::string::npos)
						{
							cerr << "\n" << Y << "No installation candidates found." << NC << "\n" << endl;
							return 1;
						}
						else
						{
							system("/usr/bin/flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo");
						}
					}
					else
					{
						throw 1;
					}
				}
			}
			catch (...)
			{
				continue;
			}
		}
		string domain_list_string;
		string_list domain_list;
		ifstream domain_conf_file("/etc/mrai/domain_list.conf");
		domain_list_string.assign((istreambuf_iterator<char>(domain_conf_file)),(istreambuf_iterator<char>()));
		domain_conf_file.close();
		split(domain_list, domain_list_string, boost::is_any_of(","));
		bool_list verified;
		for (unsigned int i = 0; i < package_vector.size(); i++)
		{
			verified.insert(verified.end(),false);
		}
		for (unsigned int i = 0; i < package_vector.size(); i++)
		{
			for (unsigned int i2 = 0; i2 < domain_list.size(); i2++)
			{
				if (package_vector[i].find(domain_list[i2]) != std::string::npos)
				{
					continue;
				}
				else
				{
					verified[i] = true;
					break;
				}
			}
			if (verified[i] == true)
			{
				continue;
			}
			else
			{
				cerr << R << "\n" << "ERROR: Package " << package_vector[i] << " not in domain list. Cannot install." << NC << "\n" << endl;
				return 2;
			}
		}
		string list1 = run(ConvertToChar("/usr/bin/flatpak remotes | /usr/bin/awk '{print $1}' | sed ':a;N;$!ba;s/\n/ /g'"));
		string_list list1_vector;
		split(list1_vector, list1, boost::is_any_of(" "));
		bool success = false;
		bool * success_address = &success;
		for (unsigned int i = 0; i < list1_vector.size(); i++)
		{
			try
			{
				if (assume_yes)
				{
					system(ConvertToChar("/usr/bin/flatpak install -y " + list1_vector[i] + " " + package_string));
				}
				else
				{
					system(ConvertToChar("/usr/bin/flatpak install " + list1_vector[i] + " " + package_string));
				}
				*success_address = true;
				break;
			}
			catch (...)
			{
				cerr << "\n" << R << "Flatpak installation from " << list1_vector[i] << " failed. Trying next remote . . ." << NC << "\n" << endl;
			}
		}
		if (! success)
		{
			cerr << "\n" << R << "No more remotes to try." << NC << "\n" << endl;
			return 1;
		}
		return 0;
	}
	else
	{
		if (arg1 == "-if" or arg2 == "--flat")
		{
			error_report(2,called_as,"Package 'flatpak' is not installed\nPlease run:\n\n\tmrai -ia flatpak\n\nto install it.\n");
		}
		return 2;
	}
}

int snapinstall(string_list &install_vector, string &arg1, string &arg2)
{
	if (is_snapd_installed())
	{
		string_list not_installable_vector;
		string list = run("/usr/bin/snap list | awk '{print $1}' | grep -v 'Name'");
		string_list list_vector = StringToVector(list);
		unsigned int i3 = 0;
		for (unsigned int i = 0; i < install_vector.size(); i++)
		{
			for (unsigned int i2 = 0; i2 < list_vector.size(); i2++)
			{
				if (install_vector[i] == list_vector[i2])
				{
					not_installable_vector[i3] = install_vector[i];
					i3++;
					break;
				}
			}
		}
		for (unsigned int i = 0; i < not_installable_vector.size(); i++)
		{
			for (unsigned int i2 = 0; i2 < install_vector.size(); i2++)
			{
				if (not_installable_vector[i] == install_vector[i2])
				{
					install_vector[i] = "";
					break;
				}
			}
		}
		string install_string = VectorToString(install_vector);
		if (not_installable_vector.size() > 0)
		{
			string not_installable_string = VectorToString(not_installable_vector);
			cout << "\n" << Y << "Some snaps that where passed cannot be installed because they are already installed.:\n" << not_installable_string << NC << "\n" << endl;
			if (install_vector.size() > 0)
			{
				cout << "The following packages will still be installed:\n" << install_string << "\n" << endl;

			}
			else
			{
				return 1;
			}
		}
		try
		{
			system(ConvertToChar("/usr/bin/snap install " + install_string));
			return 1;
		}
		catch (...)
		{
			string ans;
			cout << "\n" << R << "Standard Snap Installation Failed. Attempt Classic Snap Installation? [y/N]: " << flush;
			cin >> ans;
			if (ans == "Y" or ans == "y")
			{
				try
				{
					system(ConvertToChar("/usr/bin/snap install --classic " + install_string));
				}
				catch (...)
				{
					cerr << R << "\nERROR: " << NC << "Classic snap installation failed." << endl;
					return 2;
				}
			}
			else
			{
				return 2;
			}
		}

	}
	else
	{
		if (arg1 == "-si" or arg1 == "-is" or arg2 == "--snap")
		{
			cerr << "\n" << R << "Sorry. Package 'snapd' is not installed. Please run {mrai -ia snapd} to install it.\n" << endl;
			return 2;
		}
		else
		{
			return 2;
		}
	}
	return 2;
}

int gitmaninstall(string &repo_url, string &called_as)
{
	//we are no longer removing local repositories. This is done to make updating work better.
	chdir(ConvertToChar(gitmancache));
	if (repo_url.find("https://github.com/") != std::string::npos or repo_url.find("http://github.com/") != std::string::npos or repo_url.find("git://github.com/") != std::string::npos or repo_url.find("https://gitlab.com/") != std::string::npos or repo_url.find("http://gitlab.com/") != std::string::npos or repo_url.find("git://gitlab.com/") != std::string::npos)
	{
		system(ConvertToChar("git clone " + repo_url));
	}
	elif (repo_url.find("github.com/") != std::string::npos or repo_url.find("gitlab.com/") != std::string::npos)
	{
		try
		{
			system(ConvertToChar("git clone https://" + repo_url));
		}
		catch (...)
		{
			try
			{
				system(ConvertToChar("git clone git://" + repo_url));
			}
			catch (...)
			{
				error_report(2,called_as,"Inaccessable URL provided to git.");
				return 2;
			}
		}
	}
	else
	{
		cout << Y << "\nDue to added support for GitLab, automatic URL infrencing has been removed.\nPlease provide the FULL URL to clone from.\n" << NC << endl;
		return 1;
	}
	string PATH = GetURLFilePath(repo_url);
	chdir(ConvertToChar(PATH));
	string file_string = run("/bin/ls");
	cout << file_string << endl;
	cout << G << "Which of the above files would you like to try to run in order to install " << PATH << "? (Case-Sensitive) : " << NC << flush;
	string script;
	cin >> script;
	cout << endl;
	if (script == "cancel" or script == "exit" or script == "end" or script == "CANCEL" or script == "EXIT" or script == "END")
	{
		cerr << R << "\nAborting . . .\n" << NC << endl;
		return 0;
	}
	string root;
	cout << G << "Should this file be run with root privleges? [y/N]: " << NC << flush;
	cin >> root;
	cout << endl;
	if (root == "cancel" or root == "exit" or root == "end" or root == "CANCEL" or root == "EXIT" or root == "END")
	{
		cerr << R << "\nAborting . . .\n" << NC << endl;
		return 0;
	}
	chmod(ConvertToChar(PATH),0540);
	const char *env_var = "PWD";
	std::string PWD = getenv(env_var);
	if (root == "Y" or root == "y")
	{
		system(ConvertToChar("/usr/bin/pkexec " + PWD + "/" + script));
	}
	else
	{
		system(ConvertToChar("exec " + PWD + "/" + script));
	}
	ofstream flag;
	flag.open(gitmancache + PATH + "/man.flag",std::ios_base::app);
	flag << "ADDRESS=" << repo_url << endl;
	flag << "RUN_AS_ROOT=" << flush;
	if (root == "Y" or root == "y")
	{
		flag << "yes" << endl;
	}
	else
	{
		flag << "no" << endl;
	}
	flag << "RUN=" << script << endl;
	flag.close();
	return 0;
}

int aptremove(bool &assume_yes, string &user, string pass)
{
	try
	{
		if (assume_yes)
		{
			system(ConvertToChar("/usr/bin/pkexec " + scripts + "/aptremove 1 " + pass + " " + user));
		}
		else
		{
			system(ConvertToChar("/usr/bin/pkexec " + scripts + "/aptremove " + pass + " " + user));
		}
		return 0;
	}
	catch (...)
	{
		cerr << "\n" << R << "ERROR: " << NC << "aptremove has failed. Please fill out an issue report at\nhttps://github.com/drauger-os-development/mrai/issues\n" << endl;
		return 2;
	}
}

int gitremove(string_list &death_row, string &called_as)
{
	string_list auto_vector = gitautolist(called_as);
	string auto_string = VectorToString(auto_vector);
	string_list man_vector = gitmanlist(called_as);
	string man_string = VectorToString(man_vector);
	for (unsigned int i = 0; i < death_row.size(); i++)
	{
		if (auto_string.find(death_row[i]) != std::string::npos)
		{
			if (DoesPathExist(gitautocache + "/" + death_row[i] + "/Makefile"))
			{
				chdir(ConvertToChar(gitautocache + "/" + death_row[i]));
				try
				{
					system("/usr/bin/make uninstall || /usr/bin/make remove");
					chdir(ConvertToChar(gitautocache));
					remove(ConvertToChar(death_row[i]));
				}
				catch (...)
				{
					error_report(2,called_as,"make uninstall/remove failed, most likely due to developers not adding this flag. Please request they add it.");
					return 2;
				}
			}
			else
			{
				error_report(1,called_as,"No Makefile could be found. Therefore, mrai cannot uninstall Github package.");
				return 1;
			}
		}
		elif (man_string.find(death_row[i]) != std::string::npos)
		{
			chdir(ConvertToChar(gitmancache + "/" + death_row[i]));
			string flag;
			ifstream ifs(gitautocache + "/" + death_row[i] + "/man.flag");
			flag.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
			ifs.close();
			//parse the flag file
			string root = grep(flag, "RUN_AS_ROOT=[a-z]+");
			string script = grep(flag, "RUN=[a-zA-Z0-9|/|.]+");
			script = script.erase(0,4);
			if (root == "RUN_AS_ROOT=yes")
			{
				cout << "\n" << G << "Attempting to uninstall. Please wait . . ." << NC << "\n" << endl;
				int sent = 0;
				int num = 1;
				string opt;
				while (sent == 0)
				{
					cout << "\nTry " << num << ". . . \n" << endl;
					if (num == 1)
					{
						opt = "remove";
					}
					elif (num == 2)
					{
						opt = "--remove";
					}
					elif (num == 3)
					{
						opt = "uninstall";
					}
					elif (num == 4)
					{
						opt = "--uninstall";
					}
					else
					{
						error_report(1,called_as,"All uninstall options failed. Potentially consider adding it to this app?");
						return 1;
					}
					sleep(3);
					try
					{
						const char *env_var = "PWD";
						std::string PWD = getenv(env_var);
						system(ConvertToChar("/usr/bin/pkexec " + PWD + "/" + script + " " + opt));
						sent = 1;
						break;
					}
					catch (...)
					{
						num++;
					}
				}
			}
			elif (root == "RUN_AS_ROOT=no" or root == "RUN_AS_ROOT=null")
			{
				cout << "\n" << G << "Attempting to uninstall. Please wait . . ." << NC << "\n" << endl;
				int sent = 0;
				int num = 1;
				string opt;
				while (sent == 0)
				{
					cout << "\nTry " << num << ". . . \n" << endl;
					if (num == 1)
					{
						opt = "remove";
					}
					elif (num == 2)
					{
						opt = "--remove";
					}
					elif (num == 3)
					{
						opt = "uninstall";
					}
					elif (num == 4)
					{
						opt = "--uninstall";
					}
					else
					{
						error_report(1,called_as,"All uninstall options failed. Potentially consider adding it to this app?");
						return 1;
					}
					sleep(3);
					try
					{
						const char *env_var = "PWD";
						std::string PWD = getenv(env_var);
						system(ConvertToChar("exec " + PWD + "/" + script + " " + opt));
						sent = 1;
						break;
					}
					catch (...)
					{
						num++;
					}
				}
			}
			else
			{
				error_report(2,called_as,"Function gitremove ecountered an error");
			}
		}
		else
		{
			error_report(2,called_as,"Cannot find directory for app " + death_row[i]);
			return 2;
		}
	}
	return 0;
}

int flatremove(bool &assume_yes, string pass)
{
	try
	{
		if (assume_yes)
		{
			system(ConvertToChar("/usr/bin/flatpak remove -y " + pass));
		}
		else
		{
			system(ConvertToChar("/usr/bin/flatpak remove " + pass));
		}
		return 0;
	}
	catch (...)
	{
		cerr << "\n" << R << "ERROR: " << "Function flatremove has failed with exit code $?.\nPlease fill out an issue report on our GitHub at\nhttps://github.com/drauger-os-development/mrai/issues\n" << endl;
		return 2;
	}
}

int snapremove(string pass)
{
	try
	{
		system(ConvertToChar("/usr/bin/snap remove " + pass));
		return 0;
	}
	catch (...)
	{
		cerr << "\n" << R << "ERROR: " << NC << "Function snapremove has failed with exit code $?.\nPlease fill out an issue report on our GitHub at\nhttps://github.com/drauger-os-development/mrai/issues\n" << endl;
		return 1;
	}
}

int gitupdate(string_list &update_list, string &called_as)
{
	if (update_list.size() == 0)
	{
		cerr << Y << "\nNo Apps installed using git through mrai.\n" << NC << endl;
		return 0;
	}
	string pass;
	string_list file_list;
	string flag;
	string contents;
	string root;
	string add;
	//string call;
	string script;
	const char *env_var = "PWD";
	string PWD;
	string auto_string = run(ConvertToChar("/bin/ls " + gitautocache));
	string man_string = run(ConvertToChar("/bin/ls " + gitmancache));
	for (unsigned int each = 0; each < update_list.size(); each++)
	{
		if (auto_string.find(update_list[each]) != std::string::npos)
		{
			chdir(ConvertToChar(gitautocache + update_list[each]));
		}
		elif (man_string.find(update_list[each]) != std::string::npos)
		{
			chdir(ConvertToChar(gitmancache + update_list[each]));
		}
		else
		{
			error_report(1,called_as,"'git' app " + update_list[each] + " not found in local repository database.");
			continue;
		}
		pass = update_list[each];
		file_list = StringToVector(pass);
		for (unsigned int i = 0; i < file_list.size(); i++)
		{
			if (file_list[i].find(".flag") != std::string::npos)
			{
				flag = file_list[i];
			}
		}
		if (flag == "")
		{
			cerr << R << "\nNo flag file found in local repository of " << pass << "\n" << NC << endl;
			continue;
		}
		PWD = getenv(env_var);
		ifstream ifs(PWD + "/" + flag);
		contents.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
		ifs.close();
		root = grep(contents,"RUN_AS_ROOT=");
		add = grep(contents,"ADDRESS=");
		add.erase(0,8);
		script = grep(contents,"RUN=");
		script.erase(0,4);
		if (add.find("https://github.com/") != std::string::npos or add.find("http://github.com/") != std::string::npos or add.find("git://github.com/") != std::string::npos or add.find("https://gitlab.com/") != std::string::npos or add.find("http://gitlab.com/") != std::string::npos or add.find("git://gitlab.com/") != std::string::npos)
		{
			system(ConvertToChar("git pull " + add));
		}
		elif (add.find("github.com/") != std::string::npos or add.find("gitlab.com/") != std::string::npos)
		{
			try
			{
				system(ConvertToChar("git pull https://" + add));
			}
			catch (...)
			{
				try
				{
					system(ConvertToChar("git pull git://" + add));
				}
				catch (...)
				{
					error_report(2,called_as,"Could not update " + pass + ". URL not accessable.");
					continue;
				}
			}
		}
		if (script == "")
		{
			try
			{
				system("/usr/bin/make update");
			}
			catch (...)
			{
				try
				{
					system("/usr/bin/make");
					system("/usr/bin/make install");
				}
				catch (...)
				{
					error_report(1,called_as,"Makefile update method failed.");
					continue;
				}
			}
		}
		else
		{
			if (root == "RUN_AS_ROOT=yes")
			{
				system(ConvertToChar("/usr/bin/pkexec " + PWD + "/" + script));
			}
			else
			{
				system(ConvertToChar("exec " + PWD + "/" + script));
			}
		}
		flag = "";
	}
	return 0;
}

int flatupdate(string &called_as, string &arg1, string &arg2)
{
	if (is_flatpak_installed())
	{
		cout << "\n" << G << "Updating Flatpaks. Please Wait . . . \n" << NC << endl;
		try
		{
			system("/usr/bin/flatpak update 2>/dev/null");
		}
		catch (...)
		{
			error_report(1,called_as,"Flatpak encountered an error. Updating will continue.");
			cout << endl;
			return 1;
		}
	}
	elif (arg1 == "-uf" or arg2 == "--flat")
	{
		cout << Y << "\nWe're sorry. Package 'flatpak' is not installed. Please run:\n\n\tmrai -ia flatpak\n\nin order to install it.\n" << NC << endl;
		return 1;
	}
	return 0;
}

string_list checkinstalltype(string_list &package_names, string &called_as)
{
	string_list type;
	for (unsigned int i = 0; i < package_names.size(); i++)
	{
		type.insert(type.end(), "");
	}
	string dpkg_string = run("ls /var/lib/dpkg/info | grep '.md5sums' | sed 's/.md5sums//g'");
	string flat_string = run("/usr/bin/flatpak list | sed 's/\\// /g' | awk '{print $1}'");
	string snap_string = run("/usr/bin/snap list | awk '{print $1}' | sed 's/Name//'");
	string_list man_vector = gitmanlist(called_as);
	string man_string = VectorToString(man_vector);
	string_list auto_vector = gitautolist(called_as);
	string auto_string = VectorToString(auto_vector);
	for (unsigned int i = 0; i < package_names.size(); i++)
	{
		if (dpkg_string.find(package_names[i]) != std::string::npos)
		{
			type[i].append("1");
		}
		if (snap_string.find(package_names[i]) != std::string::npos)
		{
			type[i].append("2");
		}
		if (flat_string.find(package_names[i]) != std::string::npos)
		{
			type[i].append("3");
		}
		if (man_string.find(package_names[i]) != std::string::npos)
		{
			type[i].append("4");
		}
		if (auto_string.find(package_names[i]) != std::string::npos)
		{
			type[i].append("5");
		}
		if (type[i] == "")
		{
			type[i] = "0";
		}
	}
	return type;
}

int clean(bool &assume_yes, string &user)
{
	try
	{
		if (assume_yes)
		{
			system(ConvertToChar("/usr/bin/pkexec " + scripts + "/clean -y " + user));
		}
		else
		{
			system(ConvertToChar("/usr/bin/pkexec " + scripts + "/clean " + user));
		}
		return 0;
	}
	catch (...)
	{
		cerr << "\n" << R << "ERROR: " << NC << "clean has failed. Please fill out an issue report at\nhttps://github.com/drauger-os-development/mrai/issues\n" << NC << endl;
		return 2;
	}
}

int main(int argc, char **argv_list)
{
	string version = "1.4.8-beta6";
	string h = "mrai Package Manager: the Multiple Repo App Installer\n\n-c, --clean\t\tDelete old *.deb files, old config files, and old Github files\n\n--find\t\t\tFind the package the provides a given command\n\n-i,--install \tInstall an app, if none of the below options are given, check in the following order:\n\n\t\t\tapt, snap, flatpak, Github manual method, Github automatic method\n\n\n	-a, ---apt	\tInstall just from apt. In which case, usage will be:\n\n	\t\t\t\t\tmrai -ia {apt-package-name}\n\n\t\t\t\tTo install multiple apps at once, add a comma between each package name:\n\n\t\t\t\t\t\tmrai -ia {package-1},{package-2},{package-3},...\n\n	-g, --git,	\tInstall just from Github, In which case, usage will be:\n	--gm, --ga	\n	\t\t\t\t\tmrai -ig {/github-username/github-repo-name (or Github URL)}\n\n\t\t\t\tUnder this flag you can also use -m or -a to manually indicate whether to install from GitHub manually or\n\t\t\t\tautomaticlly. Please only use the automatic method if the Repo uses a Makefile to install it's software on your\n\t\t\t\tsystem.\n\n\n	-s, --snap\t\tInstall just from snapd, In which case, usage will be:\n\n\t\t\t\t\t	mrai -is {snap-name}\n\n\n	-f, --flat	\tInstall as Flatpak, In which case, usage will be:\n\n	\t\t\t\t\tmrai -if {flatpak-name}\n\n\n-h, --help\t\tDisplay this help dialogue and exit.\n\n\n-r, --remove	\tUninstall an app. {name-installed-under} refers to the name given to refer to the GitHub installation,\n--uninstall\t\tthe name of the apt package, the name of the snap, or the name of the flatpak, depending on how it was installed\n\n\n-S, --search,\t\tSearch for an app. For GitHub based apps, this only works if they are installed. To find apps to install from GitHub,\n--query	\t\tplease vist https://www.github.com\n\n\n\t-a, --apt	\tSearch for an app through apt\n\n\n\t-s, --snap\t\tSearch for an app through snap\n\n\n\t-f, --flat\t\tSearch for an app through flatpak\n\n\n\t--ga\t\t\tSearch for an app that was installed using GitHub Automatic Method\n\n\n\t--gm\t\t\tSearch for an app that was installed using GitHub Manual Method\n\n\n\t--git\t\t\tSearch for an app installed from GitHub, regardless of method\n\n\n\t-v, --verbose\t\tGive more information about the queried package\n\n\n-u, --update,\t\tUpdate your software. This may or may not work for packages installed from Github.\n\n	-a, --apt\t	Update from only apt\n\n\n	-f, --flat\t\tUpdate from only Flatpak\n\n\n\t-g, --git	\tUpdate from only Github\n\n\n	-s, --snap\t\tUpdate only installed snaps\n\n\n-v, --version\t\tPrint Current Version and exit\n\n\n--fix-config\t\tEssentially this runs 'sudo dpkg --configure -a', to fix package management issues\n\n\n--add-repo\t\tAdd a repository, MUST BE FOLLOWED BY ONE OF THE FOLLOWING OPTIONS:\n\n\n	-a, --apt\t\tadds a new PPA\n\n\n\t-f, --flat\t	adds a new Flatpak Remote\n\n\n-l, --list\t\tList installed apps, pass a package name after this flag to search for an installed app\n\n\n	-a, --apt\tFilter installed apps to ones installed with apt\n\n\n\t-f, --flat\tFilter installed apps to ones installed with Flatpak\n\n\n\t-s, --snap\tFilter installed apps to ones installed with Snap\n\n\n\t--ga\t\tFilter installed apps to ones installed using GitHub Automatic Method\n\n\n\t--gm\t\tFilter installed apps to ones installed using GitHub Manual Method\n\n\n\t--git\t\tFilter installed apps to ones installed from GitHub, regardless of method\n\n\n--edit-sources,\t\tEdit enabled apt repos by editing /etc/apt/sources.list or one of the files\n--edit-repos,\t\tin /etc/apt/sources.list.d\n--edit-apt-repos";
	//copy the argv list so that we can compare the entires
	//without having the weird "undefined behvior" errors
	string_list argv(argv_list, argv_list + argc);
	//if nothing is passed, print the help dialog and exit
	if (argc <= 1)
	{
		cerr << R << "\nERROR: " << NC << "No arguments passed. mrai must have arguments passed to be used.\n" << endl;
		cout << "\n" << h << "\n" << endl;
		return(0);
	}
	//help dialog
	elif (argv[1] == "--help" or argv[1] == "-h")
	{
		cout << "\n" << h << "\n" << endl;
		return(0);
	}
	//print out version
	elif (argv[1] == "--version" or argv[1] == "-v")
	{
		cout << "\n" << version << "\n" << endl;
		return(0);
	}
	//check to make sure we DO NOT have root
	uid_t uid = getuid();
	if (uid <= 0)
	{
		cerr << "\n" << R << "Please do not run mrai with root privleges. This will cause file system issues with GitHub installations." << NC << "\n" << endl;
		return 2;
	}
	//pre-config
	//check if mrai is already running
	if (DoesPathExist("/tmp/.mrai.lock"))
	{
		cout << "\n" << R << "mrai is already running.\nFor security and stability reasons, please refrain from using mrai until this process has exited." << NC << "\n";
		return 2;
	}
	//obtain lock and initialize everything that hasn't already been set
	//from this point on, use exit(), not return()
	touch("/tmp/.mrai.lock");
	atexit(unlock);
	//from here on, everything needs to be in a try/catch so that it will unlock if there is a seg fault
	try
	{
		string called_as = argv[0];
		//Who the hell am I?
		string user = run("/usr/bin/whoami");
		//This is a trash variable. Set commands you want to use to it
		//then use pass the output of the ConvertToChar() function to
		//system() or run()
		//system() will just run the command while run() returns output
		string COMMAND;
		if (argv[1] == "--fix-config")
		{
			cout << "\n" << G << "Attempting to correct package configuration. Please wait . . . \n" << NC << endl;
			try
			{
				system("/usr/bin/pkexec /usr/bin/dpkg --configure -a");
				exit(0);
			}
			catch (...)
			{
				error_report(2,called_as,"--fix-config failed. Most likely due to faulty package configurations.");
				exit(2);
			}
		}
		elif (argv[1] == "--add-repo")
		{
			if (argv[2] == "-a" or argv[2] == "--apt")
			{
				if (argv[3].find("ppa:") != std::string::npos)
				{
					COMMAND = "/usr/bin/pkexec /usr/bin/add-apt-repository " + argv[3];
				}
				else
				{
					COMMAND = "/usr/bin/pkexec /usr/bin/add-apt-repository ppa:" + argv[3];
				}
				try
				{
					system(ConvertToChar(COMMAND));
					exit(0);
				}
				catch (...)
				{
					exit(2);
				}
			}
			elif (argv[2] == "-f" or argv[2] == "--flat")
			{
				string name;
				if (argv[3].find("http://") != std::string::npos or argv[3].find("https://") != std::string::npos)
				{
					cout << "What would you like to name this repo?: " << flush;
					cin >> name;
					COMMAND = "/usr/bin/flatpak remote-add --if-not-exists \"" + name + "\" \"" + argv[3] + "\"";
				}
				else
				{
					cout << "What would you like to name this repo?: " << flush;
					cin >> name;
					COMMAND = "/usr/bin/flatpak remote-add --if-not-exists \"" + name + "\" \"http://" + argv[3] + "\"";
				}
				try
				{
					system(ConvertToChar(COMMAND));
					exit(0);
				}
				catch (...)
				{
					exit(2);
				}
			}
			else
			{
				error_report(2,called_as,"No package manager indicated.\n");
				exit(2);
			}
		}
		elif (argv[1] == "--edit-sources" or argv[1] == "--edit-repos" or argv[1] == "--edit-apt-repos")
		{
			cout << "\nTo enable an apt repo, remove the \"# \" (including the space) from the beginning of the line.\nTo disable a repo, just add the \"# \" back to the beginning of the line.\n\nLines beginning with \"deb-src\" are for source code.\n" << endl;
			if (DoesPathExist("/etc/apt/sources.list.d"))
			{
				string sources_dir_list = run("/bin/ls /etc/apt/sources.list.d");
				if (sources_dir_list == "" or sources_dir_list == " ")
				{
					try
					{
						system(ConvertToChar("/usr/bin/pkexec \"" + scripts + "/edit-apt-sources\" \"/etc/apt/sources.list\""));
						exit(0);
					}
					catch (...)
					{
						exit(2);
					}
				}
				else
				{
					cout << "\nYour sources.list.d directroy has config files in it as well.\n" << endl;
					cout << "0\t/etc/apt/sources.list" << endl;
					//convert sources_dir_list to a list data structure
					string_list sources_dir_vector;
					split(sources_dir_vector, sources_dir_list, boost::is_any_of(" "));
					for (unsigned int i1 = 0; i1 < sources_dir_vector.size(); i1++)
					{
						cout << (i1 + 1)  << "\t/etc/apt/sources.list.d/" << sources_dir_vector[i1] << endl;
					}
					bool check = true;
					unsigned int choice;
					unsigned int  * choice_address = &choice;
					while (check == true)
					{
						try
						{
							cout << "Give the number of the file you would like to edit: [0-" << (sources_dir_vector.size() + 1) << "]: " << flush;
							cin >> *choice_address;
						}
						catch (...)
						{
							cout << R << "\nValue entered not a positive integer. Please try again.\n" << NC << endl;
							continue;
						}
						if (choice >= 0 and choice <= sources_dir_vector.size() + 1)
						{
							check = false;
						}
						else
						{
							cout << "\n" << R << "ERROR: " << NC << "Not within designated range. Please try again.\n" << NC << endl;
						}
					}
					string file;
					if (choice == 0)
					{
						file = "/etc/apt/sources.list";
					}
					else
					{
						file = "/etc/apt/sources.list.d/" + sources_dir_vector[choice - 1];
					}
					try
					{
						system(ConvertToChar("/usr/bin/pkexec \"" + scripts + "/edit-apt-sources\" \"" + file + "\""));
						exit(0);
					}
					catch(...)
					{
						exit(2);
					}
				}
			}
			else
			{
				try
				{
					system(ConvertToChar("/usr/bin/pkexec \"" + scripts + "/edit-apt-sources\" \"/etc/apt/sources.list\""));
					exit(0);
				}
				catch (...)
				{
					exit(2);
				}
			}
		}
		if (DoesPathExist("/usr/sbin/apt-fast"))
		{
			if (! is_aptfast_installed())
			{
				touch(cache + "/apt-fast.flag");
			}
		}
		else
		{
			if (is_aptfast_installed())
			{
				remove(ConvertToChar(cache + "/apt-fast.flag"));
			}
		}
		if (DoesPathExist("/usr/bin/snap"))
		{
			if (! is_snapd_installed())
			{
				touch(cache + "/snapd.flag");
			}
		}
		else
		{
			if (is_snapd_installed())
			{
				remove(ConvertToChar(cache + "/snapd.flag"));
			}
		}
		if (DoesPathExist("/usr/bin/flatpak"))
		{
			if (! is_flatpak_installed())
			{
				touch(cache + "/flatpak.flag");
			}
		}
		else
		{
			if (is_flatpak_installed())
			{
				remove(ConvertToChar(cache + "/flatpak.flag"));
			}
		}
		//verbose flag system (and compound flag system. The two are now integrated together.)
		//this shit is super complex and I hate it
		//if you can find a better way of handling things without crashing, please implement it
		string_list u;
		string_list L;
		string_list S;
		string_list i;
		string_list install;
		string_list uninstall;
		bool VERBOSE = false;
		bool r = false;
		bool c = false;
		bool a = false;
		bool FIND = false;
		string look;
		string search_string;
		if (argc <= 0)
		{
			cerr << R << "\nERROR: " << NC << "No options passed.\n" << endl;
			exit(1);
		}
		if (argv[1].find("-") != std::string::npos)
		{
			//Find Flag
			if (argv[1] == "--find")
			{
				FIND = true;
				if (argc-1 >= 2)
				{
					 look = argv[2];
				}
				else
				{
					cerr << R << "\nERROR: " << NC << "Find function takes 2 arguments. 1 passed.\n" << endl;
					exit(1);
				}
			}
			//Clean Flag
			elif (argv[1] == "--clean" or argv[1].find("-c") != std::string::npos )
			{
				c = true;
				if (argv[1].find("y") != std::string::npos)
				{
					a = true;
				}
				elif (argc-1 >= 2)
				{
					if (argv[2] == "--assume-yes" or argv[2] == "-y")
					{
						a = true;
					}
				}
			}
			//Uninstall Flag
			elif (argv[1] == "--remove" or argv[1] == "--uninstall" or argv[1].find("-r") != std::string::npos)
			{
				r = true;
				if (argv[1].find("y") != std::string::npos)
				{
					a = true;
				}
				elif (argc >= 3)
				{
					if (argv[2] == "--assume-yes" or argv[2] == "-y")
					{
						a = true;
					}
				}
				if (argc >= 2)
				{
					for (int i = 2; i < argc; i++)
					{
						if (argv[i] == "--assume-yes" or argv[i] == "-y" or argv[1] == "--remove" or argv[1] == "--uninstall")
						{
							continue;
						}
						uninstall.insert(uninstall.end(),argv[i]);
					}
				}
			}
			//Update Flag
			elif (argv[1] == "--update" or argv[1].find("-u") != std::string::npos)
			{
				if ((argv[1].find("-u") != std::string::npos) and (argv[1].find("--u") == std::string::npos) and (argv[1].find("a") != std::string::npos or argv[1].find("s") != std::string::npos or argv[1].find("f") != std::string::npos or argv[1].find("g") != std::string::npos or argv[1].find("y") != std::string::npos))
				{
					if (argv[1].find("a") != std::string::npos)
					{
						u.insert(u.end(),"a");
					}
					if (argv[1].find("s") != std::string::npos)
					{
						u.insert(u.end(),"s");
					}
					if (argv[1].find("f") != std::string::npos)
					{
						u.insert(u.end(),"f");
					}
					if (argv[1].find("g") != std::string::npos)
					{
						u.insert(u.end(),"g");
					}
					if (argv[1].find("y") != std::string::npos)
					{
						a = true;
					}
				}
				elif (argc >= 1)
				{
					int sent = 2;
					while (sent <= argc-1)
					{
						if (argv[sent] == "--apt" or argv[sent] == "-a")
						{
							u.insert(u.end(),"a");
						}
						elif (argv[sent] == "--snap" or argv[sent] == "-s")
						{
							u.insert(u.end(),"s");
						}
						elif (argv[sent] == "--flat" or argv[sent] == "-f")
						{
							u.insert(u.end(),"f");
						}
						elif (argv[sent] == "--git")
						{
							u.insert(u.end(),"g");
						}
						elif (argv[sent] == "--assume-yes" or argv[sent] == "-y")
						{
							a = true;
						}
						sent++;
					}
				}
				if (u.size() <= 0)
				{
					u.insert(u.end(),"a");
					u.insert(u.end(),"s");
					u.insert(u.end(),"f");
					u.insert(u.end(),"g");
				}
			}
			//List Flag
			elif (argv[1] == "--list" or argv[1].find("-l") != std::string::npos)
			{
				if (argv[1].find("a") != std::string::npos or (argv[1].find("s") != std::string::npos and argv[1] != "--list") or argv[1].find("f") != std::string::npos or argv[1].find("g") != std::string::npos)
				{
					if (argv[1].find("a") != std::string::npos)
					{
						L.insert(L.end(),"p");
					}
					if (argv[1].find("s") != std::string::npos)
					{
						L.insert(L.end(),"s");
					}
					if (argv[1].find("f") != std::string::npos)
					{
						L.insert(L.end(),"f");
					}
					if (argv[1].find("git") != std::string::npos)
					{
						L.insert(L.end(),"gb");
					}
					if (argv[1].find("gm") != std::string::npos)
					{
						L.insert(L.end(),"gm");
					}
					if (argv[1].find("ga") != std::string::npos)
					{
						L.insert(L.end(),"ga");
					}
				}
				elif (argc >= 2)
				{
					int sent = 2;
					while (sent < argc)
					{
						if (argv[sent] == "--apt" or argv[sent] == "-a")
						{
							L.insert(L.end(),"p");
						}
						elif (argv[sent] == "--snap" or argv[sent] == "-s")
						{
							L.insert(L.end(),"s");
						}
						elif (argv[sent] == "--flat" or argv[sent] == "-f")
						{
							L.insert(L.end(),"f");
						}
						elif (argv[sent] == "--git")
						{
							L.insert(L.end(),"gb");
						}
						elif (argv[sent] == "--gm")
						{
							L.insert(L.end(),"gm");
						}
						elif (argv[sent] == "--ga")
						{
							L.insert(L.end(),"ga");
						}
						sent++;
					}
				}
				if (L.size() <= 0)
				{
					L.insert(L.end(),"p");
					L.insert(L.end(),"s");
					L.insert(L.end(),"f");
					L.insert(L.end(),"gb");
				}
			}
			//Install Flag
			elif (argv[1] == "--install" or argv[1].find("-i") != std::string::npos)
			{
				if ((argv[1].find("-i") != std::string::npos) and (argv[1].find("--i") == std::string::npos) and (argv[1].find("a") != std::string::npos or argv[1].find("s") != std::string::npos or argv[1].find("f") != std::string::npos or argv[1].find("g") != std::string::npos or argv[1].find("y") != std::string::npos))
				{
					if (argv[1].find("a") != std::string::npos)
					{
						i.insert(i.end(),"a");
					}
					if (argv[1].find("s") != std::string::npos)
					{
						i.insert(i.end(),"s");
					}
					if (argv[1].find("f") != std::string::npos)
					{
						i.insert(i.end(),"f");
					}
					if (argv[1].find("git") != std::string::npos)
					{
						i.insert(i.end(),"gb");
					}
					if (argv[1].find("gm") != std::string::npos)
					{
						i.insert(i.end(),"gm");
					}
					if (argv[1].find("ga") != std::string::npos)
					{
						i.insert(i.end(),"ga");
					}
					if (argv[1].find("y") != std::string::npos)
					{
						a = true;
					}
				}
				elif (argc >= 2)
				{
					int sent = 2;
					while (sent <= argc-1)
					{
						if (argv[sent] == "--apt" or argv[sent] == "-a")
						{
							i.insert(i.end(),"a");
						}
						elif (argv[sent] == "--snap" or argv[sent] == "-s")
						{
							i.insert(i.end(),"s");
						}
						elif (argv[sent] == "--flat" or argv[sent] == "-f")
						{
							i.insert(i.end(),"f");
						}
						elif (argv[sent] == "--git")
						{
							i.insert(i.end(),"gb");
						}
						elif (argv[sent] == "--gm")
						{
							i.insert(i.end(),"gm");
						}
						elif (argv[sent] == "--ga")
						{
							i.insert(i.end(),"ga");
						}
						elif (argv[sent] == "--assume-yes" or argv[sent] == "-y")
						{
							a = true;
						}
						sent++;
					}
				}
				if (i.size() <= 0)
				{
					i.insert(i.end(),"l");
				}
				for (int i = 2; i < argc; i++)
				{
					if (argv[i] == "--assume-yes" or argv[i] == "-y" or argv[i].find("--g") != std::string::npos or argv[i].find("-s") != std::string::npos or argv[i].find("-f") != std::string::npos or argv[i].find("-a") != std::string::npos)
					{
						continue;
					}
					install.insert(install.end(),argv[i]);
				}
			}
			//Search flag
			elif (argv[1] == "--search" or argv[1] == "--query" or argv[1].find("-S") != std::string::npos)
			{
				if ((argv[1].find("-S") != std::string::npos) and (argv[1].find("--s") == std::string::npos) and (argv[1].find("a") != std::string::npos or argv[1].find("s") != std::string::npos or argv[1].find("f") != std::string::npos or argv[1].find("g") != std::string::npos or argv[1].find("v") != std::string::npos))
				{
					if (argv[1].find("a") != std::string::npos)
					{
						S.insert(S.end(),"p");
					}
					if (argv[1].find("s") != std::string::npos)
					{
						S.insert(S.end(),"s");
					}
					if (argv[1].find("f") != std::string::npos)
					{
						S.insert(S.end(),"f");
					}
					if (argv[1].find("git") != std::string::npos)
					{
						S.insert(S.end(),"gb");
					}
					if (argv[1].find("gm") != std::string::npos)
					{
						S.insert(S.end(),"gm");
					}
					if (argv[1].find("ga") != std::string::npos)
					{
						S.insert(S.end(),"ga");
					}
					if (argv[1].find("v") != std::string::npos)
					{
						VERBOSE = true;
					}
				}
				elif (argc >= 2)
				{
					int sent = 2;
					while (sent <= argc-1)
					{
						if (argv[sent] == "--apt" or argv[sent] == "-a")
						{
							S.insert(S.end(),"p");
						}
						elif (argv[sent] == "--snap" or argv[sent] == "-s")
						{
							S.insert(S.end(),"s");
						}
						elif (argv[sent] == "--flat" or argv[sent] == "-f")
						{
							S.insert(S.end(),"f");
						}
						elif (argv[sent] == "--git")
						{
							S.insert(S.end(),"gb");
						}
						elif (argv[sent] == "--gm")
						{
							S.insert(S.end(),"gm");
						}
						elif (argv[sent] == "--ga")
						{
							S.insert(S.end(),"ga");
						}
						elif (argv[sent] == "--verbose" or argv[sent] == "-v")
						{
							VERBOSE = true;
						}
						sent++;
					}
				}
				if (S.size() <= 0)
				{
					S.insert(S.end(),"p");
					S.insert(S.end(),"s");
					S.insert(S.end(),"f");
					S.insert(S.end(),"gb");
				}
				if (argc >= 2)
				{
					for (int i = 2; i < argc; i++)
					{
						if (argv[i] == "--assume-yes" or argv[i] == "-y" or argv[i].find("--g") != std::string::npos or argv[i].find("-s") != std::string::npos or argv[i].find("-f") != std::string::npos or argv[i].find("-a") != std::string::npos)
						{
							continue;
						}
						search_string = argv[i];
						break;
					}
				}
			}
		}
		if (i.size() <= 0 and ! r and ! c and S.size() <= 0 and u.size() <= 0 and L.size() <= 0 and ! FIND)
		{
				error_report(2,called_as,"Flag system failed. Something was set and got through but was not detected before work segment. Please file a bug report at https://github.com/drauger-os-development/mrai/issues\n");
				exit(2);
		}
		else
		{
			//install flag
			if (i.size() > 0)
			{
				cout << G << "\nAttempting to Install . . .\n" << NC << endl;
				if (Vector_Contains(i,"a") and i.size() == 1)
				{
					exit(aptinstall(install,a));
				}
				elif (Vector_Contains(i,"gm") and i.size() == 1)
				{
					string URL = argv[argv.size() - 1 ];
					exit(gitmaninstall(URL,called_as));
				}
				elif (Vector_Contains(i,"ga") and i.size() == 1)
				{
					string URL = argv[argv.size() - 1 ];
					exit(gitautoinst(user,URL));
				}
				elif (Vector_Contains(i,"gb") and i.size() == 1)
				{
					string URL = argv[argv.size() - 1 ];;
					try
					{
						exit(gitautoinst(user,URL));
					}
					catch (...)
					{
						try
						{
							exit(gitmaninstall(URL,called_as));
						}
						catch (...)
						{
							error_report(1,called_as," Installation from git failed.");
							exit(1);
						}
					}
				}
				elif (Vector_Contains(i,"s") and i.size() == 1)
				{
					exit(snapinstall(install,argv[1],argv[2]));
				}
				elif (Vector_Contains(i,"f") and i.size() == 1)
				{
					exit(flatinstall(argv[1],argv[2],install,a,called_as));
				}
				elif (Vector_Contains(i,"l") and i.size() == 1)
				{
					string URL = argv[argv.size() - 1 ];
					try
					{
						aptinstall(install,a);
					}
					catch (...)
					{
						try
						{
							snapinstall(install, argv[1], argv[2]);
						}
						catch (...)
						{
							try
							{
								flatinstall(argv[1], argv[2], install, a, called_as);
							}
							catch (...)
							{
								try
								{
									gitautoinst(user,URL);
								}
								catch (...)
								{
									try
									{
										gitmaninstall(URL,called_as);
									}
									catch (...)
									{
										cerr << R << "\nAll installation methods failed.$NC\nPlease make sure that you have the Github username,\nrepo name, and package name spelled correctly\n" << NC << endl;
										exit(2);
									}
								}
							}
						}
					}
					exit(0);
				}
				else
				{
					error_report(2,called_as,"Install flag was set but nothing was set dictating what package manager to use.");
					exit(2);
				}
			}
			//uninstall flag
			elif (r)
			{
				string_list sr_list;
				string_list a_list;
				string_list fp_list;
				string_list g_list;
				string type_list;
				string_list type = checkinstalltype(uninstall,called_as);
				bool sentinal = true;
				string check;
				//each package has a string for it in the "type" vector
				//we need to check these strings for the appropriate identifiers
				if (type.size() > 0)
				{
					string ans;
					string show;
					for (unsigned int i = 0; i < uninstall.size(); i++)
					{
						if (type[i] == "1")
						{
							a_list.insert(a_list.end(),uninstall[i]);
						}
						elif (type[i] == "2")
						{
							sr_list.insert(sr_list.end(),uninstall[i]);
						}
						elif (type[i] == "3")
						{
							fp_list.insert(fp_list.end(),uninstall[i]);
						}
						elif (type[i] == "4" or type[i] == "5")
						{
							g_list.insert(g_list.end(),uninstall[i]);
						}
						elif (type[i] == "0")
						{
							cout << Y << "\nAccording to all our databases, " << uninstall[i] << " is not installed.\n" << NC << endl;
							cout << "Show packages with similar names? [y/N]: " << flush;
							cin >> ans;
							if (ans != "Y" or ans != "y")
							{
								continue;
							}
							cout << "\nApps with similar package names:\n\n" << endl;
							show = run(ConvertToChar("/usr/bin/dpkg -l | /bin/grep " + uninstall[i]));
							if (show == " " or show == "")
							{
								cerr << R << "No Apt Packages with similar names installed.\n" << NC << endl;
							}
							else
							{
								cout << "Apt Packages:\n\n" << endl;
								cout << show << "\n" << endl;
							}
							if (is_snapd_installed())
							{
								show = run(ConvertToChar("/bin/ls /snap | grep " + uninstall[i]));
								string_list snap_list = StringToVector(show);
								for (unsigned int i2 = 0; i2 < snap_list.size(); i2++)
								{
									if (snap_list[i2] == "bin" or snap_list[i2] == "README")
									{
										snap_list.erase(snap_list.begin() + i2);
									}
								}
								if (snap_list.size() <= 0)
								{
									cerr << R << "No Snaps with similar names installed.\n" << NC << endl;
								}
								else
								{
									cout << "Snaps:\n\n" << endl;
									for (unsigned int i2 = 0; i2 < snap_list.size(); i2++)
									{
										cout << snap_list[i2] << endl;
									}
								}
							}
							if (is_flatpak_installed())
							{
								show = run(ConvertToChar("/usr/bin/flatpak list | /bin/grep " + uninstall[i]));
								if (show == " " or show == "")
								{
									cerr << R << "\nNo Flatpaks with similar names installed.\n" << NC << endl;
								}
								else
								{
									cout << "Flatpaks:\n\n" << endl;
									cout << show << endl;
								}
							}

						}
						else
						{
							sentinal = true;
							type_list = type[i];
							string_list pack_man_list;
							show = "";
							string input;
							string_list check;
							string ans;
							while (sentinal)
							{
								for (char const &c: type_list)
								{
									switch (c)
									{
										case 1:
											show.append("apt");
											input.append(" 1 for apt");
											check.insert(check.end(),"1");
											break;
										case 2:
											show.append("snap");
											input.append(" 2 for snap");
											check.insert(check.end(),"2");
											break;
										case 3:
											show.append("flatpak");
											input.append(" 3 for flatpak");
											check.insert(check.end(),"3");
											break;
										case 4:
											show.append("git automatic");
											break;
										case 5:
											show.append("git manual");
											break;
										default:
											error_report(2,called_as,"Unrecognized value in variable 'type' in uninstall segment: " + type[i]);
											exit(2);
									}
								}
								for (unsigned int i2 = 0; i2 < show.size(); i2++)
								{
									if (show.find("git") != std::string::npos)
									{
										input.append(" 4 for git Manual OR Automatic");
										check.insert(check.end(),"4");
										break;
									}
								}
								cout << G << "\nThere are mutliple apps which fit the package name \"" << uninstall[i] << "\".\n\nThese are installed through: " << endl;
								cout << G << show << endl;
								cout << G << "\n\nFrom which package manager would you like to uninstall?" << NC << endl;
								cout << input << "Select ONE: " << flush;
								cin >> ans;
								while (true)
								{
									if (ans == "1" and Vector_Contains(check,"1"))
									{
										a_list.insert(a_list.end(), uninstall[i]);
										sentinal = false;
										break;
									}
									elif (ans == "2" and Vector_Contains(check,"2"))
									{
										sr_list.insert(sr_list.end(), uninstall[i]);
										sentinal = false;
										break;
									}
									elif (ans == "3" and Vector_Contains(check,"3"))
									{
										fp_list.insert(fp_list.end(), uninstall[i]);
										sentinal = false;
										break;
									}
									elif (ans == "4" and Vector_Contains(check,"4"))
									{
										g_list.insert(g_list.end(), uninstall[i]);
										sentinal = false;
										break;
									}
									elif (ans == "exit" or ans == "cancel" or ans == "EXIT" or ans == "CANCEL")
									{
										cout << Y << "\nExiting . . .\n" << NC << endl;
										exit(0);
									}
									else
									{
										cerr << R << "\nNOT A PROVIDED OPTION. PLEASE TRY AGAIN.\n" << NC << endl;
									}
								}
							}
						}
					}
				}
				else
				{
					error_report(2,called_as,"Unknown error with var type when attempting to remove package.");
					exit(2);
				}
				if (a_list.size() > 0)
				{
					aptremove(a,user,VectorToString(a_list));
				}
				if (sr_list.size() > 0)
				{
					snapremove(VectorToString(sr_list));
				}
				if (fp_list.size() > 0)
				{
					flatremove(a,VectorToString(fp_list));
				}
				if (g_list.size() > 0)
				{
					gitremove(g_list,called_as);
				}
				exit(0);
			}
			//clean flag
			elif (c)
			{
				clean(a,user);
			}
			//search flag
			elif (S.size() > 0)
			{
				if (! VERBOSE)
				{
					if (Vector_Contains(S,"p"))
					{
						system(ConvertToChar("/usr/bin/apt search " + search_string));
					}
					if (Vector_Contains(S,"f"))
					{
						if (is_flatpak_installed())
						{
							system(ConvertToChar("/usr/bin/flatpak search " + search_string));
						}
						else
						{
							error_report(1,called_as,"Flatpak is not installed. Please run \"mrai -ia flatpak\" to install it.\n");
						}
					}
					if (Vector_Contains(S,"s"))
					{
						if (is_snapd_installed())
						{
							system(ConvertToChar("/usr/bin/snap search " + search_string));
						}
						else
						{
							error_report(1,called_as,"Snapd is not installed. Please run \"mrai -ia snapd\" to install it.\n");
						}
					}
					if (Vector_Contains(S,"gm") or Vector_Contains(S,"gb"))
					{
						string_list list = gitmanlist(called_as);
						if (Vector_Contains(list, search_string))
						{
							string_list print_list;
							for (unsigned int i = 0; i < list.size(); i++)
							{
								if (list[i].find(search_string) != std::string::npos)
								{
									print_list.insert(print_list.end(), list[i]);
								}
							}
							cout << "Matching terms found installed using manual git mentod.\n\nMatching app names:" << endl;
							for (unsigned int i = 0; i < print_list.size(); i++)
							{
								cout << "\t *" << print_list[i] << endl;
							}
							cout << endl;
						}
						else
						{
							cout << Y << "\nThe search term provided was not found to be installed using git manual method.\n" << NC << endl;
						}
					}
					if (Vector_Contains(S,"ga") or Vector_Contains(S,"gb"))
					{
						string_list list = gitautolist(called_as);
						if (Vector_Contains(list, search_string))
						{
							string_list print_list;
							for (unsigned int i = 0; i < list.size(); i++)
							{
								if (list[i].find(search_string) != std::string::npos)
								{
									print_list.insert(print_list.end(), list[i]);
								}
							}
							cout << "Matching terms found installed using automatic git mentod.\n\nMatching app names:" << endl;
							for (unsigned int i = 0; i < print_list.size(); i++)
							{
								cout << "\t *" << print_list[i] << endl;
							}
							cout << endl;
						}
						else
						{
							cout << Y << "\nThe search term provided was not found to be installed using git automatic method.\n" << NC << endl;
						}
					}
				}
				else
				{
					if (Vector_Contains(S,"p"))
					{
						system(ConvertToChar("/usr/bin/apt show " + search_string));
					}
					if (Vector_Contains(S,"f"))
					{
						if (is_flatpak_installed())
						{
							error_report(1,called_as,"Flatpak does not support verbose search mode. Using normal search.\n");
							system(ConvertToChar("/usr/bin/flatpak search " + search_string));
						}
						else
						{
							error_report(1,called_as,"Flatpak is not installed. Please run \"mrai -ia flatpak\" to install it.\n");
						}
					}
					if (Vector_Contains(S,"s"))
					{
						if (is_snapd_installed())
						{
							system(ConvertToChar("/usr/bin/snap info " + search_string));
						}
						else
						{
							error_report(1,called_as,"Snapd is not installed. Please run \"mrai -ia snapd\" to install it.\n");
						}
					}
					if (Vector_Contains(S,"gm") or Vector_Contains(S,"gb"))
					{
						error_report(1,called_as,"git manual method does not support verbose searching. Falling back to standard search . . .");
						string_list list = gitmanlist(called_as);
						if (Vector_Contains(list, search_string))
						{
							string_list print_list;
							for (unsigned int i = 0; i < list.size(); i++)
							{
								if (list[i].find(search_string) != std::string::npos)
								{
									print_list.insert(print_list.end(), list[i]);
								}
							}
							cout << "Matching terms found installed using manual git mentod.\n\nMatching app names:" << endl;
							for (unsigned int i = 0; i < print_list.size(); i++)
							{
								cout << "\t *" << print_list[i] << endl;
							}
							cout << endl;
						}
						else
						{
							cout << Y << "\nThe search term provided was not found to be installed using git manual method.\n" << NC << endl;
						}
					}
					if (Vector_Contains(S,"ga") or Vector_Contains(S,"gb"))
					{
						error_report(1,called_as,"git automatic method does not support verbose searching. Falling back to standard search . . .");
						string_list list = gitautolist(called_as);
						if (Vector_Contains(list, search_string))
						{
							string_list print_list;
							for (unsigned int i = 0; i < list.size(); i++)
							{
								if (list[i].find(search_string) != std::string::npos)
								{
									print_list.insert(print_list.end(), list[i]);
								}
							}
							cout << "Matching terms found installed using automatic git mentod.\n\nMatching app names:" << endl;
							for (unsigned int i = 0; i < print_list.size(); i++)
							{
								cout << "\t * " << print_list[i] << endl;
							}
							cout << endl;
						}
						else
						{
							cout << Y << "\nThe search term provided was not found to be installed using git automatic method.\n" << NC << endl;
						}
					}
				}
			}
			//update flag
			elif (u.size() > 0)
			{
				if (Vector_Contains(u, "a"))
				{
					if (! a)
					{
						try
						{
							if (Vector_Contains(u,"s"))
							{
								system("/usr/bin/pkexec '/sbin/aptupdate' -s 2>/dev/null");
							}
							else
							{
								system("/usr/bin/pkexec '/sbin/aptupdate' 2>/dev/null");
							}
						}
						catch (...)
						{
							error_report(2,called_as,"aptupdate has had an error. Please check error log for more info.");
							exit(2);
						}
					}
					else
					{
						try
						{
							if (Vector_Contains(u,"s"))
							{
								system("/usr/bin/pkexec '/sbin/aptupdate' -sy 2>/dev/null");
							}
							else
							{
								system("/usr/bin/pkexec '/sbin/aptupdate' -y 2>/dev/null");
							}
						}
						catch (...)
						{
							error_report(2,called_as,"aptupdate has had an error. Please check error log for more info.");
							exit(2);
						}
					}
				}
				if (Vector_Contains(u,"s") and ! Vector_Contains(u,"a"))
				{
					system("/sbin/snapupdate");
				}
				if (Vector_Contains(u,"f"))
				{
					flatupdate(called_as, argv[1], argv[2]);
				}
				if (Vector_Contains(u,"g"))
				{
					int sent = 2;
					string_list pass;
					string search_output;
					while (sent < argc)
					{
						search_output = grep(argv[sent],"^-");
						if (search_output == "")
						{
							pass.insert(pass.end(),argv[sent]);
						}
						sent++;
					}
					if (pass.size() <= 0)
					{
						string_list t = gitautolist(called_as);
						string_list r = gitmanlist(called_as);
						pass.reserve(t.size() + r.size());
						pass.insert(pass.end(), t.begin(), t.end());
						pass.insert(pass.end(), r.begin(), r.end());
					}

					gitupdate(pass,called_as);
				}
			}
			//List flag
			elif (L.size() > 0)
			{
				if (argc >= 2)
				{
					if (Vector_Contains(L,"p"))
					{
						cout << G << "\nAPT:\n" << NC << endl;
						string output = run("/usr/bin/dpkg -l | /bin/grep '^ii' | /usr/bin/awk '{print $2}'");
						string_list package_list = StringToVector(output);
						string_list output_list;
						int num;
						if ((argv[1].find("-a") != std::string::npos))
						{
							if (argv[1].find("-a") != std::string::npos)
							{
								num = 2;
							}
							for (unsigned int i = 0; i < package_list.size(); i++)
							{
								if (package_list[i].find(argv[num]) != std:: string::npos)
								{
									cout << "\t * " << package_list[i] << endl;
								}
							}
						}
						elif (argc > 1  and (argv[2].find("-a") != std::string::npos))
						{
							if (argv[2].find("-a") != std::string::npos)
							{
								num = 3;
							}
							for (unsigned int i = 0; i < package_list.size(); i++)
							{
								if (package_list[i].find(argv[num]) != std:: string::npos)
								{
									cout << "\t * " << package_list[i] << endl;
								}
							}
						}
						if (Vector_Contains(L,"s"))
						{
							if (is_snapd_installed())
							{
								cout << G << "\nSnaps:\n" << NC << endl;
								system(ConvertToChar("/usr/bin/snap list " + argv[2]));
								cout << endl;
							}
							else
							{
								error_report(2, called_as, "Snapd not available. Install with 'mrai -ia snapd'");
								exit(2);
							}
						}
						if (Vector_Contains(L,"f"))
						{
							if (is_flatpak_installed())
							{
								cout << G << "\nFlatpaks:\n" << NC << endl;
								system(ConvertToChar("/usr/bin/flatpak list -a | grep " + argv[2]));
								cout << endl;
							}
							else
							{
								error_report(2,called_as,"Flatpak is not available. Install it with 'mrai -ia flatpak'");
								exit(2);
							}
						}
						if (Vector_Contains(L,"gm"))
						{
							string_list search_field = gitmanlist(called_as);
							string_list output_list;
							for (unsigned int i = 0; i < search_field.size(); i++)
							{
								if (search_field[i].find(argv[2]) != std::string::npos)
								{
									output_list.insert(output_list.end(),search_field[i]);
								}
							}
							cout << G << "\nInstalled git Apps:\nInstalled through Manual Method:\n" << NC << flush;
							for (unsigned int i = 0; i < output_list.size(); i++)
							{
								cout << "\t * " << output_list[i] << endl;
							}
						}
						if (Vector_Contains(L,"ga"))
						{
							string_list search_field = gitautolist(called_as);
							string_list output_list;
							for (unsigned int i = 0; i < search_field.size(); i++)
							{
								if (search_field[i].find(argv[2]) != std::string::npos)
								{
									output_list.insert(output_list.end(),search_field[i]);
								}
							}
							cout << G << "\nInstalled git Apps:\nInstalled through Automatic Method:\n" << NC << flush;
							for (unsigned int i = 0; i < output_list.size(); i++)
							{
								cout << "\t * " << output_list[i] << endl;
							}
						}
						if (Vector_Contains(L,"gb")  and ! Vector_Contains(L,"ga") and ! Vector_Contains(L,"gm"))
						{
							string_list list1 = gitmanlist(called_as);
							string_list list2 = gitautolist(called_as);
							if (list1.size() <= 0)
							{
								list1.insert(list1.end(), "No apps found installed using manual method.");
							}
							if (list2.size() <= 0)
							{
								list2.insert(list2.end(), "No apps found installed using automatic method.");
							}
							cout << G << "\nInstalled git Apps:\n\nInstalled through Manual Method:\n" << NC <<  endl;
							for (unsigned int i = 0; i < list1.size(); i++)
							{
								cout << list1[i] << endl;
							}
							cout << G << "Installed through Automatic Method:\n" << NC << endl;
							for (unsigned int i = 0; i < list2.size(); i++)
							{
								cout << list2[i] << endl;
							}
						}
						elif (Vector_Contains(L,"gb"))
						{
							if (Vector_Contains(L,"ga") and Vector_Contains(L,"gm"))
							{
								error_report(2,called_as,"Already specified the 'manual only' or 'automatic only' flags.Please do not use the manual only, automatic only, or unspecified flags in conjunction.");
								exit(2);
							}
						}
					}
				}
				else
				{
					cout << G << "\nAPT:\n" << NC << endl;
					system("/usr/bin/dpkg -l | /bin/grep '^ii' | /usr/bin/awk '{print $2}'");
					if (is_snapd_installed())
					{
						cout << G << "\nSnaps:\n" << NC << endl;
						system("/usr/bin/snap list");
					}
					if (is_flatpak_installed())
					{
						cout << G << "\nFlatpaks:\n" << NC << endl;
						system("/usr/bin/flatpak list -a");
					}
					string_list list1 = gitmanlist(called_as);
					string_list list2 = gitautolist(called_as);
					if (list1.size() <= 0)
					{
						list1.insert(list1.end(), "No apps found installed using manual method.");
					}
					if (list2.size() <= 0)
					{
						list2.insert(list2.end(), "No apps found installed using automatic method.");
					}
					cout << G << "\nInstalled git Apps:\n\nInstalled through Manual Method:\n" << NC <<  endl;
					for (unsigned int i = 0; i < list1.size(); i++)
					{
						cout << list1[i] << endl;
					}
					cout << G << "Installed through Automatic Method:\n" << NC << endl;
					for (unsigned int i = 0; i < list2.size(); i++)
					{
						cout << list2[i] << endl;
					}
				}
			}
			//Find flag
			elif (FIND)
			{
				string command_path = run(ConvertToChar("which " + look + " 2>/dev/null"));
				if (command_path == "")
				{
					cout << "\n" << run(ConvertToChar("type " + look + " 2>/dev/null")) << endl;
				}
				else
				{
					string package = run(ConvertToChar("dpkg -S " + command_path + " | /bin/sed 's/:/ /g' | /usr/bin/awk '{print \\$1}'"));
					if (package == "")
					{
						error_report(1,called_as,"No package providing specificed command.");
						exit(1);
					}
					else
					{
						cout << G << "\nPackage providing command '" << look << "': " << NC << package << "\n" << endl;
					}
				}
			}
			else
			{
				cerr << "\n" << h << "\n" << endl;
			}
		}
		exit(0);
	}
	catch (...)
	{
		error_report(2,"UNKNOWN"," AN ERROR HAS OCCURED.");
		exit(2);
	}
}
