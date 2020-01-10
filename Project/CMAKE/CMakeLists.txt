cmake_minimum_required(VERSION 3.6)



#Firstly, define the project name
project(VirtualMeter LANGUAGES ASM C CXX)



#Secondly, indexing the project files & search paths
set(sources
 ../../Devices/eeprom/Src/eeprom_1.c
 ../../Devices/eeprom/Src/eeprom_2.c
 ../../Devices/rtc/Src/rtc.c
 ../../Devices/serial/Src/rs485_1.c
 ../../Devices/serial/Src/rs485_2.c
 ../../Devices/battery/Src/battery.c
 ../../Devices/leds/Src/leds.c
 ../../Devices/buzzer/Src/buzzer.c
 ../../Devices/relay/Src/relay.c
 ../../Devices/lcd/Src/lcd.c
 ../../Devices/basic/Src/cpu.c
 ../../Devices/basic/Src/delay.c
 ../../Devices/basic/Src/jiffy.c
 ../../Devices/basic/Src/power.c
 ../../Devices/sensor/Src/button.c
 ../../Devices/sensor/Src/magnetic.c
 ../../Devices/keys/Src/keys.c
 ../../Devices/flash/Src/flash.c
 ../../Devices/eeprom/Src/eeprom.c
 ../../Devices/metering/Src/meter.c
 ../../Devices/buses/Src/vuart1.c
 ../../Devices/buses/Src/vuart2.c
 ../../Devices/buses/Src/vuart3.c
 ../../Devices/buses/Src/vuart4.c
 ../../Libraries/Info/Src/info.c
 ../../Libraries/trace/Src/trace.c
 ../../Libraries/Convert/Src/axdr.c
 ../../Libraries/Convert/Src/bcd.c
 ../../Libraries/Check/Src/crc.c
 ../../Libraries/Mbed/tls/library/aes.c
 ../../Libraries/Mbed/tls/library/aesni.c
 ../../Libraries/Mbed/tls/library/arc4.c
 ../../Libraries/Mbed/tls/library/aria.c
 ../../Libraries/Mbed/tls/library/asn1parse.c
 ../../Libraries/Mbed/tls/library/asn1write.c
 ../../Libraries/Mbed/tls/library/base64.c
 ../../Libraries/Mbed/tls/library/bignum.c
 ../../Libraries/Mbed/tls/library/blowfish.c
 ../../Libraries/Mbed/tls/library/camellia.c
 ../../Libraries/Mbed/tls/library/ccm.c
 ../../Libraries/Mbed/tls/library/certs.c
 ../../Libraries/Mbed/tls/library/chacha20.c
 ../../Libraries/Mbed/tls/library/chachapoly.c
 ../../Libraries/Mbed/tls/library/cipher.c
 ../../Libraries/Mbed/tls/library/cipher_wrap.c
 ../../Libraries/Mbed/tls/library/cmac.c
 ../../Libraries/Mbed/tls/library/ctr_drbg.c
 ../../Libraries/Mbed/tls/library/debug.c
 ../../Libraries/Mbed/tls/library/des.c
 ../../Libraries/Mbed/tls/library/dhm.c
 ../../Libraries/Mbed/tls/library/ecdh.c
 ../../Libraries/Mbed/tls/library/ecdsa.c
 ../../Libraries/Mbed/tls/library/ecjpake.c
 ../../Libraries/Mbed/tls/library/ecp.c
 ../../Libraries/Mbed/tls/library/ecp_curves.c
 ../../Libraries/Mbed/tls/library/entropy.c
 ../../Libraries/Mbed/tls/library/entropy_poll.c
 ../../Libraries/Mbed/tls/library/error.c
 ../../Libraries/Mbed/tls/library/gcm.c
 ../../Libraries/Mbed/tls/library/havege.c
 ../../Libraries/Mbed/tls/library/hkdf.c
 ../../Libraries/Mbed/tls/library/hmac_drbg.c
 ../../Libraries/Mbed/tls/library/md.c
 ../../Libraries/Mbed/tls/library/md_wrap.c
 ../../Libraries/Mbed/tls/library/md2.c
 ../../Libraries/Mbed/tls/library/md4.c
 ../../Libraries/Mbed/tls/library/md5.c
 ../../Libraries/Mbed/tls/library/memory_buffer_alloc.c
 ../../Libraries/Mbed/tls/library/net_sockets.c
 ../../Libraries/Mbed/tls/library/nist_kw.c
 ../../Libraries/Mbed/tls/library/oid.c
 ../../Libraries/Mbed/tls/library/padlock.c
 ../../Libraries/Mbed/tls/library/pem.c
 ../../Libraries/Mbed/tls/library/pk.c
 ../../Libraries/Mbed/tls/library/pk_wrap.c
 ../../Libraries/Mbed/tls/library/pkcs5.c
 ../../Libraries/Mbed/tls/library/pkcs11.c
 ../../Libraries/Mbed/tls/library/pkcs12.c
 ../../Libraries/Mbed/tls/library/pkparse.c
 ../../Libraries/Mbed/tls/library/pkwrite.c
 ../../Libraries/Mbed/tls/library/platform.c
 ../../Libraries/Mbed/tls/library/platform_util.c
 ../../Libraries/Mbed/tls/library/poly1305.c
 ../../Libraries/Mbed/tls/library/ripemd160.c
 ../../Libraries/Mbed/tls/library/rsa.c
 ../../Libraries/Mbed/tls/library/rsa_internal.c
 ../../Libraries/Mbed/tls/library/sha1.c
 ../../Libraries/Mbed/tls/library/sha256.c
 ../../Libraries/Mbed/tls/library/sha512.c
 ../../Libraries/Mbed/tls/library/ssl_cache.c
 ../../Libraries/Mbed/tls/library/ssl_ciphersuites.c
 ../../Libraries/Mbed/tls/library/ssl_cli.c
 ../../Libraries/Mbed/tls/library/ssl_cookie.c
 ../../Libraries/Mbed/tls/library/ssl_srv.c
 ../../Libraries/Mbed/tls/library/ssl_ticket.c
 ../../Libraries/Mbed/tls/library/ssl_tls.c
 ../../Libraries/Mbed/tls/library/threading.c
 ../../Libraries/Mbed/tls/library/timing.c
 ../../Libraries/Mbed/tls/library/version.c
 ../../Libraries/Mbed/tls/library/version_features.c
 ../../Libraries/Mbed/tls/library/x509.c
 ../../Libraries/Mbed/tls/library/x509_create.c
 ../../Libraries/Mbed/tls/library/x509_crl.c
 ../../Libraries/Mbed/tls/library/x509_crt.c
 ../../Libraries/Mbed/tls/library/x509_csr.c
 ../../Libraries/Mbed/tls/library/x509write_crt.c
 ../../Libraries/Mbed/tls/library/x509write_csr.c
 ../../Libraries/Mbed/tls/library/xtea.c
 ../../Kernel/Src/heap.c
 ../../Kernel/Src/kernel.c
 ../../Kernel/Src/disk.c
 ../../Kernel/Src/nvram.c
 ../../Tasks/Tasks/Src/tasks.c
 ../../Tasks/Calendar/Src/task_calendar.c
 ../../Tasks/Disconnect/Src/task_disconnect.c
 ../../Tasks/Logger/Src/task_logger.c
 ../../Tasks/Timed/Src/task_timed.c
 ../../Tasks/Metering/Src/task_metering.c
 ../../Tasks/Display/Src/task_display.c
 ../../Tasks/Keyboard/Src/task_keyboard.c
 ../../Tasks/Comm/Src/task_comm.c
 ../../Tasks/Protocols/Core/Src/task_protocol.c
 ../../Tasks/Protocols/proto_dlms/Src/dlms_lexicon.c
 ../../Tasks/Protocols/proto_dlms/Src/proto_dlms.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_association.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_clock.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_hdlc_setup.c
 ../../Tasks/Protocols/proto_dlms/Src/dlms_application.c
 ../../Tasks/Protocols/proto_dlms/Src/dlms_association.c
 ../../Tasks/Protocols/proto_dlms/Src/dlms_utilities.c
 ../../Tasks/Protocols/proto_dlms/Src/hdlc_datalink.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_data.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_extendedregister.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_register.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_exception.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_imagetransfer.c
 ../../Tasks/Protocols/proto_dlms/Src/cosem_objects_security_setup.c
 ../../Tasks/Protocols/proto_xmodem/Src/proto_xmodem.c)

