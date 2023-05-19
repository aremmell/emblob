#!/bin/bash

# for the benefit of another script that includes this
# one--gives them something to check for.
BASH_UTIL_INCLUDED=1

# gets the current date/time
get_datetime() {
    date +"%FT%T%Z"
}

# gets the path to the folder in which the
# script exists.
get_script_dir() {
    return $(dirname "$(readlink -f "$0")")
}

# echoes information from uname in a more
# readable and relevant way than -a
get_clean_uname() {
	echo "$(uname -s) $(uname -r) ($(uname -m)/$(uname -p))"
}

# text styling
clr_none='\033[0m'
clr_verbose='\033[0;37m'
clr_info='\033[0;97m'
clr_warning='\033[1;93m'
clr_error='\033[1;91m'
clr_success='\033[1;92m'
clr_prompt='\033[22;1;94m'
clr_prompt_def='\033[2m'

# symbols
_sym_arrowr="\xE2\x86\x92"
_sym_arrowl="\xE2\x86\x90"
_sym_bullet="\xE2\x80\xA2"

# logging to console
_myname=$(basename "$0")

# $1 level
# $2 message
# $3 color
# $4 (optional) args
_echo_base() {
	echo -e$4 "$3[$1] $_myname: $2$clr_none"
}
echo_verbose() {
    _echo_base "verb" "$1" $clr_verbose
}
echo_info() {
	_echo_base "info" "$1" $clr_info
}
echo_warning() {
	_echo_base "warn" "$1" $clr_warning
}
echo_error() {
	_echo_base "err" "$1" $clr_error
}
echo_success() {
	_echo_base " OK " "$1" $clr_success
}
# $1 = msg, $2 = default value
echo_prompt() {
	_echo_base " IN " "$1 [$clr_prompt_def$2$clr_prompt]: " $clr_prompt "n"
}

# prompt for input w/ default option. sets $prompt_val
# $1 prompt text
# $2 default value
# $3 (optional) max length
prompt_get() {
	echo_prompt "$1" "$2"

	if [ ! -z "$3" ] && [ "$3" -gt 0 ]; then
		read -n $3 user_input
        if [ ! -z "${user_input}" ]; then
            echo
        fi
	else
		read -e user_input
	fi

	if [ -z "$user_input" ]; then
		user_input=${2}
	fi
	prompt_val=${user_input}
}

# prints an error message and exits the script with an error code.
# $1 error message
# $2 (optional) code
error_exit() {
	echo_error "$1"

	if [ -n $"{2}" ]; then
		exit $2
	fi

	exit 1
}
