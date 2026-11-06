# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "/home/william/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(atme_default_default_XC8_assemble_rule target)
    set(options
        "-c"
        "${MP_EXTRA_AS_PRE}"
        "-mcpu=ATmega168PB"
        "-x"
        "assembler-with-cpp"
        "-mdfp=${PACK_REPO_PATH}/Microchip/ATmega_DFP/3.6.299/xc8"
        "-Wl,--gc-sections"
        "-O0"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-gdwarf-3"
        "-mno-const-data-in-progmem"
        "-Wa,--defsym=__MPLAB_BUILD=1")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATmega168PB__"
        PRIVATE "XPRJ_default=default")
endfunction()
function(atme_default_default_XC8_assemblePreprocess_rule target)
    set(options
        "-c"
        "${MP_EXTRA_AS_PRE}"
        "-mcpu=ATmega168PB"
        "-x"
        "assembler-with-cpp"
        "-mdfp=${PACK_REPO_PATH}/Microchip/ATmega_DFP/3.6.299/xc8"
        "-Wl,--gc-sections"
        "-O0"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-gdwarf-3"
        "-mno-const-data-in-progmem"
        "-Wa,--defsym=__MPLAB_BUILD=1")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATmega168PB__"
        PRIVATE "XPRJ_default=default")
endfunction()
function(atme_default_default_XC8_compile_rule target)
    set(options
        "-c"
        "${MP_EXTRA_CC_PRE}"
        "-mcpu=ATmega168PB"
        "-x"
        "c"
        "-mdfp=${PACK_REPO_PATH}/Microchip/ATmega_DFP/3.6.299/xc8"
        "-Wl,--gc-sections"
        "-O0"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-gdwarf-3"
        "-mno-const-data-in-progmem")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATmega168PB__"
        PRIVATE "XPRJ_default=default")
endfunction()
function(atme_default_link_rule target)
    set(options
        "-Wl,-Map=mem.map"
        "${MP_EXTRA_LD_PRE}"
        "-mcpu=ATmega168PB"
        "-Wl,--defsym=__MPLAB_BUILD=1"
        "-mdfp=${PACK_REPO_PATH}/Microchip/ATmega_DFP/3.6.299/xc8"
        "-Wl,--gc-sections"
        "-O0"
        "-ffunction-sections"
        "-fdata-sections"
        "-fshort-enums"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-Wall"
        "-gdwarf-3"
        "-mno-const-data-in-progmem"
        "-Wl,--memorysummary,memoryfile.xml")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target} PRIVATE "XPRJ_default=default")
endfunction()
function(atme_default_objcopy_avr_rule target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${OBJCOPY}
        ARGS -O ihex ${atme_default_image_name} ${atme_default_image_base_name}.hex
        WORKING_DIRECTORY ${atme_default_output_dir})
endfunction()
