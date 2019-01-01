#include <vector>
#include <iostream>

using std::vector;
using std::cin;
using std::cout;
using std::endl;

long long calculatesum(const vector<double>& vec);

int main() {
    vector <double> stuff;
    int n;
    printf("Enter vector size:\n");
    cin >> n;
    stuff.resize(n);
    printf("Enter vector values\n");
    for (auto& x : stuff)
        cin >> x;
    cout << calculatesum(stuff) << endl;
    return 0;	
}
