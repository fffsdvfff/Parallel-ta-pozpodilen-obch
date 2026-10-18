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
#===========1.2.2============

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

int i = 0;
mutex m;
condition_variable cv;

void Waits(int id) {
    cout << "Thread " << id << " is waiting..." << endl;

    unique_lock<mutex> lk(m);
    cv.wait(lk, [] { return i == 1; });

    cout << "Thread " << id << " has finished waiting" << endl; 

}

void Awake() {
    this_thread::sleep_for(chrono::milliseconds(100));

    cout << "Awake: first notification" << endl;
    cv.notify_all();

    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> guard(m);
        i = 1;
    }

    cout << "Awake: second notification" << endl;
    cv.notify_all();
}



int main()
{
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);

    thread t4(Awake);
    t4.join();

    t1.join();
    t2.join();
    t3.join();

    return 0;
}

#============1.2.3===========
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

int i = 0;
mutex m;
condition_variable cv;
condition_variable cvDone;

bool stop = false;
bool printed = false;

void Waits(int id) {
    unique_lock<mutex> lk(m);

    cv.wait(lk, [] { return i == 1 || stop; });

    if (stop) {
        return;
    }

    if (!printed) {
        cout << "Message from Thread " << id << endl;
        printed = true;
        cvDone.notify_one();
    }
}

void Notify() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> guard(m);
        i = 1;
    }

    cv.notify_one();
}

int main() {
    thread Thread1(Waits, 1);
    thread Thread2(Waits, 2);
    thread Thread3(Waits, 3);

    thread tNotify(Notify);

    tNotify.join();

    {
        unique_lock<mutex> lk(m);
        cvDone.wait(lk, [] { return printed; });
    }

    {
        lock_guard<mutex> guard(m);
        stop = true;
    }

    cv.notify_all();

    Thread1.join();
    Thread2.join();
    Thread3.join();

    return 0;
}
