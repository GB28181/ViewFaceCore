﻿using System;
using ViewFaceCore.Sharp.Model;
using System.Runtime.InteropServices;
using System.IO;

namespace ViewFaceCore.Plus
{
    /// <summary>
    /// 日志回调函数
    /// </summary>
    /// <param name="logText"></param>
    public delegate void LogCallBack(string logText);

    /// <summary>
    /// 适用于 Any CPU 的 ViewFacePlus
    /// </summary>
    static class ViewFacePlus
    {
        [DllImport("kernel32", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool SetDllDirectory(string path);

        static ViewFacePlus()
        {
            string path = null;
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                //if (!Environment.Is64BitOperatingSystem || !Environment.Is64BitProcess)
                //{
                //    throw new Exception("Not support 32 bit linux system!");
                //}
                //switch (RuntimeInformation.ProcessArchitecture)
                //{
                //    case Architecture.X86:
                //    case Architecture.Arm:
                //        throw new Exception("Not support 32 bit linux system!");
                //    case Architecture.Arm64:
                //        path = "libs/linux/x64arm/ViewFace.so";
                //        break;
                //    case Architecture.X64:
                //        path = "libs/linux/x64arm/ViewFace";
                //        break;
                //}
                //string val = Environment.GetEnvironmentVariable("LD_LIBRARY_PATH");
                //Environment.SetEnvironmentVariable("LD_LIBRARY_PATH", val + "," + GetFullPath(path));

            }
            else if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            {
                if (Environment.Is64BitProcess)
                {
                    path = "libs/win/x64/ViewFace.dll";
                }
                else
                {
                    path = "libs/win/x86/ViewFace.dll";
                }
                SetDllDirectory(GetFullPath(path));
            }
        }

        private static string GetFullPath(string path)
        {
            if (!File.Exists(path))
            {
                throw new Exception("Lib not exist!");
            }
            FileInfo fileInfo = new FileInfo(path);
            return fileInfo.DirectoryName;
        }

        /// <summary>
        /// 获取一个值，指示当前运行的进程是否是 64位
        /// </summary>
        public static bool Is64BitProcess { get; } = Environment.Is64BitProcess;

        /// <summary>
        /// 设置日志回调函数(用于日志打印)
        /// </summary>
        /// <param name="writeLog"></param>
        public static void SetLogFunction(LogCallBack writeLog)
        {
            ViewFacePlusNative.SetLogFunction(writeLog); 
        }

        /// <summary>
        /// 获取或设置人脸模型目录
        /// </summary>
        public static string ModelPath
        {
            get
            {
                return ViewFacePlusNative.GetModelPath();
            }
            set
            {
                ViewFacePlusNative.SetModelPath(value);
            }
        }

        /// <summary>
        /// 释放使用的资源
        /// </summary>
        public static void ViewDispose()
        {
            ViewFacePlusNative.ViewDispose();
        }

        /// <summary>
        /// 人脸检测器检测到的人脸数量
        /// </summary>
        /// <param name="imgData"></param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceSize">最小人脸是人脸检测器常用的一个概念，默认值为20，单位像素。
        /// <para>最小人脸和检测器性能息息相关。主要方面是速度，使用建议上，我们建议在应用范围内，这个值设定的越大越好。SeetaFace采用的是BindingBox Regresion的方式训练的检测器。如果最小人脸参数设置为80的话，从检测能力上，可以将原图缩小的原来的1/4，这样从计算复杂度上，能够比最小人脸设置为20时，提速到16倍。</para>
        /// </param>
        /// <param name="threshold">检测器阈值默认值是0.9，合理范围为[0, 1]。这个值一般不进行调整，除了用来处理一些极端情况。这个值设置的越小，漏检的概率越小，同时误检的概率会提高</param>
        /// <param name="maxWidth">可检测的图像最大宽度。默认值2000。</param>
        /// <param name="maxHeight">可检测的图像最大高度。默认值2000。</param>
        /// <param name="type">模型类型。0：face_detector；1：mask_detector；2：mask_detector。</param>
        /// <returns></returns>
        public static int DetectorSize(byte[] imgData, ref FaceImage img,
            double faceSize = 20, double threshold = 0.9, double maxWidth = 2000, double maxHeight = 2000, int type = 0)
        {
            return ViewFacePlusNative.DetectorSize(imgData, ref img, faceSize, threshold, maxWidth, maxHeight, type);
        }
        /// <summary>
        /// 人脸检测器
        /// <para>调用此方法前必须先调用 <see cref="DetectorSize(byte[], ref FaceImage, double, double, double, double, int)"/></para>
        /// </summary>
        /// <param name="score">人脸置信度集合</param>
        /// <param name="x">人脸位置集合</param>
        /// <param name="y">人脸位置集合</param>
        /// <param name="width">人脸大小集合</param>
        /// <param name="height">人脸大小集合</param>
        /// <returns></returns>
        public static bool Detector(float[] score, int[] x, int[] y, int[] width, int[] height)
        {
            return ViewFacePlusNative.Detector(score, x, y, width, height);
        }

        /// <summary>
        /// 人脸关键点数量
        /// </summary>
        /// <param name="type">模型类型。0：face_landmarker_pts68；1：face_landmarker_mask_pts5；2：face_landmarker_pts5。</param>
        /// <returns></returns>
        public static int FaceMarkSize(int type = 0)
        {
            return ViewFacePlusNative.FaceMarkSize(type);
        }
        /// <summary>
        /// 获取人脸关键点
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="pointX">存储关键点 x 坐标的 数组</param>
        /// <param name="pointY">存储关键点 y 坐标的 数组</param>
        /// <param name="type">模型类型。0：face_landmarker_pts68；1：face_landmarker_mask_pts5；2：face_landmarker_pts5。</param>
        /// <returns></returns>
        public static bool FaceMark(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, double[] pointX, double[] pointY, int type = 0)
        {
            return ViewFacePlusNative.FaceMark(imgData, ref img, faceRect, pointX, pointY, type);
        }

        /// <summary>
        /// 获取人脸特征值长度
        /// </summary>
        /// <param name="type">模型类型。0：face_recognizer；1：face_recognizer_mask；2：face_recognizer_light。</param>
        /// <returns></returns>
        public static int ExtractSize(int type = 0)
        {
            return ViewFacePlusNative.ExtractSize(type);
        }
        /// <summary>
        /// 提取人脸特征值
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="features">人脸特征值 数组</param>
        /// <param name="type">模型类型。0：face_recognizer；1：face_recognizer_mask；2：face_recognizer_light。</param>
        /// <returns></returns>
        public static bool Extract(byte[] imgData, ref FaceImage img,
            FaceMarkPoint[] points, float[] features, int type = 0)
        {
            return ViewFacePlusNative.Extract(imgData, ref img, points, features, type);
        }

        /// <summary>
        /// 计算相似度
        /// </summary>
        /// <param name="leftFeatures"></param>
        /// <param name="rightFeatures"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        public static float Similarity(float[] leftFeatures, float[] rightFeatures, int type = 0)
        {
            return ViewFacePlusNative.Similarity(leftFeatures, rightFeatures, type);
        }

        /// <summary>
        /// 活体检测器
        /// <para>单帧检测</para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="global">是否启用全局检测</param>
        /// <returns>单帧识别返回值会是 <see cref="AntiSpoofingStatus.Real"/>、<see cref="AntiSpoofingStatus.Spoof"/> 或 <see cref="AntiSpoofingStatus.Fuzzy"/></returns>
        public static int AntiSpoofing(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, bool global)
        {
            return ViewFacePlusNative.AntiSpoofing(imgData, ref img, faceRect, points, global);
        }
        /// <summary>
        /// 活体检测器
        /// <para>视频帧</para>
        /// </summary>
        /// <param name="imgData"></param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points"></param>
        /// <param name="global">是否启用全局检测</param>
        /// <returns>
        /// <para>
        /// 单帧识别返回值会是 <see cref="AntiSpoofingStatus.Real"/>、<see cref="AntiSpoofingStatus.Spoof"/>、<see cref="AntiSpoofingStatus.Fuzzy"/> 或 <see cref="AntiSpoofingStatus.Detecting"/><br />
        /// 在视频识别输入帧数不满足需求的时候，返回状态就是 <see cref="AntiSpoofingStatus.Detecting"/>
        /// </para>
        /// </returns>
        public static int AntiSpoofingVideo(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, bool global)
        {
            return ViewFacePlusNative.AntiSpoofingVideo(imgData, ref img, faceRect, points, global);
        }

        /// <summary>
        /// 获取跟踪的人脸个数
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="stable"></param>
        /// <param name="interval"></param>
        /// <param name="faceSize"></param>
        /// <param name="threshold"></param>
        /// <param name="type">模型类型。0：face_detector；1：mask_detector；2：mask_detector。</param>
        /// <returns></returns>
        public static int FaceTrackSize(byte[] imgData, ref FaceImage img,
            bool stable = false, int interval = 10,
            double faceSize = 20, double threshold = 0.9, int type = 0)
        {
            return ViewFacePlusNative.FaceTrackSize(imgData, ref img, stable, interval, faceSize, threshold, type);
        }

        /// <summary>
        /// 人脸跟踪信息
        /// </summary>
        /// <param name="score">人脸置信度分数 数组</param>
        /// <param name="PID">人脸标识ID 数组</param>
        /// <param name="x">人脸位置 x 数组</param>
        /// <param name="y">人脸位置 y 数组</param>
        /// <param name="width">人脸大小 width 数组</param>
        /// <param name="height">人脸大小 height 数组</param>
        /// <returns></returns>
        public static bool FaceTrack(float[] score, int[] PID, int[] x, int[] y, int[] width, int[] height)
        {
            return ViewFacePlusNative.FaceTrack(score, PID, x, y, width, height);
        }

        /// <summary>
        /// 亮度评估。
        /// <para>亮度评估就是评估人脸区域内的亮度值是否均匀正常，存在部分或全部的过亮和过暗都会是评价为LOW。</para>
        /// <para>
        /// 评估器会将综合的亮度从灰度值映射到level，其映射关系为： <br />
        /// • [0, v0), [v3, ~) => <see cref="QualityLevel.Low"/> <br />
        /// • [v0, v1), [v2, v3) => <see cref="QualityLevel.Medium"/> <br />
        /// • [v1, v2) => <see cref="QualityLevel.High"/> <br />
        /// </para> <br />
        /// <para><see langword="{v0, v1, v2, v3}"/> 的默认值为 <see langword="{70, 100, 210, 230}"/></para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <param name="v0"></param>
        /// <param name="v1"></param>
        /// <param name="v2"></param>
        /// <param name="v3"></param>
        /// <returns></returns>
        public static bool QualityOfBrightness(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score,
            float v0 = 70, float v1 = 100, float v2 = 210, float v3 = 230)
        {
            return ViewFacePlusNative.QualityOfBrightness(imgData, ref img, faceRect, points, pointsLength, ref level, ref score, v0, v1, v2, v3);
        }

        /// <summary>
        /// 清晰度评估。
        /// <para>清晰度这里是传统方式通过二次模糊后图像信息损失程度统计的清晰度。</para>
        /// <para>
        /// 映射关系为： <br />
        /// • [0, low) => <see cref="QualityLevel.Low"/> <br />
        /// • [low, high) => <see cref="QualityLevel.Medium"/> <br />
        /// • [high, ~) => <see cref="QualityLevel.High"/> <br />
        /// </para> <br />
        /// <para><see langword="{low, high}"/> 的默认值为 <see langword="{0.1, 0.2}"/></para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <param name="low"></param>
        /// <param name="high"></param>
        /// <returns></returns>
        public static bool QualityOfClarity(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score,
            float low = 0.1f, float high = 0.2f)
        {
            return ViewFacePlusNative.QualityOfClarity(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score,
                  low, high);
        }

        /// <summary>
        /// 完整度评估。
        /// <para>完整度评估是朴素的判断人来是否因为未完全进入摄像头而造成的不完整的情况。该方法不适用于判断遮挡造成的不完整。</para>
        /// <para>
        /// 映射关系为： <br />
        /// • 人脸外扩 high 倍数没有超出图像 => <see cref="QualityLevel.High"/> <br />
        /// • 人脸外扩 low 倍数没有超出图像 => <see cref="QualityLevel.Medium"/> <br />
        /// • 其他 => <see cref="QualityLevel.Low"/> <br />
        /// </para> <br />
        /// <para><see langword="{low, high}"/> 的默认值为 <see langword="{10, 1.5}"/></para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <param name="low"></param>
        /// <param name="high"></param>
        /// <returns></returns>
        public static bool QualityOfIntegrity(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score,
            float low = 10, float high = 1.5f)
        {
            return ViewFacePlusNative.QualityOfIntegrity(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score,
                  low, high);
        }

        /// <summary>
        /// 姿态评估。
        /// <para>此姿态评估器是传统方式，通过人脸5点坐标值来判断姿态是否为正面。</para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <returns></returns>
        public static bool QualityOfPose(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score)
        {
            return ViewFacePlusNative.QualityOfPose(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score);
        }

        /// <summary>
        /// 姿态评估 (深度)。
        /// <para>此姿态评估器是深度学习方式，通过回归人头部在yaw、pitch、roll三个方向的偏转角度来评估人脸是否是正面。</para>
        /// <para>
        /// 需要模型 <see langword="pose_estimation.csta"/> 
        /// </para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <param name="yawLow">yaw 方向低分数阈值</param>
        /// <param name="yawHigh">yaw 方向高分数阈值</param>
        /// <param name="pitchLow">pitch 方向低分数阈值</param>
        /// <param name="pitchHigh">pitch 方向高分数阈值</param>
        /// <param name="rollLow">roll 方向低分数阈值</param>
        /// <param name="rollHigh">roll 方向高分数阈值</param>
        /// <returns></returns>
        public static bool QualityOfPoseEx(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score,
            float yawLow = 25, float yawHigh = 10, float pitchLow = 20, float pitchHigh = 10, float rollLow = 33.33f, float rollHigh = 16.67f)
        {
            return ViewFacePlusNative.QualityOfPoseEx(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score,
                  yawLow, yawHigh, pitchLow, pitchHigh, rollLow, rollHigh);
        }

        /// <summary>
        /// 分辨率评估。
        /// <para>判断人脸部分的分辨率。</para>
        /// <para>
        /// 映射关系为： <br />
        /// • [0, low) => <see cref="QualityLevel.Low"/> <br />
        /// • [low, high) => <see cref="QualityLevel.Medium"/> <br />
        /// • [high, ~) => <see cref="QualityLevel.High"/> <br />
        /// </para> <br />
        /// <para><see langword="{low, high}"/> 的默认值为 <see langword="{80, 120}"/></para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <param name="low"></param>
        /// <param name="high"></param>
        /// <returns></returns>
        public static bool QualityOfResolution(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score,
            float low = 80, float high = 120)
        {
            return ViewFacePlusNative.QualityOfResolution(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score,
                  low, high);
        }

        /// <summary>
        /// 清晰度 (深度)评估。
        /// <para>
        /// 需要模型 <see langword="quality_lbn.csta"/> <br />
        /// 需要模型 <see langword="face_landmarker_pts68.csta"/> 
        /// </para>
        /// <para><see langword="{blur_thresh}"/> 的默认值为 <see langword="{0.8}"/></para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <param name="blur_thresh"></param>
        /// <returns></returns>
        public static bool QualityOfClarityEx(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score,
            float blur_thresh = 0.8f)
        {
            return ViewFacePlusNative.QualityOfClarityEx(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score,
                  blur_thresh);
        }

        /// <summary>
        /// 遮挡评估。
        /// <para>
        /// 判断人脸部分的分辨率。 <br />
        /// 需要模型 <see langword="face_landmarker_mask_pts5.csta"/> 
        /// </para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="faceRect">人脸位置信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="level">存储 等级</param>
        /// <param name="score">存储 分数</param>
        /// <returns></returns>
        public static bool QualityOfNoMask(byte[] imgData, ref FaceImage img,
            FaceRect faceRect, FaceMarkPoint[] points, int pointsLength, ref int level, ref float score)
        {
            return ViewFacePlusNative.QualityOfNoMask(imgData, ref img,
                  faceRect, points, pointsLength, ref level, ref score);
        }

        /// <summary>
        /// 年龄预测。
        /// <para>
        /// 需要模型 <see langword="age_predictor.csta"/> 
        /// </para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <returns></returns>
        public static int AgePredictor(byte[] imgData, ref FaceImage img, FaceMarkPoint[] points, int pointsLength)
        {
            return ViewFacePlusNative.AgePredictor(imgData, ref img, points, pointsLength);
        }

        /// <summary>
        /// 性别预测。
        /// <para>
        /// 需要模型 <see langword="gender_predictor.csta"/> 
        /// </para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <returns></returns>
        public static int GenderPredictor(byte[] imgData, ref FaceImage img, FaceMarkPoint[] points, int pointsLength)
        {
            return ViewFacePlusNative.GenderPredictor(imgData, ref img, points, pointsLength);
        }


        /// <summary>
        /// 眼睛状态检测。
        /// <para>
        /// 需要模型 <see langword="eye_state.csta"/> 
        /// </para>
        /// </summary>
        /// <param name="imgData">图像 BGR 数据</param>
        /// <param name="img">图像宽高通道信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <param name="pointsLength">人脸关键点 数组长度</param>
        /// <param name="left_eye"></param>
        /// <param name="right_eye"></param>
        /// <returns></returns>
        public static bool EyeStateDetector(byte[] imgData, ref FaceImage img, FaceMarkPoint[] points, int pointsLength,
            ref int left_eye, ref int right_eye)
        {
            return ViewFacePlusNative.EyeStateDetector(imgData, ref img, points, pointsLength, ref left_eye, ref right_eye);
        }
    }
}
