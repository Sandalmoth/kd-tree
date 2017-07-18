#include <iostream>
#include <vector>

#include "../kd-tree.h"


using namespace std;


int main() {
  KDTree<double, 2> kdt;
  typedef decltype(kdt)::value_type kdpos;
  vector<kdpos> v;
  v.push_back(kdpos{0.1, 3.4});
  v.push_back(kdpos{0.9, -3.4});
  v.push_back(kdpos{0.1, 89.0});
  v.push_back(kdpos{0.1, 89.0});
  v.push_back(kdpos{0.2, 0.0});
  kdt = KDTree<double, 2>(v.begin(), v.end());
  cout << kdt << endl;
  kdt.insert(kdpos{0.3, 1.0});
  kdt.insert(kdpos{0.3, 2.0});
  kdt.insert(kdpos{0.3, 3.0});
  kdt.insert(kdpos{0.3, 4.0});
  kdt.insert(kdpos{0.3, 5.0});
  cout << kdt << endl;
  cout << "PROGRAM END" << endl;
}
