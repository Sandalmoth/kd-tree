#include <iostream>
#include <random>
#include <vector>

#include "../scapegoat-tree.h"


using namespace std;


int main() {
  ScapegoatTree<int> st;

  random_device rd;
  mt19937 rng(rd());
  auto d = uniform_int_distribution<int>(0, 9);

  cout << "\nCONSTRUCTING" << std::endl;
  vector<int> v;
  for (int i = 0; i < 10; ++i) {
    v.push_back(d(rng));
  }
  st = ScapegoatTree<int>(v.begin(), v.end());
  cout << "constructed" << endl;
  cout << st.size() << '\t' << st << endl;
}
