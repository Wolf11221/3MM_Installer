# CMakeList.txt : CMake project for 3MM_Installer, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (3MM_Installer "3MM_Installer.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 3MM_Installer PROPERTY CXX_STANDARD 20)
endif()

add_subdirectory(extlibs/11Zip) # Path to the 11zip
target_link_libraries(3MM_Installer elzip)

# TODO: Add tests and install targets if needed.