cmake_minimum_required(VERSION 3.0)

project(psi-plugins C CXX)

include(FindPkgConfig)
include(GNUInstallDirs)

set(LIB_EXT ".so")
configure_file(midi_gate-psi.lv2/manifest.ttl.in      ${CMAKE_BINARY_DIR}/midi_gate-psi.lv2/manifest.ttl)
configure_file(midi_rnd-psi.lv2/manifest.ttl.in       ${CMAKE_BINARY_DIR}/midi_rnd-psi.lv2/manifest.ttl)
configure_file(sidechain_gate-psi.lv2/manifest.ttl.in ${CMAKE_BINARY_DIR}/sidechain_gate-psi.lv2/manifest.ttl)
configure_file(super_welle.lv2/manifest.ttl.in        ${CMAKE_BINARY_DIR}/super_welle.lv2/manifest.ttl)

pkg_search_module(NTK REQUIRED ntk)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

set(MIDIGATE_SRCS midi_gate-psi.lv2/midi_gate.c)
set(MIDIGATE_UI_SRCS midi_gate-psi.lv2/midi_gate_ui.cxx
                     midi_gate-psi.lv2/midi_gate_widget.cxx)

set(MIDIRND_SRCS midi_rnd-psi.lv2/midi_rnd.c)

set(SIDECHAIN_SRCS sidechain_gate-psi.lv2/sidechain_gate.c)

set(SIDECHAIN_UI_SRCS sidechain_gate-psi.lv2/sidechain_gate_ui.cxx
                      sidechain_gate-psi.lv2/sidechain_gate_widget.cxx)

set(SUPERWELLE_SRCS super_welle.lv2/super_welle.c
                    super_welle.lv2/synth.c)

set(SUPERWELLE_UI_SRCS super_welle.lv2/super_welle_ui.cxx
                       super_welle.lv2/super_welle_widget.cxx)

include_directories(.
  common
  UI
  midi_gate-psi.lv2
  midi_rnd-psi.lv2
  sidechain_gate-psi.lv2
  super_welle.lv2
  ${NTK_INCLUDE_DIRS})

add_library(midi_gate SHARED ${MIDIGATE_SRCS})
add_library(midi_gate_ui SHARED ${MIDIGATE_UI_SRCS})
target_link_libraries(midi_gate_ui ${NTK_LIBRARIES})

add_library(midi_rnd SHARED ${MIDIRND_SRCS})

add_library(sidechain_gate SHARED ${SIDECHAIN_SRCS})
add_library(sidechain_gate_ui SHARED ${SIDECHAIN_UI_SRCS})
target_link_libraries(sidechain_gate_ui ${NTK_LIBRARIES})

add_library(super_welle SHARED ${SUPERWELLE_SRCS})
add_library(super_welle_ui SHARED ${SUPERWELLE_UI_SRCS})
target_link_libraries(super_welle_ui ${NTK_LIBRARIES})

# midi_gate
install(TARGETS midi_gate
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_gate-psi.lv2/)
install(TARGETS midi_gate_ui
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_gate-psi.lv2/)
install(FILES midi_gate-psi.lv2/midi_gate.ttl
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_gate-psi.lv2/)
install(FILES ${CMAKE_BINARY_DIR}/midi_gate-psi.lv2/manifest.ttl
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_gate-psi.lv2/)

# midi_rnd
install(TARGETS midi_rnd
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_rnd-psi.lv2/)
install(FILES midi_rnd-psi.lv2/midi_rnd.ttl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_rnd-psi.lv2/)
install(FILES ${CMAKE_BINARY_DIR}/midi_rnd-psi.lv2/manifest.ttl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/midi_rnd-psi.lv2/)

# sidechain_gate
install(TARGETS sidechain_gate
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/sidechain_gate-psi.lv2/)
install(TARGETS sidechain_gate_ui
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/sidechain_gate-psi.lv2/)
install(FILES sidechain_gate-psi.lv2/sidechain_gate.ttl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/sidechain_gate-psi.lv2/)
install(FILES ${CMAKE_BINARY_DIR}/midi_rnd-psi.lv2/manifest.ttl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/sidechain_gate-psi.lv2/)

# super_welle
install(TARGETS super_welle
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/super_welle.lv2/)
install(TARGETS super_welle_ui
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/super_welle.lv2/)
install(FILES super_welle.lv2/super_welle.ttl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/super_welle.lv2/)
install(FILES ${CMAKE_BINARY_DIR}/super_welle.lv2/manifest.ttl
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/lv2/super_welle.lv2/)
