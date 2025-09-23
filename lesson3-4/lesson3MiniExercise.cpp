#include <iostream>
using namespace std;

int exerciseA()
{
    // For each identifier below, mark where it is declared, instantiated, assigned,
    // and released/deleted. Note stack vs heap.
    int n; // 1)
 //实例化   //声明
    if (true)
    {
        int n = 3;// 2) (shadows outer n)
        //int用于实例化，n命名变量即为声明，3是为变量赋值
        int *p = new int(n); // 3) pointer + heap object int *p指针指向一个整形地址，从堆中调用栈（= new int(n);
        *p += 1;             // 4)现在栈p 经过*p指向n（为3），通过计算+=1，n被刷新为4
        cout << *p << "\n";//输出刷新过后的n，即为4
        delete p; // 5)释放堆，释放内存

    } // 6)p和内部n在此被销毁
    cout << n << "\n"; // 7)输出全局变量n，由于本身没有赋值因此直接调用物理内存地址
    return 0;
}

int main()
{
    exerciseA();
    return 0;
}
