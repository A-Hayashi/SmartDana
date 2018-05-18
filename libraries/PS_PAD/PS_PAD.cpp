#include "PS_PAD.h"


PS_PAD::PS_PAD (byte ss_pin) {
	_ss_pin = ss_pin;
}
 
int PS_PAD::init () {
    const char enter_config_mode[5]  = {0x01, 0x43, 0x00, 0x01, 0x00};
    const char enable_analog_mode[9] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
    const char enable_vibration[9]   = {0x01, 0x4d, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff};
    const char exit_config_mode[9]   = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
    char buf[10];

	pinMode(_ss_pin, OUTPUT);
	digitalWrite(_ss_pin, HIGH);
	 
	/*
		Least Significant Bit First
		16MHz(Arduino NANO) / 64 = 250kHz
		Clock is High when inactive (CPOL=1)
		Data is Valid on Clock Trailing Edge (CPHA=1)
	*/
	_SPISettings = SPISettings(250000, LSBFIRST, SPI_MODE3);

	SPI.begin();
	
    _vib1 = 0;
    _vib2 = 0;
    _connected = false;
	
    send(enter_config_mode, 5, buf);
    if (buf[2] == 0xff) {
        return -1;
    }
    delay(16);
    send(enable_analog_mode, 9, buf);
    delay(16);
    send(enable_vibration, 9, buf);
    delay(16);
    send(exit_config_mode, 9, buf);
    delay(16);
    return 0;
}
 
int PS_PAD::poll () {
    const char poll_command[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int i;
    char cmd[10], buf[10];
	
    memcpy(cmd, poll_command, 9);
    cmd[3] = _vib1;
    cmd[4] = _vib2;
    send(cmd, 9, buf);
    if (buf[2] != 0x5a) {
        return -1;
    }
 
    for (i = 0; i < 6; i ++) {
        _pad[i] = buf[3 + i];
    }
    _connected = true;
    return 0;
}
 
int PS_PAD::read (TYPE t) {
    if (!_connected) {
        return 0;
    }
 
    switch (t) {
    case PAD_LEFT:
        return _pad[0] & 0x80 ? 0 : 1;
    case PAD_BOTTOM:
        return _pad[0] & 0x40 ? 0 : 1;
    case PAD_RIGHT:
        return _pad[0] & 0x20 ? 0 : 1;
    case PAD_TOP:
        return _pad[0] & 0x10 ? 0 : 1;
    case PAD_START:
        return _pad[0] & 0x08 ? 0 : 1;
    case ANALOG_LEFT:
        return _pad[0] & 0x04 ? 0 : 1;
    case ANALOG_RIGHT:
        return _pad[0] & 0x02 ? 0 : 1;
    case PAD_SELECT:
        return _pad[0] & 0x01 ? 0 : 1;
    case PAD_SQUARE:
        return _pad[1] & 0x80 ? 0 : 1;
    case PAD_X:
        return _pad[1] & 0x40 ? 0 : 1;
    case PAD_CIRCLE:
        return _pad[1] & 0x20 ? 0 : 1;
    case PAD_TRIANGLE:
        return _pad[1] & 0x10 ? 0 : 1;
    case PAD_R1:
        return _pad[1] & 0x08 ? 0 : 1;
    case PAD_L1:
        return _pad[1] & 0x04 ? 0 : 1;
    case PAD_R2:
        return _pad[1] & 0x02 ? 0 : 1;
    case PAD_L2:
        return _pad[1] & 0x01 ? 0 : 1;
    case BUTTONS:
        return ~((_pad[1] << 8) | _pad[0]) & 0xffff;
    case ANALOG_RX:
    	return _pad[2]-0x80;
    case ANALOG_RY:
    	return -(_pad[3]-0x7f);
    case ANALOG_LX:
        return _pad[4]-0x80;
    case ANALOG_LY:
        return -(_pad[5]-0x7f);
    }
    return 0;
}
 
int PS_PAD::vibration (int v1, int v2) {
    _vib1 = v1 ? 1 : 0;
    if (v2 < 0) v2 = 0;
    if (v2 > 0xff) v2 = 0;
    _vib2 = v2;
    poll();
    return 0;
}
 
int PS_PAD::send (const char *cmd, int len, char *dat) {
    int i;
	SPI.beginTransaction(_SPISettings);
	digitalWrite(_ss_pin, LOW);
    delayMicroseconds(10);
    for (i = 0; i < len; i ++) {
    	dat[i] =  SPI.transfer(cmd[i]);
        delayMicroseconds(10);
    }
    digitalWrite(_ss_pin, HIGH);
	SPI.endTransaction();
    return i;
}
 
 
    