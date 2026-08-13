// SRS 002 S5.4.1: this is deliberately *not* named applications/sqlite3-shell
// -- SRS 002 RR-3 reserves that unsuffixed name for the eventual
// sqlite-core-interface-facade-gated, C-API-compatible replacement, which
// doesn't exist yet. This is a much smaller thing: a REPL over
// sqlite-core-command-processor's single-table SELECT executor, against a
// hardcoded in-memory demo table -- no file I/O, no C API, no dot-commands.
// It exists to demonstrate the new track's compiler pipeline actually
// running real, user-typed SQL end-to-end (tokenizer -> parser ->
// code-generator -> VM), which nothing before it in this workspace did.
#include <sqlite/core/command_processor/query_error.hpp>
#include <sqlite/core/command_processor/select_executor.hpp>
#include <sqlite/core/command_processor/table_descriptor.hpp>
#include <sqlite/compiler/parser/parse_error.hpp>
#include <sqlite/compiler/parser/parser.hpp>

#include <iostream>
#include <string>

namespace cp = sqlite::core::command_processor;
namespace vm = sqlite::core::virtual_machine;

namespace {

cp::table_descriptor demo_table() {
    cp::table_descriptor t;
    t.name = "t";
    t.columns = {"id", "name", "score"};
    t.rows = {
        {vm::aux::mem::make_integer(1), vm::aux::mem::make_text("alice"), vm::aux::mem::make_real(9.5)},
        {vm::aux::mem::make_integer(2), vm::aux::mem::make_text("bob"), vm::aux::mem::make_real(7.0)},
        {vm::aux::mem::make_integer(3), vm::aux::mem::make_text("carol"), vm::aux::mem::make_null()},
        {vm::aux::mem::make_integer(4), vm::aux::mem::make_text("dave"), vm::aux::mem::make_real(7.0)},
        {vm::aux::mem::make_integer(5), vm::aux::mem::make_text("eve"), vm::aux::mem::make_real(3.25)},
    };
    return t;
}

void print_result(const cp::query_result& result) {
    for (std::size_t i = 0; i < result.column_names.size(); ++i) {
        if (i) std::cout << '|';
        std::cout << result.column_names[i];
    }
    std::cout << '\n';
    for (const auto& row : result.rows) {
        for (std::size_t i = 0; i < row.size(); ++i) {
            if (i) std::cout << '|';
            std::cout << (row[i].is_null() ? std::string() : row[i].as_text());
        }
        std::cout << '\n';
    }
}

} // namespace

int main() {
    std::cout << "sqlite-cpp-repl -- single-table SELECT only, against a built-in demo table `t(id, name, score)`.\n";
    std::cout << "Not sqlite3-shell (SRS 002 RR-3 reserves that name); type SQL, or an empty line / EOF to quit.\n";

    cp::table_descriptor table = demo_table();
    cp::select_executor executor;

    std::string line;
    std::cout << "sqlite-cpp> ";
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        try {
            sqlite::compiler::parser::parser p(line);
            auto stmt = p.parse_select();
            print_result(executor.execute(stmt, table));
        } catch (const sqlite::compiler::parser::parse_error& e) {
            std::cerr << "parse error: " << e.what() << '\n';
        } catch (const sqlite::compiler::code_generator::code_generator_error& e) {
            std::cerr << "error: " << e.what() << '\n';
        } catch (const cp::query_error& e) {
            std::cerr << "error: " << e.what() << '\n';
        }
        std::cout << "sqlite-cpp> ";
    }
    return 0;
}
