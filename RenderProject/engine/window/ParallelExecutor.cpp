#include "ParallelExecutor.h"
const uint32_t ParallelExecutor::MAX_THREADS = std::max(1u, std::thread::hardware_concurrency());
const uint32_t ParallelExecutor::HALF_THREADS = std::max(1u, std::thread::hardware_concurrency() / 2);