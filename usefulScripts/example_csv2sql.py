# Read all CSV files and converts to an sql file
# Was not designed for current x-ray format
# Written by Benoit Lefebvre

import sqlite3, os, sys

if __name__=='__main__':
    usage = 'python3 csv2sql.py list-of-csv-files...'
    if len(sys.argv)<2:
        print(usage)
        exit(0)

        
    out_db_file = 'output.db'
    if os.path.exists(out_db_file):
        raise Exception('You must delete or move the sqlite3 file output.db before you proceed.')

    # Read data from files
    cols = 'RUN_ID,MODULE,GAS_VOLUME,X_BEAM,Y_BEAM,Y_MEAS,Y_MEAS_ERROR'.split(',')
    flist = sys.argv[1:]
    xray_data = []
    for f in flist:
        fin = open(f, 'r')
        fdata = [l[:-1].split(',') for l in fin]
        fin.close()
        fcols = fdata.pop(0)
        if set(fcols) != set(cols):
            raise Exception('Invalid columns in file:', f)
        for row in fdata:
            xray_data.append({fcols[idx]:val for idx, val in enumerate(row)})
    

    # Write the data to sqlite3
    conn = sqlite3.connect(out_db_file)
    c = conn.cursor()
    c.execute('CREATE TABLE RESULTS (RUN_ID TEXT, MODULE TEXT, GAS_VOLUME INTEGER, X_BEAM REAL, Y_BEAM REAL, Y_MEAS REAL, Y_MEAS_ERROR REAL);')
    
    for e in xray_data:
        cols_str = ','.join(e.keys())
        vals_str = ','.join([ "'%s'"%(v) if k in ['RUN_ID', 'MODULE'] else str(v) for k, v in e.items()])
        cmd = 'INSERT INTO RESULTS (%s) VALUES (%s);' % (cols_str, vals_str)
        c.execute(cmd)
        
    # Commit and close the DB instance
    conn.commit()
    conn.close()
