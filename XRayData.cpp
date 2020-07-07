#define XRayData_cxx
#include "XRayData.h"

using namespace std;

XRayData::XRayData(string databaseName, AnalysisInfo &info) {
    // Get relelvant data fom base
    sqlite3* db;
    sqlite3_stmt *stmt;
    int rc;
    sqlite3_open(databaseName.c_str(), &db);
    string sql = "SELECT run_id, quad_type, gv, dq_flag, x_nom, ";
    sql += "y_nom, y_jigcmm_holdercmm ";
    sql += "FROM results "; // assumes table name is results!
    sql += "WHERE dq_flag = 'OK' AND quad_type = " ;
    sql += "\'" + info.detectortype + "\' ";
    sql += "ORDER BY x_nom"; // Order by ascending x_nom for processing
    cout << sql << endl;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) { 
        string msg(sqlite3_errmsg(db));
        throw logic_error("Unable to access xray data, SELECT failed: " + msg + "\n");
    }
    // Now do the while loop
}
