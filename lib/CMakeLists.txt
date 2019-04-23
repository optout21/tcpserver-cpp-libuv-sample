find_package(Threads)

# sources of lib
add_library(libtcp-libuv
    app.cpp
    app.hpp
    message.cpp
    message.hpp
    net_client.cpp
    net_client.hpp
    net_handler.cpp
    net_handler.hpp
	uv_socket.cpp
	uv_socket.hpp
)

# link with our library, and default platform libraries
target_link_libraries(libtcp-libuv
    uv
	${PLATFORM_LIBS}
	${CMAKE_THREAD_LIBS_INIT}
)
