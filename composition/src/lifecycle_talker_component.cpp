// Copyright 2022 Christoph Hellmann Santos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "composition/lifecycle_talker_component.hpp"

using namespace std::chrono_literals;

namespace composition
{
void LifecycleTalker::publish()
{
  static size_t count = 0;
  auto msg = std::make_unique<std_msgs::msg::String>();
  msg->data = "Lifecycle HelloWorld #" + std::to_string(++count);
  if (!pub_->is_activated()) {
    RCLCPP_INFO(
      get_logger(), "Lifecycle publisher is currently inactive. Messages are not published.");
  } else {
    RCLCPP_INFO(
      get_logger(), "Lifecycle publisher is active. Publishing: [%s]", msg->data.c_str());
  }
  pub_->publish(std::move(msg));
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
LifecycleTalker::on_configure(const rclcpp_lifecycle::State &)
{
  pub_ = this->create_publisher<std_msgs::msg::String>("lifecycle_chatter", 10);
  timer_ = this->create_wall_timer(
    1s, std::bind(&LifecycleTalker::publish, this));

  RCLCPP_INFO(get_logger(), "on_configure() is called.");
  return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
LifecycleTalker::on_activate(const rclcpp_lifecycle::State &)
{
  pub_->on_activate();
  RCUTILS_LOG_INFO_NAMED(get_name(), "on_activate() is called.");
  std::this_thread::sleep_for(2s);
  return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
LifecycleTalker::on_deactivate(const rclcpp_lifecycle::State &)
{
  pub_->on_deactivate();
  RCUTILS_LOG_INFO_NAMED(get_name(), "on_deactivate() is called.");
  return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
LifecycleTalker::on_cleanup(const rclcpp_lifecycle::State &)
{
  timer_.reset();
  pub_.reset();

  RCUTILS_LOG_INFO_NAMED(get_name(), "on cleanup is called.");
  return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}

rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
LifecycleTalker::on_shutdown(const rclcpp_lifecycle::State & state)
{
  timer_.reset();
  pub_.reset();

  RCUTILS_LOG_INFO_NAMED(
    get_name(),
    "on shutdown is called from state %s.",
    state.label().c_str());

  return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
}



}  // namespace composition

#include "rclcpp_components/register_node_macro.hpp"

// Register the component with class_loader.
// This acts as a sort of entry point, allowing the component to be discoverable when its library
// is being loaded into a running process.
RCLCPP_COMPONENTS_REGISTER_NODE(composition::LifecycleTalker)
