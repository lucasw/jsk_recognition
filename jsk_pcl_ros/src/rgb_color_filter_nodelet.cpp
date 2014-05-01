/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014, JSK Lab
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/o2r other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include "jsk_pcl_ros/rgb_color_filter.h"

#include <pluginlib/class_list_macros.h>

namespace jsk_pcl_ros
{

  void RGBColorFilter::updateCondition()
  {
    pcl::ConditionBase<pcl::PointXYZRGB>::Ptr condp (new pcl::ConditionAnd<pcl::PointXYZRGB> ());

    int r_max, r_min, g_max, g_min, b_max, b_min;
    if ( r_max_ >= r_min_ ) {
      r_max = r_max_;
      r_min = r_min_;
    }
    else {
      r_max = r_min_;
      r_min = r_max_;
    }
    if ( g_max_ >= g_min_ ) {
      g_max = g_max_;
      g_min = g_min_;
    }
    else {
      g_max = g_min_;
      g_min = g_max_;
    }
    if ( b_max_ >= b_min_ ) {
      b_max = b_max_;
      b_min = b_min_;
    }
    else {
      b_max = b_min_;
      b_min = b_max_;
    }
    
    { 
      pcl::ConditionBase<pcl::PointXYZRGB>::Ptr cond (new pcl::ConditionAnd<pcl::PointXYZRGB> ());
      pcl::ComparisonBase<pcl::PointXYZRGB>::Ptr
        le (new pcl::PackedRGBComparison<pcl::PointXYZRGB> ("r", pcl::ComparisonOps::LE, r_max));
      pcl::ComparisonBase<pcl::PointXYZRGB>::Ptr
        ge (new pcl::PackedRGBComparison<pcl::PointXYZRGB> ("r", pcl::ComparisonOps::GE, r_min));
      cond->addComparison (le);
      cond->addComparison (ge);
      condp->addCondition(cond);
    }
    { 
      pcl::ConditionBase<pcl::PointXYZRGB>::Ptr cond (new pcl::ConditionAnd<pcl::PointXYZRGB> ());
      pcl::ComparisonBase<pcl::PointXYZRGB>::Ptr
        le (new pcl::PackedRGBComparison<pcl::PointXYZRGB> ("g", pcl::ComparisonOps::LE, g_max));
      pcl::ComparisonBase<pcl::PointXYZRGB>::Ptr
        ge (new pcl::PackedRGBComparison<pcl::PointXYZRGB> ("g", pcl::ComparisonOps::GE, g_min));
      cond->addComparison (le);
      cond->addComparison (ge);
      condp->addCondition(cond);
    }
    {
      pcl::ConditionBase<pcl::PointXYZRGB>::Ptr cond (new pcl::ConditionAnd<pcl::PointXYZRGB> ());
      pcl::ComparisonBase<pcl::PointXYZRGB>::Ptr
        le (new pcl::PackedRGBComparison<pcl::PointXYZRGB> ("b", pcl::ComparisonOps::LE, b_max));
      pcl::ComparisonBase<pcl::PointXYZRGB>::Ptr
        ge (new pcl::PackedRGBComparison<pcl::PointXYZRGB> ("b", pcl::ComparisonOps::GE, b_min));
      cond->addComparison (le);
      cond->addComparison (ge);
      condp->addCondition(cond);
    }

    filter_instance_.setCondition (condp);
  }
  
  void RGBColorFilter::filter(const sensor_msgs::PointCloud2ConstPtr &input)
  {
    pcl::PointCloud<pcl::PointXYZRGB> tmp_in, tmp_out;
    sensor_msgs::PointCloud2 out;
    fromROSMsg(*input, tmp_in);

    // for (size_t i = 0; i < tmp_in.points.size(); i++) {
    //   pcl::PointXYZRGB p = tmp_in.points[i];
    //   uint32_t rgb = *reinterpret_cast<int*>(&p.rgb);
    //   uint8_t r = (rgb >> 16) & 0x0000ff;
    //   uint8_t g = (rgb >> 8)  & 0x0000ff;
    //   uint8_t b = (rgb)       & 0x0000ff;
    //   NODELET_INFO("r: %u g: %u b: %u", r, g, b);
    // }
    
    filter_instance_.setInputCloud (tmp_in.makeShared());
    filter_instance_.filter(tmp_out);
    if (tmp_out.points.size() > 0) {
      toROSMsg(tmp_out, out);
      pub_.publish(out);
    }
  }

  void RGBColorFilter::configCallback(Config &config, uint32_t level)
  {
    boost::mutex::scoped_lock lock (mutex_);
    r_max_ = config.r_limit_max;
    r_min_ = config.r_limit_min;
    g_max_ = config.g_limit_max;
    g_min_ = config.g_limit_min;
    b_max_ = config.b_limit_max;
    b_min_ = config.b_limit_min;
    updateCondition();
  }
  
  void RGBColorFilter::onInit()
  {
    PCLNodelet::onInit();
    r_max_ = 255;
    r_min_ = 0;
    g_max_ = 255;
    g_min_ = 0;
    b_max_ = 255;
    b_min_ = 0;
    filter_instance_ = pcl::ConditionalRemoval<pcl::PointXYZRGB>(true);
    updateCondition();
    pub_ = pnh_->advertise<sensor_msgs::PointCloud2>("output", 1);
    sub_input_ = pnh_->subscribe("input", 1, &RGBColorFilter::filter, this);
    srv_ = boost::make_shared <dynamic_reconfigure::Server<Config> > (*pnh_);
    dynamic_reconfigure::Server<Config>::CallbackType f =
      boost::bind (&RGBColorFilter::configCallback, this, _1, _2);
    srv_->setCallback (f);
  }
  
}

typedef jsk_pcl_ros::RGBColorFilter RGBColorFilter;
PLUGINLIB_DECLARE_CLASS (jsk_pcl, RGBColorFilter, RGBColorFilter, nodelet::Nodelet);
