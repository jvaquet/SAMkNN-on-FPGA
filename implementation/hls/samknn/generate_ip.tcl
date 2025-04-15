############################################################
## Buildscript to generate samknn ip
############################################################
open_project samknn
set_top samknn_top
add_files ./src/common/best_class.cpp
add_files ./src/common/best_class.h
add_files ./src/kn_unit/calc_dist.cpp
add_files ./src/kn_unit/calc_dist.h
add_files ./src/opt_size/candidate_size_gen.cpp
add_files ./src/opt_size/candidate_size_gen.h
add_files ./src/common/correct_class.cpp
add_files ./src/common/correct_class.h
add_files ./src/kn_unit/farthest_correct.cpp
add_files ./src/kn_unit/farthest_correct.h
add_files ./src/kn_unit/k_best_collector.cpp
add_files ./src/kn_unit/k_best_collector.h
add_files ./src/knn_out/k_best_combiner.cpp
add_files ./src/knn_out/k_best_combiner.h
add_files ./src/kn_unit/kn_unit.cpp
add_files ./src/kn_unit/kn_unit.h
add_files ./src/knn_out/knn_out.cpp
add_files ./src/knn_out/knn_out.h
add_files ./src/memory/memory.cpp
add_files ./src/memory/memory.h
add_files ./src/opt_size/performance_counter.cpp
add_files ./src/opt_size/performance_counter.h
add_files ./src/knn_out/prediction_histories.cpp
add_files ./src/knn_out/prediction_histories.h
add_files ./src/kn_unit/runs_combiner.cpp
add_files ./src/kn_unit/runs_combiner.h
add_files ./src/samknn.cpp
add_files ./src/samknn.h
add_files ./src/samknn_config.h
add_files ./src/samknn_types.h
add_files -tb ./test/main.cpp -cflags "-Wno-unknown-pragmas"
add_files -tb ./test/samknn_test.cpp -cflags "-Wno-unknown-pragmas"
add_files -tb ./test/tests.h -cflags "-Wno-unknown-pragmas"
open_solution "solution1" -flow_target vivado
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default
config_export -flow impl -format ip_catalog -rtl vhdl -vivado_clock 10
config_cosim -rtl vhdl -tool xsim
csynth_design
export_design -flow impl -rtl vhdl -format ip_catalog
