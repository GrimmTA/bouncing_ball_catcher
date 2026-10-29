// // // #include "rclcpp/rclcpp.hpp"
// // // #include "sensor_msgs/msg/image.hpp"

// // // class GameNode : public rclcpp::Node
// // // {
// // // public:
// // //   GameNode() : Node("game_node")
// // //   {
// // //     subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
// // //       "/camera/image_raw", 10,
// // //       std::bind(&GameNode::image_callback, this, std::placeholders::_1));
// // //   }

// // // private:
// // //   void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
// // //   {
// // //     RCLCPP_INFO_THROTTLE(
// // //       this->get_logger(), *this->get_clock(), 1000,
// // //       "Gambar masuk: %u x %u", msg->width, msg->height);
// // //   }

// // //   rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
// // // };

// // // int main(int argc, char ** argv)
// // // {
// // //   rclcpp::init(argc, argv);
// // //   rclcpp::spin(std::make_shared<GameNode>());
// // //   rclcpp::shutdown();
// // //   return 0;
// // // }
// // #include "rclcpp/rclcpp.hpp"
// // #include "sensor_msgs/msg/image.hpp"
// // #include "cv_bridge/cv_bridge.h"
// // #include <opencv2/opencv.hpp>
// // #include <vector>

// // class GameNode : public rclcpp::Node
// // {
// // public:
// //   GameNode() : Node("game_node")
// //   {
// //     this->declare_parameter<std::vector<int64_t>>("hsv_lower", {5, 120, 120});
// //     this->declare_parameter<std::vector<int64_t>>("hsv_upper", {20, 255, 255});
// //     this->declare_parameter<int>("min_area", 300);

// //     subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
// //       "/camera/image_raw", 10,
// //       std::bind(&GameNode::image_callback, this, std::placeholders::_1));
// //     debug_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/game/debug_image", 10);
// //   }

// // private:
// //   void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
// //   {
// //     auto lo = this->get_parameter("hsv_lower").as_integer_array();
// //     auto hi = this->get_parameter("hsv_upper").as_integer_array();
// //     int min_area = this->get_parameter("min_area").as_int();

// //     cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;

// //     cv::Mat hsv, mask;
// //     cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
// //     cv::inRange(hsv,
// //       cv::Scalar(lo[0], lo[1], lo[2]),
// //       cv::Scalar(hi[0], hi[1], hi[2]), mask);

// //     cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
// //     cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
// //     cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

// //     std::vector<std::vector<cv::Point>> contours;
// //     cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

// //     double best_area = 0;
// //     int best_idx = -1;
// //     for (size_t i = 0; i < contours.size(); ++i) {
// //       double a = cv::contourArea(contours[i]);
// //       if (a > best_area) { best_area = a; best_idx = static_cast<int>(i); }
// //     }

// //     if (best_idx >= 0 && best_area >= min_area) {
// //       cv::Rect box = cv::boundingRect(contours[best_idx]);
// //       cv::Moments m = cv::moments(contours[best_idx]);
// //       cv::Point c(static_cast<int>(m.m10 / m.m00), static_cast<int>(m.m01 / m.m00));
// //       cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
// //       cv::circle(frame, c, 5, cv::Scalar(0, 0, 255), -1);
// //     }

// //     auto out = cv_bridge::CvImage(msg->header, "bgr8", frame).toImageMsg();
// //     debug_pub_->publish(*out);
// //   }

// //   rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
// //   rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr debug_pub_;
// // };

// // int main(int argc, char ** argv)
// // {
// //   rclcpp::init(argc, argv);
// //   rclcpp::spin(std::make_shared<GameNode>());
// //   rclcpp::shutdown();
// //   return 0;
// // }
// #include "rclcpp/rclcpp.hpp"
// #include "sensor_msgs/msg/image.hpp"
// #include "cv_bridge/cv_bridge.h"
// #include <opencv2/opencv.hpp>
// #include <random>
// #include <vector>

