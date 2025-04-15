
#include "../samknn_config.h"
#include "../samknn_types.h"
#include "memory.h"
#include "../kn_unit/calc_dist.h"


namespace Memory {


    memory_idx_t stm_pointer = 0;
    memory_idx_t ltm_pointer = MEM_SIZE - 1;
    bool active_buffer = false;

    datapoint_t data[2][MEM_SIZE][N_DATAPOINT_DIMENSIONS]; 
    label_t labels[2][MEM_SIZE];
    bool invalid_flags[MEM_SIZE];
    memory_idx_t n_ltm_samples_per_class[N_CLASSES];

    void init() {
        #pragma HLS array_partition variable=labels complete dim=1
        #pragma HLS array_partition variable=labels type=cyclic factor=N_PARALLEL_RUNS dim=2
        #pragma HLS array_partition variable=data complete dim=1
        #pragma HLS array_partition variable=data type=cyclic factor=N_PARALLEL_RUNS dim=2
        #pragma HLS array_partition variable=data complete dim=3

        memory_invalid_init:
        for(memory_idx_t i = 0; i < MEM_SIZE; i++) {
            invalid_flags[i] = 0;
        }

        memory_ltm_label_counter_init:
        for(class_idx_t i = 0; i < N_CLASSES; i++) {
            n_ltm_samples_per_class[i] = 0;
        }
    }

