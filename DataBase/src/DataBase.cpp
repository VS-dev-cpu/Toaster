#include <DataBase.hpp>

DataBase::DataBase(const char *file) {
    rc = sqlite3_open(file, &db);

    if (rc != SQLITE_OK)
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    else
        fprintf(stdout, "Opened database\n");
}

DataBase::~DataBase() { sqlite3_close(db); }

const char *DataBase::execute(const char *op, ...) {
    char *sql;

    va_list args;
    va_start(args, op);
    vasprintf(&sql, op, args);
    va_end(args);

    rc = sqlite3_exec(db, sql, callback, this, nullptr);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "DataBase Error: %s\n", sqlite3_errmsg(db));
        return "";
    }

    return data.c_str();
}

int DataBase::callback(void *data, int argc, char **argv, char **azColName) {
    DataBase *it = static_cast<DataBase *>(data);

    if (argc > 0 && argv[0] != nullptr) {
        // Process the row data here
        it->data = argv[0];
    }

    return 0;
}