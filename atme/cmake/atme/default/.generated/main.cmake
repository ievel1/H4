include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(atme_default_library_list )

# Handle files with suffix (s|as|asm|AS|ASM|As|aS|Asm), for group default-XC8
if(atme_default_default_XC8_FILE_TYPE_assemble)
add_library(atme_default_default_XC8_assemble OBJECT ${atme_default_default_XC8_FILE_TYPE_assemble})
    atme_default_default_XC8_assemble_rule(atme_default_default_XC8_assemble)
    list(APPEND atme_default_library_list "$<TARGET_OBJECTS:atme_default_default_XC8_assemble>")

endif()

# Handle files with suffix S, for group default-XC8
if(atme_default_default_XC8_FILE_TYPE_assemblePreprocess)
add_library(atme_default_default_XC8_assemblePreprocess OBJECT ${atme_default_default_XC8_FILE_TYPE_assemblePreprocess})
    atme_default_default_XC8_assemblePreprocess_rule(atme_default_default_XC8_assemblePreprocess)
    list(APPEND atme_default_library_list "$<TARGET_OBJECTS:atme_default_default_XC8_assemblePreprocess>")

endif()

# Handle files with suffix [cC], for group default-XC8
if(atme_default_default_XC8_FILE_TYPE_compile)
add_library(atme_default_default_XC8_compile OBJECT ${atme_default_default_XC8_FILE_TYPE_compile})
    atme_default_default_XC8_compile_rule(atme_default_default_XC8_compile)
    list(APPEND atme_default_library_list "$<TARGET_OBJECTS:atme_default_default_XC8_compile>")

endif()

# Handle files with suffix elf, for group default-XC8
if(atme_default_default_XC8_FILE_TYPE_objcopy_avr)
add_library(atme_default_default_XC8_objcopy_avr OBJECT ${atme_default_default_XC8_FILE_TYPE_objcopy_avr})
    atme_default_default_XC8_objcopy_avr_rule(atme_default_default_XC8_objcopy_avr)
    list(APPEND atme_default_library_list "$<TARGET_OBJECTS:atme_default_default_XC8_objcopy_avr>")

endif()


# Main target for this project
add_executable(atme_default_image_coy9qmUS ${atme_default_library_list})

set_target_properties(atme_default_image_coy9qmUS PROPERTIES
    OUTPUT_NAME "default"
    SUFFIX ".elf"
    ADDITIONAL_CLEAN_FILES "${output_extensions}"
    RUNTIME_OUTPUT_DIRECTORY "${atme_default_output_dir}")
target_link_libraries(atme_default_image_coy9qmUS PRIVATE ${atme_default_default_XC8_FILE_TYPE_link})
# Add the link options from the rule file.
atme_default_link_rule( atme_default_image_coy9qmUS)


#Add objcopy steps
atme_default_objcopy_avr_rule(atme_default_image_coy9qmUS)

