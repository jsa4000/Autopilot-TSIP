#include <iostream>
#include <thread>
#include "tsip.h"

#include <boost/lambda/lambda.hpp>
#include <iterator>
#include <algorithm>

using namespace std;

void tsip::test_thread()
{
    thread t([](){
        for(int i=0;i<3000;i++){
             cout << "thread function\n";
        }
      });
    cout << "main thread\n";
    t.join();

}

void tsip::test_boost() {
    using namespace boost::lambda;
    typedef istream_iterator<int> in;

    cout << "Type in any number: ";

    std::for_each(
        in(cin), in(),cout 
                << (_1 * 10) 
                << "\nType in another number: " );

}

