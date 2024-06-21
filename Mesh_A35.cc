/*-*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*-*/

// Add Required Header Files
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

// Add Required Namespace
using namespace ns3;

// Define log component where log msgs will be saved
// Specify Log Components to Store log Messages
NS_LOG_COMPONENT_DEFINE("MeshExample35");

// Main function
int main(int argc, char *argv[]){
    // Set up Some default values for the Config
    Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(137));
    Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("14kb/s"));
    
    // Specify the Number of Nodes in the Mesh
    uint32_t nNodes = 8;
    
    // Read and Process the Command Line Arguments
    CommandLine cmd(__FILE__);
    
    // Process the command line arguments
    cmd.Parse(argc, argv);
    
    // Set up Point-to-Point helper
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // Set Data Rate
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms")); // Set Delay Attribute
    
    // Create nodes container
    NodeContainer nodes;
    nodes.Create(nNodes);
    
    // Create mesh topology using point-to-point helper
    NetDeviceContainer devices;
    for (uint32_t i = 0; i < nNodes; ++i) {
        for (uint32_t j = i + 1; j < nNodes; ++j) {
            devices.Add(pointToPoint.Install(nodes.Get(i), nodes.Get(j)));
        }
    }
    
    // Install Internet stack on nodes
    InternetStackHelper stack;
    stack.Install(nodes);
    
    // Assign IP addresses to the nodes
    Ipv4AddressHelper address;
    address.SetBase("20.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    
    // Set up On/Off applications
    OnOffHelper onOffHelper("ns3::TcpSocketFactory", Address());
    onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    
    ApplicationContainer apps;
    for (uint32_t i = 0; i < nNodes; ++i) {
        for (uint32_t j = i + 1; j < nNodes; ++j) {
            AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress(j), 30));
            onOffHelper.SetAttribute("Remote", remoteAddress);
            apps.Add(onOffHelper.Install(nodes.Get(i)));
        }
    }
    
    // Start and stop the applications
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));
    
    // Enable routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    // Enable Packet Capture
    pointToPoint.EnablePcapAll("mesh");
    
    // Create animation
    AnimationInterface anim("mesh_simulation_A35.xml");
    anim.SetMobilityPollInterval(Seconds(1.0));
    anim.EnablePacketMetadata(); // Enable tracing of packet transmissions
    
    // Run simulation
    Simulator::Run();
    
    // Clean up
    Simulator::Destroy();
    
    // Done
    NS_LOG_INFO("Simulation completed.");
    
    return 0;
}
