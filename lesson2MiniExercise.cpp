//OG wrong code
#include <iostream>
using namespace std;

int main()
{
    int a = 78;
    int b = 85;
    int c = 92;
    int tmp = a;
    int total = 0;
    total = a + b + c;
    double avg;
    avg = total / 3.0;
    bool t = true;
    if (avg < 50.0)
    {
        t = false;
    }
    cout << "Avg:" << avg << "passed?" << (t ? "yes" : "no") << endl;
    return 0;
}