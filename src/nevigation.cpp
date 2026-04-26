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
    
    double target_x_ = 0.0, target_y_ = 0.0, target_z_ = 0.0;
    double current_x_ = 0.0, current_y_ = 0.0;
    bool has_odom_ = false;

    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;
        has_odom_ = true;
    }

public:
    Nevigation(const std::string& name, const BT::NodeConfiguration& config, rclcpp::Node::SharedPtr node)
        : BT::StatefulActionNode(name, config), node_(node)
    {
        goal_pub_ = node_->create_publisher<geometry_msgs::msg::PoseStamped>("/move_base_simple/goal", 10);
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
        
        // 尝试获取端口数据，如果 XML 没填则直接失败
        if (!getInput("target_x", target_x_) || !getInput("target_y", target_y_) || !getInput("target_z", target_z_)) {
            RCLCPP_ERROR(node_->get_logger(), "Nevigation 端口输入缺失！");
            return BT::NodeStatus::FAILURE;
        }

        auto msg = geometry_msgs::msg::PoseStamped();
        //msg.header.stamp = node_->now();
        msg.header.stamp = rclcpp::Time(0, 0, node_->get_clock()->get_clock_type());
        msg.header.frame_id = "world";
        msg.pose.position.x = target_x_;
        msg.pose.position.y = target_y_;
        msg.pose.position.z = target_z_;
        msg.pose.orientation.w = 1.0;

        goal_pub_->publish(msg);
        RCLCPP_INFO(node_->get_logger(), "已向 Area 3 下发目标: [%.2f, %.2f]", target_x_, target_y_);
        
        return BT::NodeStatus::RUNNING;
    }

    BT::NodeStatus onRunning() override {
        if (!has_odom_) {
            RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), 2000, "等待里程计...");
            return BT::NodeStatus::RUNNING;
        }

        // 计算当前位置与目标的距离 (XY 平面)
        double dist = std::hypot(target_x_ - current_x_, target_y_ - current_y_);
        std::cout<<dist<<std::endl;

        if (dist < 0.3) {
            RCLCPP_INFO(node_->get_logger(), "[%s] 成功到达目标点附近 (距离: %.2f)", name().c_str(), dist);
            return BT::NodeStatus::SUCCESS;
        }

        return BT::NodeStatus::RUNNING;
    }

    void onHalted() override {
        RCLCPP_WARN(node_->get_logger(), "[%s] 导航节点被外部终止", name().c_str());
    }
};