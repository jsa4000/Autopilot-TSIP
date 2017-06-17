#include <iostream>
#include "tsip.h"
using namespace std;

int main(int argc, const char * argv[]) {

    cout << "Hello World" << endl;

    // Use the function loaded from the tsip header.
    tsip::test_thread();
    tsip::test_boost();

    return 0;
}


