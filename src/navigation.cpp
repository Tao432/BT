#include <behaviortree_cpp/behavior_tree.h>
#include <iostream>

using namespace BT;

class Nevigation : public BT::SyncActionNode
{
public:
    Nevigation(const std::string& name, const BT::NodeConfiguration& config)
        : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus tick() override {
        std::cout << "[Nevigation] 正在导航到九宫格..." << std::endl;
        // TODO: 添加导航代码
        return BT::NodeStatus::SUCCESS;
    }
}