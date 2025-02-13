﻿using System;

namespace ViewFaceCore.Configs
{
    /// <summary>
    /// 人脸跟踪器配置
    /// </summary>
    public class FaceTrackerConfig : BaseConfig
    {
        /// <summary>
        /// 视频宽度
        /// </summary>
        public int Width { get; set; }

        /// <summary>
        /// 视频高度
        /// </summary>
        public int Height { get; set; }

        /// <summary>
        /// 设置可检测的人脸大小，为人脸宽和高乘积的二次根值。
        /// <para>
        /// 最小人脸是人脸检测器常用的一个概念，默认值为20，单位像素。<br />
        /// 最小人脸和检测器性能息息相关。主要方面是速度，使用建议上，我们建议在应用范围内，这个值设定的越大越好。<see langword="SeetaFace"/> 采用的是 <c><see langword="BindingBox Regresion"/></c> 的方式训练的检测器。
        /// 如果最小人脸参数设置为 <see langword="80"/> 的话，从检测能力上，可以将原图缩小的原来的 <see langword="1/4"/> ，这样从计算复杂度上，能够比最小人脸设置为 <see langword="20"/> 时，提速到 <see langword="16"/> 倍。
        /// </para>
        /// </summary>
        public int MinFaceSize { get; set; } = 20;

        /// <summary>
        /// 检测器阈值。
        /// <para>默认值是0.9，合理范围为[0, 1]。这个值一般不进行调整，除了用来处理一些极端情况。这个值设置的越小，漏检的概率越小，同时误检的概率会提高。</para>
        /// </summary>
        public float Threshold { get; set; } = 0.9f;

        /// <summary>
        /// 是否进行检测结果的帧间平滑，使得检测结果从视觉上更好一些。
        /// </summary>
        public bool Stable { get; set; } = false;

        /// <summary>
        /// 检测间隔
        /// <para>
        /// 间隔默认值为10。这里跟踪间隔是为了发现新增PID的间隔。检测器会通过整张图像检测人脸去发现是否有新增的PID，所以这个值太小会导致跟踪速度变慢（不断做全局检测）；这个值太大会导致画面中新增加的人脸不会立马被跟踪到。
        /// </para>
        /// </summary>
        public int Interval { get; set; } = 10;

        public FaceTrackerConfig(int width, int height)
        {
            this.Width = width;
            this.Height = height;
        }

        /// <summary>
        /// 设置是否进行检测结果的帧间平滑
        /// </summary>
        /// <param name="stable"></param>
        /// <returns></returns>
        public FaceTrackerConfig SetStable(bool stable)
        {
            this.Stable = stable;
            return this;
        }

        /// <summary>
        /// 设置最小检测人脸
        /// </summary>
        /// <param name="minFaceSize"></param>
        /// <returns></returns>
        public FaceTrackerConfig SetMinFaceSize(int minFaceSize)
        {
            if (minFaceSize < 5 || minFaceSize > Math.Min(Width, Height))
            {
                throw new ArgumentException($"MinFaceSize value range from {5} to {Math.Min(Width, Height)}", nameof(minFaceSize));
            }
            this.MinFaceSize = minFaceSize;
            return this;
        }

        /// <summary>
        /// 设置检测器阈值
        /// </summary>
        /// <param name="threshold">阈值</param>
        /// <returns></returns>
        public FaceTrackerConfig SetThreshold(float threshold)
        {
            if (threshold <= 0 || threshold >= 1f)
            {
                throw new ArgumentException($"Threshold value range from 0 to 1", nameof(threshold));
            }
            this.Threshold = threshold;
            return this;
        }

        /// <summary>
        /// 设置检测间隔
        /// </summary>
        /// <param name="interval"></param>
        /// <returns></returns>
        public FaceTrackerConfig SetInterval(int interval)
        {
            this.Interval = interval;
            return this;
        }
    }
}
