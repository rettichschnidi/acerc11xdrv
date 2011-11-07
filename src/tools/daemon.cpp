#include "ProjectorC110.h"
#include "USB.h"
#include "Exception.h"

#include <iostream>
#include <string>
#include <signal.h>

using namespace std;
using namespace acerc11xdrv;
using namespace DANGER_ZONE;

bool stop = false;

void stopit(int signum) {
	cout << "Got signal " << signum << ", shutdown..." << endl;
	stop = true;
}

int main() {
	signal(SIGINT, stopit);
	try {
		ProjectorC110 acer;
		acer.setBrightness(BrightnessWidgetUSBData::HIGH);
		int i = 0;
		while(!stop) {
			i++;
			acer.run();
			if(i % 25 == 0) {
				cout << "Duration per frame: " << acer.getDurationPerFrame() << "\n";
			}
		}
	} catch(string &e) {
		cout << "Error: " << e << endl;
	} catch(USBError &e) {
		cout << "USB Error: " << e.what() << endl;
	}

	cout << flush;
	return 0;
}
