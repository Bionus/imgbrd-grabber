# Replicate the behavior of "list(FILTER ${LIST} EXCLUDE REGEX ${REGEX})" on older CMake versions
function(listFilterRegex LIST REGEX)
    foreach(ITEM ${${LIST}})
        if(NOT ITEM MATCHES "${REGEX}")
            list(APPEND OUTPUT ${ITEM})
        endif()
    endforeach()
    set(${LIST} ${OUTPUT} PARENT_SCOPE)
endfunction()
