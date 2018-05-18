import time
import smbus

def patlite(lamp, speed):
	bus.write_byte(0x08, 0x0A)
	bus.write_byte(0x08, 0x55)
	bus.write_byte(0x08, 0x01)		#dtype
	bus.write_byte(0x08, 0x03)		#len
	bus.write_byte(0x08, 0x03)		#ctype
	bus.write_byte(0x08, lamp)
	bus.write_byte(0x08, speed)

def servo(number, angle, speed):
	bus.write_byte(0x08, 0x0A)
	bus.write_byte(0x08, 0x55)
	bus.write_byte(0x08, 0x01)		#dtype
	bus.write_byte(0x08, 0x04)		#len
	bus.write_byte(0x08, 0x02)		#ctype
	bus.write_byte(0x08, number)
	bus.write_byte(0x08, angle)
	bus.write_byte(0x08, speed)
	

def s8(value):
	return -(value & 0b10000000) | (value & 0b01111111)

bus = smbus.SMBus(1)

while True:
	try:
		pspad  = bus.read_i2c_block_data(0x08, 0x30, 6)
		switch  = bus.read_i2c_block_data(0x08, 0x32, 1)
		rfid  = bus.read_i2c_block_data(0x08, 0x31, 1)
		
		print hex(pspad[0]),hex(pspad[1]),s8(pspad[2]),s8(pspad[3]),s8(pspad[4]),s8(pspad[5]), switch[0], rfid[0];
		
		if rfid[0]==1:
			patlite(pspad[0]>>7, abs(s8(pspad[3])))
		else:
			patlite(1, 45)
			
			
			
		if switch[0]==0:
			servo(0, abs(s8(pspad[4])), 30)
		else:
			servo(0, 180, 30)
		
		time.sleep(0.1)
		
		'''
		
		patlite(0,0)
		
		time.sleep(5)
		
		patlite(1,0)
		
		time.sleep(5)
		
		patlite(0,30)
		servo(0,0,30)
		
		time.sleep(5)
		
		patlite(0,60)
		
		time.sleep(5)
		
		patlite(1,90)
		servo(0,180,30)
		
		time.sleep(5)
		'''	
	except IOError:
		print("NG")