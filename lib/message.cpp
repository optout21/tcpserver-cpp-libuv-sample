#include "message.hpp"  

using namespace sample;
using namespace std;


BaseMessage::BaseMessage(MessageType type_in) :
myType(type_in)
{
}


HandshakeMessage::HandshakeMessage(string yourAddr_in, string myAddr_in) :
BaseMessage(MessageType::Handshake),
myYourAddr(yourAddr_in),
myMyAddr(myAddr_in)
{
}

void HandshakeMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.handshake(*this);
}

string HandshakeMessage::toString() const
{
    return "HandSh " + myYourAddr + " " + myMyAddr;
}


HandshakeResponseMessage::HandshakeResponseMessage(string myAddr_in, string yourAddr_in) :
BaseMessage(MessageType::HandshakeResponse),
myMyAddr(myAddr_in),
myYourAddr(yourAddr_in)
{
}

void HandshakeResponseMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.handshakeResponse(*this);
}

string HandshakeResponseMessage::toString() const
{
    return "HandShResp " + myMyAddr + " " + myYourAddr;
}


PingMessage::PingMessage(string text_in) :
BaseMessage(MessageType::Ping),
myText(text_in)
{
}

void PingMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.ping(*this);
}

string PingMessage::toString() const
{
    return "Ping " + myText;
}


PingResponseMessage::PingResponseMessage(string text_in) :
BaseMessage(MessageType::PingResponse),
myText(text_in)
{
}

void PingResponseMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.pingResponse(*this);
}

string PingResponseMessage::toString() const
{
    return "PingResp " + myText;
}


OtherPeerMessage::OtherPeerMessage(string host_in, int port_in) :
BaseMessage(MessageType::OtherPeer),
myHost(host_in),
myPort(port_in)
{
}

void OtherPeerMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.otherPeer(*this);
}

string OtherPeerMessage::toString() const
{
    return "OtherPeer " + myHost + ":" + to_string(myPort);
}
