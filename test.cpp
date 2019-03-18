#include "LeakDetector.h"
#include<iostream>
int main() {
	int *a = new int;
	int *b = new int[12];
	delete a;

	return 0;

}