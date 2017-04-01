#include <utilstring.h>
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <HttpCommon.h>

using namespace std;
using namespace fas;
using namespace fas::utils;

int main() {

   vector<string> ret;

   StringSplitBychar("GET /home/fas/  HTTP1.1", ' ', ret, 0);

   assert(ret.size() == 3);

   cout << ret.at(0) << endl;
   cout << ret.at(1) << endl;
   cout << ret.at(2) << endl;



   cout << StringTrim("   $$$  shangxiaofei>>>.   ") << endl;
   cout << StringTrimLeft("   $$$  shangxiaofei>>>.   ") << endl;
   cout << StringTrimRight("   $$$  shangxiaofei>>>.   ") << endl;

   cout << StringRemoveRepeatChar("/////shang////cccccdssss", "/") << endl;

   cout << StringGetSuffix("teststst.ccc") << endl;
   cout << StringGetSuffix("suusuus") << endl;

   fas::http::ContentTypes contype;
   cout << contype.getType("") << endl;
   cout << contype.getType("teststst.ccc") << endl;
}
