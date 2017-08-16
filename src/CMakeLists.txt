

FILE(GLOB COMMON_SRC "fermat/common/*.cc")
FILE(GLOB COMMON_HEADER "fermat/common/*.h")
FILE(GLOB COMMON_DOUBLE_SRC "fermat/common/double-conversion/*.cc")
FILE(GLOB COMMON_FOUBLE_HEADER "fermat/common/double-conversion/*.h")

SET(ALL_SRC
	${COMMON_SRC}
	${COMMON_DOUBLE_SRC}
	)


add_library(fermatStatic STATIC ${ALL_SRC} )
#add_library(fermatShared SHARED ${ALL_SRC} )
target_link_libraries(fermatStatic  pthread)
#target_link_libraries(fermatShared  pthread)
set_target_properties(fermatStatic PROPERTIES OUTPUT_NAME 
	fermat_static)
#set_target_properties(fermatShared PROPERTIES OUTPUT_NAME 
#	fermat)


install(TARGETS fermatStatic 
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib 
	ARCHIVE DESTINATION lib
	 )

install (FILES ${COMMON_HEADER}  DESTINATION include/fermat/common)
install (FILES ${COMMON_FOUBLE_HEADER}  DESTINATION include/fermat/common/double-conversion)
