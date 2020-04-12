/*
 * mrai_lib.h
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

//import libs
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <regex>
#include <stdlib.h>
#include <fstream>

//define macros
#define elif else if
#define string_list std::vector<std::string>
#define int_list std::vector<int>
#define float_list std::vector<float>
#define bool_list std::vector<bool>
#define sleep(x) usleep(x)

//define global vars
std::string R = "\033[0;31m";
std::string G = "\033[0;32m";
std::string Y = "\033[1;33m";
std::string NC = "\033[0m";
std::string cache = "/etc/mrai";
std::string scripts = "/usr/share/mrai";
std::string gitautocache = "/etc/mrai/gitauto";
std::string gitmancache = "/etc/mrai/gitman";
std::string version = "1.5.5-beta7";
std::string VERSION = version;

std::vector<string_list> defaults = {
	{"h", "mrai Package Manager: the Multiple Repo App Installer\n\n-c, --clean\t\tDelete old *.deb files, old config files, and old Github files\n\n--find\t\t\tFind the package the provides a given command\n\n-i,--install \tInstall an app, if none of the below options are given, check in the following order:\n\n\t\t\tapt, snap, flatpak, Github manual method, Github automatic method\n\n\n	-a, ---apt	\tInstall just from apt. In which case, usage will be:\n\n	\t\t\t\t\tmrai -ia {apt-package-name}\n\n\t\t\t\tTo install multiple apps at once, add a comma between each package name:\n\n\t\t\t\t\t\tmrai -ia {package-1},{package-2},{package-3},...\n\n	-g, --git,	\tInstall just from Github, In which case, usage will be:\n	--gm, --ga	\n	\t\t\t\t\tmrai -ig {/github-username/github-repo-name (or Github URL)}\n\n\t\t\t\tUnder this flag you can also use -m or -a to manually indicate whether to install from GitHub manually or\n\t\t\t\tautomaticlly. Please only use the automatic method if the Repo uses a Makefile to install it's software on your\n\t\t\t\tsystem.\n\n\n	-s, --snap\t\tInstall just from snapd, In which case, usage will be:\n\n\t\t\t\t\t	mrai -is {snap-name}\n\n\n	-f, --flat	\tInstall as Flatpak, In which case, usage will be:\n\n	\t\t\t\t\tmrai -if {flatpak-name}\n\n\n-h, --help\t\tDisplay this help dialogue and exit.\n\n\n-r, --remove	\tUninstall an app. {name-installed-under} refers to the name given to refer to the GitHub installation,\n--uninstall\t\tthe name of the apt package, the name of the snap, or the name of the flatpak, depending on how it was installed\n\n\n-S, --search,\t\tSearch for an app. For GitHub based apps, this only works if they are installed. To find apps to install from GitHub,\n--query	\t\tplease vist https://www.github.com\n\n\n\t-a, --apt	\tSearch for an app through apt\n\n\n\t-s, --snap\t\tSearch for an app through snap\n\n\n\t-f, --flat\t\tSearch for an app through flatpak\n\n\n\t--ga\t\t\tSearch for an app that was installed using GitHub Automatic Method\n\n\n\t--gm\t\t\tSearch for an app that was installed using GitHub Manual Method\n\n\n\t--git\t\t\tSearch for an app installed from GitHub, regardless of method\n\n\n\t-v, --verbose\t\tGive more information about the queried package\n\n\n-u, --update,\t\tUpdate your software. This may or may not work for packages installed from Github.\n\n	-a, --apt\t	Update from only apt\n\n\n	-f, --flat\t\tUpdate from only Flatpak\n\n\n\t-g, --git	\tUpdate from only Github\n\n\n	-s, --snap\t\tUpdate only installed snaps\n\n\n-v, --version\t\tPrint Current Version and exit\n\n\n--fix-config\t\tEssentially this runs 'sudo dpkg --configure -a', to fix package management issues\n\n\n--add-repo\t\tAdd a repository, MUST BE FOLLOWED BY ONE OF THE FOLLOWING OPTIONS:\n\n\n	-a, --apt\t\tadds a new PPA\n\n\n\t-f, --flat\t	adds a new Flatpak Remote\n\n\n-l, --list\t\tList installed apps, pass a package name after this flag to search for an installed app\n\n\n	-a, --apt\tFilter installed apps to ones installed with apt\n\n\n\t-f, --flat\tFilter installed apps to ones installed with Flatpak\n\n\n\t-s, --snap\tFilter installed apps to ones installed with Snap\n\n\n\t--ga\t\tFilter installed apps to ones installed using GitHub Automatic Method\n\n\n\t--gm\t\tFilter installed apps to ones installed using GitHub Manual Method\n\n\n\t--git\t\tFilter installed apps to ones installed from GitHub, regardless of method\n\n\n--edit-sources,\t\tEdit enabled apt repos by editing /etc/apt/sources.list or one of the files\n--edit-repos,\t\tin /etc/apt/sources.list.d\n--edit-apt-repos"},
	{"failure", "has failed."},
	{"error", "ERROR: "},
	{"report", "Please fill out an issue report\non our GitHub at https://github.com/drauger-os-development/mrai/issues\n"},
	{"all_installed", "All passed packages are already installed.\n"},
	{"no_candidates", "No installation candidates found."},
	{"not_in_domain_list", " not in domain list. Cannot install."},
	{"no_remotes", "No more remotes to try."},
	{"flatpak_remote_failed", "Flatpak installation from \v failed. Trying next remote . . ."},
	{"flatpak_not_installed", "Package 'flatpak' is not installed\nPlease run:\n\n\tmrai -ia flatpak\n\nto install it.\n"},
	{"already_installed_snaps", "Some snaps that where passed cannot be installed because they are already installed.:\n"},
	{"snaps_still_installing", "The following packages will still be installed:\n"},
	{"snap_standard_failure", "Standard Snap Installation Failed. Attempt Classic Snap Installation?"},
	{"snap_classic_failure", "Classic snap installation failed."},
	{"snapd_not_installed", "Sorry. Package 'snapd' is not installed. Please run {mrai -ia snapd} to install it.\n"},
	{"url_not_accessable", "Inaccessable URL provided to git."},
	{"url_infrencing_error", "\nDue to added support for GitLab, automatic URL infrencing has been removed.\nPlease provide the FULL URL to clone from.\n"},
	{"git_file_prompt", "Which of the above files would you like to try to run in order to complete installation? (Case-Sensitive) : "},
	{"abort", "\nAborting . . .\n"},
	{"run_with_root_prompt", "Should this file be run with root privleges?"},
	{"make_uninstall_failure", "most likely due to developers not adding this flag. Please request they add it."},
	{"makefile_not_found_uninstall", "No Makefile could be found. Therefore, mrai cannot uninstall git package."},
	{"attempt_uninstall", "Attempting to uninstall. Please wait . . ."},
	{"git_uninstall_failed", "All uninstall options failed. Potentially consider adding it to this app?"},
	{"encountered_error", "Function \v ecountered an error"},
	{"directory_not_found", "Cannot find directory for app"},
	{"no_git_apps", "\nNo Apps installed using git through mrai.\n"},
	{"no_git_auto_apps", "No apps found installed using automatic method."},
	{"no_git_manual_apps", "No apps found installed using manual method."},
	{"git_app_not_found", "'git' app \v not found in local repository database."},
	{"no_flag_file", "\nNo flag file found in local repository of "},
	{"url_inaccessable_git_update", "Could not update \v. URL not accessable."},
	{"makefile_failed", "Makefile update method failed."},
	{"updating_flatpak", "Updating Flatpaks. Please Wait . . . \n"},
	{"flatpak_update_error", "Flatpak encountered an error. Updating will continue."},
	{"clean_failure", "clean has failed. Please fill out an issue report at\nhttps://github.com/drauger-os-development/mrai/issues\n"},
	{"no_arguments", "No arguments passed. mrai must have arguments passed to be used.\n"},
	{"run_with_root_error", "Please do not run mrai with root privleges. This will cause file system issues with GitHub installations."},
	{"mrai_already_running", "mrai is already running.\nFor security and stability reasons, please refrain from using mrai until this process has exited."},
	{"fix_config", "Attempting to correct package configuration. Please wait . . . \n"},
	{"fix_config_failure", "--fix-config failed. Most likely due to faulty package configurations."},
	{"name_repo_prompt", "What would you like to name this repo?: "},
	{"package_manager_error", "No package manager indicated.\n"},
	{"sources_list_pre_open", "\nTo enable an apt repo, remove the \"# \" (including the space) from the beginning of the line.\nTo disable a repo, just add the \"# \" back to the beginning of the line.\n\nLines beginning with \"deb-src\" are for source code.\n"},
	{"sources_list_directory_notice", "\nYour sources.list.d directroy has config files in it as well.\n"},
	{"open_file_prompt", "Give the number of the file you would like to edit:"},
	{"value_not_unsigned_int", "\nValue entered not a positive integer. Please try again.\n"},
	{"not_within_range", "Not within designated range. Please try again.\n"},
	{"no_opt_passed", "No options passed.\n"},
	{"find_function_error", "Find function takes 2 arguments. 1 passed.\n"},
	{"flag_failure", "Flag system failed. Something was set and got through but was not detected before work segment. Please file a bug report at https://github.com/drauger-os-development/mrai/issues\n"},
	{"attempt_install", "\nAttempting to Install . . .\n"},
	{"git_install_failure", "Installation from git failed."},
	{"install_failed", "\nAll installation methods failed.$NC\nPlease make sure that you have the Github username,\nrepo name, and package name spelled correctly\n"},
	{"install_flag_error", "Install flag was set but nothing was set dictating what package manager to use."},
	{"package_not_installed", "\nAccording to all our databases, \v is not installed.\n"},
	{"show_similar_names_prompt", "Show packages with similar names?"},
	{"show_similar_names", "\nApps with similar package names:\n\n"},
	{"no_similar_debs", "No Apt Packages with similar names installed.\n"},
	{"similar_debs", "Apt Packages:\n\n"},
	{"no_similar_snaps", "No Snaps with similar names installed.\n"},
	{"similar_snaps", "Snaps:\n\n"},
	{"no_similar_flatpaks", "\nNo Flatpaks with similar names installed.\n"},
	{"similar_flatpaks", "Flatpaks:\n\n"},
	{"error_var_type", "Unrecognized value in variable 'type' in uninstall segment: "},
	{"multiple_installations", "\nThere are mutliple apps which fit the package name \"\v\".\n\nThese are installed through: "},
	{"select_package_manager_prompt","\n\nFrom which package manager would you like to uninstall?"},
	{"select_one", "Select ONE: "},
	{"not_provided_option", "\nNOT A PROVIDED OPTION. PLEASE TRY AGAIN.\n"},
	{"unknown_error_type", "Unknown error with var type when attempting to remove package."},
	{"matching_terms_git_manual", "Matching terms found installed using manual git mentod.\n\nMatching app names:"},
	{"matching_terms_git_auto", "Matching terms found installed using automatic git mentod.\n\nMatching app names:"},
	{"no_matching_terms_git_manual", "\nThe search term provided was not found to be installed using git manual method.\n"},
	{"no_matching_terms_git_auto", "\nThe search term provided was not found to be installed using git automatic method.\n"},
	{"flatpak_not_verbose", "Flatpak does not support verbose search mode. Using normal search.\n"},
	{"git_manual_not_verbose", "git manual method does not support verbose searching. Falling back to standard search . . ."},
	{"git_auto_not_verbose", "git automatic method does not support verbose searching. Falling back to standard search . . ."},
	{"aptupdate_error", "aptupdate has had an error. Please check error log for more info."},
	{"installed_git", "\nInstalled git Apps:\n"},
	{"installed_git_manual", "Installed through Manual Method:\n"},
	{"installed_git_auto", "Installed through Automatic Method:\n"},
	{"multiple_git_flag_error", "Already specified the 'manual only' or 'automatic only' flags. Please do not use the manual only, automatic only, or unspecified flags in conjunction."},
	{"no_providing_package", "No package providing specificed command."},
	{"providing_package", "\nPackage providing command '\v': \v\n"},
	{"unknown_error", "AN ERROR HAS OCCURED."},
	{"aptupdate_seperate_flags", "aptupdate does not support setting flags seperately. Please use -sy or -ys.\n"},
	{"aptupdate_apt_cache", "Updating apt cache . . ."},
	{"aptupdate_cache_error", "An error has occured. Most likely your computer cannot communicate with one or more servers.\nPlease make sure you are connected to the internet and/or try again later.\n"},
	{"aptupdate_catastrophic_error","An error has occured mid-update.\nRecovery has been attempted. Shutting down mrai for safey."},
	{"snapupdate_opt_invalid", "Option not recognized.\n"},
	{"snapupdate_force_update","Force Updating Snaps. Please Wait . . . "},
	{"snapupdate_error", "Snap has had an error. Most likely network issues."},
	{"aptremove_root_error","aptremove has failed with fatal error: Not running with correct permission set.\n"},
	{"aptremove_purge_error", "apt purge has failed. Most likely a configuration bug with an app."},
	{"aptremove_clean_prompt", "Would you like mrai to clean up left-over, unused dependencies?"},
	{"aptremove_clean_fail", "clean has failed"},
	{"clean_apt_clean", "Deleting old *.deb packages . . . "},
	{"clean_apt_clean_failed", "apt clean has failed. Most likly due to file system permission issues."},
	{"clean_autoremove", "Removing old and out dated dependencies, Deleting old config files . . . "},
	{"clean_autoremove_error", "apt autoremove has failed. Most likly due to app configuration issues."},
	{"clean_git_clean", "Cleaning up old GitHub files . . ."},
	{"clean_git_error", "GitHub clean up failed. Most likly due to incorrect file system permissions"},
	{"clean_done", "Clean up complete."},
	{"edit_apt_sources_parse_error", "Could not parse selected_editor.conf or .selected-editor. Most likly file system permissions issue."},
	{"edit_apt_sources_edit_error", "Could not manipulate \v. Most likely not running as root or incorrect file system permissions."},
	{"edit_apt_sources_done", "Configuring Apt Repos Complete!\n"},
	{"gitautoinst_clone_failure", "git clone failed. Either because the provided URL is not GitHub/GitLab, or because the cloning is being done as root, or there is no internet."},
	{"gitautoinst_makefile_found", "\nA Makefile has been detected. Some apps require you to run a seperate file before the Makefile."},
	{"gitautoinst_preconfig_script_prompt", "Would you like to run such a file?"},
	{"gitautoinst_preconfig_script_selection_prompt", "Which of the above files would you like to run? (Press enter with no input to cancel and continue with running the Makefile. Type 'exit' to exit.):"},
	{"gitautoinst_mark_exec_fail", "Could not mark file as executable."},
	{"gitautoinst_preconfig_script_error", "Pre-Makefile script failed to run correctly."},
	{"gitautoinst_make_depend_error", "make depend not recognized inside Makefile. Continuing . . ."},
	{"gitautoinst_make_failure", "make has failed"},
	{"gitautoinst_make_install_failure", "make install has failed"},
	{"gitautoinst_no_makefile", "\nNo makefile detected. Removing local repository . . .\n"}
};




int error_report(int error_code, std::string called_as, std::string error_message)
{
	if (error_code <= 1)
	{
		std::cerr << Y << "WARNING: " << NC << error_message << std::endl;
	}
	else
	{
		std::cerr << R << "ERROR: " << NC << error_message << std::endl;
	}
	std::string scripts = "/usr/share/mrai";
	const char *env_var = "PWD";
	std::string PWD = getenv(env_var);
	std::string COMMAND = scripts + "/log-out ";
	COMMAND += std::to_string(error_code);
	COMMAND = COMMAND + error_message + " mrai " + PWD + called_as;
	int len = COMMAND.length();
	char run[len + 1];
	strcpy(run, COMMAND.c_str());
	system(run);
	return 0;
}

bool DoesPathExist(const std::string &s)
{
  struct stat buffer;
  return (stat (s.c_str(), &buffer) == 0);
}

const char* ConvertToChar(const std::string &s)
{
	const char * WORKING_STRING = s.c_str();
	return WORKING_STRING;
}

std::string GetURLFilePath(const std::string& url) {
    auto last_slash_location = url.rfind('/');
    if(last_slash_location == std::string::npos || last_slash_location + 1 >= url.size()) {
        return "";
    }
    return url.substr(last_slash_location + 1);
}

int ConvertToInt(std::string &convert)
{
	std::stringstream sti(convert);
	int output;
	sti >> output;
	return output;
}

int base_spinner(std::string &message, std::string &PID)
{
	std::string sp[5] = {"/", "-", "\\", "|"};
	int i = 1;
	std::string DIR = "/proc/" + PID;
	while (DoesPathExist(DIR))
	{
		sleep(250000);
		std::cout << "\r\033[K";
		std::cout << "[" << sp[i] << "] " << message << std::flush;;
		if ( i == 3)
		{
			i = 0;
		}
		else
		{
			i++;
		}
	}
	std::cout << "\r\033[K";
	return 0;
}

std::string run(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result += buffer;
        }
    }
    catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int touch(std::string path)
{
	try
	{
		std::ofstream file;
		file.open(ConvertToChar(path), std::ios_base::app);
		file << "" << std::flush;
		file.close();
		return 0;
	}
	catch (...)
	{
		return 2;
	}
}

std::string VectorToString(string_list list)
{
	std::string exit_string;
	for (unsigned int i = 0; i < list.size(); i++)
	{
		if (i == 0)
		{
			exit_string = list[0];
		}
		else
		{
			exit_string = exit_string + " " + list[i];
		}
	}
	return(exit_string);
}

string_list StringToVector(std::string &string_to_convert)
{
	std::istringstream iss(string_to_convert);
	string_list results(std::istream_iterator<std::string>{iss},std::istream_iterator<std::string>());
	return results;
}

std::string grep(std::string search_field, std::string search_pattern)
{
	std::regex rgx(search_pattern);
	std::smatch match;
	const std::string s = search_field;
	std::regex_search(s.begin(), s.end(), match, rgx);
	return match[0];
}

bool Vector_Contains(string_list &vector, std::string search)
{
	for (unsigned int i = 0; i < vector.size(); i++)
	{
		if (vector[i] == search)
		{
			return true;
		}
	}
	return false;
}

bool is_snapd_installed()
{
	return DoesPathExist("/usr/bin/snap");
}

bool is_aptfast_installed()
{
	return DoesPathExist("/usr/sbin/apt-fast");
}

string_list split(std::string input, char term)
{
	const char *char_input = ConvertToChar(input);
	std::stringstream ss(char_input);
	std::string to;
	string_list output;

	if (char_input != NULL)
	{
		while(std::getline(ss,to,term))
		{
			output.insert(output.end(), to);
		}
	}
	return output;
}

std::string translate(std::string var_name, std::string parse_in1, std::string parse_in2)
{
	std::string LANG_in = getenv("LANG");
	std::string LANG = "";
	for (int i = 0; i < 5; i++)
	{
		LANG = LANG + LANG_in[i];
	}
	//check if /etc/drauger-locales/$LANG exists, minus the .UTF-8 at the end
	if (DoesPathExist("/etc/drauger-locales/" + LANG))
	{
		std::ifstream config_file;
		config_file.open("/etc/drauger-locales/" + LANG + "/mrai.conf");
		std::string config = "";
		if ( ! config_file)
		{
			error_report(1, "func translate", "ERROR: /etc/drauger-locales/" + LANG + "/mrai.conf CANNOT BE READ");
			return "Null";
		}
		else
		{
			config_file >> config;
			string_list config_array = split(config, '\n');
			delete &config;
			std::vector<string_list> config;
			for (unsigned int i = 0; i < config_array.size(); i++)
			{
				config.insert(config.end(), split(config_array[i], '\t'));
			}
			//we have our config parsed into a 2D array. Now search for our var
			for (unsigned int i = 0; i < config.size(); i++)
			{
				for (unsigned int i2 = 0; i < config[i].size(); i2++)
				{
					if (config[i][i2] == var_name)
					{
						//parse in parse_in1 and parse_in2
						string_list value = split(config[i][i2], '\v');
						std::string output = value[0] + parse_in1;
						if (value.size() >= 2)
						{
							output = output + value[1] + parse_in2;
						}
						if (value.size() >= 3)
						{
							output = output + value[2];
						}
						return output;
					}
				}
			}
		}
	}
	//it doesn't exist, so use $defaults
	else
	{
		for (unsigned int i = 0; i < defaults.size(); i++)
		{
			for (unsigned int i2 = 0; i < defaults[i].size(); i2++)
			{
				if (defaults[i][i2] == var_name)
				{
					//parse in parse_in1 and parse_in2
					string_list value = split(defaults[i][i2], '\v');
					std::string output = value[0] + parse_in1;
					if (value.size() >= 2)
					{
						output = output + value[1] + parse_in2;
					}
					if (value.size() >= 3)
					{
						output = output + value[2];
					}
					return output;
				}
			}
		}
	}
	return "ERROR: VAR NOT FOUND";
}
