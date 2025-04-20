#pragma once
#include <unordered_map>
#include "../orderbook.h"
#include "messages.hpp"
#include "../concurrentqueue.h"

class MatchingEngine {
public:
    explicit MatchingEngine(moodycamel::ConcurrentQueue<hft::ExecutionReport>& out)
        : out_q_(out) {}
    void on_nos(const NOS& nos);

private:
    std::unordered_map<std::string, orderbook> books_;
    moodycamel::ConcurrentQueue<hft::ExecutionReport>& out_q_;
    void fill_exec(const NOS& src, uint32_t fill_px, uint32_t fill_qty, bool reject, std::string text="");
};