set(includes 
 ../../Devices/common/Inc
 ../../Devices/battery/Inc
 ../../Devices/basic/Inc
 ../../Devices/buses/Inc
 ../../Devices/leds/Inc
 ../../Devices/eeprom/Inc
 ../../Devices/buzzer/Inc
 ../../Devices/keys/Inc
 ../../Devices/lcd/Inc
 ../../Devices/rtc/Inc
 ../../Devices/sensor/Inc
 ../../Devices/serial/Inc
 ../../Devices/metering/Inc
 ../../Devices/relay/Inc
 ../../Devices/flash/Inc
 ../../Libraries/Check/Inc
 ../../Libraries/Info/Inc
 ../../Libraries/Mbed/tls
 ../../Libraries/Convert/Inc
 ../../Libraries/trace/Inc
 ../../Kernel/Inc
 ../../Tasks/Tasks/Inc
 ../../Tasks/Comm/Inc
 ../../Tasks/Protocols/Core/Inc
 ../../Tasks/Protocols/proto_1107/Inc
 ../../Tasks/Protocols/proto_dlms/Inc
 ../../Tasks/Protocols/proto_dlt645/Inc
 ../../Tasks/Protocols/proto_dlt698/Inc
 ../../Tasks/Protocols/proto_xmodem/Inc
 ../../Tasks/Protocols/proto_modbus/Inc
 ../../Tasks/Timed/Inc
 ../../Tasks/Calendar/Inc
 ../../Tasks/Console/Inc
 ../../Tasks/Display/Inc
 ../../Tasks/Disconnect/Inc
 ../../Tasks/Keyboard/Inc
 ../../Tasks/Logger/Inc
 ../../Tasks/Metering/Inc
 ../../Tasks/Utilities/Inc)




