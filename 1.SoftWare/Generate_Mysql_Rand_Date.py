import time,datetime,random,pymysql

class Mysql_Class():
    def __init__(self):
        self.conn = pymysql.connect(host='175.178.128.27',user ='monitor',passwd='monitor',port= 3306,db='monitor',charset='utf8')
        self.cursor=self.conn.cursor()
    def GenerateRandomData(self,Data):
        sql = "INSERT INTO `monitor`.`monitor` (`Data`, `SoilTemp`, `SoilHum`, `AirTemp`, `AirHum`, `illu`, `worm`, `ph`, `ID`) VALUES ('{}',' {:.2f}', '{:.2f}', '{:.2f}', '{:.2f}','{}', '{}', '{:.2f}', NULL);".format(Data, random.uniform(12.0,25.0),random.uniform(30.0,40.0), random.uniform(12.0,25.0), random.uniform(30.0,80.0), random.randint(10,70),random.randint(0,1), random.uniform(6.5,8.0))
        try:
            # 执行sql语句
            self.cursor.execute(sql)
            # 提交到数据库执行
            self.conn.commit()
        except:
            # 发生错误时回滚
            self.cursor.rollback()
    def __del__(self):
        self.conn.close()


def Generate_Date(start,end,interval_min=5,frmt="%Y-%m-%d %H:%M:%S"):
    time_time = []
    start_time = datetime.datetime.strptime(start,frmt)
    end_time = datetime.datetime.strptime(end,frmt)
    time_time.append(start_time)
    i = 0
    while(time_time[-1] < end_time):    
        time_time.append(start_time+datetime.timedelta(minutes=interval_min*i))
        i+=1
    return time_time

#generate_time = Generate_Date("2022-2-1 0:0:0","2022-3-1 0:0:0")
generate_time = Generate_Date("2022-4-15 0:0:0","2022-5-16 0:5:0")
mysql_mysql = Mysql_Class()
for g_time in generate_time:
    mysql_mysql.GenerateRandomData(g_time)
    
