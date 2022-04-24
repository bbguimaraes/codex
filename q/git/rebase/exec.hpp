#ifndef CODEX_GIT_REBASE_EXEC_H
#define CODEX_GIT_REBASE_EXEC_H

#include <span>
#include <string>
#include <system_error>
#include <tuple>

namespace codex {

/** Executes a command and reads its stdout. */
std::tuple<int, std::errc> exec(
    std::span<const char *const> args, std::string *out);

}

#endif
