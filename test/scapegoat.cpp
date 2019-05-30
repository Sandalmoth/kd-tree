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
  for (int i = 0; i < 5; ++i) {
    v.push_back(d(rng));
  }
  st = ScapegoatTree<int>(v.begin(), v.end());
  cout << st.size() << '\t' << st << endl;

  cout << "\nINSERTING" << std::endl;
  for (int i = 1; i <= 10; ++i) {
    int x = d(rng);
    cout << "inserting: " << x << endl;
    st.insert(x);
    cout << st.size() << '\t' << st << endl;
  }

  cout << "\nERASING" << std::endl;
  for (auto &x: v) {
    cout << "erasing: " << x << endl;
    st.erase(x);
    cout << st.size() << '\t' << st << endl;
  }

}
