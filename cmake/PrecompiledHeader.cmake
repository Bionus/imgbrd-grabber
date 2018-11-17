# Function for setting up precompiled headers. Usage:
#
#   add_library/executable(target
#       pchheader.c pchheader.cpp pchheader.h)
#
#   add_precompiled_header(target pchheader.h
#       [FORCEINCLUDE]
#       [SOURCE_C pchheader.c]
#       [SOURCE_CXX pchheader.cpp])
#
# Options:
#
#   FORCEINCLUDE: Add compiler flags to automatically include the
#   pchheader.h from every source file. Works with both GCC and
#   MSVC. This is recommended.
#
#   SOURCE_C/CXX: Specifies the .c/.cpp source file that includes
#   pchheader.h for generating the pre-compiled header
#   output. Defaults to pchheader.c. Only required for MSVC.
#
# Caveats:
#
#   * Its not currently possible to use the same precompiled-header in
#     more than a single target in the same directory (No way to set
#     the source file properties differently for each target).
#
#   * MSVC: A source file with the same name as the header must exist
#     and be included in the target (E.g. header.cpp). Name of file
#     can be changed using the SOURCE_CXX/SOURCE_C options.
#
# License:
#
# Copyright (C) 2009-2017 Lars Christensen <larsch@belunktum.dk>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the 'Software') deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

include(CMakeParseArguments)

macro(combine_arguments _variable)
  set(_result "")
  foreach(_element ${${_variable}})
    set(_result "${_result} \"${_element}\"")
  endforeach()
  string(STRIP "${_result}" _result)
  set(${_variable} "${_result}")
endmacro()

function(export_all_flags _filename)
  set(_include_directories "$<TARGET_PROPERTY:${_target},INCLUDE_DIRECTORIES>")
  set(_compile_definitions "$<TARGET_PROPERTY:${_target},COMPILE_DEFINITIONS>")
  set(_compile_flags "$<TARGET_PROPERTY:${_target},COMPILE_FLAGS>")
  set(_compile_options "$<TARGET_PROPERTY:${_target},COMPILE_OPTIONS>")
  set(_include_directories "$<$<BOOL:${_include_directories}>:-I$<JOIN:${_include_directories},\n-I>\n>")
  set(_compile_definitions "$<$<BOOL:${_compile_definitions}>:-D$<JOIN:${_compile_definitions},\n-D>\n>")
  set(_compile_flags "$<$<BOOL:${_compile_flags}>:$<JOIN:${_compile_flags},\n>\n>")
  set(_compile_options "$<$<BOOL:${_compile_options}>:$<JOIN:${_compile_options},\n>\n>")

  # CUSTOM:start
  get_target_property(_target_type ${_target} TYPE)
  if(_target_type STREQUAL "EXECUTABLE")
    set(_pic "$<$<BOOL:$<TARGET_PROPERTY:${_target},POSITION_INDEPENDENT_CODE>>:-fPIE\n>")
  else()
    set(_pic "$<$<BOOL:$<TARGET_PROPERTY:${_target},POSITION_INDEPENDENT_CODE>>:-fPIC\n>")
  endif()

  set(_standard_check "")
  set(_cxx_standard "$<TARGET_PROPERTY:${_target},CXX_STANDARD>")
  set(_cxx_extensions "$<TARGET_PROPERTY:${_target},CXX_EXTENSIONS>")
  set(_has_extensions "$<OR:$<STREQUAL:${_cxx_extensions},>,$<BOOL:${_cxx_extensions}>>") # CXX_EXTENSIONS defaults to true
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},17>,$<NOT:${_has_extensions}>>:${CMAKE_CXX17_STANDARD_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},14>,$<NOT:${_has_extensions}>>:${CMAKE_CXX14_STANDARD_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},11>,$<NOT:${_has_extensions}>>:${CMAKE_CXX11_STANDARD_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},98>,$<NOT:${_has_extensions}>>:${CMAKE_CXX98_STANDARD_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},17>,${_has_extensions}>:${CMAKE_CXX17_EXTENSION_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},14>,${_has_extensions}>:${CMAKE_CXX14_EXTENSION_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},11>,${_has_extensions}>:${CMAKE_CXX11_EXTENSION_COMPILE_OPTION}>")
  set(_standard_check "${_standard_check}$<$<AND:$<STREQUAL:${_cxx_standard},98>,${_has_extensions}>:${CMAKE_CXX98_EXTENSION_COMPILE_OPTION}>")

  file(GENERATE OUTPUT "${_filename}" CONTENT "${_compile_definitions}${_include_directories}${_compile_flags}${_compile_options} ${_standard_check} ${_pic}\n")
  # CUSTOM:end
endfunction()

