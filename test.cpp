#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ev3api/display.h"
#include "ev3api/debug.h"

using namespace ev3api;

class MyClass
{
public:
	int x;
};

int main(int argc, const char *argv[])
{
	MyClass inst = {};
	inst.x = 1;

	display.setFont("Uni2-VGA16");
    display.print("Hello World! %i\n", inst.x++);
    display.print("Hello World! %i\n", inst.x++);
    display.print("Hello World! %i\n", inst.x++);
    display.print("Hello World! %i\n", inst.x++);
    display.print("Hello World! %i\n", inst.x++);
    display.print("Hello World! %i\n", inst.x++);
    display.print("Hello World! %i\n", inst.x++);

	display.clear();
    display.print("Hello World! %i\n", inst.x++);

	debug.print("This is stderr new test %i\n", inst.x);

	//system("beep");
	//system("beep");
	sleep(2);
    return 0;
}
