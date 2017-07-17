#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
	//open arduino device file (linux)
        std::ofstream arduino;
	arduino.open( "/dev/ttyACM0");

	//write to it
        arduino << "250";
	arduino.close();

	return 0;
}