function(add_precompiled_header _target _input)
  cmake_parse_arguments(_PCH "FORCEINCLUDE" "SOURCE_CXX;SOURCE_C" "" ${ARGN})

  get_filename_component(_input_we ${_input} NAME_WE)
  if(NOT _PCH_SOURCE_CXX)
    set(_PCH_SOURCE_CXX "${_input_we}.cpp")
  endif()
  if(NOT _PCH_SOURCE_C)
    set(_PCH_SOURCE_C "${_input_we}.c")
  endif()

  if(MSVC)
    set(_pch_cxx_pch "${CMAKE_CFG_INTDIR}/cxx_${_input_we}.pch")
    set(_pch_c_pch "${CMAKE_CFG_INTDIR}/c_${_input_we}.pch")

    get_target_property(sources ${_target} SOURCES)
    foreach(_source ${sources})
      set(_pch_compile_flags "")
      if(_source MATCHES \\.\(cc|cxx|cpp|c\)$)
        if(_source MATCHES \\.\(cpp|cxx|cc\)$)
          set(_pch_header "${_input}")
          set(_pch "${_pch_cxx_pch}")
        else()
          set(_pch_header "${_input}")
          set(_pch "${_pch_c_pch}")
        endif()

        if(_source STREQUAL "${_PCH_SOURCE_CXX}")
          set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" \"/Yc${_input}\"")
          set(_pch_source_cxx_found TRUE)
          set_source_files_properties("${_source}" PROPERTIES OBJECT_OUTPUTS "${_pch_cxx_pch}")
        elseif(_source STREQUAL "${_PCH_SOURCE_C}")
          set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_c_pch}\" \"/Yc${_input}\"")
          set(_pch_source_c_found TRUE)
          set_source_files_properties("${_source}" PROPERTIES OBJECT_OUTPUTS "${_pch_c_pch}")
        else()
          if(_source MATCHES \\.\(cpp|cxx|cc\)$)
            set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" \"/Yu${_input}\"")
            set(_pch_source_cxx_needed TRUE)
            set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_pch_cxx_pch}")
          else()
            set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_c_pch}\" \"/Yu${_input}\"")
            set(_pch_source_c_needed TRUE)
            set_source_files_properties("${_source}" PROPERTIES OBJECT_DEPENDS "${_pch_c_pch}")
          endif()
          if(_PCH_FORCEINCLUDE)
            set(_pch_compile_flags "${_pch_compile_flags} /FI${_input}")
          endif(_PCH_FORCEINCLUDE)
        endif()

        get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
        if(NOT _object_depends)
          set(_object_depends)
        endif()
        if(_PCH_FORCEINCLUDE)
          list(APPEND _object_depends "${CMAKE_CURRENT_SOURCE_DIR}/${_pch_header}")
        endif()

        set_source_files_properties(${_source} PROPERTIES
          COMPILE_FLAGS "${_pch_compile_flags}"
          OBJECT_DEPENDS "${_object_depends}")
      endif()
    endforeach()

    if(_pch_source_cxx_needed AND NOT _pch_source_cxx_found)
      message(FATAL_ERROR "A source file ${_PCH_SOURCE_CXX} for ${_input} is required for MSVC builds. Can be set with the SOURCE_CXX option.")
    endif()
    if(_pch_source_c_needed AND NOT _pch_source_c_found)
      message(FATAL_ERROR "A source file ${_PCH_SOURCE_C} for ${_input} is required for MSVC builds. Can be set with the SOURCE_C option.")
    endif()
  endif(MSVC)

  if(CMAKE_COMPILER_IS_GNUCXX)
    get_filename_component(_name ${_input} NAME)
    set(_pch_header "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    set(_pch_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/${_target}_pch")
    set(_pchfile "${_pch_binary_dir}/${_input}")
    set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_target}_pch/${_name}.gch")
    file(MAKE_DIRECTORY "${_outdir}")
    set(_output_cxx "${_outdir}/.c++")
    set(_output_c "${_outdir}/.c")

    set(_pch_flags_file "${_pch_binary_dir}/compile_flags.rsp")
    export_all_flags("${_pch_flags_file}")
    set(_compiler_FLAGS "@${_pch_flags_file}")
    add_custom_command(
      OUTPUT "${_pchfile}"
      COMMAND "${CMAKE_COMMAND}" -E copy "${_pch_header}" "${_pchfile}"
      DEPENDS "${_pch_header}"
      COMMENT "Updating ${_name}")
    add_custom_command(
      OUTPUT "${_output_cxx}"
      COMMAND "${CMAKE_CXX_COMPILER}" ${_compiler_FLAGS} -x c++-header -o "${_output_cxx}" "${_pchfile}"
      DEPENDS "${_pchfile}" "${_pch_flags_file}"
      COMMENT "Precompiling ${_name} for ${_target} (C++)")
    add_custom_command(
      OUTPUT "${_output_c}"
      COMMAND "${CMAKE_C_COMPILER}" ${_compiler_FLAGS} -x c-header -o "${_output_c}" "${_pchfile}"
      DEPENDS "${_pchfile}" "${_pch_flags_file}"
      COMMENT "Precompiling ${_name} for ${_target} (C)")

    get_property(_sources TARGET ${_target} PROPERTY SOURCES)
    foreach(_source ${_sources})
      set(_pch_compile_flags "")

      if(_source MATCHES \\.\(cc|cxx|cpp|c\)$)
        get_source_file_property(_pch_compile_flags "${_source}" COMPILE_FLAGS)
        if(NOT _pch_compile_flags)
          set(_pch_compile_flags)
        endif()
        separate_arguments(_pch_compile_flags)
        list(APPEND _pch_compile_flags -Winvalid-pch)
        if(_PCH_FORCEINCLUDE)
          list(APPEND _pch_compile_flags -include "${_pchfile}")
        else(_PCH_FORCEINCLUDE)
          list(APPEND _pch_compile_flags "-I${_pch_binary_dir}")
        endif(_PCH_FORCEINCLUDE)

        get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
        if(NOT _object_depends)
          set(_object_depends)
        endif()
        list(APPEND _object_depends "${_pchfile}")
        if(_source MATCHES \\.\(cc|cxx|cpp\)$)
          list(APPEND _object_depends "${_output_cxx}")
        else()
          list(APPEND _object_depends "${_output_c}")
        endif()

        combine_arguments(_pch_compile_flags)
        set_source_files_properties(${_source} PROPERTIES
          COMPILE_FLAGS "${_pch_compile_flags}"
          OBJECT_DEPENDS "${_object_depends}")
      endif()
    endforeach()
  endif(CMAKE_COMPILER_IS_GNUCXX)
endfunction()
