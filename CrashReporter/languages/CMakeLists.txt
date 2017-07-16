set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt5LinguistTools)

file(GLOB TS_FILES "*.ts")
qt5_create_translation(QM_FILES ${SOURCES} ${TS_FILES})
add_custom_target(CrashReporter_translations ALL DEPENDS ${QM_FILES})

if(APPLE AND UNIX)
    install(FILES ${QM_FILES} DESTINATION Grabber.app/Contents/Resources/crashreporter)
else()
    install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/Grabber/crashreporter)
endif()
