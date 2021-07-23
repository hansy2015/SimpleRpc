#include <string>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include "Simplerpc.h"

using namespace std;

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

int main() {
    Simplerpc client;
    client.as_client("127.0.0.1", 5555);
    client.set_timeout(2000);
    int callcnt = 0;
    while (1) {
        cout << "current call count: " << ++callcnt << endl;
        client.call<void>("foo_1");
        client.call<void>("foo_2", 10);
        int foo3r = client.call<int>("foo_3", 10).val();
        cout << "foo3r == " << foo3r << endl;
        int foo4r = client.call<int>("foo_4", 10, "rpc", 100, (float)10.8).val();
        cout << "foo4r == " << foo4r << endl;
        PersonInfo dd = {
            10,
            "rpc",
            170
        };
        dd = client.call<PersonInfo>("foo_5", dd, 120).val();
        cout << "age == " << dd.age << "name == " << dd.name << "height == " << dd.height << endl;
        int foo6r = client.call<int>("foo_6", 10, "rpc", 100).val();
        cout << "foo6r == " << foo6r << endl;

//        usleep(1000);
        sleep(5);
    }
    return 0;
}
