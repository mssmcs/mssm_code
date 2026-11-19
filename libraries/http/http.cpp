#include "http.h"

#include <curl/curl.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <utility>

namespace mssm {
namespace {

class CurlRuntime {
public:
    CurlRuntime()
    {
        const auto code = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (code != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(code));
        }
    }

    ~CurlRuntime() { curl_global_cleanup(); }
};

size_t appendBody(char* data, size_t size, size_t count, void* output) noexcept
{
    const auto bytes = size * count;
    try {
        static_cast<std::string*>(output)->append(data, bytes);
        return bytes;
    } catch (...) {
        return 0;
    }
}

struct Cancellation {
    const std::atomic<bool>& clientStopping;
    const std::atomic<bool>& requestCancelled;

    bool requested() const noexcept
    {
        return clientStopping.load() || requestCancelled.load();
    }
};

int checkStop(void* token, curl_off_t, curl_off_t, curl_off_t, curl_off_t)
{
    return static_cast<const Cancellation*>(token)->requested();
}

HttpResult cancelled()
{
    return {0, {}, "Request cancelled"};
}

HttpResult transfer(const std::string& url, const std::string& body,
                    const std::optional<std::string>& contentType,
                    Cancellation cancellation)
{
    if (cancellation.requested()) {
        return cancelled();
    }

    HttpResult result;
    std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> headers(
        nullptr, curl_slist_free_all);
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(
        curl_easy_init(), curl_easy_cleanup);
    if (!curl) {
        return {0, {}, "Could not create a curl handle"};
    }

    // Keep the first configuration error so it can be reported to the caller.
    CURLcode code = CURLE_OK;
    const auto set = [&](CURLoption option, auto value) {
        if (code == CURLE_OK) {
            code = curl_easy_setopt(curl.get(), option, value);
        }
    };
    set(CURLOPT_URL, url.c_str());
    set(CURLOPT_PROTOCOLS_STR, "http,https");
    set(CURLOPT_TIMEOUT, 15L);
    set(CURLOPT_NOSIGNAL, 1L);
    set(CURLOPT_WRITEFUNCTION, appendBody);
    set(CURLOPT_WRITEDATA, &result.body);
    set(CURLOPT_XFERINFOFUNCTION, checkStop);
    set(CURLOPT_XFERINFODATA, &cancellation);
    set(CURLOPT_NOPROGRESS, 0L);

    if (contentType) {
        const std::string header = contentType->empty()
            ? "Content-Type:" : "Content-Type: " + *contentType;
        headers.reset(curl_slist_append(nullptr, header.c_str()));
        if (!headers) {
            return {0, {}, "Could not allocate the Content-Type header"};
        }
        set(CURLOPT_HTTPHEADER, headers.get());
        set(CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.size()));
        set(CURLOPT_POSTFIELDS, body.c_str());
    }

    if (code == CURLE_OK) {
        code = curl_easy_perform(curl.get());
    }
    if (code == CURLE_ABORTED_BY_CALLBACK) {
        return cancelled();
    }
    long status = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status);
    result.status = static_cast<int>(status);
    if (code != CURLE_OK) {
        result.error = curl_easy_strerror(code);
    }
    return result;
}

} // namespace

struct HttpClient::Impl {
    using Job = std::packaged_task<HttpResult(std::atomic<bool>&)>;

    std::mutex mutex;
    std::condition_variable available;
    std::queue<Job> pending;
    std::atomic<bool> stopping{false};
    std::thread worker;

    Impl() : worker([this] { run(); }) {}

    ~Impl()
    {
        {
            std::lock_guard lock(mutex);
            stopping.store(true);
        }
        available.notify_one();
        worker.join();
    }

    void run()
    {
        for (;;) {
            Job job;
            {
                std::unique_lock lock(mutex);
                available.wait(lock, [this] { return stopping.load() || !pending.empty(); });
                if (pending.empty()) {
                    return;
                }
                job = std::move(pending.front());
                pending.pop();
            }
            // After shutdown starts, queued jobs publish cancellation results.
            job(stopping);
        }
    }
};

HttpRequest::HttpRequest(std::future<HttpResult> result,
                         std::shared_ptr<std::atomic<bool>> cancelled)
    : result_(std::move(result)), cancelled_(std::move(cancelled))
{
}

void HttpRequest::cancel() noexcept
{
    if (cancelled_) {
        cancelled_->store(true);
    }
}

std::optional<HttpResult> HttpRequest::takeResult()
{
    if (!result_.valid() ||
        result_.wait_for(std::chrono::seconds{0}) != std::future_status::ready) {
        return std::nullopt;
    }
    return result_.get();
}

HttpResult HttpRequest::wait()
{
    return result_.get();
}

HttpClient::HttpClient()
{
    static const CurlRuntime runtime;
    impl_ = std::make_unique<Impl>();
}

HttpClient::~HttpClient() = default;

HttpRequest HttpClient::get(std::string url)
{
    return enqueue(std::move(url), {}, std::nullopt);
}

HttpRequest HttpClient::post(std::string url, std::string body, std::string contentType)
{
    if (contentType.find_first_of("\r\n") != std::string::npos) {
        throw std::invalid_argument("Content-Type must be a single header value");
    }
    return enqueue(std::move(url), std::move(body), std::move(contentType));
}

HttpRequest HttpClient::enqueue(std::string url, std::string body,
                                std::optional<std::string> contentType)
{
    auto cancelled = std::make_shared<std::atomic<bool>>(false);
    Impl::Job job([cancelled, url = std::move(url), body = std::move(body),
                   contentType = std::move(contentType)](std::atomic<bool>& stopping) {
        return transfer(url, body, contentType, {stopping, *cancelled});
    });
    auto result = job.get_future();
    {
        std::lock_guard lock(impl_->mutex);
        impl_->pending.push(std::move(job));
    }
    impl_->available.notify_one();
    return HttpRequest{std::move(result), std::move(cancelled)};
}

} // namespace mssm
