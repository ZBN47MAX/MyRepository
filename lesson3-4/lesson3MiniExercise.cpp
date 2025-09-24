///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A OG code
#include <iostream>
using namespace std;

int main()
{
    int n; // 1)
    if (true)
    {
        int n = 3;           // 2) (shadows outer n)
        int *p = new int(n); // 3) pointer + heap object
        *p += 1;             // 4)
        cout << *p << "\n";
        delete p; // 5)
    } // 6)
    cout << n << "\n"; // 7)
}
// A1 For each identifier below, mark where it is declared, instantiated, assigned, and released/deleted. Note stack vs heap.
#include <iostream>
using namespace std;

int main()
{
    int n; // 1)外部n声明，实例化但未赋值
    if (true)
    {
        int n = 3;           // 2) 内部n声明，实例化在栈，赋值
        int *p = new int(n); // 3) 指针p声明，实例化在栈，并指向堆上New的n
        *p += 1;             // 4)堆上的int值从3变为4
        cout << *p << "\n";
        delete p; // 5)释放内存，p指针保留在栈上
    } // 6)内部n被销毁
    cout << n << "\n"; // 7)
} // 8)外部n被销毁

// A2 Explain what is released at (5) vs (6). What happens at (7) if outer n was never assigned?
// 5释放了内存但保留了栈上的指针p，6销毁了内部n和指针p。如果为给外部n赋值，则在7时会输出垃圾数据

// A3 Rewrite so outer n is safely initialised and no shadowing occurs.
#include <iostream>
using namespace std;

int main()
{
    int outerN;
    if (true)
    {
        int innerN = 3;
        int *p = new int(innerN);
        *p += 1;
        cout << *p << "\n";
        delete p;
    }
    cout << outerN << "\n";
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// B OG Code
int *makeVal()
{
    int x = 42;
    return &x; // ❌ returns address of a dead stack variable
}

int &pick(bool b)
{
    int a = 1, c = 2;
    return b ? a : c; // ❌ returns reference to dead locals
}

// B1 Identify the lifecycle error(s). Refactor to a safe design (return by value, or use RAII like std::unique_ptr).
// errors
// 第一块中return &x；返回了一个即将被销毁的地址，在该代码块结束后，指针指向的地址已经被销毁
// 第二块return b？a：c；中，返回了一个即将被销毁的引用，在该代码块结束后，引用的A或C已经被销毁了
// Refactor见下题B2的回答

// B2 Provide two fixes for makeVal (value-return and RAII pointer).
// based on Value-return
int makeVal()
{ // 去掉*使返回的从指针变为值
    int x = 42;
    return x; // 去掉&使返回的从指针变为值
}
// based on RAII
#include <memory>
std::unique_ptr<int> makeVal() // 返回unique_ptr
{
    auto ptr = std::make_unique<int>(42); // 在堆上分配
    return ptr;                           // 返回智能指针，自动管理生命周期
}

// B3 Rewrite pick so it is safe and communicates ownership clearly.
// based on return-value
int pick(bool b)
{ // 去掉&使返回的从引用变为值
    int a = 1, c = 2;

    return b ? a : c;
}
// based on RAII
std::unique_ptr<int> pick(bool b)
{
    if (b)
    {
        return std::make_unique<int>(1); // 堆上分配
    }
    else
    {
        return std::make_unique<int>(2); // 堆上分配
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C OG Code
int main()
{
    int *p = new int{5};
    delete p;
    std::cout << *p << "\n"; // ❌ use-after-free

    int *q = new int[3]{1, 2, 3};
    delete q; // ❌ wrong deallocator (should be delete[])
    // ...
}
// C1 Find the bugs, then rewrite using RAII containers.
// 首先堆上的p在被手动销毁后被尝试调用，造成空指针并使*p变为未定义行为。其次是q在被销毁时使用的关键词错误，应该使用适配数组的deleter[]q

// C2 Correct the manual version (delete[] q;, nulling pointers, etc.).
int main()
{
    int *p = new int{5};
    std::cout << *p << "\n";
    delete p;
    p = nullptr;

    int *q = new int[3]{1, 2, 3};
    delete[] q;
    q = nullptr;
}

// C3 Refactor to std::unique_ptr<int[]> or std::vector<int> so release is implicit.
#include <iostream>
#include <memory>
#include <vector>

int main()
{
    // 使用 unique_ptr 管理单个int
    auto p = make_unique<int>(5);
    cout << *p << "\n"; // 使用方式与原始指针类似

    vector<int> q = {1, 2, 3}; // 直接初始化

    // 更安全的访问方式
    cout << q[0] << " " << q[1] << " " << q[2] << "\n";
    // 或者使用at()进行边界检查
    // std::cout << q.at(0) << " " << q.at(1) << " " << q.at(2) << "\n";

    return 0; // ✅ 自动释放
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// D OG Code
int count = 10;
for (int i = 0; i < count; ++i) {
    int count = i;    // ❌ shadows outer 'count'
    // ...
}
//D1 Eliminate shadowing and tighten scope. Mark immutable values const.
const int total = 10;
for (int i = 0; i < total; ++i) {
    int currentValue = i;    
    // ...
}
//D2 Rename variables to avoid shadowing; limit the lifetime of loop-local variables.
//重命名同上完成避免阴影，控制变量生命时间无需修改，在大括号结束时自动销毁

//D3 Which identifiers can be const, and why?
//total可以是，因为这个数值不会进行变化。currentValue可以是，因为每次循环开始时都会被重新定义一次。

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// E OG Code
#include <iostream>
#include <string>
struct Trace {
    std::string name;
    Trace(std::string n): name(std::move(n)) { std::cout << "+" << name << "\n"; }
    ~Trace() { std::cout << "-" << name << "\n"; }
};

int main(){
    Trace a("a");
    {
        Trace b("b");
        {
            Trace c("c");
        }
    }
}
//E1 Predict the exact output order (line by line) and annotate where each object is destroyed.
/*
a+
b+
c+
c-
b-
a-
*/

//Now add a heap allocation inside the inner block and manage it with std::unique_ptr<Trace>. When is the heap object released?
#include <iostream>
#include <string>
struct Trace {
    std::string name;
    Trace(std::string n): name(std::move(n)) { std::cout << "+" << name << "\n"; }
    ~Trace() { std::cout << "-" << name << "\n"; }
};

int main(){
    Trace a("a");
    {
        Trace b("b");
        {
            Trace c("c");
            auto d = std::make_unique<Trace>("d");  // 堆对象d
        }
    }
}
/*
a+
b+
c+
d+
c-
d-
b-
a-
*/