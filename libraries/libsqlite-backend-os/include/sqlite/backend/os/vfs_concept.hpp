#pragma once

#include "open_flags.hpp"
#include "result.hpp"

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>

// Legacy source: os.h's sqlite3OsOpen/Delete/Access/FullPathname/Randomness/
// Sleep/CurrentTimeInt64 wrapper functions, forwarding through the
// sqlite3_vfs vtable (SRS S3.3.1). Deferred: xDlOpen/xDlError/xDlSym/xDlClose
// (extension loading -- belongs with sqlite-core-interface::extension, SRS
// S3.4.3, once that phase exists) and xGetLastError.
namespace sqlite::backend::os {

template<class V>
concept vfs = requires(V v, std::string_view path, typename V::file_type& out_file, bool& out_exists, void* buf, int n) {
    { v.open(path, open_flags::none, out_file) } -> std::same_as<result>;
    { v.delete_file(path, /*sync_dir=*/true) } -> std::same_as<result>;
    { v.access(path, out_exists) } -> std::same_as<result>;
    { v.full_pathname(path) } -> std::same_as<std::string>;
    { v.randomness(n, buf) } -> std::same_as<int>;
    { v.sleep(n) } -> std::same_as<int>;
    { v.current_time_millis() } -> std::same_as<std::int64_t>;
};

} // namespace sqlite::backend::os
