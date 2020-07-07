/*
 * aptremove.cxx
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
 * VERSION: 0.1.7-beta3
 */

#include "mrai_lib.hpp"
#include <boost/algorithm/string.hpp>
#include <experimental/filesystem>

using namespace std;

int main(int argc, char **argv)
{
    string called_as = argv[0];
    string pass = argv[1];
    bool assume_yes = false;
    if (pass == "1")
    {
        pass = argv[2];
        assume_yes = true;
    }
    vector<string> passv;
    vector<string> * passv_address = &passv;
    if (getuid() != 0)
    {
        error_report(2,called_as,translate("aptremove_root_error", "", ""));
        return 2;
    }
    try
    {
        string aptfast_location = is_aptfast_installed();
        if (aptfast_location.length() > 0)
        {
            if (assume_yes)
            {
                split(*passv_address, pass, boost::is_any_of(","));
                system(ConvertToChar(aptfast_location + " -y purge " + pass));
            }
            else
            {
                split(*passv_address, pass, boost::is_any_of(","));
                system(ConvertToChar(aptfast_location + " purge " + pass));
            }
        }
        else
        {
            if (assume_yes)
            {
                split(*passv_address, pass, boost::is_any_of(","));
                system(ConvertToChar("/usr/bin/apt -y purge " + pass));
            }
            else
            {
                split(*passv_address, pass, boost::is_any_of(","));
                system(ConvertToChar("/usr/bin/apt purge " + pass));
            }
        }
    }
    catch (...)
    {
        error_report(2,called_as,translate("aptremove_purge_error", "", ""));
        return 2;
    }
    try
    {
        if (assume_yes)
        {
            string convert = "/usr/share/mrai/clean -y ";
            convert.append(argv[3]);
            const char * COMMAND = ConvertToChar(convert);
            system(COMMAND);
        }
        else
        {
            string ans;
            cout << translate("aptremove_clean_prompt", "", "") << " [y/N]: " << flush;
            cin >> ans;
            if (ans == "Y" or ans == "y")
            {
                string convert = "/usr/share/mrai/clean ";
                convert.append(argv[2]);
                const char * COMMAND = ConvertToChar(convert);
                system(COMMAND);
            }
        }
    }
    catch (...)
    {
        error_report(2, called_as, translate("aptremove_clean_fail", "", ""));
    }
    return 0;
}

