#include <behaviortree_cpp/behavior_tree.h>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

using namespace BT;

class Nevigation : public BT::StatefulActionNode
{
private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr goal_pub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    
    // 目标坐标与当前坐标
    double target_x_ = 0.0, target_y_ = 0.0, target_z_ = 0.0;
    double current_x_ = 0.0, current_y_ = 0.0;
    bool is_goal_reached_ = false;

    // 里程计回调：通过 /drone_0_odom 监控进度
    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;
        
        // 计算 XY 平面欧氏距离
        double dist = std::hypot(target_x_ - current_x_, target_y_ - current_y_);
        if (dist < 0.25) { // 阈值可根据 ROBOCON 赛道精度调整
            is_goal_reached_ = true;
        }
    }

public:
    Nevigation(const std::string& name, const BT::NodeConfiguration& config, rclcpp::Node::SharedPtr node)
        : BT::StatefulActionNode(name, config), node_(node)
    {
        // 精确匹配脚本中的话题名
        goal_pub_ = node_->create_publisher<geometry_msgs::msg::PoseStamped>("/move_base_simple/goal", 10);
        
        // 订阅 README 约定的标准输出话题
        odom_sub_ = node_->create_subscription<nav_msgs::msg::Odometry>(
            "/drone_0_odom", 10, std::bind(&Nevigation::odomCallback, this, std::placeholders::_1));
    }

    static BT::PortsList providedPorts() {
        return { 
            BT::InputPort<double>("target_x"), 
            BT::InputPort<double>("target_y"), 
            BT::InputPort<double>("target_z") 
        };
    }

    BT::NodeStatus onStart() override {
        std::cout << "\033[32m[Executing]\033[0m -> " << this->name() << std::endl;
        if (!getInput("target_x", target_x_) || !getInput("target_y", target_y_) || !getInput("target_z", target_z_)) {
            return BT::NodeStatus::FAILURE;
        }

        is_goal_reached_ = false;

        // 构造并发布消息，完全模拟 send_goal_once.sh 的行为
        auto msg = geometry_msgs::msg::PoseStamped();
        msg.header.stamp = node_->now();
        msg.header.frame_id = "world"; //
        msg.pose.position.x = target_x_;
        msg.pose.position.y = target_y_;
        msg.pose.position.z = target_z_;
        msg.pose.orientation.w = 1.0; //

        goal_pub_->publish(msg);
        RCLCPP_INFO(node_->get_logger(), "已下发导航目标至 Area 3");
        return BT::NodeStatus::RUNNING;
    }

    BT::NodeStatus onRunning() override {
        // 持续检查里程计回调更新的状态
        if (is_goal_reached_) {
            return BT::NodeStatus::SUCCESS;
        }
        return BT::NodeStatus::RUNNING;
    }

    void onHalted() override {
        // 如果行为树切换了状态（如紧急停机），可以在此发布当前位置为目标来停止 Ego-Planner
    }
};