import serial
from sys import stdout

ser = serial.Serial('/dev/tty.usbmodem641', 9600,timeout=5)
#ser = serial.Serial('COM9', 9600,timeout=5)
while True:
	read = ser.readline().rstrip()
	read_arr = read.decode("utf-8").split(':')
	if len(read_arr)==5:
		#print("supermean: "+read_arr[0]+"\t\t\t"+"mean: "+read_arr[1]+"\t\t\t"+"Taronja: "+read_arr[2]+"\t\t\t"+"Vermell: "+read_arr[3]+"\t\t\t"+"Release: "+read_arr[4]+"\r",end='')
		print("supermean: "+read_arr[0]+" "+"mean: "+read_arr[1]+" "+"Taronja: "+read_arr[2]+" "+"Vermell: "+read_arr[3]+" "+"Watch   : "+read_arr[4],end='')
		print("            \r",end='')

		#print("Nivell sonor: "+read_arr[0]+"\t\t\t"+"Taronja: "+read_arr[1]+"\t\t"+"Vermell: "+read_arr[2]+"\t\t"+"Watch: "+read_arr[3]+"\r")
		stdout.flush()
