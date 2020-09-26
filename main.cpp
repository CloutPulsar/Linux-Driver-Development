#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>    
using namespace std;
// A simple user space program that reads from the MPU6050 device registered on the Raspberry Pi 3
int main(void){
    string result;
    char buf[1];
    int fd = open("/dev/mpu605000", O_RDWR);
    if(fd < 0)
        cout  << "Error opening device file\n";
        return -1;
    if(read(fd, buf, sizeof(uint8_t)) < 0)
        cout << "Error reading data from device file... \n";
        return -1;
    result = buf;
    cout << result << "\n";


}
