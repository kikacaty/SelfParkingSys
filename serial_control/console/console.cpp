#include "Serial.h"
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <device> <baudrate>\n", argv[0]);
        fprintf(stderr, "Example: %s /dev/ttyACM0 115200\n", argv[0]);
        exit(0);
    }

    Serial arduino = Serial(argv[1], atoi(argv[2]));

    string cmd = "";
    char output[512] = "";

    const int INIT_SPEED = 250;
    const int INIT_DIR = 240;

    int speed = INIT_SPEED;
    int direction = INIT_DIR;

    char control;

    bool isEnd = false;

    while (!isEnd) {
		std::cout << "============================================\nSpeed: " << speed << '\t' << "Direction: " << direction << "\n============================================\n";

		std::cin >> control;

		if (control == 'a') direction -= 20;
		else if (control == 'd') direction += 20;
		else if (control == 'w') speed += 5;
		else if (control == 's') speed -= 5;
		else if (control == 'r') {
			cmd = "s 250\n";
        		arduino.Write(cmd.c_str(), cmd.size());
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			cmd = "d 240\n";
        		arduino.Write(cmd.c_str(), cmd.size());
			speed = 205;
			speed = INIT_SPEED;
			direction = INIT_DIR;
			}
		else if (control == 'e') isEnd = true;
		else if (control == 'b') {
			cmd = "s 200\n";
        		arduino.Write(cmd.c_str(), cmd.size());
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			cmd = "s 205\n";
        		arduino.Write(cmd.c_str(), cmd.size());
			speed = 205;
		}
		else if (control == 'g') {
			cmd = "s 320\n";
        		arduino.Write(cmd.c_str(), cmd.size());
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			cmd = "s 315\n";
        		arduino.Write(cmd.c_str(), cmd.size());
			speed = 310;
		}
		//write to it

		if (control == 'd' || control == 'a') { 
			cmd = "d " + to_string(direction) + "\n";
        		arduino.Write(cmd.c_str(), cmd.size());
		}
		else if (control == 'w' || control == 's'){ 
			cmd = "s " + to_string(speed) + "\n";
        		arduino.Write(cmd.c_str(), cmd.size());
		}

		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

        	arduino.Read(output, sizeof(output));
        	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" << output << "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";

	}

    return 0;
}
