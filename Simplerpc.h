#include <string>
#include <map>
#include <sstream>
#include <functional>
#include "zmq.hpp"
#include "Serializer.h"

using namespace std;

template<typename T>
struct type_xx {
    typedef T type;
};

template<>
struct type_xx<void> {
    typedef int8_t type;
};

class Simplerpc {
public:
    enum rpc_role {
        RPC_CLIENT,
        RPC_SERVER
    };
    enum rpc_err_code {
        RPC_ERR_SUCCESS = 0,
        RPC_ERR_FUNCTIION_NOT_BIND,
        RPC_ERR_RECV_TIMEOUT
    };
    template<typename T>
    class value_t {
    public:
        typedef typename type_xx<T>::type type;
        typedef std::string msg_type;
        typedef uint16_t code_type;

        value_t() {
            code_ = 0;
            msg_.clear();
        }
        bool valid() {
            return (code_ == 0 ? true : false);
        }
        int error_code() {
            return code_;
        }
        type val() {
            return val_;
        }

        void set_val(const type& val) {
            val_ = val;
        }
        void set_code(code_type code) {
            code_ = code;
        }
        void set_msg(msg_type msg) {
            msg_ = msg;
        }
        friend Serializer& operator >> (Serializer& in, value_t<T>& d) {
            in >> d.code_ >> d.msg_;
            if (d.code_ == 0) {
                in >> d.val_;
            }
            return in;
        }
        friend Serializer& operator << (Serializer& out, value_t<T> d) {
            out << d.code_ << d.msg_ << d.val_;
            return out;
        }
    private:
        code_type code_;
        msg_type msg_;
        type val_;
    };
    Simplerpc();
    ~Simplerpc();

    void as_client(string ip, int port);
    void as_server(int port);
    void send(zmq::message_t& data);
    void recv(zmq::message_t& data);
    void set_timeout(uint32_t ms);
    void run();

public:
    template<typename F>
    void bind(std::string name, F func);
    
    template<typename F, typename S>
    void bind(std::string name, F func, S* s);

    template<typename R>
    value_t<R> call(string name);
    
    template<typename R, typename P1>
    value_t<R> call(string name, P1);

    template<typename R, typename P1, typename P2>
    value_t<R> call(string name, P1, P2);

     template<typename R, typename P1, typename P2, typename P3>
    value_t<R> call(string name, P1, P2, P3);

    template<typename R, typename P1, typename P2, typename P3, typename P4>
    value_t<R> call(string name, P1, P2, P3, P4);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
    value_t<R> call(string name, P1, P2, P3, P4, P5);
private:
    Serializer* call_(std::string name, const char* data, int len);
    template<typename R>
	value_t<R> net_call(Serializer& ds);

	template<typename F>
	void callproxy(F fun, Serializer* pr, const char* data, int len);

	template<typename F, typename S>
	void callproxy(F fun, S* s, Serializer* pr, const char* data, int len);

