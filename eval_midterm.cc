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
  cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon,SplitHorizon, PoisonReverse)", SplitHorizon);
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
  if (SplitHorizon == "NoSplitHorizon")
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

  
   
    //cmd.Parse(argc, argv);
    std::string animFile = "evaluation_pr.xml";

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(11);

    NodeContainer c13 = NodeContainer(nodes.Get(0), nodes.Get(2));
    NodeContainer c12 = NodeContainer(nodes.Get(0), nodes.Get(1));
    NodeContainer c26 = NodeContainer(nodes.Get(1), nodes.Get(5));
    NodeContainer csma1 = NodeContainer(nodes.Get(1), nodes.Get(3), nodes.Get(4));
    NodeContainer c57 = NodeContainer(nodes.Get(4), nodes.Get(6));
    NodeContainer c78 = NodeContainer(nodes.Get(6), nodes.Get(7));
    NodeContainer c79 = NodeContainer(nodes.Get(6), nodes.Get(8));
    NodeContainer c8_10 = NodeContainer(nodes.Get(7), nodes.Get(9));
    NodeContainer c9_10 = NodeContainer(nodes.Get(8), nodes.Get(9));
    NodeContainer csma2 = NodeContainer(nodes.Get(9), nodes.Get(10));

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("3Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("5ms"));

    NetDeviceContainer d13 = p2p.Install(c13);
    NetDeviceContainer d26 = p2p.Install(c26);

    PointToPointHelper p2pn;
    p2pn.SetDeviceAttribute("DataRate", StringValue("6Mbps"));
    p2pn.SetChannelAttribute("Delay", StringValue("10ms"));

    NetDeviceContainer d12 = p2pn.Install(c12);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(MilliSeconds(10)));

    NetDeviceContainer dc1 = csma.Install(csma1);

    p2pn.SetDeviceAttribute("DataRate", StringValue("9Mbps"));
    p2pn.SetChannelAttribute("Delay", StringValue("15ms"));

    NetDeviceContainer d57 = p2pn.Install(c57);

    p2p.SetDeviceAttribute("DataRate", StringValue("9Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("15ms"));
    p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));

    NetDeviceContainer d78 = p2p.Install(c78);
    NetDeviceContainer d79 = p2p.Install(c79);
    NetDeviceContainer d8_10 = p2p.Install(c8_10);
    NetDeviceContainer d9_10 = p2p.Install(c9_10);

    CsmaHelper csma22;
    csma22.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma22.SetChannelAttribute("Delay", TimeValue(MilliSeconds(100)));

    NetDeviceContainer dc2 = csma22.Install(csma2);


    InternetStackHelper stack;
    stack.Install(nodes);

    TrafficControlHelper tch;
    tch.SetRootQueueDisc ("ns3::RedQueueDisc", "MaxSize", StringValue ("25p"));
    tch.Install(d13);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer if13;
    if13 = address.Assign(d13);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer if12;
    if12 = address.Assign(d12);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ifc1;
    ifc1 = address.Assign(dc1);

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer if57;
    if57 = address.Assign(d57);

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer if78;
    if78 = address.Assign(d78);

    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer if8_10;
    if8_10 = address.Assign(d8_10);

    address.SetBase("10.1.7.0", "255.255.255.0");
    Ipv4InterfaceContainer ifc2;
    ifc2 = address.Assign(dc2);

    address.SetBase("10.1.8.0", "255.255.255.0");
    Ipv4InterfaceContainer if26;
    if26 = address.Assign(d26);

    address.SetBase("10.1.9.0", "255.255.255.0");
    Ipv4InterfaceContainer if79;
    if79 = address.Assign(d79);

    address.SetBase("10.1.10.0", "255.255.255.0");
    Ipv4InterfaceContainer if9_10;
    //if9_10 = address.Assign(d9_10);



    //Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>(&std::cout);

    // Ipv4GlobalRoutingHelper::PrintRoutingTableEvery(Seconds(3.0), nodes.Get(6), routingStream);
    //Ipv4GlobalRoutingHelper::PrintRoutingTableEvery(Seconds(3.0), nodes.Get(7), routingStream);

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(c13.Get(1));
    serverApps.Start(Seconds(2.0));
    serverApps.Stop(Seconds(12.0));

    UdpEchoClientHelper echoClient(if13.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(25));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(2.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(c8_10.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(12.0));
    

    uint16_t port = 9; 
    BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (if13.GetAddress (1), port));
    ApplicationContainer sourceApps = source.Install (nodes.Get (5));
    sourceApps.Start (Seconds (5.0));
    sourceApps.Stop (Seconds (10.0));
    
    PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sink.Install (nodes.Get (2));
    sinkApps.Start (Seconds (5.0));
    sinkApps.Stop (Seconds (11.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    //Simulator::Schedule(Seconds(8), &TearDownLink, c8_10.Get(0), c8_10.Get(1), 2, 1);

    AsciiTraceHelper ascii;
    //p2pn.EnableAscii(ascii.CreateFileStream("evaluation_pr.tr"), d12.Get(1));
    p2pn.EnableAscii("evaluation_pr1.tr", d12);
   // csma22.EnableAsciiAll(ascii.CreateFileStream("eval_full.tr"));
    //p2pn.EnableAsciiAll(ascii.CreateFileStream("evaluation_full.tr"))  ;

    AnimationInterface anim(animFile);
    Simulator::Stop(Seconds(14));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
