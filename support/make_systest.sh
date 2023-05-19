#!/bin/bash

source "./bash-util.sh"

if [[ ${BASH_UTIL_INCLUDED} -ne 1 ]]; then
	echo "Failed to source bash-util.sh!"
	exit 1
fi

name=systest
out_bin_dir="../build/bin"
out_obj_dir="../build/obj"
outfile="${out_bin_dir}/${name}"

_c_flags="-02 -Wall -std=c11 -DNDEBUG"

# $1 = boolean. if true, do not prompt to run the program
compile_systest() {
	if [[ -f ${outfile} ]]; then
		rm -f ${outfile}
	fi

	echo_info "Building ${name}.c ${_sym_arrowr} ${outfile}..."

	cc -o ${out_obj_dir}/${name}.o -c ${name}.c && cc -o ${out_bin_dir}/${name} ${out_obj_dir}/${name}.o

	if [[ $? -ne 0 ]]; then
		echo_error "Failed to build! Exit code: $?"
	else
		echo_success "Successfully built ${outfile}. Platform info: $(uname -a)"
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