	// PROXY FUNCTION POINT
	template<typename R>
	void callproxy_(R(*func)(), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R()>(func), pr, data, len);
	}

	template<typename R, typename P1>
	void callproxy_(R(*func)(P1), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2>
	void callproxy_(R(*func)(P1, P2), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2, typename P3>
	void callproxy_(R(*func)(P1, P2, P3), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(R(*func)(P1, P2, P3, P4), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4)>(func), pr, data, len);
	}

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(R(*func)(P1, P2, P3, P4, P5), Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4, P5)>(func), pr, data, len);
	}

	// PROXY CLASS MEMBER
	template<typename R, typename C, typename S>
	void callproxy_(R(C::* func)(), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R()>(std::bind(func, s)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1>
	void callproxy_(R(C::* func)(P1), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1)>(std::bind(func, s, std::placeholders::_1)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2>
	void callproxy_(R(C::* func)(P1, P2), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2)>(std::bind(func, s, std::placeholders::_1, std::placeholders::_2)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3>
	void callproxy_(R(C::* func)(P1, P2, P3), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(R(C::* func)(P1, P2, P3, P4), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(R(C::* func)(P1, P2, P3, P4, P5), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4, P5)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)), pr, data, len);
	}

	// PORXY FUNCTIONAL
	template<typename R>
	void callproxy_(std::function<R()>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1>
	void callproxy_(std::function<R(P1)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2>
	void callproxy_(std::function<R(P1, P2)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3>
	void callproxy_(std::function<R(P1, P2, P3)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(std::function<R(P1, P2, P3, P4)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(std::function<R(P1, P2, P3, P4, P5)>, Serializer* pr, const char* data, int len);
private:
    map<string, function<void(Serializer*, const char*, int)>> m_handlers;

    zmq::context_t m_context;
    zmq::socket_t* m_socket;

    rpc_err_code m_error_code;

    int m_role;

};


Simplerpc::Simplerpc() : m_context(1) {
    m_error_code = RPC_ERR_SUCCESS;
}

Simplerpc::~Simplerpc() {
    m_socket->close();
    delete m_socket;
    m_context.close();
}

void Simplerpc::as_client(string ip, int port) {
    m_role = RPC_CLIENT;
	m_socket = new zmq::socket_t(m_context, ZMQ_REQ); // ZMQ_REQ 代表 request
	ostringstream os;
	os << "tcp://" << ip << ":" << port;
	m_socket->connect (os.str());
}

void Simplerpc::as_server(int port) {
    m_role = RPC_SERVER;
	m_socket = new zmq::socket_t(m_context, ZMQ_REP); // ZMQ_REP 代表 respone
	ostringstream os;
	os << "tcp://*:" << port;
	m_socket->bind (os.str());
}

void Simplerpc::send(zmq::message_t& data) {
    m_socket->send(data);
}

void Simplerpc::recv(zmq::message_t& data) {
    m_socket->recv(&data);
}

inline void Simplerpc::set_timeout(uint32_t ms) {
    if (m_role == RPC_CLIENT) {
		m_socket->setsockopt(ZMQ_RCVTIMEO, ms);
	}
}

void Simplerpc::run() {
    // only server can call
	if (m_role != RPC_SERVER) {
		return;
	}
	while (1){
		zmq::message_t data;
		// 先接收客户端传来的数据
		recv(data);
		// 转换成StreamBuffer类型
		StreamBuffer iodev((char*)data.data(), data.size());
		// 再把这个StreamBuffer给序列化
		Serializer ds(iodev);

		std::string funname;
		// 获得函数名再执行相应的函数就行
		ds >> funname;
		// 将函数执行后的结果序列化
		Serializer* r = call_(funname, ds.current(), ds.size()- funname.size());
		
		zmq::message_t retmsg (r->size());
		memcpy (retmsg.data (), r->data(), r->size());
		send(retmsg);
		delete r;
	}
}

Serializer* Simplerpc::call_(string name, const char* data, int len) {
    Serializer* ds = new Serializer();
    if (m_handlers.find(name) == m_handlers.end()) {
		(*ds) << value_t<int>::code_type(RPC_ERR_FUNCTIION_NOT_BIND);
		(*ds) << value_t<int>::msg_type("function not bind: " + name);
		return ds;
	}
	auto fun = m_handlers[name];
	fun(ds, data, len);
	ds->reset();
	return ds;
}





template<typename F>
void Simplerpc::bind( std::string name, F func )
{
	m_handlers[name] = std::bind(&Simplerpc::callproxy<F>, this, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F, typename S>
inline void Simplerpc::bind(std::string name, F func, S* s)
{
	m_handlers[name] = std::bind(&Simplerpc::callproxy<F, S>, this, func, s, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}








template<typename F>
void Simplerpc::callproxy( F fun, Serializer* pr, const char* data, int len )
{
	callproxy_(fun, pr, data, len);
}

template<typename F, typename S>
inline void Simplerpc::callproxy(F fun, S * s, Serializer * pr, const char * data, int len)
{
	callproxy_(fun, s, pr, data, len);
}

// help call return value type is void function
template<typename R, typename F>
typename std::enable_if<std::is_same<R, void>::value, typename type_xx<R>::type >::type call_helper(F f) {
	f();
	return 0;
}

template<typename R, typename F>
typename std::enable_if<!std::is_same<R, void>::value, typename type_xx<R>::type >::type call_helper(F f) {
	return f();
}

template<typename R>
void Simplerpc::callproxy_(std::function<R()> func, Serializer* pr, const char* data, int len)
{
	typename type_xx<R>::type r = call_helper<R>(std::bind(func));

	value_t<R> val;
	val.set_code(RPC_ERR_SUCCESS);
	val.set_val(r);
	(*pr) << val;
}

template<typename R, typename P1>
void Simplerpc::callproxy_(std::function<R(P1)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1;
	ds >> p1;
	typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1));

	value_t<R> val;
	val.set_code(RPC_ERR_SUCCESS);
	val.set_val(r);
	(*pr) << val;
}

template<typename R, typename P1, typename P2>
void Simplerpc::callproxy_(std::function<R(P1, P2)> func, Serializer* pr, const char* data, int len )
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2;
	ds >> p1 >> p2;
	typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2));
	
	value_t<R> val;
	val.set_code(RPC_ERR_SUCCESS);
	val.set_val(r);
	(*pr) << val;
}

template<typename R, typename P1, typename P2, typename P3>
void Simplerpc::callproxy_(std::function<R(P1, P2, P3)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2; P3 p3;
	ds >> p1 >> p2 >> p3;
	typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2, p3));
	value_t<R> val;
	val.set_code(RPC_ERR_SUCCESS);
	val.set_val(r);
	(*pr) << val;
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
void Simplerpc::callproxy_(std::function<R(P1, P2, P3, P4)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2; P3 p3; P4 p4;
	ds >> p1 >> p2 >> p3 >> p4;
	typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2, p3, p4));
	value_t<R> val;
	val.set_code(RPC_ERR_SUCCESS);
	val.set_val(r);
	(*pr) << val;
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
void Simplerpc::callproxy_(std::function<R(P1, P2, P3, P4, P5)> func, Serializer* pr, const char* data, int len)
{
	Serializer ds(StreamBuffer(data, len));
	P1 p1; P2 p2; P3 p3; P4 p4; P5 p5;
	ds >> p1 >> p2 >> p3 >> p4 >> p5;
	typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2, p3, p4, p5));
	value_t<R> val;
	val.set_code(RPC_ERR_SUCCESS);
	val.set_val(r);
	(*pr) << val;
}

