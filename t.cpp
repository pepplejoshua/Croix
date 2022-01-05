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

void scope(int a) {
    int a = 5;
    cout << a << endl;
}

int main() {
    // T* res = test(new T());
    // cout << res->y << " " << res->x << endl;

    scope(200);
    return 0;
}