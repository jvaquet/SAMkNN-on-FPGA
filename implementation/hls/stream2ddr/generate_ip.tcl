############################################################
## Buildscript to generate stream2ddr ip
############################################################
open_project stream2ddr
set_top stream2ddr
add_files ./src/stream2ddr.cpp
add_files ./src/stream2ddr.h
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_export -flow syn -format ip_catalog -rtl vhdl -vivado_clock 10
csynth_design
export_design -flow impl -rtl vhdl -format ip_catalog
