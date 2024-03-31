/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// Add Required Header Files

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"

// Add Required Namespace
using namespace ns3;

// Define log component where log msgs will be saved
// Specify Log Components to Store log Messages
NS_LOG_COMPONENT_DEFINE("StarExample");

// Main function
int main(int argc, char *argv[]){
	// On the Hub Node We Install Packet Sync Application and Packets Contain On-Off Application
	// Set up Some default values for the Config
	// Setting the Default Values of On/Off Application when App is On State
	Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(137));
	Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("14kb/s"));
	
	// Specify the Number of Spoke Nodes
	uint32_t nSpokes = 8;
	
	// Read and Process the Command Line Arguments
	CommandLine cmd(__FILE__);
	
	// Process the command line arguments
	cmd.Parse(argc, argv);
	
	// Configure p2p Net Devices And Channels to be install on the Spoke Devices
	PointToPointHelper pointToPoint;
	
	// Configure the net Device
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // Set Data Rate
	
	// Configure the Channel
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); // Set Delay Attribute
	
	// Star Helper Class
	PointToPointStarHelper star (nSpokes, pointToPoint);
	
	// Message we want to Display
	NS_LOG_INFO("Star Topology Created"); // Display Informational Message
	
	// Install Protocol Stack on Nodes
	InternetStackHelper stack;
	star.InstallStack(stack); // install stack on all nodes in star topology
	
	NS_LOG_INFO("Installed Protocol stack on all nodes in Topology");
	
	// Assign IP Addresses to interfaces of Spoke nodes and hub
	star.AssignIpv4Addresses(Ipv4AddressHelper("10.0.0.0", "255.0.0.0"));
	
	NS_LOG_INFO("Ipv4 Addresses are Assign to spoke nodes and hub interfaces");
	NS_LOG_INFO("IP Addresses assigned to Interfaces of Hub are : ");
	
	for(uint32_t h_interfaces = 0; h_interfaces < star.SpokeCount(); ++h_interfaces){
		NS_LOG_INFO("Address of Hub Interfaces: " << star.GetHubIpv4Address(h_interfaces));
	}
	
	NS_LOG_INFO("IP Addresses of Spoke Nodes are : ");
	
	for(uint32_t ip_interfaces = 0; ip_interfaces < star.SpokeCount(); ++ip_interfaces){
		NS_LOG_INFO("Address of Spoke Node : " << ip_interfaces);
		NS_LOG_INFO("Address of Spoke: " << star.GetSpokeIpv4Address(ip_interfaces));
	}
	
	// configuring Packet Sink Application on HUB
	uint16_t port = 50000; // Specifying port No. of Hub
	
	// Configure the Socket Address of HUB
	Address hubLocalAddress (InetSocketAddress (Ipv4Address::GetAny(), port));
	
	PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", hubLocalAddress);
	
	// Application Container create object to store server application and install on node(1)
	ApplicationContainer hubApp = packetSinkHelper.Install(star.GetHub());
	
	// Configure the Start & Stop Time of Packet Sink Application
	hubApp.Start(Seconds(1.0));
	hubApp.Stop(Seconds(10.0));
	
	// Configure On/Off Application on the Spoke Nodes
	OnOffHelper onOffHelper("ns3::TcpSocketFactory", Address());
	
	// Configure the ON And OFF
	onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
	
	onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
	
	// install on off application of all spoke nodes
	ApplicationContainer spokeApps;
	
	for(uint32_t i = 0; i < star.SpokeCount(); ++i){
		AddressValue remoteAddress(InetSocketAddress(star.GetHubIpv4Address (i), port));
		onOffHelper.SetAttribute("Remote", remoteAddress);
		spokeApps.Add(onOffHelper.Install(star.GetSpokeNode(i)));
	}
	
	// Configure Start & Stop time of ON OFF Application
	spokeApps.Start(Seconds(1.0));
	spokeApps.Stop(Seconds(10.0));
	
	// Enable Routing Using Global IP V4 Class Method
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	// Enable Packets Capture on All Nodes
	pointToPoint.EnablePcapAll("star");

	// Animate the Star Topology 	
	AnimationInterface anim("hub_star_A35.xml");
	star.BoundingBox(1, 1, 100, 100);
	
	// Simulation on Run and start
	Simulator::Run();
	
	// Destory this Resourses
	Simulator::Destroy();
	
	
	// For the Exporting CMD >> '$ export NS_LOG=Star=info '
	NS_LOG_INFO("Done");

	return 0;
		
}



