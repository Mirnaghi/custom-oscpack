/* 
    Example of two different ways to process received OSC messages using oscpack.
    Receives the messages from the SimpleSend.cpp example.
*/

#include <iostream>
#include <cstring>

#if defined(__BORLANDC__) // workaround for BCB4 release build intrinsics bug
namespace std {
using ::__strcmp__;  // avoid error: E2316 '__strcmp__' is not a member of 'std'.
}
#endif

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"


#define PORT 5000

class ExamplePacketListener : public osc::OscPacketListener {
protected:

    virtual void ProcessMessage( const osc::ReceivedMessage& m, 
				const IpEndpointName& remoteEndpoint )
    {
        (void) remoteEndpoint; // suppress unused parameter warning

        try{
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.
            
            if( std::strcmp( m.AddressPattern(), "/dmx/set/ch/1/9" ) == 0 ){
                // example #1 -- argument stream interface
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                //bool a1;
                //osc::int32 a2;
                float a3;
                //const char *a4;
                args >> a3 >> osc::EndMessage;
                
                std::cout << "received" << m.AddressPattern() << " message with arguments: "
                    << a3 << "\n";
                
            }else if( std::strcmp( m.AddressPattern(), "/dmx/set/ch/1/9" ) != 0 ){
                // example #2 -- argument iterator interface, supports
                // reflection for overloaded messages (eg you can call 
                // (*arg)->IsBool() to check if a bool was passed etc).
                osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
                //bool a1 = (arg++)->AsBool();
                //int a2 = (arg++)->AsInt32();
                float a3 = (arg++)->AsFloat();
                //const char *a4 = (arg++)->AsString();
                if( arg != m.ArgumentsEnd() )
                    throw osc::ExcessArgumentException();
                
                std::cout << "received " << m.AddressPattern() << " message with arguments: "
                    << a3 << "\n";
            }
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }
};

int main(int argc, char* argv[])
{
    (void) argc; // suppress unused parameter warnings
    (void) argv; // suppress unused parameter warnings
    // IpEndpointName::ANY_ADDRESS, 
    ExamplePacketListener listener;
    UdpListeningReceiveSocket s(
            IpEndpointName( "172.26.1.121", PORT ),
            &listener );

    std::cout << "press ctrl-c to end\n";

    s.RunUntilSigInt();

    return 0;
}

