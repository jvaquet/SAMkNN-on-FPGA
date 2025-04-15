#ifndef K_BEST_COLLECTOR
#define K_BEST_COLLECTOR

#include "../samknn_config.h"
#include "../samknn_types.h"

namespace KBestCollector {
    void append(dist_t dist, label_t label,
            dist_t dists[K_NEIGHBORS], label_t labels[K_NEIGHBORS]);
};

#endif