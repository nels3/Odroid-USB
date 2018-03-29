#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

int set_interface_attribs(int fd, int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;         /* 8-bit characters */
	tty.c_cflag &= ~PARENB;     /* no parity bit */
	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */
								/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void set_mincount(int fd, int mcount)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		printf("Error tcgetattr: %s\n", strerror(errno));
		return;
	}

	tty.c_cc[VMIN] = mcount ? 1 : 0;
	tty.c_cc[VTIME] = 5;        /* half second timer */

	if (tcsetattr(fd, TCSANOW, &tty) < 0)
		printf("Error tcsetattr: %s\n", strerror(errno));
}


int main(){
	char* portname = (char*)"/dev/ttyACM1";
	int usb;
	int wlen;
	usb = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (usb < 0) {
		printf("Error opening\n");
		return -1;
	}
	else
		printf("I have opened\n");

	set_interface_attribs(usb, B115200);
	char keyboard[15];
	unsigned char buf[80];

	int usblen;

	while(true){
		//PISANIE
		if (scanf("%s", &keyboard[0])==1)
			printf("Wysylam %s\n", keyboard);
	
		wlen = write(usb, keyboard, 1);
		if (wlen != 1) {
			printf("Error from write: %d, %d\n", wlen, errno);
		}
		else
			printf("I wrote\n");
		tcdrain(usb);    
	
		//ODBIERANIE
		usblen = read(usb, buf, sizeof(buf) - 1);
		if (usblen > 0) {
		//#ifdef DISPLAY_STRING
			buf[usblen] = 0;
			printf("Read %d:\"%s\"\n", usblen, buf);
		//#else 
		//	unsigned char   *p;
		//	printf("Read %d:", rdlen);
		//	for (p = buf; rdlen-- > 0; p++)
		//		printf(" 0x%x", *p);
		//	printf("\n");
		//#endif
		}
		else if (usblen < 0) {
			printf("Error from read: %d: %s\n", usblen, strerror(errno));
		}
	}

}

