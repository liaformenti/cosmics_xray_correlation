#define XRayData_cxx
#include "XRayData.h"

using namespace std;

XRayData::XRayData(AnalysisInfo* _cinfo,
                   InputInfo* _myInfo, PlotManager* _pm) : 
cinfo(_cinfo), myInfo(_myInfo), pm(_pm) {

    // Get relevant data from database
    sqlite3* db;
    sqlite3_stmt *stmt;
    int rc;
    sqlite3_open(myInfo->database.c_str(), &db);
    // gv is gasvolume == layer
    string sql = "SELECT quad_name, gv, x_beam, y_beam, offset, offset_error";
    sql += " FROM xraydata WHERE quad_name='" + myInfo->quadname + "' ORDER BY x_beam;";
    // Order by ascending x_beam necessary so only unique beam
    // positions are recorded in ptVec (vector of XRayPt's with each point's layer data)

    // Prepare query
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    // if query fails, stop construction
    if (rc != SQLITE_OK) { 
        string msg(sqlite3_errmsg(db));
        cout << "Unable to access xray data, SELECT failed: " + msg + "\n";
        sqlite3_finalize(stmt);
        return;
    }

    // Vars to hold column entries 
    // Temp var names match xray data database
    string quad_name;
    UShort_t gv; 
    Double_t x_beam, y_beam, offset, offset_error;
    // For indexing xray points
    // Each time an XRayPt is added to the ptVec, for each time the
    // constructor is called, num is incremented.
    static Int_t num = 0; 

    // For all selected rows
    Int_t rowCount = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        // Get column values
        gv = (UShort_t)(sqlite3_column_int(stmt, 1));
        x_beam = (Double_t)(sqlite3_column_double(stmt, 2));
        y_beam = (Double_t)(sqlite3_column_double(stmt, 3));
        offset = (Double_t)(sqlite3_column_double(stmt, 4));
        offset_error = (Double_t)(sqlite3_column_double(stmt, 5));

        // If position is new, add point entry to pointVec
        // Deal with 1st entry separately to prevent seg fault
        XRayPt point;
        if (pointVec.size() == 0) {
            // Initialize point with column values
            point.num = num;
            point.xbeam = x_beam;
            point.ybeam = y_beam;
            point.offsets.insert(pair<UShort_t, Double_t>(gv, offset));
            point.offsetErrors.insert(
                pair<UShort_t, Double_t>(gv, offset_error));
            // Add to member vector
            pointVec.push_back(point);
            num++; // Increment num for indexing
        }
        // If last x and y position are different, initialize new point
        // and push_back. This is same procedure as for first point.
        // ******* UNTIL YOU CONFIRM THERE IS A Y_BEAM CORRECTION
        // ******* PER LAYER, THE CONDITION FOR "DIFFERENT XRAY POINT"
        // ******* MAY CHANGE.
        // ******* 0.1MM FOR X WAS BASED ON APRIL RESULTS QS3P06
        // ******* 3MM FOR Y WAS BASED ON OCT RESULTS FOR QL2P06
        // ******* Could also preprocess run_id to get which platform
        // ******* and which ball
        else if ((abs(x_beam - pointVec.back().xbeam) > 0.1) || 
             (abs(y_beam - pointVec.back().ybeam) > 3)) {
            // Initialize point
            point.num = num;
            point.xbeam = x_beam; 
            point.ybeam = y_beam;
            point.offsets.insert(pair<UShort_t, Double_t>(gv, offset));
            point.offsetErrors.insert(
                pair<UShort_t, Double_t>(gv, offset_error));
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
                pointVec.back().offsets.insert(pair<UShort_t, Double_t>
                    (gv, offset));
                pointVec.back().offsetErrors.insert(
                    pair<UShort_t, Double_t>(gv, offset_error));
            }
            else {
                cout << "Warning: found duplicate row for xray data\n";
                cout << gv << ' ' << x_beam << ' ' << y_beam << "\n\n"; 
            }
        } 
        rowCount += 1;
    } // End row loop 

    // Print a warning if no rows were returned
    if (rowCount == 0) {
        cout << "Warning: no rows were returned from query.";
        cout << " Check that you entered the quarduplet name";
        cout << "  and database name were entered correctly\n\n";
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
    f << "layer, offset, offset error (as exists, in mm)\n";
    for (auto p=pointVec.begin(); p!=pointVec.end(); p++) {
        f << p->num << ' ' << p->xbeam << ' ' << p->ybeam << ' ';
        for (auto off=p->offsets.begin(); off!=p->offsets.end(); off++)
        {
            f << off->first << ' ' << off->second << ' ';
            f << p->offsetErrors.at(off->first) << ' ';
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
