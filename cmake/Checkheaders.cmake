INCLUDE(${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)

##########################################
#
# help function for check header files
##########################################

function(checkHeaderAssert HFILE)
	check_include_file_cxx(${ARGV0} HAVE_${ARGV0})
	if(NOT HAVE_${ARGV0})
		message(FATAL_ERROR "not have ${ARGV0}")
	else()
		message(STATUS "have ${ARGV0}")
	endif()
endfunction(checkHeaderAssert)

function(checkHeaderWarn HFILE)
	check_include_file_cxx(${ARGV0} HAVE_${ARGV0})
	if(NOT HAVE_${ARGV0})
		message(WARNING "not have ${ARGV0}")
	else()
		message(WARNING "have ${ARGV0}")
	endif()
endfunction(checkHeaderWarn)
##############################################

checkHeaderAssert(cstdint)
checkHeaderAssert(cstddef)
checkHeaderAssert(cassert)
checkHeaderAssert(cstdarg)
checkHeaderAssert(cstdlib)
checkHeaderAssert(cstring)
checkHeaderAssert(cstdio)
checkHeaderAssert(cmath)
checkHeaderAssert(unordered_map)
checkHeaderAssert(unordered_set)
checkHeaderAssert(sys/epoll.h)
checkHeaderAssert(poll.h)
checkHeaderAssert(functional)
checkHeaderAssert(memory)
checkHeaderAssert(type_traits)
checkHeaderAssert(sys/eventfd.h)
checkHeaderAssert(sys/timerfd.h)
checkHeaderAssert(sys/signalfd.h)