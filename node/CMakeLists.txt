# sources of this exec
add_executable(tcp-libuv-node
	endpoint.cpp
	endpoint.hpp
	main.cpp
	node.cpp
    node.hpp
    peer_conn.cpp
    peer_conn.hpp
)

# link with our library, and default platform libraries
target_link_libraries(tcp-libuv-node
	libtcp-libuv
	${PLATFORM_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
)

if(NOT APPLE)
	install(TARGETS tcp-libuv-node
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
