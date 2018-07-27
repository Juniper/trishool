/**
 * Copyright(C) 2018, Juniper Networks, Inc.
 * All rights reserved
 *
 * This SOFTWARE is licensed under the license provided in the LICENSE.txt
 * file.  By downloading, installing, copying, or otherwise using the
 * SOFTWARE, you agree to be bound by the terms of that license.  This
 * SOFTWARE is not an official Juniper product.
 *
 * Third-Party Code: This SOFTWARE may depend on other components under
 * separate copyright notice and license terms.  Your use of the source
 * code for those components is subject to the term and conditions of
 * the respective license as noted in the Third-Party source code.
 */

/**
 * @author Anoop Saldanha
 */

#include "trishool-bt-common.h"
#include "trishool/util-log.h"
#include "trishool/util-log.h"
#include "trishool/status.h"
#include "sqlite3.h"

static sqlite3 *g_sqlite_ctx = NULL;

#if 0
tri_status_t tri_db_insert_call_graph(uint32_t caller_trace_addr,
                                      uint32_t caller_bb_addr,
                                      uint32_t caller_ins_addr,
                                      uint32_t callee_trace_addr,
                                      uint32_t callee_bb_addr,
                                      uint32_t callee_ins_addr)
{
    sqlite3_stmt *stmt = NULL;
    int stmt_arg_idx = 1;
    char *sql_stmt;
    tri_status_t status;

    if (tri_db_sqlite_begin_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error beginning transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    sql_stmt =
        "INSERT INTO call_graph "
        "(caller_trace_addr, caller_bb_addr, caller_ins_addr, "
        "callee_trace_addr, callee_bb_addr, callee_ins_addr) "
        "VALUES ($caller_trace_addr, $caller_bb_addr, $caller_ins_addr, "
        "        $callee_trace_addr, $callee_bb_addr, $callee_ins_addr)";

    if (sqlite3_prepare_v2(g_sqlite_ctx, sql_stmt,
                           -1, &stmt, NULL) != SQLITE_OK)
    {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, caller_trace_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, caller_bb_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, caller_ins_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, callee_trace_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, callee_bb_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, callee_ins_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        tri_log_error("sqlite3_step() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_db_sqlite_commit_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error commiting transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
    }
    return status;
}
#endif

static tri_status_t tri_db_setup_tables()
{
    char *sql_stmt;
    tri_status_t status;

#if 0
    char *sql_stmt =
        "CREATE TABLE IF NOT EXISTS call_graph ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "caller_trace_addr INT,"
        "caller_bb_addr INT,"
        "caller_ins_addr INT,"
        "callee_trace_addr INT,"
        "callee_bb_addr INT,"
        "callee_ins_addr INT"
        ");";
#endif
    sql_stmt =
        "CREATE TABLE IF NOT EXISTS call_graph ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "caller_block_addr INT,"
        "caller_ins_addr INT,"
        "callee_block_addr INT,"
        "callee_ins_addr INT"
        ");";

    if (sqlite3_exec(g_sqlite_ctx, sql_stmt, 0, 0, 0) != SQLITE_OK) {
        tri_log_error("sqlite3_exec() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    sql_stmt =
        "CREATE TABLE IF NOT EXISTS block ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "block_addr INT,"
        "block_name TEXT"
        ");";

    if (sqlite3_exec(g_sqlite_ctx, sql_stmt, 0, 0, 0) != SQLITE_OK) {
        tri_log_error("sqlite3_exec() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static tri_status_t tri_db_sqlite_begin_transaction()
{
    tri_status_t status;

    char *sql_stmt =
        "BEGIN EXCLUSIVE";

    if (sqlite3_exec(g_sqlite_ctx, sql_stmt, 0, 0, 0) != SQLITE_OK) {
        tri_log_error("sqlite3_exec() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static tri_status_t tri_db_sqlite_commit_transaction()
{
    tri_status_t status;

    char *sql_stmt =
        "COMMIT";

    if (sqlite3_exec(g_sqlite_ctx, sql_stmt, 0, 0, 0) != SQLITE_OK) {
        tri_log_error("sqlite3_exec() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_db_init()
{
    tri_status_t status;

    if (sqlite3_open("trishool.db", &g_sqlite_ctx) != SQLITE_OK) {
        tri_log_error("sqlite3_open() failure - %s\n",
                      sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_db_setup_tables() != TRI_STATUS_OK) {
        tri_log_error("Error setting up db tables.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    if (status != TRI_STATUS_OK) {
        if (g_sqlite_ctx != NULL) {
            sqlite3_close(g_sqlite_ctx);
            g_sqlite_ctx = NULL;
        }
    }
    return status;
}


tri_status_t tri_db_insert_call_graph(uint32_t caller_block_addr,
                                      uint32_t caller_ins_addr,
                                      uint32_t callee_block_addr,
                                      uint32_t callee_ins_addr)
{
    sqlite3_stmt *stmt = NULL;
    int stmt_arg_idx = 1;
    char *sql_stmt;
    tri_status_t status;

    tri_log_debug("Call Graph Insertion: "
                  "{ "
                  "\'caller_block\': \'%x\', "
                  "\'caller_ins\': \'%x\', "
                  "\'callee_block\': \'%x\', "
                  "\'callee_ins\': \'%x\' "
                  " }",
                  caller_block_addr, caller_ins_addr,
                  callee_block_addr, callee_ins_addr);

    return TRI_STATUS_OK;

    if (tri_db_sqlite_begin_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error beginning transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

#if 0
    sql_stmt =
        "INSERT INTO call_graph "
        "(caller_block_addr, caller_ins_addr, "
        "callee_block_addr, callee_ins_addr) "
        "VALUES ($caller_block_addr, $caller_ins_addr, "
        "        $callee_block_addr, $callee_ins_addr)";
#endif

    sql_stmt =
        "INSERT INTO call_graph "
        "(caller_block_addr, caller_ins_addr, "
        "callee_block_addr, callee_ins_addr) "
        "VALUES (?, ?, ?, ?)";


    if (sqlite3_prepare_v2(g_sqlite_ctx, sql_stmt,
                           -1, &stmt, NULL) != SQLITE_OK)
    {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, caller_block_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, caller_ins_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, callee_block_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, callee_ins_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        tri_log_error("sqlite3_step() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_db_sqlite_commit_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error commiting transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
    }
    return status;
}

tri_status_t tri_db_insert_block(uint32_t block_addr, char *block_name)
{
    sqlite3_stmt *stmt = NULL;
    int stmt_arg_idx = 1;

    char *sql_stmt;
    tri_status_t status;

    if (tri_db_sqlite_begin_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error beginning transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    sql_stmt =
        "INSERT INTO block "
        "(block_addr, block_name) "
        "VALUES ($block_addr, $block_name)";

    if (sqlite3_prepare_v2(g_sqlite_ctx, sql_stmt,
                           -1, &stmt, NULL) != SQLITE_OK)
    {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_int64(stmt, stmt_arg_idx++, block_addr) != SQLITE_OK) {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_bind_text(stmt, stmt_arg_idx++, block_name,
                          -1, NULL) != SQLITE_OK)
    {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        tri_log_error("sqlite3_step() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_db_sqlite_commit_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error commiting transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
    }
    return status;
}


tri_status_t tri_db_print_call_graph()
{
    uint64_t caller_block;
    uint64_t caller_addr;
    uint64_t callee_block;
    uint64_t callee_addr;

    sqlite3_stmt *stmt = NULL;
    int stmt_arg_idx = 1;
    char *sql_stmt;
    int status_sql;
    tri_status_t status;

    if (tri_db_sqlite_begin_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error beginning transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    sql_stmt = "SELECT * FROM call_graph";

    if (sqlite3_prepare_v2(g_sqlite_ctx, sql_stmt,
                           -1, &stmt, NULL) != SQLITE_OK)
    {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    printf("Column Count: %d\n", sqlite3_column_count(stmt));
    fflush(stdout);

    tri_log_debug("CALL GRAPH:");
    int i = 0;
    do {
        status_sql = sqlite3_step(stmt);
        if (status_sql == SQLITE_ROW) {
            caller_block = sqlite3_column_int64(stmt, 1);
            caller_addr = sqlite3_column_int64(stmt, 2);
            callee_block = sqlite3_column_int64(stmt, 3);
            callee_addr = sqlite3_column_int64(stmt, 4);
            printf("%x %x %x %x\n", caller_block, caller_addr,
                   callee_block, callee_addr);
            fflush(stdout);
            i++;
        } else if (status_sql == SQLITE_DONE) {
            break;
        } else if (status_sql == SQLITE_ERROR) {
            tri_log_error("sqlite3_step() failure against context %p - %s",
                          g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
            status = TRI_STATUS_ERROR;
            goto return_status;
        }
    } while (1);

    if (tri_db_sqlite_commit_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error commiting transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
    }
    return status;
}

tri_status_t tri_db_print_blocks()
{
    uint64_t block_addr;
    const unsigned char *block_name;

    sqlite3_stmt *stmt = NULL;
    int stmt_arg_idx = 1;
    char *sql_stmt;
    int status_sql;
    tri_status_t status;

    if (tri_db_sqlite_begin_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error beginning transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    sql_stmt = "SELECT * FROM block";

    if (sqlite3_prepare_v2(g_sqlite_ctx, sql_stmt,
                           -1, &stmt, NULL) != SQLITE_OK)
    {
        tri_log_error("sqlite3_prepare() failure against context %p - %s",
                      g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    tri_log_debug("BLOCK:");
    int i = 0;
    do {
        status_sql = sqlite3_step(stmt);
        if (status_sql == SQLITE_ROW) {
            block_addr = sqlite3_column_int64(stmt, 1);
            block_name = sqlite3_column_text(stmt, 2);
            printf("%x %s\n", block_addr, block_name);
            fflush(stdout);
            i++;
        } else if (status_sql == SQLITE_DONE) {
            break;
        } else if (status_sql == SQLITE_ERROR) {
            tri_log_error("sqlite3_step() failure against context %p - %s",
                          g_sqlite_ctx, sqlite3_errmsg(g_sqlite_ctx));
            status = TRI_STATUS_ERROR;
            goto return_status;
        } else {
            BUG_ON(1);
        }
    } while (1);

    if (tri_db_sqlite_commit_transaction() != TRI_STATUS_OK) {
        tri_log_error("Error commiting transaction.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    if (stmt != NULL) {
        sqlite3_finalize(stmt);
    }
    return status;
}