// struct Ball
// {
//   cv::Point2f pos;
//   cv::Point2f vel;
//   int radius;
//   cv::Scalar color;
// };

// class GameNode : public rclcpp::Node
// {
// public:
//   GameNode() : Node("game_node"), rng_(std::random_device{}())
//   {
//     this->declare_parameter<std::vector<int64_t>>("hsv_lower", {5, 120, 120});
//     this->declare_parameter<std::vector<int64_t>>("hsv_upper", {20, 255, 255});
//     this->declare_parameter<int>("min_area", 300);
//     this->declare_parameter<int>("num_balls", 3);
//     this->declare_parameter<int>("ball_radius", 40);
//     this->declare_parameter<double>("ball_speed", 25.0);

//     subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
//       "/camera/image_raw", 10,
//       std::bind(&GameNode::image_callback, this, std::placeholders::_1));
//     debug_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/game/debug_image", 10);
//   }

// private:
//   void init_balls(int w, int h)
//   {
//     int n = this->get_parameter("num_balls").as_int();
//     int r = this->get_parameter("ball_radius").as_int();
//     double speed = this->get_parameter("ball_speed").as_double();

//     std::uniform_real_distribution<float> rx(r, w - r);
//     std::uniform_real_distribution<float> ry(r, h - r);
//     std::uniform_real_distribution<float> ang(0.0f, 6.2831853f);
//     std::uniform_int_distribution<int> col(80, 255);

//     balls_.clear();
//     for (int i = 0; i < n; ++i) {
//       float a = ang(rng_);
//       Ball b;
//       b.pos = cv::Point2f(rx(rng_), ry(rng_));
//       b.vel = cv::Point2f(std::cos(a) * speed, std::sin(a) * speed);
//       b.radius = r;
//       b.color = cv::Scalar(col(rng_), col(rng_), col(rng_));
//       balls_.push_back(b);
//     }
//   }

//   void update_balls(double dt, int w, int h)
//   {
//     for (auto & b : balls_) {
//       b.pos += b.vel * static_cast<float>(dt);
//       if (b.pos.x - b.radius < 0)     { b.pos.x = b.radius;     b.vel.x = -b.vel.x; }
//       if (b.pos.x + b.radius > w)     { b.pos.x = w - b.radius; b.vel.x = -b.vel.x; }
//       if (b.pos.y - b.radius < 0)     { b.pos.y = b.radius;     b.vel.y = -b.vel.y; }
//       if (b.pos.y + b.radius > h)     { b.pos.y = h - b.radius; b.vel.y = -b.vel.y; }
//     }
//   }

//   void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
//   {
//     auto lo = this->get_parameter("hsv_lower").as_integer_array();
//     auto hi = this->get_parameter("hsv_upper").as_integer_array();
//     int min_area = this->get_parameter("min_area").as_int();

//     cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;

//     // --- waktu dari timestamp ---
//     double t = rclcpp::Time(msg->header.stamp).seconds();
//     double dt = (last_time_ > 0) ? (t - last_time_) : 0.0;
//     last_time_ = t;

//     if (balls_.empty()) {
//       init_balls(frame.cols, frame.rows);
//     }
//     update_balls(dt, frame.cols, frame.rows);

//     // --- deteksi objek (sama seperti Langkah 2) ---
//     cv::Mat hsv, mask;
//     cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
//     cv::inRange(hsv,
//       cv::Scalar(lo[0], lo[1], lo[2]),
//       cv::Scalar(hi[0], hi[1], hi[2]), mask);

//     cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
//     cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
//     cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

//     std::vector<std::vector<cv::Point>> contours;
//     cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

//     double best_area = 0;
//     int best_idx = -1;
//     for (size_t i = 0; i < contours.size(); ++i) {
//       double a = cv::contourArea(contours[i]);
//       if (a > best_area) { best_area = a; best_idx = static_cast<int>(i); }
//     }

