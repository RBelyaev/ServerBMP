cmake_minimum_required(VERSION 3.10)

project(client)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)

    add_executable(client main.cpp Client.cpp)

    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES}) 

else()
    message("Boost not found")
endif() 
