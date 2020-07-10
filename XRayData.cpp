#define XRayData_cxx
#include "XRayData.h"

using namespace std;

XRayData::XRayData(string databaseName, AnalysisInfo* _cinfo,
                   InputInfo* _myInfo) : cinfo(_cinfo), 
                   myInfo(_myInfo) {
    // Check if there is xray data for quad
    if (myInfo->wedgeid == "") {
        cout << "No xray data available for quadruplet ";
        cout << myInfo->quadname << ". XRayData object not filled ";
        cout << "(XRayData constructor).\n\n";
        return;
    }
    
    // Get relelvant data fom base
    sqlite3* db;
    sqlite3_stmt *stmt;
    int rc;
    sqlite3_open(databaseName.c_str(), &db);
    // run_id is to check run0 vs run1 (gun angle bias)
    // mtf is to check you have the correct wedge for the quad
    // gv is gasvolume == layer
    // For now, only take pts with dq_flag OK
    // xnom and ynom are so you know where to bin
    // y_jigcmm_holdercmm is used to calculate offset
    string sql = "SELECT run_id, mtf, quad_type, gv, dq_flag, x_nom, ";
    sql += "y_nom, y_jigcmm_holdercmm ";
    sql += "FROM results "; // assumes table name is results!
    sql += "WHERE dq_flag = 'OK' AND quad_type = " ;
    sql += "\'" + cinfo->detectortype + "\' ";
    sql += "ORDER BY x_nom"; 
    // Order by ascending x_nom necessary so only unique nominal
    // positions are recorded in xnoms and ynoms 

    // Prepare query
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    // if query fails, stop construction
    if (rc != SQLITE_OK) { 
        string msg(sqlite3_errmsg(db));
        cout << "Unable to access xray data, SELECT failed: " + msg + "\n";
        sqlite3_finalize(stmt);
        return;
    }

    // Vars to hold column entries and calculated offset
    string runid, mtf;
    UShort_t gv; 
    Double_t xnom, ynom, yjigcmmholdercmm, offset;

    // For all selected rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        runid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        mtf = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        // If wrong wedge or run1 instead of run0, skip
        if ((mtf != myInfo->wedgeid) || 
            (runid.substr(runid.size() - 4) != "run0")) {
            continue;
        }
        // Add unique xnoms and y noms to object vectors
        xnom = (Double_t)(sqlite3_column_double(stmt, 5));
        ynom = (Double_t)(sqlite3_column_double(stmt, 6));
        gv = (UShort_t)(sqlite3_column_int(stmt, 3));
        yjigcmmholdercmm = (Double_t)(sqlite3_column_double(stmt, 7));
        offset = ynom - yjigcmmholdercmm;

        // If position is new, add it to vectors
        // Deal with 1st entry separately to prevent seg fault
        if (xnoms.size() == 0) { 
            xnoms.push_back(xnom);
            ynoms.push_back(ynom);
            // Add new offset for gas volume
            // First, create new map
            map<UShort_t, Double_t> offsetPerGV;
            // Second, add new key
            offsetPerGV.insert(pair<UShort_t, Double_t>(gv, offset));
            // Finally, pushback new map
            offsets.push_back(offsetPerGV);
        }
        else if ((abs(xnom - xnoms.back()) > 0.1) || (abs(ynom - ynoms.back()) > 0.1)) {
            xnoms.push_back(xnom);
            ynoms.push_back(ynom);
            // Add new offset for gas volume
            // First, create new map
            map<UShort_t, Double_t> offsetPerGV;
            // Second, add new key
            offsetPerGV.insert(pair<UShort_t, Double_t>(gv, offset));
            // Finally, pushback new map
            offsets.push_back(offsetPerGV);
        }
        else {
            // Add offset for new gv for last push_backed map
            // If gv key DNE, add new gv offset to map
            // else, print warning and do not overwrite
            if (offsets.back().find(gv) == offsets.back().end()) {
                offsets.back().insert(pair<UShort_t, Double_t>(gv, 
                                                         offset));
            }
            else {
                cout << "Warning: found duplicate row for xray data\n";
                cout << runid << ' ' << mtf << ' ' << gv << ' '; 
                cout << xnom << ' ' << ynom << "\n\n";
            }
        }
    }
    sqlite3_finalize(stmt);
}
