#include <Timestamp.h>
#include <iostream>

using namespace fas;
using namespace std;

int main() {
    Timestamp stamp;
    stamp = Timestamp::now();
    cout << stamp.toString();
    return 0;
}
