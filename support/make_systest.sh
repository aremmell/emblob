#!/bin/bash

current_dir="$(dirname "$(readlink -f "$0")")"

source "${current_dir}/bash-util.sh"

if [[ ${BASH_UTIL_INCLUDED} -ne 1 ]]; then
	echo "Failed to source bash-util.sh!"
	exit 1
fi

name=systest
out_bin_dir="${current_dir}/../build/bin"
out_obj_dir="${current_dir}/../build/obj"
out_bin_dir_relative="../build/bin"
outfile="${out_bin_dir}/${name}"
_c_flags="-O2 -Wall -std=c11 -DNDEBUG"

# $1 = boolean. if true, do not prompt to run the program
compile_systest() {
	if [[ -f ${outfile} ]]; then
		rm -f ${outfile}
	fi

	echo_info "Building ${name}.c ${_sym_arrowr} ${outfile}..."

	cc -o ${out_obj_dir}/${name}.o -c ${current_dir}/${name}.c ${_c_flags} && cc -o ${out_bin_dir}/${name} ${out_obj_dir}/${name}.o ${_c_flags}

	if [[ $? -ne 0 ]]; then
		echo_error "Failed to build! Exit code: $?"
	else
		echo_success "Successfully built ${out_bin_dir_relative}/${name}. Platform info: $(get_clean_uname)"
	fi

	if [[ ${1} = true ]]; then
		echo_info "Skipping prompt due to --no-prompt."
	else
		prompt_get "Would you like to run ${name} now? (y/n):" "y"
		if [[ ${prompt_val} == "y" || ${prompt_val} = "Y" ]]; then
			${outfile}
		fi
	fi
}

_args=("$@")
_no_prompt=false

case ${_args[0]} in
	"--no-prompt")
		_no_prompt=true
		;;
	*)
		echo "Usage: $0 [--no-prompt]\tWon't prompt for user input, and won't run ${name}."
		;;
esac

compile_systest "${_no_prompt}"
