#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>    
using namespace std;

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