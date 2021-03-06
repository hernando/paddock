#pragma once

#include "core/Log.hpp"

#include "utils/Expected.hpp"

#include <QCoreApplication>

#include <future>
#include <type_traits>

namespace paddock
{

template <typename F, typename... Args>
std::future<std::invoke_result_t<F, Args...>> execInMainThreadAsync(
    F&& f, Args&&... args)
{
    using ReturnType = std::invoke_result_t<F, Args...>;

    std::packaged_task<ReturnType()> task(
        [f = std::forward<F>(f), ... args = std::forward<Args>(args)] {
            return f(args...);
        });
    auto future = task.get_future();

    QMetaObject::invokeMethod(QCoreApplication::instance(),
                              [task = std::move(task)]() mutable { task(); });
    return future;
}

template <typename F, typename... Args>
tl::expected<std::invoke_result_t<F, Args...>, std::error_code>
    execInMainThread(F&& f, Args&&... args)
{
    using ReturnType = std::invoke_result_t<F, Args...>;

    auto future = execInMainThreadAsync(std::forward<F>(f), std::forward<Args>(args)...);

    try
    {
        if constexpr (std::is_same_v<ReturnType, void>)
        {
            future.get();
            return tl::expected<void, std::error_code>{};
        }
        else
            return future.get();
    }
    catch (const std::future_error& error)
    {
        return tl::unexpected(error.code());
    }
    catch (const std::system_error& error)
    {
        return tl::unexpected(error.code());
    }
    catch (...)
    {
        throw std::runtime_error("Unknown error in execInMainThread");
    }
}

} // namespace paddock
