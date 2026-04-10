#include <behaviortree_cpp/behavior_tree.h>
#include <iostream>

using namespace BT;

// 判断是否足够高的条件节点
class IsHighEnough : public BT::ConditionNode
{
public:
    IsHighEnough(const std::string& name, const BT::NodeConfiguration& config)
        : BT::ConditionNode(name, config) {}

    static BT::PortsList providedPorts() {
        return {}; // 如果需要从黑板读取高度阈值，可以在这里定义 InputPort
    }

    BT::NodeStatus tick() override {
        std::cout << "[IsHighEnough] 检查当前高度..." << std::endl;
        
        // TODO: 替换为实际的高度检测逻辑
        bool is_high_enough = false; 

        if (is_high_enough) {
            return BT::NodeStatus::SUCCESS;
        } else {
            return BT::NodeStatus::FAILURE; // 返回失败时，外部的 <reverse> 会将其翻转为 SUCCESS
        }
    }
};