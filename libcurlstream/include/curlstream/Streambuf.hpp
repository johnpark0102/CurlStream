#ifndef CURL_STREAM_STREAMBUF_HPP_
#define CURL_STREAM_STREAMBUF_HPP_

#include <curl/curl.h>

#include <algorithm>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>

#include "Exception.hpp"
#include "Timer.hpp"
#include "Utils.hpp"

namespace curlstream {

class Streambuf : public std::basic_streambuf<char>, private curlstream::Timer {
 public:
  using curlstream::Timer::get_timeout_ms;
  using curlstream::Timer::set_timeout;

  Streambuf()
      : curl_(init_curl_handle()),
        curlm_(init_curlm_handle()),
        rbuf_(CURL_MAX_READ_SIZE),
        wbuf_(CURL_MAX_WRITE_SIZE) {
    throw_if_err(curl_easy_setopt(curl_.get(), CURLOPT_NOSIGNAL, 1L));
  }

  ~Streambuf() noexcept {
    if (is_open()) {
      close();
    }
  }

  CURL* curl() { return curl_.get(); }

  bool is_open() const {
    return eback() && gptr() && egptr() && pbase() && pptr() && epptr();
  }

  Streambuf* open(const std::string& url,
                  const std::string& username = std::string(),
                  const std::string& password = std::string()) {
    if (is_open()) {
      close();
    }

    try {
      CURL* curl = curl_.get();
      CURLM* curlm = curlm_.get();
      const auto TMOUT_MS = get_timeout_ms();

      throw_if_err(curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, TMOUT_MS));
      throw_if_err(curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, TMOUT_MS));
      throw_if_err(curl_easy_setopt(curl, CURLOPT_URL, url.c_str()));

      throw_if_err(
          curl_easy_setopt(curl_.get(), CURLOPT_USERNAME, username.c_str()));

      throw_if_err(
          curl_easy_setopt(curl_.get(), CURLOPT_PASSWORD, password.c_str()));

      throw_if_err(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fill_buffer));
      throw_if_err(curl_easy_setopt(curl, CURLOPT_WRITEDATA, this));

      throw_if_err(curl_easy_setopt(curl, CURLOPT_READFUNCTION, flush_buffer));
      throw_if_err(curl_easy_setopt(curl, CURLOPT_READDATA, this));

      throw_if_err(curl_multi_add_handle(curlm, curl));

      setg(rbuf_.data(), rbuf_.data() + rbuf_.size(),
           rbuf_.data() + rbuf_.size());

      setp(wbuf_.data(), wbuf_.data() + wbuf_.size());

      return this;
    } catch (const std::exception&) {
      return nullptr;
    }
  }

  int_type underflow() override {
    const auto TMOUT_MS = get_timeout_ms();
    CURLM* curlm = curlm_.get();
    int running = 1;

    while (running && (gptr() == egptr())) {
      if (curl_multi_perform(curlm, &running) == CURLM_OK) {
        if (curl_multi_wait(curlm, nullptr, 0, TMOUT_MS, nullptr) != CURLM_OK) {
          break;
        }
      }
    }

    if (gptr() == egptr()) {
      return traits_type::eof();
    }

    return traits_type::to_int_type(*gptr());
  }

  int_type overflow(int_type ch = traits_type::eof()) override {
    const auto TMOUT_MS = get_timeout_ms();
    CURLM* curlm = curlm_.get();
    int running = 1;

    while (running && (pbase() < pptr())) {
      if (curl_multi_perform(curlm, &running) == CURLM_OK) {
        if (curl_multi_wait(curlm, nullptr, 0, TMOUT_MS, nullptr) != CURLM_OK) {
          break;
        }
      }
    }

    if (pbase() < pptr()) {
      return traits_type::eof();
    }

    setp(wbuf_.data(), wbuf_.data() + wbuf_.size());

    return traits_type::not_eof(ch);
  }

  int sync() override {
    if (!is_open()) {
      return 0;
    }

    int ret = (pbase() < pptr()) ? overflow() : traits_type::not_eof(0);

    setg(rbuf_.data(), rbuf_.data() + rbuf_.size(),
         rbuf_.data() + rbuf_.size());

    setp(wbuf_.data(), wbuf_.data() + wbuf_.size());

    return (ret == traits_type::eof()) ? (-1) : 0;
  }

  Streambuf* close() {
    if (!is_open()) {
      return this;
    }

    int ret = (pbase() < pptr()) ? overflow() : traits_type::not_eof(0);

    curl_multi_remove_handle(curlm_.get(), curl_.get());

    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);

    return (ret == traits_type::eof()) ? nullptr : this;
  }

 private:
  static size_t fill_buffer(char* ptr, size_t, size_t n, Streambuf* sb) {
    char* buf = sb->rbuf_.data();
    std::copy(ptr, ptr + n, buf);
    sb->setg(buf, buf, buf + n);
    return n;
  }

  static size_t flush_buffer(char* ptr, size_t size, size_t n, Streambuf* sb) {
    char* const buf_beg = sb->pbase();
    char* const buf_cur = sb->pptr();

    size_t size_data_buf = buf_cur - buf_beg;
    size_t flush_size = std::min(n * size, size_data_buf);
    std::copy(buf_beg, buf_beg + flush_size, ptr);

    sb->setp(buf_beg + flush_size, sb->epptr());
    sb->pbump((buf_cur - buf_beg) - flush_size);

    return flush_size;
  }

  CurlPtr curl_;
  CurlmPtr curlm_;
  std::vector<char> rbuf_;
  std::vector<char> wbuf_;
};

}  // namespace curlstream

#endif
