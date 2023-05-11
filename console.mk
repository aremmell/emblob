#####################################################
#                                                   #
# 				   console.mk                       #
#                                                   #
#     Utility functions for reporting important     #
#     make progress messages to the user by way     #
#     of color-coded console messages.              #
#                                                   #
#####################################################

# escape sequences used to create color-coded console messages.
 # bright-red
_ERROR_SEQ   := 1;31;49
# bright-yellow
_WARNING_SEQ := 1;33;49
# normal-light green
_SUCCESS_SEQ := 1;92;49
# bright-light gray
_INFO_SEQ    := 1;37;49
# normal-dark gray
_DEBUG_SEQ   := 0;90;49

export CONSOLE_MSG_ERROR := 5

_MINIMUM_MSG_LEVEL := 1

# corresponding numeric values for each type of console message,
# listed in descending order of relative importance. error messages
# are of the highest importance, while debug messages are of the least.

define set_min_console_msg_level =
	_MINIMUM_MSG_LEVEL = $(1)
endef

define echo_color =
	@echo "\033[$(1)m$(2)\033[0m"
endef

define echo_error =
	$(call echo_color,$(_ERROR_SEQ),$(1))
endef

define echo_warning =
	$(call echo_color,$(_WARNING_SEQ),$(1))
endef

define echo_success =
	$(call echo_color,$(_SUCCESS_SEQ),$(1))
endef

define echo_info =
	$(call echo_color,$(_INFO_SEQ),$(1))
endef

define echo_debug =
	$(call echo_color,$(_DEBUG_SEQ),$(1))
endef

# specializations of the above functions which are
# likely to be used frequently, and are defined here to
# prevent redudancy, extraneous code, and discrepancies in messages.

define echo_build_success =
	$(call echo_success,built $(1) successfully [$$(date -R)])
endef
