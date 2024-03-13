/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// add required header files

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"

// Adding namespace declaration
using namespace ns3;

// Define log component where log msgs will be saved
NS_LOG_COMPONENT_DEFINE("BusTopology");

// Main function
int main(int argc, char *argv[]){
	uint32_t nCsma = 3; // declare number of nodes in bus Topology
	
	// read the command line arguments
	CommandLine cmd(__FILE__);
	
	// Process the command line arguments
	cmd.Parse(argc, argv);
	
	// Set time Resolution to 1 nano second
	Time::SetResolution(Time::NS);
	
	// Logging
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
	
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
	
	// p2p topology
	
	// create topology for the p2p Nodes
	NodeContainer p2pNodes;
	p2pNodes.Create(2);
	
	// create net devices poinToPoint devices
	PointToPointHelper pointToPoint;
	
	// Configure the net Device
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // Set Data Rate
	
	// Configure the Channel
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); // Set Delay Attribute
	
	// Install p2p net Devices on P2p nodes connect with channels
	NetDeviceContainer p2pDevices;
	p2pDevices = pointToPoint.Install(p2pNodes);
	
	// Create Bus Topology Nodes
	NodeContainer csmaNodes;
	csmaNodes.Add(p2pNodes.Get(1)); // Adding node n1 to topology
	csmaNodes.Create(nCsma); // creating extra nCsma nodes
	
	// Configure Csma Net Devices and Channels
	CsmaHelper csma;
	
	// Set Channel Attribute
	csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
	csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds(6560)));
	
	// install csma net devices on csma nodes and connect them with channel
	NetDeviceContainer csmaDevices;
	csmaDevices = csma.Install(csmaNodes);
	
	// Configure the protocol stacks
	InternetStackHelper stack;
	
	// install stack on p2pNodes
	stack.Install(p2pNodes.Get(0));
	
	// install stack on csmaNodes
	stack.Install(csmaNodes);
	
	// Assign IP addresses or Configure IP addresses
	Ipv4AddressHelper address;
	
	// Configure Network Ip Address for p2p Topology
	address.SetBase("10.0.0.0","255.0.0.0");
	
	// Assign Ip address to interfaces p2p net devices
	Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
	
	// Configure network Ip address for bus topology
	address.SetBase("20.0.0.0","255.0.0.0");
	
	// Assign IP addresses to the interfaces of netDevices
	Ipv4InterfaceContainer csmaInterfaces = address.Assign(csmaDevices);
	
	// configure server Application
	UdpEchoServerHelper echoServer(9);// Setting port number of server application
	
	// install server application on last node
	// Application Container create object to store server application and install on node(3)
	ApplicationContainer serverApp = echoServer.Install(csmaNodes.Get(3)); // indexed 4 server
	
	// Configure start and stop of server application
	serverApp.Start(Seconds(1.0)); // server app should start first
	serverApp.Stop(Seconds(10.0)); // server app should stop 
	
	// configure client application
	// Configure UdpEchoClientApplication
	UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(3),9);
	
	// Configure the attribute of client Application
	echoClient.SetAttribute("MaxPackets", UintegerValue (3));
	echoClient.SetAttribute("Interval", TimeValue (Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue (1024));
	
	// Install Client Application on Node 0
	ApplicationContainer clientApp = echoClient.Install(p2pNodes.Get(0));
	
	// Configure Start and Stop Time
	clientApp.Start(Seconds(2.0));
	clientApp.Stop(Seconds(10.0));
	
	
	// for Running the code
	//AnimationInterface anim("busAnimation.xml");
	//anim.SetConstantPosition(nodes.Get(0),20.0,30.0);
	//anim.SetConstantPosition(nodes.Get(1),40.0,30.0);
	
	// Enables Routing between two networks 10.0.0.0 and 20.0.0.0
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	// Captures the Packets
	pointToPoint.EnablePcapAll("p2p_Packet");
	csma.EnablePcap("bus_packet", csmaDevices.Get(1), true);
	
	// for Running the code
	AnimationInterface anim("busAnimation.xml");
	anim.SetConstantPosition(p2pNodes.Get(0),20.0,30.0);
	anim.SetConstantPosition(p2pNodes.Get(1),40.0,30.0);
	anim.SetConstantPosition(csmaNodes.Get(1),50.0,30.0);
	anim.SetConstantPosition(csmaNodes.Get(2),60.0,30.0);
	anim.SetConstantPosition(csmaNodes.Get(3),70.0,30.0);
	
	// Simulation on Run and start
	Simulator::Run();
	
	// Destory this Resourses
	Simulator::Destroy();

	return 0;
}


