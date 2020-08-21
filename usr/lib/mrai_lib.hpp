/*
 * mrai_lib.hpp
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

#ifndef mrai_lib_hpp__
#define mrai_lib_hpp__

extern int error_report(int error_code, std::string called_as, std::string error_message);

extern bool DoesPathExist(const std::string &s);

extern const char* ConvertToChar(const std::string &s);

extern std::string GetURLFilePath(const std::string& url;

extern int ConvertToInt(std::string &convert);

extern int base_spinner(std::string &message, std::string &PID);

extern std::string run(const char* cmd);

extern int touch(std::string path);

extern std::string VectorToString(string_list list);

extern string_list StringToVector(std::string &string_to_convert);

extern std::string grep(std::string search_field, std::string search_pattern);

extern bool Vector_Contains(string_list &vector, std::string search);

extern bool is_snapd_installed();

extern std::string is_aptfast_installed();

extern string_list split(std::string input, char term);

extern std::string translate(std::string var_name, std::string parse_in1, std::string parse_in2);



#endif //mrai_lib_hpp__
