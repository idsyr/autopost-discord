cmake_minimum_required (VERSION 3.6)
project(discord_autopost)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT SLEEPY_DISCORD_CMAKE)
	add_subdirectory(sleepy-discord)
	include_directories(sleepy-discord/include)
endif()

add_executable(${PROJECT_NAME}_0.x
	app.hpp
	sleepy_discord_executor.cpp
)

target_link_libraries(${PROJECT_NAME}_0.x
	sleepy-discord
)
