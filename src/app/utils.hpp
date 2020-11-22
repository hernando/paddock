#pragma once

#include "core/Log.hpp"

#include <tl/expected.hpp>

#include <QCoreApplication>

#include <future>
#include <type_traits>

namespace paddock
{
template <typename F, typename... Args>
tl::expected<std::invoke_result_t<F, Args...>, std::error_code>
    execInMainThread(F&& f, Args&&... args)
{
    using ReturnType = std::invoke_result_t<F, Args...>;

    std::packaged_task<ReturnType()> task(
        [f = std::forward<F>(f), ... args = std::forward<Args>(args)] {
            return f(args...);
        });
    auto future = task.get_future();

    QMetaObject::invokeMethod(QCoreApplication::instance(),
                              [task = std::move(task)]() mutable { task(); });
    try
    {
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
