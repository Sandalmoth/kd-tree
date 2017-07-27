#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cassert>


#define __ENABLE_KDTREE_DEBUG__
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
  cout << kdt.size() << '\t' << kdt << endl;

  cout << "\nINSERTING" << std::endl;
  for (int i = 1; i <= 5; ++i) {
    kdt.insert(kdpos{rnd1(i*d(rng)), rnd1(i*d(rng))});
    cout << kdt.size() << '\t' << kdt << endl;
  }

  cout << "\nERASING" << std::endl;
  for (auto &p: v) {
    cout << "erasing: "; for(auto x: p) cout << x << ' '; cout << endl;
    kdt.erase(p);
    cout << kdt.size() << '\t' << kdt << endl;
  }

  cout << "\nUNBALANCED INSERTION" << std::endl;
  vector<kdpos> v2;
  for (int i = 1; i <= 10; ++i) {
    v2.push_back(kdpos{i * 1.0, i * 1.0});
  }
  for (auto p: v2) {
    kdt.insert(p);
    cout << kdt.size() << '\t' << kdt << endl;
  }

  cout << "\nUNBALANCED ERASURE" << std::endl;
  for (auto &p: v2) {
    cout << "erasing: "; for(auto x: p) cout << x << ' '; cout << endl;
    kdt.erase(p);
    cout << kdt.size() << '\t' << kdt << endl;
  }


  cout << "\nNEAREST NEIGHBOUR" << std::endl;
  for (int i = 1; i <= 5; ++i) {
    auto kp = kdpos{rnd1(i*d(rng)), rnd1(i*d(rng))};
    auto p = kdt.nearest(kp);
    cout << " # ";
    for (auto x: kp) cout << x << ' ';
    cout << "is closest to ";
    for (auto x: p) cout << x << ' ';
    cout << endl;
    auto tp = kdt.print_distances(kp);
    assert(p == tp);
    cout << " ---\n";
  }


  cout << "\nPROGRAM END" << endl;
}