    void append_stm(datapoint_t datapoint[N_DATAPOINT_DIMENSIONS], label_t label) {
        //#pragma HLS INLINE
        memory_append_stm_datacopy:
        for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
            //#pragma HLS UNROLL
            data[active_buffer][stm_pointer][i] = datapoint[i];
        }
        labels[active_buffer][stm_pointer] = label;
        stm_pointer++;
    }

    void invalidate(memory_idx_t address) {
        //#pragma HLS INLINE
        invalid_flags[address] = true;
    }


    void clean(memory_idx_t& n_stm_samples_cleaned) {
        bool read_buf = active_buffer;
        bool write_buf = !active_buffer;

        // Clean STM
        memory_idx_t write_addr = 0;
        memory_clean_stm_loop:
        for(memory_idx_t read_addr = 0; read_addr < stm_pointer; read_addr++) {
            #pragma HLS loop_tripcount min=0 max=MEM_SIZE avg=MEM_SIZE/2

            if(invalid_flags[read_addr]) {
                invalid_flags[read_addr] = false;
                continue;
            }

            // Copy data
            memory_clean_stm_copydata:
            for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
                //#pragma HLS UNROLL
                data[write_buf][write_addr][i] = data[read_buf][read_addr][i];
            }
            labels[write_buf][write_addr] = labels[read_buf][read_addr];
            write_addr++;
        }
        // Count cleaned samples and adjust STM pointer
        n_stm_samples_cleaned = stm_pointer - write_addr;
        stm_pointer = write_addr;

        // Clean LTM
        write_addr = MEM_SIZE - 1;
        memory_clean_ltm_loop:
        for(memory_idx_t read_addr = write_addr; read_addr > ltm_pointer; read_addr--) {
            #pragma HLS loop_tripcount min=0 max=MEM_SIZE avg=MEM_SIZE/2

            if(invalid_flags[read_addr]) {
                n_ltm_samples_per_class[labels[read_buf][read_addr]]--;
                invalid_flags[read_addr] = false;
                continue;
            }

            // Copy data
            memory_clean_ltm_copydata:
            for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
                //#pragma HLS UNROLL
                data[write_buf][write_addr][i] = data[read_buf][read_addr][i];
            }
            labels[write_buf][write_addr] = labels[read_buf][read_addr];
            write_addr--;
        }

        // Adjust LTM pointer
        ltm_pointer = write_addr;

        // Change active buffer
        active_buffer = write_buf;
    }


    void stm_to_ltm(memory_idx_t stm_split_pointer) {
        bool read_buf = active_buffer;
        bool write_buf = !active_buffer;

        // Copy LTM -> LTM
        memory_s2l_l2l_loop:
        for(memory_idx_t address = ltm_pointer; address < MEM_SIZE; address++) {
            #pragma HLS loop_tripcount min=0 max=MAX_LTM_SIZE avg=MAX_LTM_SIZE/2

            memory_s2l_l2l_datacopy:
            for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
                //#pragma HLS UNROLL
                data[write_buf][address][i] = data[read_buf][address][i];
            }
            labels[write_buf][address] = labels[read_buf][address];
        }

        // Copy STM -> LTM
        memory_s2l_s2l_loop:
        for(memory_idx_t read_address = 0; read_address < stm_split_pointer; read_address++) {
            #pragma HLS loop_tripcount min=0 max=MEM_SIZE-MIN_STM_SIZE avg=MIN_STM_SIZE*2

            memory_idx_t write_addr = read_address + ltm_pointer + 1 - stm_split_pointer;
            memory_s2l_s2l_datacopy:
            for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
                //#pragma HLS UNROLL
                data[write_buf][write_addr][i] = data[read_buf][read_address][i];
            }
            labels[write_buf][write_addr] = labels[read_buf][read_address];
            n_ltm_samples_per_class[labels[read_buf][read_address]]++;
        }

        // Copy STM -> STM
        memory_s2l_s2s_loop:
        for(memory_idx_t write_addr = 0; write_addr < stm_pointer - stm_split_pointer; write_addr++) {
            #pragma HLS loop_tripcount min=0 max=MEM_SIZE avg=MEM_SIZE/2

            memory_idx_t read_address = write_addr + stm_split_pointer;
            memory_s2l_s2s_datacopy:
            for(dimension_idx_t i = 0; i < N_DATAPOINT_DIMENSIONS; i++) {
                //#pragma HLS UNROLL
                data[write_buf][write_addr][i] = data[read_buf][read_address][i];
            }
            labels[write_buf][write_addr] = labels[read_buf][read_address];
        }

        // Adjust new pointers
        stm_pointer -= stm_split_pointer;
        ltm_pointer -= stm_split_pointer;
        active_buffer = write_buf;

    }

    void cluster_down() {
        
        // Create Buffers
        centroid_acc_t centroids_acc_buffer[MEM_SIZE/2][N_DATAPOINT_DIMENSIONS];
        memory_idx_t n_centroids_acc_contributors[MEM_SIZE/2];

        // 0: Create partitioning
        memory_idx_t centroids_buf_partitioning[N_CLASSES+1];
        #pragma HLS array_partition variable=centroids_buf_partitioning complete
        centroids_buf_partitioning[0] = 0;
        cluster_down_gen_centroids_partitioning:
        for(class_idx_t i = 0; i < N_CLASSES; i++) {
            centroids_buf_partitioning[i+1] = centroids_buf_partitioning[i] + ((n_ltm_samples_per_class[i]+1) / 2);
        }

        // 1: Fill centroids buffer (inactive memory buffer)
        bool centroids_buf = !active_buffer;
        memory_idx_t centroids_buf_idcs[N_CLASSES];
        cluster_down_make_start_idcs:
        for(class_idx_t i = 0; i < N_CLASSES; i++) {
            centroids_buf_idcs[i] = centroids_buf_partitioning[i];
        }

        cluster_down_init_centroids:
        for(memory_idx_t i = ltm_pointer; i < MEM_SIZE; i++) {
            label_t cur_label = labels[active_buffer][i];
            memory_idx_t centroid_buf_idx = centroids_buf_idcs[cur_label];
            if (centroid_buf_idx < centroids_buf_partitioning[cur_label+1]) {
                for (dimension_idx_t dim = 0; dim < N_DATAPOINT_DIMENSIONS; dim++) {
                    data[centroids_buf][centroid_buf_idx][dim] = data[active_buffer][i][dim];
                    centroids_buf_idcs[cur_label]++;
                }
            }
        }

        // Main kmeans loop
        // Fixed 100 iterations is a good middle ground
        cluster_down_main_loop:
        for (int iter = 0; iter < 100; iter++) {
            // reset accumulators
            cluster_down_reset_centroid_accs:
            for (memory_idx_t i = 0; i < centroids_buf_partitioning[N_CLASSES]; i++) {
                for (dimension_idx_t dim = 0; dim < N_DATAPOINT_DIMENSIONS; dim++) {
                    cluster_down_reset_centroid_accs_dimiter:
                    centroids_acc_buffer[i][dim] = 0;
                }
                n_centroids_acc_contributors[i] = 0;
            }

            // 2: Accumulate new centroids
            cluster_down_acc_centroids:
            for(memory_idx_t i = ltm_pointer+1; i < MEM_SIZE; i++) {
                label_t cur_label = labels[active_buffer][i];
                datapoint_t cur_datapoint[N_DATAPOINT_DIMENSIONS];
                cluster_down_acc_centroids_copy_cur_data:
                for (dimension_idx_t dim = 0; dim < N_DATAPOINT_DIMENSIONS; dim++) {
                    cur_datapoint[dim] = data[active_buffer][i][dim];
                }

                // Calculate distance to all centroids of current label
                dist_t closest_distance = MAX_DIST;
                memory_idx_t closest_centroid_idx = centroids_buf_partitioning[cur_label];

                cluster_down_acc_centroids_find_closest:
                for (memory_idx_t centroid_buf_idx = centroids_buf_partitioning[cur_label]; centroid_buf_idx < centroids_buf_partitioning[cur_label+1]; centroid_buf_idx++) {
                    dist_t cur_dist;
                    calc_dist(cur_datapoint, data[centroids_buf][centroid_buf_idx], cur_dist);
                    if (cur_dist < closest_distance) {
                        closest_distance = cur_dist;
                        closest_centroid_idx = centroid_buf_idx;
                    }
                }

                // Add datapoint to closest centroid accumulator
                cluster_down_acc_centroids_add_closest:
                for (dimension_idx_t dim = 0; dim < N_DATAPOINT_DIMENSIONS; dim++) {
                    centroids_acc_buffer[closest_centroid_idx][dim] += cur_datapoint[dim];
                }
                n_centroids_acc_contributors[closest_centroid_idx]++;
            }

            // Re-normalize accumulated centroids and write back to centroids buffer
            cluster_down_normalize_centroids:
            for (memory_idx_t i = 0; i < centroids_buf_partitioning[N_CLASSES]; i++) {
                memory_idx_t n_contributors = n_centroids_acc_contributors[i];
                if (n_contributors == 0) {
                    n_contributors = 1;
                }
                cluster_down_normalize_centroids_dimiter:
                for (dimension_idx_t dim = 0; dim < N_DATAPOINT_DIMENSIONS; dim++) {
                    data[centroids_buf][i][dim] = centroids_acc_buffer[i][dim] / n_contributors;
                }
            }
        }

        // Write back data to active buffer
        label_t cur_label = 0;
        cluster_down_write_back:
        for (memory_idx_t i = 0; i < centroids_buf_partitioning[N_CLASSES]; i++) {

            // Step to next label
            cluster_down_write_back_advance_label:
            while (i == centroids_buf_partitioning[cur_label+1]) {
                #pragma HLS loop_tripcount min=0 max=N_CLASSES avg=0
                cur_label++;
            }

            // Copy datapoint
            cluster_down_write_back_dimiter:
            for (dimension_idx_t dim = 0; dim < N_DATAPOINT_DIMENSIONS; dim++) {
                data[active_buffer][MEM_SIZE-1-i][dim] = data[centroids_buf][i][dim];
            }

            // Set label
            labels[active_buffer][MEM_SIZE-1-i] = cur_label;
        }

        // adjust ltm pointer
        ltm_pointer = MEM_SIZE-1-centroids_buf_partitioning[cur_label+1];

        // adjust ltm label counts
        cluster_down_adjust_ltm_samples_count:
        for (class_idx_t i = 0; i < N_CLASSES; i++) {
            n_ltm_samples_per_class[i] = centroids_buf_partitioning[i+1] - centroids_buf_partitioning[i];
        }
    }
}
