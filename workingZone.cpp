#include <iostream>
using namespace std;

int main() {
    int n;                    // 1)
    if (true) {
        int n = 3;            // 2) 内部n声明，实例化，赋值
        int* p = new int(n);  // 3) 堆指针p声明，实例化，并指向n进行赋值
        *p += 1;              // 4)
        cout << *p << "\n";
        delete p;             // 5)p被销毁
    }                         // 6)内部n被销毁
    cout << n << "\n";        // 7)
}                             // 8)外部n被销毁  
