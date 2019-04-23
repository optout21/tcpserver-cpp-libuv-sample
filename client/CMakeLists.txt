# sources of this exec
add_executable(tcp-libuv-client
	client.cpp
)

# link with our library, and default platform libraries
target_link_libraries(tcp-libuv-client
	libtcp-libuv
	${PLATFORM_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
)

if(NOT APPLE)
	install(TARGETS tcp-libuv-client
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
