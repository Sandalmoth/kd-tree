#include <iostream>
#include <vector>

#include "../kd-tree.h"


using namespace std;


int main() {
  KDTree<double, 2, char> kdt;
  vector<typename decltype(kdt)::element_type> v;
  v.push_back(make_pair(decltype(kdt)::position_type{0.1, 3.4}, 'a'));
  v.push_back(make_pair(decltype(kdt)::position_type{0.9, -3.4}, 'b'));
  v.push_back(make_pair(decltype(kdt)::position_type{0.1, 89.0}, 'd'));
  v.push_back(make_pair(decltype(kdt)::position_type{0.1, 89.0}, 'c'));
  v.push_back(make_pair(decltype(kdt)::position_type{0.2, 0.0}, 'e'));
  kdt = KDTree<double, 2, char>(v.begin(), v.end());
  cout << kdt << endl;
  cout << kdt.nearest_neighbour(decltype(kdt)::position_type{0.0, 0.0}) << endl;
  cout << "PROGRAM END" << endl;
}
