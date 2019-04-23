#pragma once

#include <string>

namespace sample
{
    enum MessageType
    {
        Invalid = 0,
        Handshake = 1,
        HandshakeResponse = 2,
        Ping = 3,
        PingResponse = 4,
        OtherPeer = 5
    };

    class MessageVisitorBase;  // forward decl

    /**
     * Base class for messages.
     */
    class BaseMessage
    {
    public:
        BaseMessage(MessageType type_in);
        virtual ~BaseMessage() = default;
        MessageType getType() const { return myType; }
        virtual void visit(MessageVisitorBase & visitor_in) const = 0;
        virtual std::string toString() const = 0;

    private:
        MessageType myType;
    };

    class HandshakeMessage: public BaseMessage
    {
    public:
        HandshakeMessage(std::string yourAddr_in, std::string myAddr_in);
        std::string getYourAddr() const { return myYourAddr; }
        std::string getMyAddr() const { return myMyAddr; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;

    private:
        std::string myYourAddr;
        std::string myMyAddr;
    };

    class HandshakeResponseMessage: public BaseMessage
    {
    public:
        HandshakeResponseMessage(std::string myAddr_in, std::string yourAddr_in);
        std::string getMyAddr() const { return myMyAddr; }
        std::string getYourAddr() const { return myYourAddr; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;

    private:
        std::string myMyAddr;
        std::string myYourAddr;
    };

    class PingMessage: public BaseMessage
    {
    public:
        PingMessage(std::string text_in);
        std::string getText() const { return myText; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;

    private:
        std::string myText;
    };

    class PingResponseMessage: public BaseMessage
    {
    public:
        PingResponseMessage(std::string text_in);
        std::string getText() const { return myText; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;

    private:
        std::string myText;
    };

    /// Contains info about a 3rd peer
    class OtherPeerMessage: public BaseMessage
    {
    public:
        OtherPeerMessage(std::string host_in, int port_in);
        std::string getHost() const { return myHost; }
        int getPort() const { return myPort; }
        void visit(MessageVisitorBase & visitor_in) const;
        std::string toString() const;

    private:
        std::string myHost;
        int myPort;
    };

    class MessageVisitorBase
    {
    public:
	    virtual void handshake(HandshakeMessage const & msg_in) = 0;
	    virtual void handshakeResponse(HandshakeResponseMessage const & msg_in) = 0;
        virtual void ping(PingMessage const & msg_in) = 0;
        virtual void pingResponse(PingResponseMessage const & msg_in) = 0;
        virtual void otherPeer(OtherPeerMessage const & msg_in) = 0;
	    virtual ~MessageVisitorBase() = default;
    };
}
