// Takes in an old-format x-ray csv file and converts it to an sqlite3 database.
// Was written for an old format of the x-ray data, but can serve as an example of how to do this.
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// Compile line: 
// conda activate cern_root (Lia's ubuntu)
// g++ -I ~/miniconda3/include -L ~/miniconda3/lib -lsqlite3 csv2db.cpp

void parseRunID(string runID, string& platformID, string& positionNumber) {
        char token = '_';
        string wedgeID, gasVol, runNumber;
        stringstream ss(runID);
        getline(ss, wedgeID, token);
        getline(ss, gasVol, token);
        getline(ss, platformID, token);
        int platID = stoi(platformID);
        getline(ss, positionNumber, token);
        getline(ss, runNumber);
        return;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "You must provide the name of the input file.\n";
        return 0;
    }
    // Get input csv
    string inFileName = argv[1];
    ifstream inFile;
    inFile.open(inFileName);
    if (!inFile.is_open()) {
        throw runtime_error("Error opening .csv file.\n");
    }
    
    // Create output db
    sqlite3 *db;
    // db name is same as input csv file => cut off .csv
    string dbName = inFileName.substr(0, inFileName.find(".")) + ".db";
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
        return 0;
    }

    // Parse out column names
    int ncols = 9;
    string colNames[ncols];
    string headers;
    getline(inFile, headers);
    headers += ",PLATFORM_ID,POSITION_NUMBER";

    string valuesString;
    stringstream valuesStream;
    string token = "";
    string runID, platID, posNum;
    string sql = "";
    // int count = 0;
    
    while (getline(inFile, valuesString)) {
       //  cout << valuesString << '\n';
        valuesStream << valuesString;
        valuesString = ""; // Clear values string to be refilled with same but correct sql format
        getline(valuesStream, runID, ',');
        // cout << runID << '\n';

        // Parse out the survery point information from the run_id and put it in the values array
        parseRunID(runID, platID, posNum);
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
        valuesStream = stringstream(); // clear the stream by resetting it to default
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
    return 0;
}
