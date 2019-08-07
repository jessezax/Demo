#include "/usr/include/c++/4.8.2/ext/hash_map"
#include <iostream>
#include <string>
using namespace std;
using namespace __gnu_cxx;

int main(){
  hash_map<string,string> map;
  map["2"]="2";
  cout<<map["2"];
}


