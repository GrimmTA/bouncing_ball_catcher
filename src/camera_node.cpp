#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "cv_bridge/cv_bridge.h"
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <string>

class CameraNode : public rclcpp::Node
{
public:
  CameraNode() : Node("camera_node")
  {
    // 1. Deklarasi parameter + default
    this->declare_parameter<std::string>("source", "0");
    this->declare_parameter<double>("fps", 30.0);
    this->declare_parameter<int>("width", 640);
    this->declare_parameter<int>("height", 480);

    // 2. Baca nilainya
    std::string source = this->get_parameter("source").as_string();
    double fps = this->get_parameter("fps").as_double();
    width_ = this->get_parameter("width").as_int();
    height_ = this->get_parameter("height").as_int();

    // 3. Buka webcam (kalau source angka) atau file video
    bool is_number = !source.empty() &&
      std::all_of(source.begin(), source.end(), ::isdigit);
    if (is_number) {
      cap_.open(std::stoi(source));
    } else {
      cap_.open(source);
    }
    if (!cap_.isOpened()) {
      RCLCPP_ERROR(this->get_logger(), "Gagal membuka sumber: %s", source.c_str());
    }

    publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/image_raw", 10);

    // 4. Interval timer dari fps
    auto period = std::chrono::milliseconds(static_cast<int>(1000.0 / fps));
    timer_ = this->create_wall_timer(
      period, std::bind(&CameraNode::timer_callback, this));
  }

private:
  void timer_callback()
  {
    cv::Mat frame;
    if (!cap_.read(frame)) {
      cap_.set(cv::CAP_PROP_POS_FRAMES, 0);  // video habis: ulang dari awal
      return;
    }

    cv::resize(frame, frame, cv::Size(width_, height_));

    std_msgs::msg::Header header;
    header.stamp = this->now();
    header.frame_id = "camera";

    auto msg = cv_bridge::CvImage(header, "bgr8", frame).toImageMsg();
    publisher_->publish(*msg);
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
  cv::VideoCapture cap_;
  int width_;
  int height_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CameraNode>());
  rclcpp::shutdown();
  return 0;
}