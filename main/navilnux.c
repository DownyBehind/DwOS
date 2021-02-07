#include <pxa255.h>
#include <time.h>
#include <gpio.h>
#include <stdio.h>
#include <string.h>

int main(void){
	while(1){
		printf("hello world!\n");
		msleep(1000);
		printf("this is DwOS!\n");
		msleep(1000);
	}
	return 0;
}