template<typename R>
inline Simplerpc::value_t<R> Simplerpc::net_call(Serializer& ds)
{
	zmq::message_t request(ds.size() + 1);
	memcpy(request.data(), ds.data(), ds.size());
	if (m_error_code != RPC_ERR_RECV_TIMEOUT) {
		send(request);
	}
	zmq::message_t reply;
	recv(reply);
	value_t<R> val;
	if (reply.size() == 0) {
		// timeout
		m_error_code = RPC_ERR_RECV_TIMEOUT;
		val.set_code(RPC_ERR_RECV_TIMEOUT);
		val.set_msg("recv timeout");
		return val;
	}
	m_error_code = RPC_ERR_SUCCESS;
	ds.clear();
	ds.write_raw_data((char*)reply.data(), reply.size());
	ds.reset();

	ds >> val;
	return val;
}

template<typename R>
inline Simplerpc::value_t<R> Simplerpc::call(std::string name)
{
	Serializer ds;
	ds << name;
	return net_call<R>(ds);
}

template<typename R, typename P1>
inline Simplerpc::value_t<R> Simplerpc::call(std::string name, P1 p1)
{
	Serializer ds;
	ds << name << p1;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2>
inline Simplerpc::value_t<R> Simplerpc::call( std::string name, P1 p1, P2 p2 )
{
	Serializer ds;
	ds << name << p1 << p2;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3>
inline Simplerpc::value_t<R> Simplerpc::call(std::string name, P1 p1, P2 p2, P3 p3)
{
	Serializer ds;
	ds << name << p1 << p2 << p3;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
inline Simplerpc::value_t<R> Simplerpc::call(std::string name, P1 p1, P2 p2, P3 p3, P4 p4)
{
	Serializer ds;
	ds << name << p1 << p2 << p3 << p4;
	return net_call<R>(ds);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
inline Simplerpc::value_t<R> Simplerpc::call(std::string name, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	Serializer ds;
	ds << name << p1 << p2 << p3 << p4 << p5;
	return net_call<R>(ds);
}
















