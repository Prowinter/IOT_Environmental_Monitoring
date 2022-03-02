from flask import Flask,request
import time
import pymysql 
import json


class WebHookMysql():
	def __init__(self):
		self.conn = pymysql.connect(host = '127.0.0.1',user ='monitor',passwd='monitor',port= 3306,db='monitor',charset='utf8')
		self.cursor=self.conn.cursor()
	def AddItem(self,sql):
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


app = Flask(__name__)
webhookmysql = WebHookMysql()

@app.route('/mqtt/webhook',methods=['POST'])
def webhook():
	obj = request.get_json(force=True)
	if 'ts' in obj and 'payload' in obj:
		dt = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(obj['ts']/1000))
		dt_json = json.loads(obj['payload'])
		SoilTemp = str(dt_json['soil_temp_int']) + '.' + str(dt_json['soil_temp_deci'])
		SoilHum = str(dt_json['soil_hum_int']) + '.' + str(dt_json['soil_hum_deci'])		
		AirTemp = str(dt_json['air_temp_int']) + '.' + str(dt_json['air_temp_deci'])
		AirHum = str(dt_json['air_hum_int']) + '.' + str(dt_json['air_hum_deci'])
		illu = str(dt_json['illu'])
		worm = str(dt_json['worm'])
		ph = str(dt_json['ph_int']) + '.' + str(dt_json['ph_deci'])
		sql = "INSERT INTO `monitor`.`monitor` (`Data`, `SoilTemp`, `SoilHum`, `AirTemp`, `AirHum`, `illu`, `worm`, `ph`, `ID`) VALUES ('{}',' {}', '{}', '{}', '{}','{}', '{}', '{}', NULL);".format(dt,SoilTemp,SoilHum,AirTemp,AirHum,illu,worm,ph)
		webhookmysql.AddItem(sql)

	return "200"

if __name__ == '__main__':
	app.run(host='0.0.0.0', port=5000)
