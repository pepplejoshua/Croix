#include <iostream>

using namespace std;

struct T {
    int x;
    int y;
};

T* test(T *t) {
    t->x = 300;
    t->y = 200;
    return t;
}

int main() {
    T* res = test(new T());
    cout << res->y << " " << res->x << endl;

    return 0;
}