#include <string>
#include <iostream>
#include "Simplerpc.h"

using namespace std;

void foo_1() {
    cout << "foo_1" << endl;
    return;
}

void foo_2(int arg1) {
    cout << "foo_2" << "arg1 == " << arg1 << endl;
}

int foo_3(int arg1) {
    return arg1 * arg1;
}

int foo_4(int arg1, string arg2, int arg3, float arg4) {
    return arg1 * arg3;
}

class ClassMem {
public:
    int bar(int arg1, string arg2, int arg3) {
        return arg1 * arg3;
    }
};

struct PersonInfo {
    int age;
    string name;
    float height;

    friend Serializer& operator >> (Serializer& in, PersonInfo& d) {
		in >> d.age >> d.name >> d.height;
		return in;
	}
	friend Serializer& operator << (Serializer& out, PersonInfo d) {
		out << d.age << d.name << d.height;
		return out;
	}
};

PersonInfo foo_5(PersonInfo d, int weight) {
    PersonInfo ret;
    ret.age = d.age + 10;
    ret.name = d.name + "is good";
    ret.height = d.height + 10;
    return ret;
}

int main() {
    Simplerpc server;
    server.as_server(5555);
    server.bind("foo_1", foo_1);
    server.bind("foo_2", foo_2);
    server.bind("foo_3", function<int(int)>(foo_3));
    server.bind("foo_4", foo_4);
    server.bind("foo_5", foo_5);

    ClassMem s;
    server.bind("foo_6", &ClassMem::bar, &s);
    cout << "run rpc server on: " << 5555 << endl;
    server.run();

    return 0;
}
