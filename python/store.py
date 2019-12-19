import paho.mqtt.client as mqtt
import mysql.connector
import time

mysql_url = "localhost"
mysql_user = "root"
mysql_password = "TomBrecht"
mysql_database = "netrf"

connection = mysql.connector.connect(host=mysql_url, user=mysql_user, passwd=mysql_password, database=mysql_database)
cursor = connection.cursor()

def on_message(client, userdata, message):
	payload = str(message.payload.decode("utf-8"))
	print(payload)
	topic = str(message.topic).split("/")
	print(topic)
	sql_command = ""
	dataTuple = None
	print("Message: ", message)
	measurement = ""
	if(topic[1] == "data"):
		thing_id = topic[2]
		sensor_id = 0
		if(len(topic) == 4):
			sensor_id = topic[3]
		value = payload;
		millis = int(round(time.time() * 1000))
		sql_command = """INSERT INTO data(thing_id, sensor_id, value, time) VALUES(%s, %s, %s, %s)"""
		dataTuple = (thing_id, sensor_id, value, millis)
	elif(topic[1] == "setup"):
		print("iot/setup/" + payload)
		client.publish("test", payload="testing", qos=0, retain=False);
		print("Not data topic")
	print(sql_command)
	print(dataTuple)
	cursor.execute(sql_command, dataTuple)
	connection.commit()

broker_url = "localhost"
broker_port = 1883
client = mqtt.Client()
client.connect(broker_url, broker_port)
client.on_message = on_message
client.subscribe([("iot/setup", 0), ("iot/data/#", 0)])
print("Client setup")
client.loop_forever()


