#!/bin/bash

current_dir="$(dirname "$(readlink -f "$0")")"

source "${current_dir}/bash-util.sh"

if [[ ${BASH_UTIL_INCLUDED} -ne 1 ]]; then
	echo "Failed to source bash-util.sh!"
	exit 1
fi

name=systest
out_bin_dir="$(realpath ${current_dir}/../build/bin)"
out_obj_dir="$(realpath ${current_dir}/../build/obj)"
out_bin_dir_relative="../build/bin"
outfile="${out_bin_dir}/${name}"
_c_flags="-O2 -Wall -std=c11 -DNDEBUG"

# creates some files in the same dir as
# systest; used to test relative path file existence functionality.
create_test_files() {
	local file1="${out_bin_dir}/i_exist"
	touch "${file1}" && echo_success "Created ${file1}"
}

# $1 = boolean; if true, do not prompt to run the program
# $2 = boolean; if true, do not execute the program after building
compile_systest() {
	if [[ -f ${outfile} ]]; then
		rm -f ${outfile}
	fi

	echo_info "Building ${name}.c ${_sym_arrowr} ${outfile}..."

	cc -o ${out_obj_dir}/${name}.o -c ${current_dir}/${name}.c ${_c_flags} && cc -o ${out_bin_dir}/${name} ${out_obj_dir}/${name}.o ${_c_flags}

	_err=$?
	if [[ $_err -ne 0 ]]; then
		echo_error "Failed to build! Exit code: $_err"
		return 1
	else
		echo_success "Successfully built ${out_bin_dir_relative}/${name}. Platform info: $(get_clean_uname)"
	fi

	if ! create_test_files; then
		echo_error "Couldn't create test file(s). Expect file existence tests to fail!"
	fi

	local _exec=true

	if [ ${2} = true ]; then
		echo_info "Skipping execution due to --no-execute."
		_exec=false
	fi

	if [[ ${1} = true ]]; then
		echo_info "Skipping prompt due to --no-prompt."
	else
		prompt_get "Would you like to run ${name} now? (y/n):" "y"
		if [[ ${prompt_val} == "y" || ${prompt_val} = "Y" ]]; then
			_exec=true
		fi
	fi

	if [[ ${_exec} = true ]]; then
		if [[ ${3} = true ]]; then
			echo_info "Executing ${out_bin_dir_relative}/${name}..."
			${outfile}&
		else
			echo_info "Changing directory to ${out_bin_dir_relative} and executing ${name}..."
			cd "${out_bin_dir}" || error_exit "Failed to change directories; bailing."
			./${name}&	
		fi
	fi
}

print_help() {
	echo -e "Usage: $0\n\t[--no-prompt]\tWon't prompt for user input to execute ${name}." \
			"\n\t[--no-execute]\tWon't execute ${name} after building." \
			"\n\t[--no-cd]\tWon't change directories to ${out_bin_dir_relative} before executing ${name}." \
			"\n\t[--help|-h]\tPrints this help message."	
}

_args=("$@")
_no_prompt=false
_no_execute=false
_no_cd=false

for i in "${_args[@]}"; do
	#echo "proccessing arg: '${i}'"
	case ${i} in
		"--no-prompt")
			_no_prompt=true
			;;
		"--no-execute")
			_no_execute=true
			;;
		"--no-cd")
			_no_cd=true
			;;
		"--help" | "-h")
			print_help
			exit 0
			;;
		*)
			echo "Unknown argument: ${i}"
			print_help
			exit 1
			;;
	esac
done

compile_systest "${_no_prompt}" "${_no_execute}" "${_no_cd}"
