#include <vector>
#include <iostream>
#include <cstdio>
#include <dlfcn.h>

using std::vector;
using std::cin;
using std::cout;
using std::endl;

int main() {
    vector <double> stuff;
    int n;
    printf("Enter vector size:\n");
    cin >> n;
    stuff.resize(n);
    printf("Enter vector values\n");
    for (auto& x : stuff)
        cin >> x;
    void* handle = dlopen("./libcalculator_shared.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }
    long long (*calculatesum)(const vector<double>& vec);
    void* addr = dlsym(handle, "calculatesum");
    if (addr == NULL) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;        
    }
    *(void**)(&calculatesum) = addr;
    cout << calculatesum(stuff) << endl;
    return 0;	
}
