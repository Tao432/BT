#include <behaviortree_cpp/behavior_tree.h>
#include <iostream>

using namespace BT;

// 决策节点 
class Dicision : public BT::SyncActionNode
{
public:
    Dicision(const std::string& name, const BT::NodeConfiguration& config)
        : BT::SyncActionNode(name, config) {}

    static BT::PortsList providedPorts() {
        // 定义一个输出端口，对应 XML 中的变量
        return { BT::OutputPort<std::string>("kfs_location") };
    }

    BT::NodeStatus tick() override {
        std::cout << "[Dicision] 执行决策逻辑..." << std::endl;
        
        // TODO: 实际决策逻辑
        
        // 将结果写入黑板
        setOutput("kfs_location", kfs_location);
        
        return BT::NodeStatus::SUCCESS;
    }
}