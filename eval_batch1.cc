#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/netanim-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/csma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/red-queue-disc.h"
#include "ns3/traffic-control-module.h"
#include "ns3/packet-sink.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("eval");

/*void TearDownLink(Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
    nodeA->GetObject<Ipv4>()->SetDown(interfaceA);
    nodeB->GetObject<Ipv4>()->SetDown(interfaceB);
}
*/


int main(int argc, char *argv[])
{
  bool verbose = false;
  bool printRoutingTables = false;
  bool showPings = false;
  std::string SplitHorizon ("PoisonReverse");
  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds",printRoutingTables);
  cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
 // cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon,SplitHorizon, PoisonReverse)", SplitHorizon);
  cmd.Parse (argc, argv);
if (verbose)
{
  LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE));
  LogComponentEnable ("RipSimpleRouting", LOG_LEVEL_INFO);
  LogComponentEnable ("Rip", LOG_LEVEL_ALL);
  LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
  LogComponentEnable ("Icmpv4L4Protocol", LOG_LEVEL_ALL);
  LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
  LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
  LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);
}
 /* if (SplitHorizon == "NoSplitHorizon")
  {
  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));
  }
  else if (SplitHorizon == "SplitHorizon")
  {
  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));
  }
  else
  {
  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));
  }
*/
  
   
    //cmd.Parse(argc, argv);
    

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(10);

    NodeContainer c23= NodeContainer(nodes.Get(1), nodes.Get(2));
    NodeContainer c12 = NodeContainer(nodes.Get(0), nodes.Get(1));
    NodeContainer csma1 = NodeContainer(nodes.Get(1), nodes.Get(3), nodes.Get(4),nodes.Get(5),nodes.Get(6));
    csma1.Add(nodes.Get(7));
    NodeContainer c89 = NodeContainer(nodes.Get(7), nodes.Get(8));
    NodeContainer c9_10 = NodeContainer(nodes.Get(8), nodes.Get(9));
    

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("6Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("10ms"));

    NetDeviceContainer d12 = p2p.Install(c12);
    

    PointToPointHelper p2pn;
    p2pn.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
    p2pn.SetChannelAttribute("Delay", StringValue("5ms"));
p2pn.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));

    NetDeviceContainer d23 = p2pn.Install(c23);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(10)));

    NetDeviceContainer dc1 = csma.Install(csma1);

    p2pn.SetDeviceAttribute("DataRate", StringValue("9Mbps"));
    p2pn.SetChannelAttribute("Delay", StringValue("15ms"));

    NetDeviceContainer d89 = p2pn.Install(c89);
    NetDeviceContainer d9_10 = p2pn.Install(c9_10);

    InternetStackHelper stack;
    stack.Install(nodes);

    /*TrafficControlHelper tch;
    tch.SetRootQueueDisc ("ns3::RedQueueDisc", "MaxSize", StringValue ("25p"));
    tch.Install(d13);
*/

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer if12;
    if12 = address.Assign(d12);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer if23;
    if23 = address.Assign(d23);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ifc1;
    ifc1 = address.Assign(dc1);

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer if89;
    if89 = address.Assign(d89);

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer if9_10;
    if9_10 = address.Assign(d9_10);


    //Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);

    // Ipv4GlobalRoutingHelper::PrintRoutingTableEvery(Seconds(3.0), nodes.Get(6), routingStream);
    //Ipv4GlobalRoutingHelper::PrintRoutingTableEvery(Seconds(3.0), nodes.Get(7), routingStream);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(c9_10.Get(1));
    serverApps.Start(Seconds(4.0));
    serverApps.Stop(Seconds(13.0));

    UdpEchoClientHelper echoClient(if9_10.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(25));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(c23.Get(1));
    clientApps.Start(Seconds(4.0));
    clientApps.Stop(Seconds(13.0));
    
    
    uint16_t port = 9; 
    BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (if12.GetAddress (0), port));
    ApplicationContainer sourceApps = source.Install (nodes.Get (0));
    sourceApps.Start (Seconds (2.0));
    sourceApps.Stop (Seconds (12.0));
    
    PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sink.Install (nodes.Get (8));
    sinkApps.Start (Seconds (2.0));
    sinkApps.Stop (Seconds (11.0));
    

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    //Simulator::Schedule(Seconds(8), &TearDownLink, c8_10.Get(0), c8_10.Get(1), 2, 1);

    AsciiTraceHelper ascii;
    p2p.EnableAscii("eval_batch1_node1.tr", d12);
     p2pn.EnableAscii("eval_batch1_node3.tr", d23);
     csma.EnableAsciiAll(ascii.CreateFileStream("eval_csma.tr"));
     
    //p2pn.EnableAscii("evaluation_pr1.tr", d12);
   // csma22.EnableAsciiAll(ascii.CreateFileStream("eval_full.tr"));
    //p2pn.EnableAsciiAll(ascii.CreateFileStream("evaluation_full.tr"))  ;
    
   // AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("eval_batch1_full.tr");
  p2p.EnableAsciiAll (stream);
  csma.EnableAsciiAll (stream);
  p2pn.EnableAsciiAll (stream);
 

    std::string animFile = "batch1eval.xml";
    AnimationInterface anim(animFile);
    Simulator::Stop(Seconds(14));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
