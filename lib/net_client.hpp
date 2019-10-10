#pragma once

#include "uv_socket.hpp"
#include "message.hpp"

#include <uv.h>

#include <queue>
#include <string>

namespace sample
{
    class BaseApp; // forward
    class ServerApp; // forward

    /**
     * Represents a connection.
     */
    class NetClientBase: public IUvSocket
    {
    public:
        enum State
        {
            Undefined = 0,
            NotConnected,
            Connecting,
            Connected,
            Accepted,
            Sending,
            Sent,
            Receiving,
            Received,
            Closing,
            Closed
        };

    public:
        NetClientBase(BaseApp* app_in, std::string const & peerAddr_in);
        virtual ~NetClientBase();
	    std::string getPeerAddr() const { return myPeerAddr; }
        // Send a message to this peer
        int sendMessage(BaseMessage const & msg_in);
        int close();
        void onRead(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
        void onWrite(uv_write_t* req, int status);
        void onClose(uv_handle_t* handle);
        int doRead();
        virtual void process() { }
        bool isConnected() const;

    protected:
        void setUvStream(uv_tcp_t* stream_in);
        
    private:
        static void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
        static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
        static void on_write(uv_write_t* req, int status);
        static void on_close(uv_handle_t* handle);
        void doProcessReceivedBuffer();

    protected:
        BaseApp* myApp;
        State myState;

    private:
        std::string myPeerAddr;
        std::string myReceiveBuffer;
        uv_tcp_t* myUvStream;
    };

    /**
     * Represents an incoming connection.
     */
    class NetClientIn: public NetClientBase
    {
    public:
        NetClientIn(ServerApp* app_in, uv_tcp_t* client_in, std::string const & peerAddr_in);
    };

    /**
     * Represents an outgoing connection.
     */
    class NetClientOut: public NetClientBase
    {
    public:
        NetClientOut(BaseApp* app_in, std::string const & host_in, int port_in, int pingToSend_in);
        int connect();
        // Perform state-dependent next action in the client state diagram
        virtual void process();
        void onConnect(uv_connect_t* req, int status);
        
    private:
        static void on_connect(uv_connect_t* req, int status);

    private:
        std::string myHost;
        int myPort;
        int myPingToSend;
        int mySendCounter;
    };
}
