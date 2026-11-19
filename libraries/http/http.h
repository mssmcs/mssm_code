#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <optional>
#include <string>

namespace mssm {

struct HttpResult {
    int status = 0;
    std::string body;
    std::string error;
};

class HttpRequest {
public:
    // Requests cancellation without waiting. Completed results are preserved.
    void cancel() noexcept;

    // Returns immediately. A completed result can be taken only once.
    std::optional<HttpResult> takeResult();

    // Blocks until completion and consumes the result.
    // Throws std::future_error if the result was already consumed or moved.
    HttpResult wait();

private:
    friend class HttpClient;
    HttpRequest(std::future<HttpResult> result, std::shared_ptr<std::atomic<bool>> cancelled);

    std::future<HttpResult> result_;
    std::shared_ptr<std::atomic<bool>> cancelled_;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;

    // Requests run one at a time, with a 15-second timeout per transfer.
    // Destruction cancels queued work, signals the active transfer, and joins.
    HttpRequest get(std::string url);
    HttpRequest post(std::string url, std::string body,
                     std::string contentType = "text/plain");

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    HttpRequest enqueue(std::string url, std::string body,
                        std::optional<std::string> contentType);
};

} // namespace mssm
