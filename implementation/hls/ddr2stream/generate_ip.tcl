############################################################
## Buildscript to generate ddr2stream ip
############################################################
open_project ddr2stream
set_top ddr2stream
add_files ./src/ddr2stream.cpp
add_files ./src/ddr2stream.h
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_export -flow syn -format ip_catalog -rtl vhdl -vivado_clock 10
csynth_design
export_design -flow impl -rtl vhdl -format ip_catalog
