#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <chrono>
#include <thread>


const int INIT_DIR=240;
const int INIT_SPEED=250;


int main(int argc, char* argv[])
{
	//direction range: 100 ~ 600, center: 340
	//speed range: 200 ~ 320, center: 280
	//open arduino device file (linux)
	FILE *arduino;

	char control;
	bool isEnd = false;

	int direction = INIT_DIR;
	int speed = INIT_SPEED;
	bool isDash = false;

	arduino = fopen( "/dev/ttyACM0", "w");

	while (!isEnd) {
		std::cout << "Speed: " << speed << '\t' << "Direction: " << direction << std::endl;

		std::cin >> control;

		if (control == 'a') direction -= 20;
		else if (control == 'd') direction += 20;
		else if (control == 'w') speed += 5;
		else if (control == 's') speed -= 5;
		else if (control == 'r') {
			fprintf(arduino, "s %d\n", INIT_SPEED);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			fprintf(arduino, "d %d\n", INIT_DIR);
			speed = INIT_SPEED;
			direction = INIT_DIR;
			}
		else if (control == 'e') isEnd = true;
		else if (control == 'b') {
			fprintf(arduino, "s 200\n");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			fprintf(arduino, "s 205\n");
			speed = 205;
		}
		//write to it

		if (control == 'd' || control == 'a') fprintf(arduino, "d %d\n", direction);
		else if (control == 'w' || control == 's') fprintf(arduino, "s %d\n",speed);
	

	}

	fclose(arduino);

	return 0;

}
