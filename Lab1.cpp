// ==================== 1.2.1 ====================

#include <iostream>
#include <thread>

using namespace std;
void thread1(){
  cout << "1";
}
void thread2 () {
  cout << "2";
}
int main()
{
  thread t1(thread1);
  thread t2(thread2);
}

// ==================== 1.2.2 ====================

#include <iostream>
#include <thread>

using namespace std;
void thread1() {
  cout << "1";
}
void thread2() {
  cout << "2";
}
int main()
{
  thread t1(thread1);
  thread t2(thread2);
  t1.detach();
  t2.detach();
}

// ==================== 1.2.3 ====================
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
using namespace std;
list<int> l;
void AddToList(int a) {
for (int i = 0; i < 10; i++) {
l.push_back(a+i);
cout << "Added:" << a+i << endl;
}
}
void ListContains(int a) {
for (int i = 0; i < 10; i++) {
if (find(l.begin(), l.end(), a) != l.end()) {
cout << a << " is in the list " << endl;
}
else {
cout << a << " is not in the list " << endl;
}
}
}
int main()
{
int a = 5;
thread t1(AddToList, a);
thread t2(ListContains, a);
t1.join();
t2.join();
}

// ==================== 1.2.4 ====================
#include <list>
#include <algorithm>
#include <mutex>
using namespace std;
list<int> l;
mutex m;
void AddToList(int a) {
  for (int i = 0; i < 10; i++) {
    m.lock();
    l.push_back(a+i);
    cout << "Added:" << a+i << endl;
    m.unlock();
  }
}
void ListContains(int a) {
  for (int i = 0; i < 10; i++) {
    m.lock();
    if (find(l.begin(), l.end(), a) != l.end()) {
    cout << a << " is in the list " << endl;
  }
  else {
    cout << a << " is not in the list " << endl;
  }
  m.unlock();
 }
}
int main()
{
  int a = 5;
  thread t1(AddToList, a);
  thread t2(ListContains, a);
  t1.join();
  t2.join();
}
// ==================== 1.2.5 ====================
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>

using namespace std;
list<int> l;
mutex m;

void AddToList(int a) {
  lock_guard<mutex> guard(m);
  l.push_back(a);
  cout << "Added:" << a << endl;
}

void ListContains(int a) {
  lock_guard<mutex> guard(m);
  if (find(l.begin(), l.end(), a) != l.end()) {
  cout << a << " is in the list " << endl;
  }
  else {
    cout << a << " is not in the list " << endl;
  }
}
int main() {
  int a = 5;
  for (int i = 0; i < 10; i++) {
    thread t1(AddToList, a + i);
    thread t2(ListContains, a);
    t1.detach();
    t2.detach();
  }
}

// ==================== 1.2.6 ====================

#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <string>
#include <functional>

using namespace std;

class someData {
public:
  string name;
  string nachname;
  string adress;
  int alt;
};

class exchangePerson {
public:
  someData data;
  mutex m;
  static void JohnDoe(exchangePerson& person) {
    lock_guard<mutex> guard(person.m);
    person.data.name = "John";
    person.data.nachname = "Doe";
    person.data.adress = "Unknown";
    person.data.alt = 120;
}

  static void JacobSmith(exchangePerson& person) {
    lock_guard<mutex> guard(person.m);
    person.data.name = "Jacob";
    person.data.nachname = "Smith";
    person.data.adress = "Known";
    person.data.alt = 1;
  }

  static void Swap(exchangePerson& person1, exchangePerson& person2) {
    lock(person1.m, person2.m);
    lock_guard<mutex> guard1(person1.m, adopt_lock);
    lock_guard<mutex> guard2(person2.m, adopt_lock);
    swap(person1.data, person2.data);
  }
};
int main() {
  exchangePerson person1;
  exchangePerson person2;
  thread t1(exchangePerson::JohnDoe, ref(person1));
  thread t2(exchangePerson::JacobSmith, ref(person2));
  t1.detach();
  t2.detach();
  
  thread t3(exchangePerson::Swap, ref(person1), ref(person2));
  t3.join();
cout << person1.data.name << " "
  << person1.data.nachname << " "
  << person1.data.adress << " "
  << person1.data.alt << endl;
cout << person2.data.name << " "
  << person2.data.nachname << " "
  << person2.data.adress << " "
  << person2.data.alt << endl;
}
// ==================== 1.2.7 ====================
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include <string>
#include <functional>

using namespace std;

class someData {
public:
  string name;
  string nachname;
  string adress;
  int alt;
};

class exchangePerson {
public:
  someData data;
  mutex m;
  static void JohnDoe(exchangePerson& person) {
    lock_guard<mutex> guard(person.m);
    person.data.name = "John";
    person.data.nachname = "Doe";
    person.data.adress = "Unknown";
    person.data.alt = 120;
}

  static void JacobSmith(exchangePerson& person) {
    lock_guard<mutex> guard(person.m);
    person.data.name = "Jacob";
    person.data.nachname = "Smith";
    person.data.adress = "Known";
    person.data.alt = 1;
  }

  static void Swap(exchangePerson& person1, exchangePerson& person2) {
    unique_lock<mutex> guard1(person1.m, defer_lock);
    unique_lock<mutex> guard2(person2.m, defer_lock);
    lock(guard1, guard2);
    swap(person1.data, person2.data);
  }
};
int main() {
  exchangePerson person1;
  exchangePerson person2;
  thread t1(exchangePerson::JohnDoe, ref(person1));
  thread t2(exchangePerson::JacobSmith, ref(person2));
  t1.detach();
  t2.detach();
  
  thread t3(exchangePerson::Swap, ref(person1), ref(person2));
  t3.join();
cout << person1.data.name << " "
  << person1.data.nachname << " "
  << person1.data.adress << " "
  << person1.data.alt << endl;
cout << person2.data.name << " "
  << person2.data.nachname << " "
  << person2.data.adress << " "
  << person2.data.alt << endl;
}
