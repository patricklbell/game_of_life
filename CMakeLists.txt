cmake_minimum_required (VERSION 3.8)

project ("game_of_life")

find_package(SFML 2.5.1 COMPONENTS graphics audio window system REQUIRED)
add_executable (game_of_life "game_of_life.cpp")
target_link_libraries(game_of_life sfml-graphics sfml-window sfml-audio)