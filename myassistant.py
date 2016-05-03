import mysql.connector

from mysql.connector import errorcode



#open database connection
try:
                cnx = mysql.connect(user='jessica',password='pwd'host='127.0.0.1',database = 'RIVA')
except:
                if err.errno = connector.Error as err:
                                print("your input is wrong.")
                elif err.errno = errorcode.ER_BAD_DB_ERROR:
                                 print("Database does not exist")
                else:
                                print(err)
                                
else:
                cnx = close()


#prepare a cursor object

cursor = mysql.cursor()

# Prepare SQL query 
sql = "SELECT * FROM DEV_List"

#write data to a file
with open(fname, 'r') as FILE:
     
try:
   # Execute the SQL command
   cursor.execute(sql)
   # Fetch all the rows in a list of lists.
   results = cursor.fetchall()
   for row in results:
      Device_Name = row[0]
      Dev_Table_Name_Raw = row[1]
      Daily_Last_Update = row[2]
      Dev_Table_Name_Hourly = row[3]
      Hourly_Last_Update = row[4]
      Dev_Table_Name_State = row[5]
      State_Last_Update = row[6]

      # Now print fetched result
      FILE.write("Device_Name=%s,Dev_Table_Name_Raw=%s,Daily_Last_Update=%s,Dev_Table_Name_Hourly=%s,Dev_Table_Name_State=%s,State_Last_Update=%s\n" % 
             Device_Name,Dev_Table_Name_Raw,Daily_Last_Update,Dev_Table_Name_Hourly,Dev_Table_Name_State,State_Last_Update)
except:
   print "Error: unable to fecth data"
else:
                 cursor.close ()

