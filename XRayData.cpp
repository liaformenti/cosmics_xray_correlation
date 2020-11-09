#define XRayData_cxx
#include "XRayData.h"

using namespace std;

XRayData::XRayData(string databaseName, AnalysisInfo* _cinfo,
                   InputInfo* _myInfo, PlotManager* _pm) : 
cinfo(_cinfo), myInfo(_myInfo), pm(_pm) {
    // Check if there is xray data for quad
    // Do so by checking if wedgeid was filled
    if (myInfo->wedgeid == "") {
        cout << "No xray data available for quadruplet ";
        cout << myInfo->quadname << ". XRayData object not filled ";
        cout << "(XRayData constructor).\n\n";
        return;
    }
    
    // Get relevant data from database
    sqlite3* db;
    sqlite3_stmt *stmt;
    int rc;
    sqlite3_open(databaseName.c_str(), &db);
    // run_id is to check run0 vs run1 (gun angle bias)
    // mtf is to check you have the correct wedge for the quad
    // gv is gasvolume == layer
    // Take dq_flags 'OK', 'LARGEOFFSET' and 'WARNING_*'
    // my xbeam = xray database xbeam
    // my ybeam = xray database y_jigcmm_holdercmm
    // y_meas is used to calculate offset
    string sql = "SELECT run_id, mtf, quad_type, gv, dq_flag, x_nom, ";
    sql += "y_jigcmm_holdercmm, y_meas, y_meas_error ";
    sql += "FROM results "; // assumes table name is results!
    sql += "WHERE dq_flag in ('OK', 'LARGEOFFSET', 'WARNING_VMMEDGE',";
    sql += "'WARNING_LARGEOFFSET', 'WARNING_NEAR_WIRE_SUPPORT')";
    sql += "AND quad_type = \'" + cinfo->detectortype + "\' ";
    sql += "ORDER BY x_nom"; 
    //*** GET RID OF NOMs here
    // Order by ascending x_nom necessary so only unique nominal
    // positions are recorded in ptVec (vector of XRayPt's)

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
    // Temp var names match xray data database
    string run_id, mtf, dq_flag;
    UShort_t gv; 
    Double_t xbeam, y_jigcmm_holdercmm, y_meas, y_meas_error; 
    Double_t offset, offsetError;
    // For indexing xray points
    // Each time an XRayPt is added to the ptVec, for EACH TIME THE 
    // CONSTRUCTOR IS CALLED, num is incremented.
    static Int_t num = 0; 

    // For all selected rows
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Get and check row data
        run_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        mtf = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        // If wrong wedge or run1 instead of run0, skip
        // Note that you could have done mtf cut with SELECT statement
        if ((mtf != myInfo->wedgeid) || 
            (run_id.substr(run_id.size() - 4) != "run0")) {
            continue;
        }

        // Get column values
        gv = (UShort_t)(sqlite3_column_int(stmt, 3));
        dq_flag = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        xbeam = (Double_t)(sqlite3_column_double(stmt, 5));
        y_jigcmm_holdercmm = (Double_t)(sqlite3_column_double(stmt, 6));
        y_meas = (Double_t)(sqlite3_column_double(stmt, 7));
        y_meas_error = (Double_t)(sqlite3_column_double(stmt, 8));
        offset = y_jigcmm_holdercmm - y_meas;
        offsetError = y_meas_error; // Adding 1 error in quadrature

        // cout << run_id << ' ' << mtf << ' ' << gv << ' ' << dq_flag << ' ' << xbeam << ' ' << y_jigcmm_holdercmm << ' ' << y_meas << ' ' << y_meas_error << ' ' << offset << ' ' << offsetError << '\n';
       
        // If position is new, add point entry to pointVec
        // Deal with 1st entry separately to prevent seg fault
        XRayPt point;
        if (pointVec.size() == 0) {
            // Initialize point with column values
            point.num = num;
            point.xbeam = xbeam;
            point.ybeam = y_jigcmm_holdercmm;
            point.dqFlags.insert(pair<UShort_t, string>(gv, dq_flag));
            point.offsets.insert(pair<UShort_t, Double_t>(gv, offset));
            point.offsetErrors.insert(
                pair<UShort_t, Double_t>(gv, offsetError));
            // Add to member vector
            pointVec.push_back(point);
            num++; // Increment num for indexing
        }
        // If last x and y position are different, initialize new point
        // and push_back. This is same procedure as for first point.
        else if ((abs(xbeam - pointVec.back().xbeam) > 0.1) || 
             (abs(y_jigcmm_holdercmm - pointVec.back().ybeam) > 0.1)) {
            // Initialize point
            point.num = num;
            point.xbeam = xbeam; 
            point.ybeam = y_jigcmm_holdercmm;
            point.dqFlags.insert(pair<UShort_t, string>(gv, dq_flag));
            point.offsets.insert(pair<UShort_t, Double_t>(gv, offset));
            point.offsetErrors.insert(
                pair<UShort_t, Double_t>(gv, offsetError));
            // Add member to vector
            pointVec.push_back(point);
            num++;
        }
        else {
            // Looking at data for same xbeam, ybeam
            // Add new gv data to maps of last pushed point
            // This works because you ordered rows by xbeam in SELECT,
            // so last point pushed back is always of same xbeam, ybeam
            // since you handled the case of unique xbeams and ybeams
            // already.
            // If gv key DNE, add new gv offset to map
            // else, print warning and do not overwrite
            if (pointVec.back().offsets.find(gv) == 
                pointVec.back().offsets.end()) {
                pointVec.back().dqFlags.insert(pair<UShort_t, string>
                    (gv, dq_flag));
                pointVec.back().offsets.insert(pair<UShort_t, Double_t>
                    (gv, offset));
                pointVec.back().offsetErrors.insert(
                    pair<UShort_t, Double_t>(gv, offsetError));
            }
            else {
                cout << "Warning: found duplicate row for xray data\n";
                cout << run_id << ' ' << mtf << ' ' << gv << ' '; 
                cout << y_meas << ' ' << y_meas_error << ' ';
                cout << dq_flag << ' ' << xbeam << ' ';
                cout << y_jigcmm_holdercmm << "\n\n";
            }
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return;
}

// Plots the positions of all the xray point in pointVec
// Should add this to plot manager (need to send in plot manager)
void XRayData::PlotPositions() {
    if (pointVec.size() == 0) {
        cout << "Warning: no xray data positions. Position plots not created (XRayData::PlotPositions).\n\n";
        return;
    }    
    // Copy positions for binning into arrays
    Double_t x[pointVec.size()];
    Double_t y[pointVec.size()];
    for (UInt_t i=0; i<pointVec.size(); i++) {
        x[i] = pointVec.at(i).xbeam;
        y[i] = pointVec.at(i).ybeam;
    }
    // Initialize plot
    pm->Add("xray_positions_" + myInfo->quadname, 
            "X-ray positions for" + myInfo->quadname +";x [mm];y [mm]",
            pointVec.size(), x, y, myTGraph);
    // Draw
    TCanvas* c = new TCanvas();
    TGraph* xRayGraph = (TGraph*)pm->Get(
        "xray_positions_" + myInfo->quadname);
    xRayGraph->Draw("ap");
    c->Print((myInfo->outpath + myInfo->quadname + "_xray_positions.pdf").c_str());
    delete c;

    return;
}

void XRayData::WriteOutXRayData() {
    // To print out offsets for each xray position to file
    // Caution: no guard against map members not being
    // the same length (shouldn't happen, controlled in constructor)
    ofstream f;
    f.open(myInfo->outpath + myInfo->quadname + "_xray_data_offsets.txt");
    f << "Point number, beam x position, beam y position, ";
    f << "layer, dq flag, offset, offset error (as exists, in mm)\n";
    for (auto p=pointVec.begin(); p!=pointVec.end(); p++) {
        f << p->num << ' ' << p->xbeam << ' ' << p->ybeam << ' ';
        for (auto off=p->offsets.begin(); off!=p->offsets.end(); off++)
        {
            f << off->first << ' ' << p->dqFlags.at(off->first) << ' ';
            f << off->second << ' ' << p->offsetErrors.at(off->first);
            f << ' ';
        }
        f << '\n';
    }
    f.close();
    return;
}

// Note: keys are guaranteed to be ordered smallest to largest
// (feature of map object).
// Returned pairs are in enforced order (used throughout this analysis)
// Technically, and map from the desired point struct would do as 
// argument, but keep name as offset for simplicity and readability
vector<pair<UShort_t, UShort_t>> XRayPt::GetDiffCombos() {
    vector<pair<UShort_t, UShort_t>> diffCombos;
    UShort_t size = offsets.size();
    // Get keys of offsets map and copy them to array
    UShort_t keys[size];
    Int_t i = 0;
    for (auto m=offsets.begin(); m!=offsets.end(); m++) {
        keys[i] = m->first;
        i++;
    }
    // Brute force determine different pairs of two layers to take
    // offset difference
    switch (size) {
        case 1:
            // return empty if not enough data to take difference
            break;
        case 2:
            diffCombos.push_back(make_pair(keys[0], keys[1]));
            break;
        case 3:
            diffCombos.push_back(make_pair(keys[0], keys[1]));
            diffCombos.push_back(make_pair(keys[0], keys[2]));
            diffCombos.push_back(make_pair(keys[1], keys[2]));
            break;
        case 4:
            diffCombos.push_back(make_pair(keys[0], keys[1]));
            diffCombos.push_back(make_pair(keys[0], keys[2]));
            diffCombos.push_back(make_pair(keys[0], keys[3]));
            diffCombos.push_back(make_pair(keys[1], keys[2]));
            diffCombos.push_back(make_pair(keys[1], keys[3]));
            diffCombos.push_back(make_pair(keys[2], keys[3]));
        default:
            cout << "Warning: Layer-xray offset map sent to XRayData::GetDiffCombos has less than one or more than four entries.\n\n";
            break;
    }
    return diffCombos;

}
