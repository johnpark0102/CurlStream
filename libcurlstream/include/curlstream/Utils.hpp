#ifndef CURL_STREAM_UTILS_HPP_
#define CURL_STREAM_UTILS_HPP_

#include <curl/curl.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

namespace curlstream {

using CurlPtr = std::unique_ptr<CURL, std::function<void(CURL*)>>;

CurlPtr init_curl_handle() {
  CurlPtr handle(curl_easy_init(), curl_easy_cleanup);
  if (!handle) {
    throw std::runtime_error("curl_easy_init() failed");
  }

  return handle;
}

using CurlmPtr = std::unique_ptr<CURLM, std::function<CURLMcode(CURLM*)>>;

CurlmPtr init_curlm_handle() {
  CurlmPtr handle(curl_multi_init(), curl_multi_cleanup);
  if (!handle) {
    throw std::runtime_error("curl_multi_init() failed");
  }

  return handle;
}

using CurlSlistPtr =
    std::unique_ptr<curl_slist, std::function<void(curl_slist*)>>;

template <typename Iterator>
CurlSlistPtr to_curl_slist(
    typename std::enable_if<
        std::is_same<typename Iterator::value_type, std::string>::value,
        Iterator>::type beg,
    Iterator end) {
  curl_slist* list = nullptr;

  for (; beg != end; ++beg) {
    curl_slist* tmp = curl_slist_append(list, beg->c_str());
    if (!tmp) {
      curl_slist_free_all(list);
      throw std::runtime_error("curl_slist_append() failed");
    }

    list = tmp;
  }

  return CurlSlistPtr(list, curl_slist_free_all);
}

using CurlMimePtr = std::unique_ptr<curl_mime, std::function<void(curl_mime*)>>;

struct MimePart {
  std::string name;
  std::string filename;
  std::string data;
  std::string type;
};

template <typename Iterator>
CurlMimePtr to_curl_mime(
    CURL* curl,
    typename std::enable_if<
        std::is_same<typename Iterator::value_type, MimePart>::value,
        Iterator>::type beg,
    Iterator end) {
  CurlMimePtr mime(curl_mime_init(curl), curl_mime_free);
  if (!mime) {
    throw std::runtime_error("curl_mime_init() failed.");
  }

  for (; beg != end; ++beg) {
    curl_mimepart* part = curl_mime_addpart(mime.get());
    if (!part) {
      throw std::runtime_error("curl_mime_addpart() failed.");
    }

    // TO Do
  }

  return mime;
}

}  // namespace curlstream

#endif  // !CURL_STREAM_UTILS_HPP_
