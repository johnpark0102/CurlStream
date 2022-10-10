#ifndef CURL_STREAM_TIMER_HPP_
#define CURL_STREAM_TIMER_HPP_

#include <chrono>
#include <limits>
#include <stdexcept>

namespace curlstream {

class Timer {
 public:
  Timer() : tmout_(std::chrono::seconds(120)) {}

  void set_timeout(const std::chrono::milliseconds& new_tmout) {
    auto tmout_ms = new_tmout.count();
    if (tmout_ms < 0 || (std::numeric_limits<int>::max() < tmout_ms)) {
      throw std::out_of_range("set_timeout() value out of range");
    }

    tmout_ = new_tmout;
  }

  int get_timeout_ms() const { return static_cast<int>(tmout_.count()); }

 private:
  std::chrono::milliseconds tmout_;
};

}  // namespace curlstream

#endif
