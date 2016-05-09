from collections import namedtuple
import mysql.connector
import json

from mysql.connector import errorcode

USER = 'root'
PWD = 'Jqz19941127.'
HOST = '127.0.0.1'
DB = 'EMMA'

#Device = namedtuple('DeviceID')

def _get_connect():
    try:
        cnx = mysql.connector.connect(user=USER, password=PWD,host=HOST, database=DB)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("your username or password is wrong.")
        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("Database does not exist")
        else:

            print(err)
    finally:
        if cnx:
            _retrieve_data()
            cnx.close()




def _retrieve_data():
    # prepare a cursor object
    cnx = mysql.connector.connect(user=USER, password=PWD,
                                  host=HOST, database=DB)
    cursor = cnx.cursor()

    # # Prepare SQL query
    sql = ("SELECT * FROM Device")

    try:
        #    # Execute the SQL command
        cursor.execute(sql)
        #        # Fetch all the rows in a list of lists.
        results = cursor.fetchall()
        rowarray_list = []
        encoder = json.JSONEncoder
        for row in results:
            DeviceID = row[0]
            Name = row[1]
            WeeklyUsage = row[2]
            Watts = row[3]
            Status = row[4]
            data = {'Name': row[1], 'WeeklyUsage': row[2], 'Watts': row[3], 'Status': row[4]}
            record ={}
            record[DeviceID] = data
            print( record[DeviceID])
            rowarray_list.append(record)
        js = json.dumps(rowarray_list)
        JSFILE = 'jsontest2.js'
        FILE = open(JSFILE,'w')
        FILE.write(js)
        FILE.close()


    except:
        print("Error: unable to fecth data")





_get_connect()