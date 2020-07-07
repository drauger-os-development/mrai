/*
 * edit-apt-sources.cxx
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
 * VERSION 0.2.3-beta2
 */


#include "mrai_lib.hpp"

using namespace std;

int main(int argc, char **argv)
{
	string file = argv[1];
	int i = 0;
	string usr = argv[2];
	string called_as = argv[0];
	string contents;
	string editor;
	string * editor_location = &editor;
	try
	{
		while ( i == 0 )
		{
			ifstream f("/home/" + usr + "/.selected_editor");
			cout << "/home/" + usr + "/.selected_editor" << endl;
			ifstream g(cache + "/selected_editor.conf");
			if ( f.good() || g.good() )
			{
				if ( ! f.good() && g.good() )
				{
					ifstream ifs(cache + "/selected_editor.conf");
					contents.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
					contents.erase(0,55);
					contents.erase(9,10);
					*editor_location = contents;
					i = 1;
				}
				else if ( f.good() && ! g.good() )
				{
					ifstream ifs("/home/" + usr + "/.selected_editor");
					ofstream conf;
					conf.open (cache + "/selected_editor.conf", std::ios_base::app);
					conf << contents << endl;
					conf.close();
					contents.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
					contents.erase(0,55);
					contents.erase(9,10);
					*editor_location = contents;
					i = 1;
				}
				else if ( f.good() && g.good() )
				{
					ifstream ifs(cache + "/selected_editor.conf");
					contents.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
					contents.erase(0,55);
					contents.erase(9,10);
					*editor_location = contents;
					i = 1;
				}
				else
				{
					return 2;
				}
			}
			else
			{
				system("/usr/bin/select-editor");
			}
		}
	}
	catch (...)
	{
		error_report(2, argv[0], translate("edit_apt_sources_parse_error", "", ""));
		return 2;
	}
	try
	{
		contents = "";
		ifstream ifs(file);
		contents.assign((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));
		ofstream backup;
		backup.open(file + ".bak", std::ios_base::app);
		backup << contents << endl;
		backup.close();
		string COMMAND = editor + " " + file;
		//convert string to array of chars
		const char * run = ConvertToChar(COMMAND);
		system(run);
	}
	catch (...)
	{
			error_report(2,argv[0],translate("edit_apt_sources_edit_error", file, ""));
			return 2;
	}
	try
	{
		cout << "\n" << G << "\b" << translate("aptupdate_apt_cache", "", "") << NC << endl;
		system("/usr/bin/apt update");
	}
	catch (...)
	{
		error_report(2,argv[0],translate("aptupdate_cache_error", "", ""));
		return 2;
	}
	cout << "\n" << G << "\b" << translate("edit_apt_sources_done", "", "") << NC << endl;
	return 0;
}

