cmake_minimum_required (VERSION 3.1) # 3.1 adds C_STANDARD, target_compile_features
project (libsh C)

include (${CMAKE_ROOT}/Modules/CheckSymbolExists.cmake)

set (CMAKE_REQUIRED_LIBRARIES ${CMAKE_DL_LIBS} -lcurl)

# _POSIX_C_SOURCE, because I want to get all POSIX functions
# _GNU_SOURCE, т. к. я хочу получить strcasestr, splice и другие функции
set (CMAKE_REQUIRED_DEFINITIONS "-D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE=1")

check_symbol_exists (sigsetjmp "setjmp.h" SH_HAVE_sigsetjmp)

include (funcs.cmake)

configure_file (ex.h.in "${PROJECT_BINARY_DIR}/ex.h")
configure_file (funcs.h.in "${PROJECT_BINARY_DIR}/funcs.h")

include_directories ("${PROJECT_BINARY_DIR}")

add_library (sh SHARED ex.c funcs.c etc.c)
set_property (TARGET sh PROPERTY C_STANDARD 11) # Because we want noreturn. Cmake can fail back to C99 or C89
target_link_libraries (sh ${CMAKE_DL_LIBS} -lcurl)

# I have to comment this, because cmake 3.1.0 doesn't know that Visual Studio 2013 supports varadic macros
# target_compile_features(sh PUBLIC c_variadic_macros)

install (TARGETS sh DESTINATION lib)
install (FILES "${PROJECT_BINARY_DIR}/ex.h" "${PROJECT_BINARY_DIR}/funcs.h" etc.h arg.hpp DESTINATION include/libsh)
install (FILES libsh.h DESTINATION include)
