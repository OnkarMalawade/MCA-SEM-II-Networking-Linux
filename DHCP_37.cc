/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// add required header files

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-global-routing-helper.h"

// Adding namespace declaration
using namespace ns3;

//Define log component where log msgs will be saved
NS_LOG_COMPONENT_DEFINE("DhcpExample");

// Main function
int main(int argc, char *argv[])
{
	// read the command line arguments
	CommandLine cmd(__FILE__);
	
	// Process the command line arguments
	cmd.Parse(argc, argv);
	
	// Set time Resolution to 1 nano second
	Time::SetResolution(Time::NS);
	
	// Logging
	LogComponentEnable("DhcpServer",LOG_LEVEL_ALL);
	LogComponentEnable("DhcpClient",LOG_LEVEL_ALL);
	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
	
	// Message to be Display
	NS_LOG_INFO("Creates Nodes. ");
	// Create NodeContainer object to store our nodes
	NodeContainer nodes; // Dhcp Nodes Client
	NodeContainer router; // Dhcp Nodes Router
	// Create 2 nodes 
	nodes.Create(3);
	router.Create(2);
	
	// Combine Router Object and Client Node Object in Single Object
	NodeContainer net(nodes,router);  
	
	// Configure Net Devices and Channels
	// Create Object Csma 
	CsmaHelper csma;
	
	csma.SetChannelAttribute("DataRate", StringValue("5Mbps"));
	csma.SetChannelAttribute("Delay", StringValue("2ms"));
	csma.SetDeviceAttribute("Mtu", UintegerValue(1500));
	
	// All Nodes Install Net Devices and channels Connect Them
	NetDeviceContainer devices = csma.Install(net);
	
	// Creating Point to point Topology
	NodeContainer p2pNodes;
	
	// Add router to R1 to p2p Topology
	p2pNodes.Add(net.Get(4));
	
	// Create new Node A
	p2pNodes.Create(1);
	
	// Configure p2p net devices and p2p channel
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
	
	// install p2p net devices on router R1 and node A and Connect with p2p channel
	NetDeviceContainer p2pDevices;
	p2pDevices = pointToPoint.Install(p2pNodes);
	
	// install protocol stack on all nodes devices
	InternetStackHelper stack;
	// Installed Dhcp on Nodes
	stack.Install(net);
	// Installed on Node A
	stack.Install(p2pNodes.Get(1));
	
	// assign IP address to point to point interfaces
	Ipv4AddressHelper address;
	address.SetBase("20.0.0.0", "255.0.0.0");
	Ipv4InterfaceContainer p2pInterfaces;
	p2pInterfaces = address.Assign(p2pDevices);

	// Configure DHCP Server
	DhcpHelper dhcpHelper;
	
	
	// Setting fixed IP address for default gateway r1
	Ipv4InterfaceContainer fixedNodes = dhcpHelper.InstallFixedAddress(devices.Get(4), Ipv4Address("10.0.0.17"), Ipv4Mask("/8"));
	
	// Enable IP Fowarding of Packets from R1
	fixedNodes.Get(0).first->SetAttribute("IpForward", BooleanValue(true));
	
	// enable routing between 2 networks
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	// Configure and install Dhcp server Router on R0
	ApplicationContainer dhcpServerApp = dhcpHelper.InstallDhcpServer(devices.Get(3), Ipv4Address("10.0.0.12"), Ipv4Address("10.0.0.0"), Ipv4Mask("/8"), Ipv4Address("10.0.0.10"), Ipv4Address("10.0.0.15"), Ipv4Address("10.0.0.17"));
	
	// Configure the Start and Stop time of the Server
	dhcpServerApp.Start(Seconds(0.0));
	dhcpServerApp.Stop(Seconds(20.0));
	
	// Configuring DHCP Client
	NetDeviceContainer dhcpClientNetDevs;
	
	//Combine net devices on dhcp client nodes in single object
	dhcpClientNetDevs.Add(devices.Get(0));
	dhcpClientNetDevs.Add(devices.Get(1));
	dhcpClientNetDevs.Add(devices.Get(2));
	
	// install dhcp client application on node n0, n1 and n2
	ApplicationContainer dhcpClients = dhcpHelper.InstallDhcpClient(dhcpClientNetDevs);
	
	// Configure start and stop time of DHCPClient
	dhcpClients.Start(Seconds(1.0));
	dhcpClients.Stop(Seconds(20.0));
	
	// Configure UdpEchoServer Application node A
	UdpEchoServerHelper echoServer(9); // Setting port number of server application
	
	// Application Container create object to store server application and install on node(1)
	ApplicationContainer serverApps = echoServer.Install(p2pNodes.Get(1)); // indexed 1 server
	
	// Configure start and stop time of server Application
	serverApps.Start(Seconds(0.0)); // server app should start first
	serverApps.Stop(Seconds(20.0)); // server app should stop 
	
	// Configure UdpEchoClient App on Node N1
	// Configure UdpEchoClientApplication
	UdpEchoClientHelper echoClient(p2pInterfaces.GetAddress(1),9);
	
	// Configure the attribute of client Application
	echoClient.SetAttribute("MaxPackets", UintegerValue (1));
	echoClient.SetAttribute("Interval", TimeValue (Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue (1024));
	
	// Install Client Application on Node 0
	// ApplicationContainer clientApp = echoClient.Install(nodes.Get(1));
	ApplicationContainer clientApp = echoClient.Install(net.Get(1));
	
	// Configure start and stop time
	clientApp.Start(Seconds(10.0));
	clientApp.Stop(Seconds(20.0));
	
	// Configure Stop time of Simulator
	Simulator::Stop(Seconds(30.0));
	
	// Simulation on Run and start
	Simulator::Run();
	
	// Destory this Resourses
	Simulator::Destroy();
	
	return 0;
	
}

