#include <behaviortree_cpp/behavior_tree.h>
#include <iostream>

using namespace BT;

class PutKfs : public BT::SyncActionNode
{
public:
    PutKfs(const std::string& name, const BT::NodeConfiguration& config)
        : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts() { return {}; }

    BT::NodeStatus tick() override {
        std::cout << "[PutKfs] 放置 KFS..." << std::endl;
        // TODO: 添加放置逻辑
        return BT::NodeStatus::SUCCESS;
    }
};