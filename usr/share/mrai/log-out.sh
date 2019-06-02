#!/bin/bash
# -*- coding: utf-8 -*-
#
#  log-out.sh
#  
#  Copyright 2019 Thomas Castleman <contact@draugeros.org>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#
#
#  For the record, this script DOES NOT LOG YOU OUT.
#  It's a universal log manager to keep things consistent across all logs

set -o pipefail
set -e

#Package Name
name="mrai"

#get input from whatever script
ERROR_CODE="$1"
SCRIPT_PATH="$2"
ERROR_MESSAGE="$3"
CALL_METHOD="$4"
PWD="$5"

#set filesystem locations
LOG_LOCATION="/tmp/$name/log.txt"

#Collect necissary system info
UNAME=$(uname -a)
DATE=$(date)
DISPLAY_SERVER="$XDG_SESSION_TYPE"
if [ -f /usr/drauger/os-info.txt ]; then
	DRAUGER_OS_VERSION=$(cat /usr/drauger/os-info.txt)
else
	DRAUGER_OS_VERSION="NOT A DRAUGER OS INSTALLATION"
fi

log () {
	OUTPUT_LOG="Date_Error_Occured: $DATE\nuname: $UNAME\nCurrent_Session: $DISPLAY_SERVER\nDrauger_OS_Version: $DRAUGER_OS_VERSION\nProgram: $SCRIPT_PATH\nError_Code: $ERROR_CODE\nError_Message: $ERROR_MESSAGE\n\n"
	if [[ ! -d "/tmp/$name" ]]; then
		mkdir "/tmp/$name"
	fi
	if [[ ! -f "$LOG_LOCATION" ]]; then
		touch $LOG_LOCATION
	fi
	echo -e "$OUTPUT_LOG" >> "$LOG_LOCATION"
}
log
#Determine the extent of the damage
#if [ "$ERROR_CODE" == "1" ]; then
#	#One is normally just a warning or a minor issue. Just report it in the log.
#	log
#else
#	#If it's anything other than 1, we have an issue.
#	#Anything with an output of 0 will not be logging anything, to help with I/O
#	log
#	#Make a crash file in /var/crash
#	NAME=$(echo $SCRIPT_PATH | sed 's/\//_/g')
#	CRASH_FILE="/var/crash/$NAME.1000.crash"
#	if [ "$DRAUGER_OS_VERSION" == "NOT A DRAUGER OS INSTALLATION" ]; then
#		DRAUGER_OS_VERSION=$(lsb_release -d | sed 's/Description:	//')
#	fi
#	ARCH=$(uname -m)
#	TIMESTAMP=$(date +%s)
#	LOCALE=$(locale)
#	OUTPUT_CRASH="ProblemType: Crash\nArchitecture: $ARCH\nCrashCounter: 1\nCurrentDesktop: $XDG_CURRENT_DESKTOP\nDate: $DATE\nDistroRelease: $DRAUGER_OS_VERSION\nExecutablePath: $SCRIPT_PATH\nExecutableTimestamp: $TIMESTAMP\nProcCmdline: $CALL_METHOD\nProcCwd: $PWD\nProcEnviron: \n$LOCALE\nSHELL: $SHELL\n"
#	echo -e "$OUTPUT_CRASH" > "$CRASH_FILE"
#fi
