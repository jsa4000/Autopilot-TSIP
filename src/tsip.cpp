#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include "tsip.h"

using namespace std;

void tsip::print() {
     cout << "Print Function" <<  endl; 
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

