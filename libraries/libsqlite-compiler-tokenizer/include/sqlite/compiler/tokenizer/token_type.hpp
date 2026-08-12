#pragma once

// Legacy source: parse.h's generated TK_* #defines (lemon writes these from
// the %token declarations in parse.y; SRS S3.5.1, sqlite::compiler::tokenizer).
// FR-5: replaced by a scoped enumeration. Covers every token
// sqlite3GetToken() (tokenize.c) can produce: the 148 SQL keywords
// (tool/mkkeywordhash.c's keyword table) plus operators, punctuation, and
// literal classes. A handful of members carry a trailing underscore
// (`and_`, `case_`, `default_`, `delete_`, `do_`, `else_`, `for_`, `if_`,
// `not_`, `or_`, `union_`, `using_`, `virtual_`) because the bare SQL
// keyword spelling collides with a C++ keyword or alternative-spelling
// token (`and`, `or`, `not`, etc. are reserved words in C++, not just C).
//
// Legacy's TK_JOIN_KW/TK_LIKE_KW/TK_CTIME_KW "fallback classes" (CROSS/
// FULL/INNER/LEFT/NATURAL/OUTER/RIGHT all lex to TK_JOIN_KW; GLOB/LIKE/
// REGEXP to TK_LIKE_KW; the three CURRENT_* to TK_CTIME_KW) are *not*
// collapsed here -- each keyword keeps its own distinct enum value
// (`cross`, `full`, `inner`, ... rather than one shared `join_kw`), since
// this phase has no parser yet to consume the collapsed class and a
// tokenizer that can tell a caller exactly which keyword it saw is strictly
// more informative on its own.
namespace sqlite::compiler::tokenizer {

enum class token_type {
    // Stream control (not a legacy TK_* -- this tokenizer's own sentinel
    // for "no more input", where the caller loop naturally stops).
    end_of_input,
    illegal,

    // Trivia.
    space,
    comment,

    // Identifiers and literals.
    id,
    string,
    blob,
    integer,
    float_,
    qnumber,     // integer/real with a digit-separator (e.g. 1_000_000)
    variable,    // ?, ?N, :name, @name, $name, #name

    // Punctuation and operators.
    lp, rp, semi, comma, dot,
    plus, minus, star, slash, rem,
    eq, ne, lt, le, gt, ge,
    lshift, rshift,
    bitand_, bitor_, bitnot, concat,
    ptr,          // -> or ->>

    abort,    // ABORT
    action,    // ACTION
    add,    // ADD
    after,    // AFTER
    all,    // ALL
    alter,    // ALTER
    always,    // ALWAYS
    analyze,    // ANALYZE
    and_,    // AND
    as,    // AS
    asc,    // ASC
    attach,    // ATTACH
    autoincrement,    // AUTOINCREMENT
    before,    // BEFORE
    begin,    // BEGIN
    between,    // BETWEEN
    by,    // BY
    cascade,    // CASCADE
    case_,    // CASE
    cast,    // CAST
    check,    // CHECK
    collate,    // COLLATE
    column,    // COLUMN
    commit,    // COMMIT
    conflict,    // CONFLICT
    constraint,    // CONSTRAINT
    create,    // CREATE
    cross,    // CROSS
    current,    // CURRENT
    current_date,    // CURRENT_DATE
    current_time,    // CURRENT_TIME
    current_timestamp,    // CURRENT_TIMESTAMP
    database,    // DATABASE
    default_,    // DEFAULT
    deferred,    // DEFERRED
    deferrable,    // DEFERRABLE
    delete_,    // DELETE
    desc,    // DESC
    detach,    // DETACH
    distinct,    // DISTINCT
    do_,    // DO
    drop,    // DROP
    end,    // END
    each,    // EACH
    else_,    // ELSE
    escape,    // ESCAPE
    except,    // EXCEPT
    exclusive,    // EXCLUSIVE
    exclude,    // EXCLUDE
    exists,    // EXISTS
    explain,    // EXPLAIN
    fail,    // FAIL
    filter,    // FILTER
    first,    // FIRST
    following,    // FOLLOWING
    for_,    // FOR
    foreign,    // FOREIGN
    from,    // FROM
    full,    // FULL
    generated,    // GENERATED
    glob,    // GLOB
    group,    // GROUP
    groups,    // GROUPS
    having,    // HAVING
    if_,    // IF
    ignore,    // IGNORE
    immediate,    // IMMEDIATE
    in,    // IN
    index,    // INDEX
    indexed,    // INDEXED
    initially,    // INITIALLY
    inner,    // INNER
    insert,    // INSERT
    instead,    // INSTEAD
    intersect,    // INTERSECT
    into,    // INTO
    is,    // IS
    isnull,    // ISNULL
    join,    // JOIN
    key,    // KEY
    last,    // LAST
    left,    // LEFT
    like,    // LIKE
    limit,    // LIMIT
    match,    // MATCH
    materialized,    // MATERIALIZED
    natural,    // NATURAL
    no,    // NO
    not_,    // NOT
    nothing,    // NOTHING
    notnull,    // NOTNULL
    null,    // NULL
    nulls,    // NULLS
    of,    // OF
    offset,    // OFFSET
    on,    // ON
    or_,    // OR
    order,    // ORDER
    others,    // OTHERS
    outer,    // OUTER
    over,    // OVER
    partition,    // PARTITION
    plan,    // PLAN
    pragma,    // PRAGMA
    preceding,    // PRECEDING
    primary,    // PRIMARY
    query,    // QUERY
    raise,    // RAISE
    range,    // RANGE
    recursive,    // RECURSIVE
    references,    // REFERENCES
    regexp,    // REGEXP
    reindex,    // REINDEX
    release,    // RELEASE
    rename,    // RENAME
    replace,    // REPLACE
    restrict,    // RESTRICT
    returning,    // RETURNING
    right,    // RIGHT
    rollback,    // ROLLBACK
    row,    // ROW
    rows,    // ROWS
    savepoint,    // SAVEPOINT
    select,    // SELECT
    set,    // SET
    table,    // TABLE
    temp,    // TEMP
    temporary,    // TEMPORARY
    then,    // THEN
    ties,    // TIES
    to,    // TO
    transaction,    // TRANSACTION
    trigger,    // TRIGGER
    unbounded,    // UNBOUNDED
    union_,    // UNION
    unique,    // UNIQUE
    update,    // UPDATE
    using_,    // USING
    vacuum,    // VACUUM
    values,    // VALUES
    view,    // VIEW
    virtual_,    // VIRTUAL
    when,    // WHEN
    where,    // WHERE
    window,    // WINDOW
    with,    // WITH
    within,    // WITHIN
    without,    // WITHOUT
};

} // namespace sqlite::compiler::tokenizer
