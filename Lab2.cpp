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

#============1.2.4===========
    
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

queue<int> numbers;
mutex m;
condition_variable cv;

bool finished = false;

void DataPreparation() {
    int num;
    cout << "Enter numbers (enter g to finish): ";

    while (cin >> num) {
        lock_guard<mutex> guard(m);
        numbers.push(num);
    }

    {
        lock_guard<mutex> guard(m);
        finished = true;
    }

    cv.notify_one();
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

    cv.wait(lk, [] {
        return !numbers.empty() || finished;
        });

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

int main() {
    thread t1(DataPreparation);
    thread t2(DataProcessing);

    t1.join();
    t2.join();

    return 0;
}

#============1.2.5===========
    
#include <iostream>
#include <future>
#include <cmath>
#include <chrono>

using namespace std;
using namespace chrono;

bool IsPrime(int n) {
    if (n < 2) return false;

    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) return false;
    }

    return true;
}

int FindNprime(int n) {
    int count = 0, num = 1;

    while (count < n) {
        num++;

        if (IsPrime(num)) count++;
    }

    return num;
}

int main() {
    int n, mode, choice;

    cout << "Enter the prime number position: ";
    cin >> n;

    if (!cin || n < 1) {
        cout << "Invalid position!" << endl;
        return 1;
    }

    cout << "\nChoose execution mode:\n1. Deferred\n2. Async\nYour choice: ";
    cin >> mode;

    if (!cin || (mode != 1 && mode != 2)) {
        cout << "Invalid mode!" << endl;
        return 1;
    }

    future<int> result;

    if (mode == 1)
        result = async(launch::deferred, FindNprime, n);
    else
        result = async(launch::async, FindNprime, n);

    cout << "\nChoose a mathematical function:\n1. Square root\n2. Sine\n3. Natural logarithm\nYour choice: ";
    cin >> choice;

    if (!cin) {
        cout << "Invalid choice!" << endl;
        return 1;
    }

    switch (choice) {
        case 1:
            cout << "Square root: " << sqrt(n) << endl;
            break;
        case 2:
            cout << "Sine: " << sin(n) << endl;
            break;
        case 3:
            cout << "Natural logarithm: " << log(n) << endl;
            break;
        default:
            cout << "Invalid choice!" << endl;
    }

    auto start = steady_clock::now();
    int prime = result.get();
    auto end = steady_clock::now();

    double waitingTime = duration<double>(end - start).count();

    cout << "\nPrime number: " << prime << endl;
    cout << "Waiting time: " << waitingTime << " seconds" << endl;

    return 0;
}

#============1.2.6===========
#include <iostream>
#include <thread>
#include <deque>
#include <future>
#include <mutex>
#include <condition_variable>
#include <string>

using namespace std;

deque<packaged_task<int(int)>> tasks;
deque<int> numbers;
mutex m;
condition_variable cv;
bool finished = false;

bool IsPrime(int n) {
	if (n < 2) return false;

	for (int i = 2; i <= n / i; i++) {
		if (n % i == 0) return false;
	}

	return true;
}

int FindNprime(int n) {
	int count = 0, num = 1;

	while (count < n) {
		num++;

		if (IsPrime(num)) count++;
	}

	return num;
}


void Worker() {
	while (true) {
		unique_lock<mutex> lock(m);
		cv.wait(lock, [] {
			return !tasks.empty() || finished ;
		});
		if (tasks.empty() && finished) {
			break;
		}
		auto task = move(tasks.front());
		int n = numbers.front();

		tasks.pop_front();
		numbers.pop_front();
		lock.unlock();

		task(n);

	}
}
 
int main() {
	thread worker(Worker);

	int input;
	deque<future<int>> results;

	while (true) {
		cout << "Enter n (or q to stop): ";

		if (!(cin >> input)){
			break;
		}
		if (input < 1) {
			cout << "Invalid number!" << endl;
			continue;
		}

		packaged_task<int(int)> task(FindNprime);

		future<int> result = task.get_future();

		{
			lock_guard<mutex> lock(m);

			tasks.push_back(move(task));
			numbers.push_back(input);
		}

		cv.notify_one();
		results.push_back(move(result));
	}
	{
		lock_guard<mutex> lock(m);
		finished = true;
	}

	cv.notify_one();

	worker.join();

	for (auto& result : results) {
		cout << "Prime number: " << result.get() << endl;
	}

	return 0;
 
}
#============1.2.7===========
    
#include <iostream>
#include <thread>
#include <future>
#include <cmath>
#include <chrono>
#include <utility>

using namespace std;

bool IsPrime(int n) {
    if (n < 2) return false;

    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) return false;
    }

    return true;
}

int FindNprime(int n) {
    int count = 0, num = 1;

    while (count < n) {
        num++;

        if (IsPrime(num)) count++;
    }

    return num;
}

void FirstThread(int n, promise<int> p1,
    promise<bool> signal, promise<int> p2) {

    int prime1 = FindNprime(n);
    p1.set_value(prime1);

    signal.set_value(true);

    int prime2 = FindNprime(n * 10);
    p2.set_value(prime2);
}

void SecondThread(int n, future<bool> signal) {
    if (signal.get()) {
        this_thread::sleep_for(chrono::seconds(2));
        cout << "Square root: " << sqrt(n) << endl;
    }
}

int main() {
    int n;

    cout << "Enter n: ";
    cin >> n;

    if (!cin || n < 1 || n > 1000000) {
        cout << "Invalid number!" << endl;
        return 1;
    }

    promise<int> p1;
    promise<bool> signal;
    promise<int> p2;

    future<int> f1 = p1.get_future();
    future<bool> fs = signal.get_future();
    future<int> f2 = p2.get_future();

    thread t1(FirstThread, n, move(p1),
        move(signal), move(p2));

    thread t2(SecondThread, n, move(fs));

    cout << "Prime number n: " << f1.get() << endl;
    cout << "Prime number n*10: " << f2.get() << endl;

    t1.join();
    t2.join();

    return 0;
}
