#include "message.hpp"  

using namespace sample;
using namespace std;


BaseMessage::BaseMessage(MessageType type_in) :
myType(type_in)
{
}


HandshakeMessage::HandshakeMessage(std::string yourAddr_in, string myAddr_in) :
BaseMessage(MessageType::Handshake),
myYourAddr(yourAddr_in),
myMyAddr(myAddr_in)
{
}

void HandshakeMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.handshake(*this);
}

std::string HandshakeMessage::toString() const
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

std::string HandshakeResponseMessage::toString() const
{
    return "HandShResp " + myMyAddr + " " + myYourAddr;
}


PingMessage::PingMessage(string myText_in) :
BaseMessage(MessageType::Ping),
myText(myText_in)
{
}

void PingMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.ping(*this);
}

std::string PingMessage::toString() const
{
    return "Ping " + myText;
}


PingResponseMessage::PingResponseMessage(string myText_in) :
BaseMessage(MessageType::PingResponse),
myText(myText_in)
{
}

void PingResponseMessage::visit(MessageVisitorBase & visitor_in) const
{
    visitor_in.pingResponse(*this);
}

std::string PingResponseMessage::toString() const
{
    return "PingResp " + myText;
}
