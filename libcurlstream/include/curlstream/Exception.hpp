#ifndef CURL_STREAM_EXCEPTION_HPP_
#define CURL_STREAM_EXCEPTION_HPP_

#include <curl/curl.h>

#include <stdexcept>

namespace curlstream {

class Exception : public std::exception {
 public:
  Exception() : code_(CURLE_OK), mcode_(CURLM_OK) {}
  explicit Exception(CURLcode code) : code_(code), mcode_(CURLM_OK) {}
  explicit Exception(CURLMcode mcode) : code_(CURLE_OK), mcode_(mcode) {}

  const char* what() const noexcept {
    return (mcode_ != CURLM_OK) ? curl_multi_strerror(mcode_)
                                : curl_easy_strerror(code_);
  }

 private:
  CURLcode code_;
  CURLMcode mcode_;
};

void throw_if_err(CURLcode code) {
  if (code != CURLE_OK) {
    throw curlstream::Exception(code);
  }
}

void throw_if_err(CURLMcode code) {
  if (code != CURLM_OK) {
    throw curlstream::Exception(code);
  }
}

}  // namespace curlstream

#endif
