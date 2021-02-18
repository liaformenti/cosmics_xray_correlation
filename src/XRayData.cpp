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
    // Select statement chooses desired quadruplet and groups layer data by gun position.
    string sql = "SELECT quad_name, gv, x_beam, y_beam, offset, offset_error, platform_id, ";
    sql += "position_number FROM xraydata WHERE quad_name='" + myInfo->quadname; 
    sql += "' ORDER BY platform_id, position_number;";
    // cout << sql << '\n';
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
    string quad_name, position_number;
    UShort_t gv; 
    Int_t platform_id;
    Double_t x_beam, y_beam, offset, offset_error;
    // For indexing xray points
    // Each time an XRayPt is added to the ptVec, for each time the
    // constructor is called, num is incremented.
    // static Int_t num = 0; 

    // For all selected rows
    Int_t rowCount = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        // Get column values
        gv = (UShort_t)(sqlite3_column_int(stmt, 1));
        x_beam = (Double_t)(sqlite3_column_double(stmt, 2));
        y_beam = (Double_t)(sqlite3_column_double(stmt, 3));
        offset = (Double_t)(sqlite3_column_double(stmt, 4));
        offset_error = (Double_t)(sqlite3_column_double(stmt, 5));
        platform_id = (Int_t)(sqlite3_column_int(stmt, 6));
        position_number = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        // cout << gv << ' ' << x_beam << ' ' << y_beam << ' ' << offset << ' ';
        // cout << offset_error << ' ' << platform_id << ' ' << position_number << '\n';

        // If position is new, add point entry to pointVec
        // Deal with 1st entry separately to prevent seg fault
        XRayPt point;
        if (pointVec.size() == 0) {
            // Initialize point with column values
            // point.num = num;
            point.platformID = platform_id;
            point.positionNumber = position_number;
            // point.xbeam = x_beam;
            // point.ybeam = y_beam;
            point.xbeams.insert(pair<UShort_t, Double_t>(gv, x_beam));
            point.ybeams.insert(pair<UShort_t, Double_t>(gv, y_beam));
            point.offsets.insert(pair<UShort_t, Double_t>(gv, offset));
            point.offsetErrors.insert(
                pair<UShort_t, Double_t>(gv, offset_error));
            // Add to member vector
            pointVec.push_back(point);
        }
        // If we're at a new gun position (platform ID and position number),
        else if (platform_id != pointVec.back().platformID or 
                 position_number != pointVec.back().positionNumber) {
            // Initialize point
            // point.num = num;
            point.platformID = platform_id;
            point.positionNumber = position_number;
            // point.xbeam = x_beam; 
            // point.ybeam = y_beam;
            point.xbeams.insert(pair<UShort_t, Double_t>(gv, x_beam));
            point.ybeams.insert(pair<UShort_t, Double_t>(gv, y_beam));
            point.offsets.insert(pair<UShort_t, Double_t>(gv, offset));
            point.offsetErrors.insert(
                pair<UShort_t, Double_t>(gv, offset_error));
            // Add member to vector
            pointVec.push_back(point);
            // num++;
        }
        else {
            // Looking at data for same gun position
            // Add new gv data to maps of last pushed point
            // This works because you ordered rows by platform id and position number in SELECT,
            // so last point pushed back is always from the same gun position.
            // You handled the case of new gun positions above.
            // If gv key DNE, add new gv offset to map
            // else, print warning and do not overwrite
            if (pointVec.back().offsets.find(gv) == pointVec.back().offsets.end()) {
                pointVec.back().xbeams.insert(pair<UShort_t, Double_t>(gv, x_beam));
                pointVec.back().ybeams.insert(pair<UShort_t, Double_t>(gv, y_beam));
                pointVec.back().offsets.insert(pair<UShort_t, Double_t>(gv, offset));
                pointVec.back().offsetErrors.insert(pair<UShort_t, Double_t>(gv, offset_error));
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

    // Check
    /*cout << "CHECK\n";
    for (auto point=pointVec.begin(); point!=pointVec.end(); point++) {
        for (auto off=point->offsets.begin(); off!=point->offsets.end(); off++) {
        cout << point->platformID << ' ' << point->positionNumber << ' '<< point->xbeams.at(off->first) << ' ' << point->ybeams.at(off->first) << ' ' << off->first << ' ' << off->second << ' ' << point->offsetErrors.at(off->first) << '\n';
        }
        cout << '\n';
    }*/
    return;
}

// Plots the positions of all the xray point in pointVec
// Uses the average beam position across all four layers.
// Should add this to plot manager (need to send in plot manager)
void XRayData::PlotAverageBeamPositions() {
    if (pointVec.size() == 0) {
        cout << "Warning: no xray data positions. Position plots not created (XRayData::PlotPositions).\n\n";
        return;
    }    
    // Copy positions for binning into arrays
    Double_t x[pointVec.size()];
    Double_t y[pointVec.size()];
    for (UInt_t i=0; i<pointVec.size(); i++) {
        Double_t avgX = 0;
        for (auto x=pointVec.at(i).xbeams.begin(); x!=pointVec.at(i).xbeams.end(); x++) {
            avgX += x->second;
        }
        avgX /= pointVec.at(i).xbeams.size();
        x[i] = avgX;
        Double_t avgY = 0;
        for (auto y=pointVec.at(i).ybeams.begin(); y!=pointVec.at(i).ybeams.end(); y++) {
            avgY += y->second;
        }
        avgY /= pointVec.at(i).ybeams.size();
        y[i] = avgY;
    }
    // Initialize plot
    pm->Add("xray_positions_" + myInfo->quadname, 
            "X-ray gun positions for " + myInfo->quadname +";x [mm];y [mm]",
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
    f << "Platform ID, position number, layer, beam x position, beam y position, ";
    f << "offset, offset error\n";
    for (auto p=pointVec.begin(); p!=pointVec.end(); p++) {
        for (auto off=p->offsets.begin(); off!=p->offsets.end(); off++) { 
            f << p->platformID << ' ' << p->positionNumber << ' ' << off->first << ' '; 
            f << p->xbeams.at(off->first) << ' ' << p->ybeams.at(off->first) << ' ';
            f << off->second << ' ' << p->offsetErrors.at(off->first);
            f << '\n';
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