//     if (best_idx >= 0 && best_area >= min_area) {
//       cv::Rect box = cv::boundingRect(contours[best_idx]);
//       cv::Moments m = cv::moments(contours[best_idx]);
//       cv::Point c(static_cast<int>(m.m10 / m.m00), static_cast<int>(m.m01 / m.m00));
//       cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
//       cv::circle(frame, c, 5, cv::Scalar(0, 0, 255), -1);
//     }

//     // --- gambar bola di atas frame ---
//     for (const auto & b : balls_) {
//       cv::circle(frame, b.pos, b.radius, b.color, 2);
//     }

//     auto out = cv_bridge::CvImage(msg->header, "bgr8", frame).toImageMsg();
//     debug_pub_->publish(*out);
//   }

//   rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
//   rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr debug_pub_;
//   std::vector<Ball> balls_;
//   std::mt19937 rng_;
//   double last_time_ = -1.0;
// };

// int main(int argc, char ** argv)
// {
//   rclcpp::init(argc, argv);
//   rclcpp::spin(std::make_shared<GameNode>());
//   rclcpp::shutdown();
//   return 0;
// }
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/int32.hpp"
#include "cv_bridge/cv_bridge.h"
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <random>
#include <string>
#include <vector>

struct Ball
{
  cv::Point2f pos;
  cv::Point2f vel;
  int radius;
  cv::Scalar color;
  double hold_time = 0.0;
};

class GameNode : public rclcpp::Node
{
public:
  GameNode() : Node("game_node"), rng_(std::random_device{}())
  {
    this->declare_parameter<std::vector<int64_t>>("hsv_lower", {5, 120, 120});
    this->declare_parameter<std::vector<int64_t>>("hsv_upper", {20, 255, 255});
    this->declare_parameter<int>("min_area", 300);
    this->declare_parameter<int>("num_balls", 3);
    this->declare_parameter<int>("ball_radius", 40);
    this->declare_parameter<double>("ball_speed", 150.0);
    this->declare_parameter<double>("hold_duration", 2.0);

    subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
      "/camera/image_raw", 10,
      std::bind(&GameNode::image_callback, this, std::placeholders::_1));
    debug_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/game/debug_image", 10);
    score_pub_ = this->create_publisher<std_msgs::msg::Int32>("/game/score", 10);

    publish_score();  // skor awal 0
  }

