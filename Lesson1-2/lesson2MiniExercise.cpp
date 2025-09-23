//OG wrong code
#include <iostream>
using namespace std;

int block1() {
    //Block1
    int a = 78;
    int b = 85;
    int c = 92;
    //int tmp = a;
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

int block2() {

    //Block2
    int leftsocre = 40;
    int rightsocre = 60;
    int sum = leftsocre + rightsocre;
    double average = sum / 2.0;
    bool passed = average > 50.0;//const
    cout << "avg:" << average << "passed? " << (passed ? "yes" : "no") << endl;
    return 0;
}

int block3() {
    //Block3
    double average;
    int n;
    cin >> n;
    int sum = 0;
    for (int i = 0;i < n; ++i) {
        int v;
        cin >> v;
        sum += v;
    }
    average = sum / static_cast<double>(n);
    cout << "average:" << average << endl;
    return 0;
}

int block4() {
    int maxVal = 0;
    int count;
    cin >> count;
    for (int i = 0; i < count; ++i) {
        int val;
        cin >> val;
        if (val > maxVal)
            maxVal = val;
    }
    cout << "max:" << maxVal << endl;
    return 0;
}

int main()
{
    block1();
    block2();
    block3();
    block4();
    return 0;
}
