#include <iostream>
#include <fstream>
#include <unistd.h>

int main(int argc, char* argv[])
{
	//direction range: 100 ~ 600, center: 340
	//speed range: 240 ~ 320, center: 280
	//open arduino device file (linux)
	std::ofstream arduino;

	char control;
	bool isEnd = false;

	int direction = 340;
	int speed = 280;
	bool isDash = false;


	arduino.open( "/dev/ttyACM0");

	while (!isEnd) {
		std::cout << "Speed: " << speed << '\t' << "Direction: " << direction << std::endl;
		
		std::cin >> control;
		

		if (control == 'a') direction -= 20;
		else if (control == 'd') direction += 20;
		else if (control == 'w') speed += 5;
		else if (control == 's') speed -= 5;
		else if (control == 'r') {
			arduino << "s " << 280;
			arduino.flush();
			arduino << "d " << 340;
			speed = 280; 
			direction = 340;}
		else if (control == 'e') isEnd = true;
		else if (control == 'p') {
			arduino << "s " << 320;
			arduino.close();
			arduino.open("/dev/ttyACM0");
			usleep(50);
			arduino << "s " << 280;
			speed = 280;
		}
		else if (control == 'l') {
			arduino << "s " << 235;
			arduino.close();
			arduino.open("/dev/ttyACM0");
			usleep(50);
			arduino << "s " << 280;
			speed = 280;
		}
		//write to it

		if (control == 'd' || control == 'a') arduino << "d " << direction;
		else if (control == 'w' || control == 's') arduino << "s " << speed;
		
		arduino.flush();
	}


	return 0;

}
