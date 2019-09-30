#include "message.hpp"  

using namespace sample;
using namespace std;


BaseMessage::BaseMessage(MessageType type_in) :
myType(type_in)
{
}


HandshakeMessage::HandshakeMessage(string myVersion_in, string yourAddr_in, string myAddr_in) :
BaseMessage(MessageType::Handshake),
myMyVersion(myVersion_in),
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
    return "HandSh " + myMyVersion + " " + myYourAddr + " " + myMyAddr;
}


HandshakeResponseMessage::HandshakeResponseMessage(string myVersion_in, string myAddr_in, string yourAddr_in) :
BaseMessage(MessageType::HandshakeResponse),
myMyVersion(myVersion_in),
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
    return "HandShResp " + myMyVersion + " " + myMyAddr + " " + myYourAddr;
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


void SerializerMessageVisitor::handshake(HandshakeMessage const & msg_in)
{
    myMessage = "HANDSH " + msg_in.getMyVersion() + " " + msg_in.getYourAddr() + " " + msg_in.getMyAddr();
}

void SerializerMessageVisitor::handshakeResponse(HandshakeResponseMessage const & msg_in)
{
    myMessage = "HANDSHRESP " + msg_in.getMyVersion() + " " + msg_in.getMyAddr() + " " + msg_in.getYourAddr();
}

void SerializerMessageVisitor::ping(PingMessage const & msg_in)
{
    myMessage = "PING " + msg_in.getText();
}

void SerializerMessageVisitor::pingResponse(PingResponseMessage const & msg_in)
{
    myMessage = "PINGRESP " + msg_in.getText();
}

void SerializerMessageVisitor::otherPeer(OtherPeerMessage const & msg_in)
{
    myMessage = "OPEER " + msg_in.getHost() + " " + to_string(msg_in.getPort());
}

BaseMessage* MessageDeserializer::parseMessage(std::vector<std::string> const & tokens)
{
    if (tokens.size() == 0)
    {
        return nullptr;
    }
    if (tokens[0] == "HANDSH" && tokens.size() >= 4)
    {
        return new HandshakeMessage(tokens[1], tokens[2], tokens[3]);
    }
    else if (tokens[0] == "HANDSHRESP" && tokens.size() >= 4)
    {
        return new HandshakeResponseMessage(tokens[1], tokens[2], tokens[3]);
    }
    else if (tokens[0] == "PING" && tokens.size() >= 2)
    {
        return new PingMessage(tokens[1]);
    }
    else if (tokens[0] == "PINGRESP" && tokens.size() >= 2)
    {
        return new PingResponseMessage(tokens[1]);
    }
    else if (tokens[0] == "OPEER" && tokens.size() >= 3)
    {
        return new OtherPeerMessage(tokens[1], stoi(tokens[2]));
    }
    return nullptr;
}
