
> 转载请注明出处,文章内容如有侵权请邮件告知删除。

<br/>

> [EMQx](https://www.emqx.io/) An Open-Source, Cloud-Native, Distributed MQTT Broker for IoT

>[ESP8266 Web](https://github.com/tzapu/WiFiManager) Espressif ESPx WiFi Connection manager with fallback web configuration portal

<br/>

## EMQx开源版

由于EMQx限制开源版使用插件[emqx_auth_mysql](https://www.emqx.io/docs/zh/v4.4/advanced/auth-mysql.html)可以使用[emqx_web_hook](https://www.emqx.io/docs/zh/v4.2/advanced/webhook.html)(网络钩子)配合flask将接收到的数据解析过滤后保存到Mysql数据库中

<br/>


## emqx_web_hook配置

参考官方提供的文档，emqx_web_hook配置文件位于`etc/plugins/emqx_web_hook.conf`,Docker用户在容器内部对应位置。

api.url -> 事件需要转发的目的服务器地址

message.publish -> 服务端在发布（路由）消息前

------
## Flask

提供路由功能，可以利用路由功能监听WebHook服务的Post请求。

Python Mysql库：pymysql

`pip install pymysql`

flask简单例子:
```flask
from flask import Flask
app = Flask(__name__)

@app.route("/")
def hello():
    return "Hello World!"

if __name__ == "__main__":
    app.run()
```
<br/>

## Screen Tmux
由于Linux终端结束的同时会将运行的程序一同关闭，由此无法保证关闭终端后进行进行数据持久化。就需要使用终端复用器在后台运行相关的程序。Screen 和 Tmux 可以提供对应的功能。具体使用方式查询对应命令文档即可。

<br/>

## 数据处理
每天的数据可以通过Mysql内置的平均函数求平均值AVG()，每天的数据可以通过时间进行分组，通过Mysql上提前对数据进行预处理可以大大降低后续在QT上位机上的开发难度。

<br/>

```Mysql
筛选三十天数据：
WHERE DATE_SUB(CURDATE(), INTERVAL 30 DAY) <= date(#时间字段)
时间分组：
GROUP BY DATE_FORMAT(#时间字段,'%Y-%m-%d')
```

<br/>

## 数据虚构
仓库内同时提供前期数据量不足时的数据生成工具`Generate_Mysql_Rand_Date.py`。生成工具由python编写。

<br/>

## Mysql-DATE_FORMAT常用转换
|  格式   | 描述  |
|  ----  | ----  |
|%Y|年，4 位|
|%y|年，2 位|
|%M|月名|
|%m|月，数值(00-12)|
|%d|月的天，数值(00-31)|
|%e|月的天，数值(0-31)|
|%r|时间，12-小时（hh:mm:ss AM 或 PM）|
|%H|小时 (00-23)|
|%h|小时 (01-12)|
|%i|分钟，数值(00-59)|
|%S|秒(00-59)|
|%s|秒(00-59)|

<br/>

## QT X轴转时间轴

```
ui->myCustomPlot->graph(0)->rescaleAxes();//自动缩放
ui->myCustomPlot->graph(0)->rescaleKeyAxis();//自动缩放键的范围
ui->myCustomPlot->graph(0)->rescaleValueAxis();//自动缩放值的范围
ui->myCustomPlot->replot(); //重绘数据
```
<br/>
## 时间轴作X轴

> X轴的数据以1970-01-01 00:00:00至当前时间的总秒数，若某个点的X值=5，则该点在图中的X坐标会显示为1970-01-01 00:00:05。最后需要缩放下X轴的显示。

```
QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);//日期做X轴
dateTicker->setDateTimeFormat("yyyy-MM-dd hh:mm:ss");//日期格式
dateTicker->setTickCount(5);//X轴标签个数
```
<div align=center><img src="3.Doc/images/Windows.png" width="70%" height="70%"/></div>

<br/>

## 拓展

>[IoTManager](https://iotmanager.ru/)

IoTManager支持Widget的方式构建，可以通过MQTT的方式传输指定的协议，构建自定义的程序页面。详情信息点击上文链接。APK程序存储在仓库DOC目录

<div align=center><img src="3.Doc/images/IoTmanager.PNG" width="50%" height="50%"/></div>