#Then we define the platform specific files & paths
if(WIN32 OR UNIX)
	list(APPEND sources 
			 ../../Devices/serial/Src/console.c
			 ../../Devices/common/Src/comm_socket.c
			 ../../Libraries/Lua/Src/lapi.c
			 ../../Libraries/Lua/Src/lauxlib.c
			 ../../Libraries/Lua/Src/lbaselib.c
			 ../../Libraries/Lua/Src/lcode.c
			 ../../Libraries/Lua/Src/ldblib.c
			 ../../Libraries/Lua/Src/ldebug.c
			 ../../Libraries/Lua/Src/ldo.c
			 ../../Libraries/Lua/Src/ldump.c
			 ../../Libraries/Lua/Src/lfunc.c
			 ../../Libraries/Lua/Src/lgc.c
			 ../../Libraries/Lua/Src/linit.c
			 ../../Libraries/Lua/Src/liolib.c
			 ../../Libraries/Lua/Src/llex.c
			 ../../Libraries/Lua/Src/lmathlib.c
			 ../../Libraries/Lua/Src/lmem.c
			 ../../Libraries/Lua/Src/loadlib.c
			 ../../Libraries/Lua/Src/lobject.c
			 ../../Libraries/Lua/Src/lopcodes.c
			 ../../Libraries/Lua/Src/loslib.c
			 ../../Libraries/Lua/Src/lparser.c
			 ../../Libraries/Lua/Src/lstate.c
			 ../../Libraries/Lua/Src/lstring.c
			 ../../Libraries/Lua/Src/lstrlib.c
			 ../../Libraries/Lua/Src/ltable.c
			 ../../Libraries/Lua/Src/ltablib.c
			 ../../Libraries/Lua/Src/ltm.c
			 ../../Libraries/Lua/Src/lundump.c
			 ../../Libraries/Lua/Src/lvm.c
			 ../../Libraries/Lua/Src/lzio.c
			 ../../Libraries/Lua/Src/print.c
			 ../../Kernel/Src/vm_basis.c
			 ../../Tasks/Comm/Src/vm_comm.c
			 ../../Tasks/Metering/Src/vm_metering.c
			 ../../Tasks/Calendar/Src/vm_calendar.c
			 ../../Tasks/Disconnect/Src/vm_disconnect.c
			 ../../Tasks/Display/Src/vm_display.c
			 ../../Tasks/Keyboard/Src/vm_keyboard.c
			 ../../Tasks/Logger/Src/vm_logger.c
			 ../../Tasks/Timed/Src/vm_timed.c
			 ../../Tasks/Protocols/Core/Src/vm_protocol.c
			 ../../Tasks/Console/Src/task_console.c)

	list(APPEND includes 
			 ../../Libraries/Lua/Inc)
