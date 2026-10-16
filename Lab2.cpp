#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

using namespace std;

queue<int> numbers;
mutex m;
bool flag = false;

void DataPreparation() {
    int num;
    cout << "Enter numbers (enter g to finish): ";

    while (cin >> num) {
        numbers.push(num);
    }
    lock_guard<mutex> guard(m);
    flag = true;
}

bool IsPrime(int n) {
    if (n < 2) return false;

    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

void DataProcessing() {
    unique_lock<mutex> lk(m);

    while (!flag) {
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
        lk.lock();
    }
    cout << "Prime numbers: ";

    while (!numbers.empty()) {

        int num = numbers.front();

        numbers.pop();

        if (IsPrime(num)) {
            cout << num << " ";
        }
    }
    cout << endl;

}


int main()
{
    thread t1(DataPreparation);
    t1.detach();

    thread t2(DataProcessing);
    t2.join();

    return 0;
}