private:
  void respawn_ball(Ball & b, int w, int h)
  {
    double speed = this->get_parameter("ball_speed").as_double();
    int r = b.radius;
    std::uniform_real_distribution<float> rx(r, w - r);
    std::uniform_real_distribution<float> ry(r, h - r);
    std::uniform_real_distribution<float> ang(0.0f, 6.2831853f);
    float a = ang(rng_);
    b.pos = cv::Point2f(rx(rng_), ry(rng_));
    b.vel = cv::Point2f(std::cos(a) * speed, std::sin(a) * speed);
    b.hold_time = 0.0;
  }

  void init_balls(int w, int h)
  {
    int n = this->get_parameter("num_balls").as_int();
    int r = this->get_parameter("ball_radius").as_int();
    std::uniform_int_distribution<int> col(80, 255);

    balls_.clear();
    for (int i = 0; i < n; ++i) {
      Ball b;
      b.radius = r;
      b.color = cv::Scalar(col(rng_), col(rng_), col(rng_));
      respawn_ball(b, w, h);
      balls_.push_back(b);
    }
  }

  void update_balls(double dt, int w, int h)
  {
    for (auto & b : balls_) {
      b.pos += b.vel * static_cast<float>(dt);
      if (b.pos.x - b.radius < 0)  { b.pos.x = b.radius;     b.vel.x = -b.vel.x; }
      if (b.pos.x + b.radius > w)  { b.pos.x = w - b.radius; b.vel.x = -b.vel.x; }
      if (b.pos.y - b.radius < 0)  { b.pos.y = b.radius;     b.vel.y = -b.vel.y; }
      if (b.pos.y + b.radius > h)  { b.pos.y = h - b.radius; b.vel.y = -b.vel.y; }
    }
  }

  void publish_score()
  {
    std_msgs::msg::Int32 m;
    m.data = score_;
    score_pub_->publish(m);
  }

  void image_callback(const sensor_msgs::msg::Image::SharedPtr msg)
  {
    auto lo = this->get_parameter("hsv_lower").as_integer_array();
    auto hi = this->get_parameter("hsv_upper").as_integer_array();
    int min_area = this->get_parameter("min_area").as_int();
    double hold_duration = this->get_parameter("hold_duration").as_double();

    cv::Mat frame = cv_bridge::toCvCopy(msg, "bgr8")->image;
    int w = frame.cols, h = frame.rows;

    // waktu dari timestamp
    double t = rclcpp::Time(msg->header.stamp).seconds();
    double dt = (last_time_ > 0) ? (t - last_time_) : 0.0;
    last_time_ = t;

    if (balls_.empty()) {
      init_balls(w, h);
    }
    update_balls(dt, w, h);

    // deteksi objek
    cv::Mat hsv, mask;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(hsv,
      cv::Scalar(lo[0], lo[1], lo[2]),
      cv::Scalar(hi[0], hi[1], hi[2]), mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double best_area = 0;
    int best_idx = -1;
    for (size_t i = 0; i < contours.size(); ++i) {
      double a = cv::contourArea(contours[i]);
      if (a > best_area) { best_area = a; best_idx = static_cast<int>(i); }
    }

    bool detected = false;
    cv::Point2f centroid;
    if (best_idx >= 0 && best_area >= min_area) {
      cv::Rect box = cv::boundingRect(contours[best_idx]);
      cv::Moments m = cv::moments(contours[best_idx]);
      centroid = cv::Point2f(
        static_cast<float>(m.m10 / m.m00), static_cast<float>(m.m01 / m.m00));
      detected = true;
      cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
      cv::circle(frame, centroid, 5, cv::Scalar(0, 0, 255), -1);
    }

    // aturan game: cari bola terdekat yang berisi centroid
    int target = -1;
    double best_dist = 1e9;
    if (detected) {
      for (size_t i = 0; i < balls_.size(); ++i) {
        double d = cv::norm(centroid - balls_[i].pos);
        if (d <= balls_[i].radius && d < best_dist) {
          best_dist = d;
          target = static_cast<int>(i);
        }
      }
    }

    // update timer: hanya target bertambah, lainnya reset
    for (size_t i = 0; i < balls_.size(); ++i) {
      if (static_cast<int>(i) == target) {
        balls_[i].hold_time += dt;
        if (balls_[i].hold_time >= hold_duration) {
          score_++;
          respawn_ball(balls_[i], w, h);
          publish_score();
        }
      } else {
        balls_[i].hold_time = 0.0;
      }
    }

    // gambar bola, progres, dan skor
    for (const auto & b : balls_) {
      cv::circle(frame, b.pos, b.radius, b.color, 2);
      double ratio = std::min(1.0, b.hold_time / hold_duration);
      if (ratio > 0) {
        cv::ellipse(frame, b.pos, cv::Size(b.radius, b.radius),
          -90, 0, 360 * ratio, cv::Scalar(0, 255, 255), 4);
      }
    }
    cv::putText(frame, "Skor: " + std::to_string(score_), cv::Point(10, 30),
      cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 0, 255), 2);

    auto out = cv_bridge::CvImage(msg->header, "bgr8", frame).toImageMsg();
    debug_pub_->publish(*out);
  }

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr debug_pub_;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr score_pub_;
  std::vector<Ball> balls_;
  std::mt19937 rng_;
  double last_time_ = -1.0;
  int score_ = 0;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<GameNode>());
  rclcpp::shutdown();
  return 0;
}