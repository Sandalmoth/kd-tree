#include <iostream>
#include <vector>
#include <random>
#include <cmath>


#include "../kd-tree.h"


using namespace std;

double rnd1(double x) {
  return floor(x * 10 + 0.5) / 10;
}

int main() {
  KDTree<double, 2> kdt;
  typedef decltype(kdt)::value_type kdpos;

  random_device rd;
  mt19937 rng(rd());
  auto d = uniform_real_distribution<double>(-1.0, 1.0);

  cout << "\nCONSTRUCTING" << std::endl;
  vector<kdpos> v;
  for (int i = 1; i <= 5; ++i) {
    v.push_back(kdpos{rnd1(i*d(rng)), rnd1(i*d(rng))});
  }
  kdt = KDTree<double, 2>(v.begin(), v.end());
  cout << kdt << endl;

  cout << "\nINSERTING" << std::endl;
  for (int i = 1; i <= 5; ++i) {
    kdt.insert(kdpos{rnd1(i*d(rng)), rnd1(i*d(rng))});
    cout << kdt << endl;
  }

  cout << "\nERASING" << std::endl;
  for (auto &p: v) {
    cout << "erasing: "; for(auto x: p) cout << x << ' '; cout << endl;
    kdt.erase(p);
    cout << kdt << endl;
  }
  cout << "PROGRAM END" << endl;
}
