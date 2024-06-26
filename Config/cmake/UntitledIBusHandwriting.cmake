function(custom_setup_step)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(IBus REQUIRED ibus-1.0)

    if(IBus_FOUND)
        include_directories(${IBus_INCLUDE_DIRS})
        link_directories(${IBus_LIBRARY_DIRS})
    endif(IBus_FOUND)
endfunction()

function(custom_compile_step)
    if (NOT WIN32)
        target_link_libraries(${APP_LIB_TARGET} ${IBus_LIBRARIES})
    endif()
endfunction()

set(USE_OS_MODULE ON)
set(ENABLE_UFONT_UTILS ON)

set(USE_THEME_MODULE ON)
set(USE_I18N_MODULE ON)
