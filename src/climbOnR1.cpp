#include <behaviortree_cpp/behavior_tree.h>
#include <iostream>

using namespace BT;

class ClimbOnR1 : public BT::SyncActionNode
{
public:
    ClimbOnR1(const std::string& name, const BT::NodeConfiguration& config)
        : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus tick() override {
        std::cout << "[ClimbOnR1] 开始攀爬 R1..." << std::endl;
        // TODO: 添加爬升逻辑
        return BT::NodeStatus::SUCCESS;
    }
}