else()
	list(APPEND sources 
			 ../../Devices/basic/Src/STM32F0xx/cmsis/gcc/startup_stm32f0xx.s
			 ../../Devices/basic/Src/STM32F0xx/cmsis/system_stm32f0xx.c
			 ../../Devices/basic/Src/STM32F0xx/Interrupts/stm32f0xx_it.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_adc.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_can.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_cec.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_comp.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_crc.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_crs.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_dac.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_dbgmcu.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_dma.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_exti.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_flash.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_gpio.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_i2c.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_iwdg.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_misc.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_pwr.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_rcc.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_rtc.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_spi.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_syscfg.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_tim.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_usart.c
			 ../../Devices/basic/Src/STM32F0xx/STM32F0xx_StdPeriph_Lib/stm32f0xx_wwdg.c
			 ../../Devices/buses/Src/viic1.c
			 ../../Devices/buses/Src/viic2.c
			 ../../Devices/buses/Src/viic3.c
			 ../../Devices/buses/Src/vspi1.c
			 ../../Devices/buses/Src/vspi2.c)

	list(APPEND includes 
			 ../../Devices/basic/Inc/STM32F0xx/cmsis
			 ../../Devices/basic/Inc/STM32F0xx/Interrupts
			 ../../Devices/basic/Inc/STM32F0xx/STM32F0xx_StdPeriph_Lib)
endif()




#Specify the compile parameters
if(WIN32 OR UNIX)
if(WIN32)
	list(APPEND flags -O3 -std=c99)
	list(APPEND libraries ws2_32 Winmm)
if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	list(APPEND defines MAKE_RUN_FOR_DEBUG)
endif()
else()
	list(APPEND flags -O3 -std=gnu99)
	list(APPEND libraries pthread m rt)
if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	list(APPEND defines MAKE_RUN_FOR_DEBUG)
endif()
endif()
else()
	set(CMAKE_ASM_FLAGS "-march=armv6-m -mthumb -msoft-float -mthumb-interwork -ffunction-sections -fdata-sections -Os -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs")
	set(CMAKE_C_FLAGS "-std=gnu99 -march=armv6-m -mthumb -msoft-float -mthumb-interwork -ffunction-sections -fdata-sections -Os -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs")
	set(CMAKE_CXX_FLAGS "-std=c++11 -march=armv6-m -mthumb -msoft-float -mthumb-interwork -ffunction-sections -fdata-sections -Os -fno-common -fmessage-length=0 -specs=nosys.specs -specs=nano.specs")
	set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F0.ld")
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")
	add_definitions(-D__weak=__attribute__\(\(weak\)\) -D__packed=__attribute__\(\(__packed__\)\))
	add_definitions(-DUSE_STDPERIPH_DRIVER -DSTM32F091 -DDEMO_STM32F091)
if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	add_definitions(-DMAKE_RUN_FOR_DEBUG)
endif()
endif()




#Make project
if(WIN32 OR UNIX)
	add_executable(${PROJECT_NAME} ${sources})
	include_directories(${includes})
	target_compile_definitions(${PROJECT_NAME} PRIVATE ${defines})
	target_compile_options(${PROJECT_NAME} PRIVATE ${flags})
	target_link_libraries(${PROJECT_NAME} ${libraries})
else()
	add_executable(${PROJECT_NAME}.elf ${sources} ${LINKER_SCRIPT})
	include_directories(${includes})
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")
	add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
         COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin
		 COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex
         COMMENT "Output converting")
endif()
