#!/usr/bin/env sh

########################################################################

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2024 Ryan M. Lederman

########################################################################

# Clean up files previously generated. Run with -f to avoid interactive prompts.
_arg="-i"
if [ "${1}" = "-f" ]; then
    _arg="-f"
fi

# *.S (linker assembly)
rm ${_arg} ./*.S || true

# *.o (linker object)
rm ${_arg} ./*.o || true

# emblob_*.h (generated header)
rm ${_arg} emblob_*.h || true
