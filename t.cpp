#include <iostream>

using namespace std;

int main() {
    int a[5] = { 1, 3, 4, 5, 6};

    for (int i = 0; 5 > i; ++i) {
        cout << "index " << i << " is " << a[i];
        cout << ", index " << i + 1 << " is " << a[i+1] << endl;
    }
}