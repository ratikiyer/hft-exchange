#pragma once

#include <chrono>
#include <cstring>
#include <fstream>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

#include "./types.h"
#include "./concurrentqueue.h"

enum class log_event_kind : uint8_t { ADD, CANCEL, MODIFY, MATCH };

struct log_event_t {
   uint64_t timestamp;
   char order_id [ ORDER_ID_LEN ];
   log_event_kind kind;
   uint32_t price;
   size_t qty;
   order_side side;

   char order_id_secondary [ ORDER_ID_LEN ];
   uint32_t price_secondary;
   size_t qty_secondary;
   order_side side_secondary;

   log_event_t() = default;

   log_event_t(
      uint64_t ts,
      const char* id,
      log_event_kind k,
      uint32_t p,
      size_t q,
      order_side s
   )
      : timestamp(ts)
      , kind(k)
      , price(p)
      , qty(q)
      , side(s)
      , price_secondary(0)
      , qty_secondary(0)
      , side_secondary(order_side::BUY) // default
   {
      std::memcpy(order_id, id, ORDER_ID_LEN);
   }
};

class logger {
public:
   // Use text mode here for human‐readable logs. If you really want binary, use:
   // out_file_(filename, std::ios::binary | std::ios::out)
   explicit logger(const std::string& filename)
      : out_file_(filename),   // text mode for debugging
        running_(true)
   {
      if (!out_file_.is_open()) {
         throw std::runtime_error("Failed to open log file: " + filename);
      }
      // Launch background thread
      thread_ = std::thread(&logger::run, this);
   }

   void push(const log_event_t& event) {
      // Enqueue the event and notify the background thread
      queue_.enqueue(event);

      {
         std::lock_guard<std::mutex> lock(mutex_);
      }
      cv_.notify_one();
   }

   ~logger() {
      {
         std::lock_guard<std::mutex> lock(mutex_);
         running_ = false;
      }
      // Wake up the background thread so it can exit
      cv_.notify_one();
      // Join thread before closing file
      if (thread_.joinable()) {
         thread_.join();
      }
      out_file_.flush();
      out_file_.close();
   }

private:
   std::ofstream out_file_;
   moodycamel::ConcurrentQueue<log_event_t> queue_;
   std::atomic<bool> running_;
   std::thread thread_;

   // For condition variable
   std::mutex mutex_;
   std::condition_variable cv_;

   void run() {
      while (true) {
         // 1) Drain the queue of all log events currently pending
         log_event_t ev;
         while (queue_.try_dequeue(ev)) {
            // For debugging, we write each field as text
            // If you want binary, see the old code with out_file_.write(...)
            out_file_ << "TIMESTAMP=" << ev.timestamp
                      << " KIND=" << static_cast<int>(ev.kind)
                      << " PRICE=" << ev.price
                      << " QTY=" << ev.qty
                      << " SIDE=" << static_cast<int>(ev.side)
                      << " PRICE2=" << ev.price_secondary
                      << " QTY2=" << ev.qty_secondary
                      << " SIDE2=" << static_cast<int>(ev.side_secondary)
                      << " ORDID=" << std::string(ev.order_id, ORDER_ID_LEN)
                      << " ORDID2=" << std::string(ev.order_id_secondary, ORDER_ID_LEN)
                      << "\n";
         }
         out_file_.flush();

         // 2) Check if we're done; otherwise wait for new events
         std::unique_lock<std::mutex> lock(mutex_);
         if (!running_) {
            break;
         }
         // Wait up to 500ms or until push() calls notify_one()
         cv_.wait_for(lock, std::chrono::milliseconds(500));
      }

      // 3) Drain any final events after we've been told to shut down
      log_event_t ev;
      while (queue_.try_dequeue(ev)) {
         out_file_ << "TIMESTAMP=" << ev.timestamp
                   << " KIND=" << static_cast<int>(ev.kind)
                   << " PRICE=" << ev.price
                   << " QTY=" << ev.qty
                   << " SIDE=" << static_cast<int>(ev.side)
                   << " PRICE2=" << ev.price_secondary
                   << " QTY2=" << ev.qty_secondary
                   << " SIDE2=" << static_cast<int>(ev.side_secondary)
                   << " ORDID=" << std::string(ev.order_id, ORDER_ID_LEN)
                   << " ORDID2=" << std::string(ev.order_id_secondary, ORDER_ID_LEN)
                   << "\n";
      }
      out_file_.flush();
   }
};
