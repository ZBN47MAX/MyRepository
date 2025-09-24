#include <iostream>
using namespace std;

//ExerciseA
//For each identifier below, mark where it is declared, instantiated, assigned, and released/deleted. Note stack vs heap.
/*
#include <iostream>
using namespace std;

int main() {
    int n;                    // 1)
    if (true) {
        int n = 3;            // 2) (shadows outer n)
        int* p = new int(n);  // 3) pointer + heap object
        *p += 1;              // 4)
        cout << *p << "\n";
        delete p;             // 5)
    }                         // 6)
    cout << n << "\n";        // 7)
}
//Explain what is released at (5) vs (6). What happens at (7) if outer n was never assigned?
//Rewrite so outer n is safely initialised and no shadowing occurs.
*/

//////////////////////////////////////////////////////////////////////
//rewrite to fix outer n
/*
#include <iostream>
using namespace std;

int main() {
    int n=0;                    // 1)
    if (true) {
        int n = 3;            // 2) (shadows outer n)
        int* p = new int(n);  // 3) pointer + heap object
        *p += 1;              // 4)
        cout << *p << "\n";
        delete p;             // 5)
    }                         // 6)
    cout << n << "\n";        // 7)
}
*/


//////////////////////////////////////////////////////////////////////
//ExerciseB
//OG
/*
int* makeVal() {
    int x = 42;
    return &x;      // ❌ returns address of a dead stack variable
}

int& pick(bool b) {
    int a = 1, c = 2;
    return b ? a : c;  // ❌ returns reference to dead locals
}
    */


//////////////////////////////////////////////////////////////////////
//based on value return
/*
int makeVal() {
    return 42;  // 甚至不需要临时变量
}

int pick(bool b) {
    return b ? 1 : 2;
}
*/


//////////////////////////////////////////////////////////////////////
//based on auto pointer
/*
#include <memory>

// 明确表达"我创建一个新对象，调用者负责生命周期"
std::unique_ptr<int> makeVal() {
    return std::make_unique<int>(42);
}

// 如果确实需要引用语义，但又要安全
std::unique_ptr<int> pick(bool b) {
    return b ? std::make_unique<int>(1) 
             : std::make_unique<int>(2);
}
*/


//////////////////////////////////////////////////////////////////////
//exerciseC
//OG
/*
Find the bugs, then rewrite using RAII containers.
int main() {
    int* p = new int{5};
    delete p;
    std::cout << *p << "\n";   // ❌ use-after-free

    int* q = new int[3]{1,2,3};
    delete q;                 // ❌ wrong deallocator (should be delete[])
    // ...
}
Correct the manual version (delete[] q;, nulling pointers, etc.).
Refactor to std::unique_ptr<int[]> or std::vector<int> so release is implicit.
*/


//////////////////////////////////////////////////////////////////////
//based on human fix
/*
#include <iostream>
int main() {
    // 修复第一个bug
    int* p = new int{5};
    std::cout << *p << "\n";   // ✅ 先使用再释放
    delete p;                  // ✅ 正确释放
    p = nullptr;               // ✅ 最佳实践：释放后置空，避免悬空指针

    // 修复第二个bug  
    int* q = new int[3]{1,2,3};
    // ... 使用数组
    delete[] q;               // ✅ 使用正确的释放方式
    q = nullptr;              // ✅ 释放后置空

    return 0;
}
*/

//////////////////////////////////////////////////////////////////////
//based on RAII
/*
#include <iostream>
#include <memory>

int main() {
    // 使用 unique_ptr 管理单个int对象
    auto p = std::make_unique<int>(5);  // ✅ 自动管理内存
    std::cout << *p << "\n";            // ✅ 安全使用
    // 不需要手动delete！离开作用域时自动释放

    // 使用 unique_ptr 管理数组
    auto q = std::make_unique<int[]>(3); // ✅ 创建动态数组
    q[0] = 1; q[1] = 2; q[2] = 3;       // ✅ 像普通数组一样使用
    
    // 或者使用初始化列表（C++20）
    // auto q = std::make_unique<int[]>(3, {1, 2, 3});
    
    std::cout << q[0] << "," << q[1] << "," << q[2] << "\n";
    
    // 不需要手动delete[]！离开作用域时自动释放
    return 0;
} // ✅ 在这里，p和q自动被释放！
*/


//////////////////////////////////////////////////////////////////////
//OG
//Eliminate shadowing and tighten scope. Mark immutable values const.
/*
int count = 10;
for (int i = 0; i < count; ++i) {
    int count = i;    // ❌ shadows outer 'count'
    // ...
}
    */
//Rename variables to avoid shadowing; limit the lifetime of loop-local variables.
//Which identifiers can be const, and why?

/*
{
    const int total_iterations = 10;  // ✅ 明确含义，const保护
    for (int i = 0; i < total_iterations; ++i) {
        const int current_value = i;  // ✅ 明确含义，const保护
        // ... 使用 current_value
    }
}*/


//////////////////////////////////////////////////////////////////////
//OG
//Predict the exact output order (line by line) and annotate where each object is destroyed.
/*
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
*/
//Now add a heap allocation inside the inner block and manage it with std::unique_ptr<Trace>. When is the heap object released?

//predict output as
/*
+a
+b
+c
-c
-b
-a
*/

//based on RAII
/*
#include <iostream>
#include <string>
#include <memory>

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
            auto heap_obj = std::make_unique<Trace>("heap");  // 堆分配
        } // c被销毁，但heap_obj仍然存在
    } // b被销毁，heap_obj仍然存在
} // a被销毁，heap_obj在这里被销毁！
 */
