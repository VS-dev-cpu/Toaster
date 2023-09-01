#include <DataBase.hpp>

DataBase::DataBase(const char *file) {
    rc = sqlite3_open(file, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open DataBase: %s\n", sqlite3_errmsg(db));
        db = nullptr;
    } else
        fprintf(stdout, "Opened DataBase\n");
}

DataBase::~DataBase() {
    if (db != nullptr) {
        sqlite3_close(db);
        fprintf(stdout, "Closed DataBase\n");
    }
}

bool DataBase::run(const char *op, ...) {
    char *sql;

    va_list args;
    va_start(args, op);
    vasprintf(&sql, op, args);
    va_end(args);

    rc = sqlite3_exec(db, sql, callback, this, nullptr);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "DataBase Error: %s\n", sqlite3_errmsg(db));
        return false;
    }

    return true;
}

int DataBase::callback(void *data, int argc, char **argv, char **azColName) {
    DataBase *it = static_cast<DataBase *>(data);

    for (int i = 0; i < argc; i++) {
        if (argv[i] != nullptr)
            it->data.push_back(argv[0]);
    }

    return 0;
}