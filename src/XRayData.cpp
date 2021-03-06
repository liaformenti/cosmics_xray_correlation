#define XRayData_cxx
#include "XRayData.h"

// This is the file you need to edit if the format of the x-ray data changes or you want to change
// the selection of which x-ray measurements to use.

using namespace std;

XRayData::XRayData(AnalysisInfo* _cinfo,
                   InputInfo* _myInfo, PlotManager* _pm) : 
cinfo(_cinfo), myInfo(_myInfo), pm(_pm) {

    // Check quad name
    if (quadToWedge.find(myInfo->quadname) == quadToWedge.end()) {
        throw runtime_error("Cannot find wedge corresponding to quadruplet. Check that x-ray data is available for the given quadruplet and that the format of the quadruplet name was entered correctly. Eg. correct format: QS3P06.\n\n");
    }

    // Get relevant data from database
    sqlite3* db;
    sqlite3_stmt *stmt;
    int rc;
    sqlite3_open((myInfo->database).c_str(), &db);
    // gas_volume == layer
    // Select statement chooses desired quadruplet and groups layer data by gun position.
    // Current select statement only selects production runs with newest ball mount for x-ray
    // platform. Cuts entries with the data quality flag "BADCHANNEL" and "WIRESUPPORT"
    // ("STRUCT_" or "WARNING_" are OK)
    // Orders by xnom to proxy ordering by platform ID and position number (gun position).
    // Edit select statement to change cuts on x-ray data.
    string moduleType = myInfo->quadname.substr(0, 4);
    string mtfStr = quadToWedge.at(myInfo->quadname);
    string sql = "SELECT run_id, mtf, run_type, mount_type, quad_type, gv, y_meas_raw_w, ";
    sql += "y_meas_raw_error_w, dq_flag, x_nom, y_corrangle from results ";
    sql += "where mtf=\"" + mtfStr + "\" and ";
    sql += "run_type=\"PRODUCTION\" and ";
    // Stergios says you can use all mount types
    // sql += "mount_type=\"NEW_BALL\" and ";
    sql += "quad_type=\"" + moduleType + "\" and ";
    sql += "dq_flag not like \"%BADCHANNEL%\" and dq_flag not like \"WIRESUPPORT%\" and ";
    sql += "dq_flag not like \"%LOWNENTRIES%\" and ";
    sql += "y_corrangle is not null ";
    sql += "order by x_nom;";
    cout << sql << '\n';

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
    // Example: run_id=WLAP00003_GV1_0_A_run0
    //     - platform_id = "0"
    //     - position_number = "A"
    //     - run_number = "run0"
    string run_id, platform_id, position_number, run_number;
    UShort_t gv; 
    // x_ and y_beam are expected x-ran gun profile centroids,
    // y_meas is measured x-ray gun profile centroid.
    Double_t x_beam, y_beam, y_meas, offset, offset_error;
    /***
     * Note about offset_error: there is an offset error recorded in the database, but it is
     * only the uncertainty on the fitted centroid, and does not account for systematics. Currently
     * hard coding the uncertainty as 120um as in Benoit's email with subject "Final X-ray dataset"
     * sent on 2020-12-08.
     ***/

    // For all selected rows
    Int_t rowCount = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {

        // Get column values
        run_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        ParseRunID(run_id, platform_id, position_number, run_number);
        // Skip entry if run_id is not labelled with run number zero.
        // Other runs are R&D runs
        if (run_number!="run0") {
            continue;
        }
        gv = (UShort_t)(sqlite3_column_int(stmt, 5));
        x_beam = (Double_t)(sqlite3_column_double(stmt, 9));
        y_beam = (Double_t)(sqlite3_column_double(stmt, 10));
        y_meas = (Double_t)(sqlite3_column_double(stmt, 6)); 
        offset = y_meas - y_beam;
        // cout << run_id << ' ' << platform_id << ' ' << position_number << ' ' << gv << ' ' << x_beam << ' ' << y_beam << ' ' << y_meas << ' ' << offset << ' ' << offset_error << '\n';
        // Use fixed offset error of 120um based on Benoit's email "Final X-ray dataset",
        // 2020-12-08
        // UNCOMMENT NEXT LINE!! JUST SEEING WHAT RECRODED OFFSET_ERROR IS NOW
        offset_error = 0.120; // mm, from Benoit's email 2020-12-08

        // If position is new, add point entry to pointVec
        // Deal with 1st entry separately to prevent seg fault
        XRayPt point;
        if (pointVec.size() == 0) {
            // Initialize point with column values
            // point.num = num;
            point.platformID = stoi(platform_id);
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
        else if (stoi(platform_id) != pointVec.back().platformID or 
                 position_number != pointVec.back().positionNumber) {
            // Initialize point
            // point.num = num;
            point.platformID = stoi(platform_id);
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
        }
        else {
            // Looking at data for same gun position
            // Add new gv data to maps of last pushed point
            // This works because you ordered rows by x_nom in SELECT,
            // which orders by platform id and position number,
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

/*void XRayData::csv2db(string inFileName, string outFileName) {
    // Get input csv
    ifstream inFile;
    inFile.open(inFileName);
    if (!inFile.is_open()) {
        throw runtime_error("Error opening .csv file.\n");
    }

    // Create output db
    sqlite3 *db;
    // db name is same as input csv file => cut off .csv
    string dbName = outFileName;
    sqlite3_open(dbName.c_str(), &db); 
    
    // Prepare for sqlite3 commands
    // sqlite3_stmt* stmt;
    int rc;
    char* zErrMsg = 0;

    // Create output table
    string sqlCreateTable = "CREATE TABLE RESULTS (RUN_ID TEXT, MODULE TEXT, GAS_VOLUME INTEGER, X_BEAM REAL, Y_BEAM REAL, Y_MEAS REAL, Y_MEAS_ERROR REAL, PLATFORM_ID INTEGER, POSITION_NUMBER TEXT);";
    rc = sqlite3_exec(db, sqlCreateTable.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        cout << "sqlite3 error: " << zErrMsg << '\n';
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        throw runtime_error("Error creating database from x-ray csv file.\n\n");
    }

    // Parse out column names
    int ncols = 9;
    string colNames[ncols];
    string headers;
    getline(inFile, headers);
    headers += ",PLATFORM_ID,POSITION_NUMBER";

    string valuesString;
    string token = "";
    string runID, platID, posNum;
    string sql = "";
    // int count = 0;
    
    while (getline(inFile, valuesString)) {
       //  cout << valuesString << '\n';
        stringstream valuesStream;
        valuesStream << valuesString;
        valuesString = ""; // Clear values string to be refilled with same but correct sql format
        getline(valuesStream, runID, ',');
        // cout << runID << '\n';

        // Parse out the survery point information from the run_id and put it in the values array
        ParseRunID(runID, platID, posNum);
        valuesString += "\'" + runID + "\',"; // RUN_ID

        getline(valuesStream, token, ',');
        valuesString += "\'" + token + "\',"; // MODULE
        
        for (int i=0; i<ncols-4; i++) { // For the non-string values,
            getline(valuesStream, token, ',');
            valuesString += token + ",";
        }    

        valuesString += platID + "," + "\'" + posNum + "\'"; // Add the survery pt info
        // cout << valuesString << '\n';


        sql = "INSERT INTO RESULTS (" + headers + ") VALUES (" + valuesString + ");";
        // cout << sql << '\n';
        // valuesStream = stringstream(); // clear the stream by resetting it to default
        valuesString = "";

        // NOW DO THE SQL
        rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cout << "sqlite3 error: " << zErrMsg << '\n';
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
            throw runtime_error("Error inserting into table.\n");
        }
        // if (count>3) break;
        // count++;
        // cout << '\n';
    }
    sqlite3_close(db);
    inFile.close();    
    return;
}*/

void XRayData::ParseRunID(string runID, string& platformID, string& positionNumber,
                          string& runNumber) {
    char token = '_';
    string wedgeID, gasVol;
    stringstream ss(runID);
    getline(ss, wedgeID, token);
    getline(ss, gasVol, token);
    getline(ss, platformID, token);
    getline(ss, positionNumber, token);
    getline(ss, runNumber);
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
