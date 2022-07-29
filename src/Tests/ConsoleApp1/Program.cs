﻿using SkiaSharp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using ViewFaceCore;
using ViewFaceCore.Model;

namespace ConsoleApp1
{
    internal class Program
    {
        private readonly static NLog.Logger logger = NLog.LogManager.GetCurrentClassLogger();
        private readonly static string imagePath = @"images\Jay_3.jpg";
        private readonly static string logPath = "logs";

        static void Main(string[] args)
        {
            if (Directory.Exists(logPath))
            {
                Directory.Delete(logPath, true);
            }

            //人脸识别和标记测试，开始：2022-07-28 17:11，结束：,结果：
            //FaceDetectorAndFaceMarkTest();

            //活体检测测试，通过24h测试，20220728
            //AntiSpoofingTest();

            //质量评估测试，开始：2022-07-28 09:57，结束：,结果：不通过，有三个评估方式有问题
            //FaceQualityTest();

            //人脸追踪测试，开始：2022/07/29 16:45:18，结束：2022/07/29 17:50:01,结果：通过
            //FaceTrackTest();

            //人脸特征值测试
            //ExtractTest();

            //年龄预测测试
            //FaceAgePredictorTest();

            //性别预测测试
            //FaceGenderPredictorTest();

            //眼睛状态检测测试
            FaceEyeStateDetectorTest();


            Console.WriteLine("Hello, World!");
        }

        private static void FaceDetectorAndFaceMarkTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            Stopwatch sw = new Stopwatch();

            Worker((sw, i) =>
            {
                var infos = viewFace.FaceDetector(bitmap);
                var info = infos.First();
                var markPoints = GetFaceMarkPoint(viewFace, bitmap);

                logger.Info($"第{i + 1}次识别，结果：{markPoints.Count()}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        private static void FaceQualityTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            var infos = viewFace.FaceDetector(bitmap);
            var info = infos.First();
            var markPoints = GetFaceMarkPoint(viewFace, bitmap);

            Worker((sw, i) =>
            {
                //var brightnessResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.Brightness);
                //logger.Info($"第{i + 1}次{QualityType.Brightness}评估，结果：{brightnessResult}，耗时：{sw.ElapsedMilliseconds}ms");
                //var resolutionResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.Resolution);
                //logger.Info($"第{i + 1}次{QualityType.Resolution}评估，结果：{resolutionResult}，耗时：{sw.ElapsedMilliseconds}ms");
                //var clarityResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.Clarity);
                //logger.Info($"第{i + 1}次{QualityType.Clarity}评估，结果：{clarityResult}，耗时：{sw.ElapsedMilliseconds}ms");
                //var clarityExResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.ClarityEx);
                //logger.Info($"第{i + 1}次{QualityType.ClarityEx}评估，结果：{clarityExResult}，耗时：{sw.ElapsedMilliseconds}ms");
                //var integrityExResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.Integrity);
                //logger.Info($"第{i + 1}次{QualityType.Integrity}评估，结果：{integrityExResult}，耗时：{sw.ElapsedMilliseconds}ms");

                //
                var poseResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.Pose);
                logger.Info($"第{i + 1}次{QualityType.Pose}评估，结果：{poseResult}，耗时：{sw.ElapsedMilliseconds}ms");
                //var poseExeResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.PoseEx);
                //logger.Info($"第{i + 1}次{QualityType.PoseEx}评估，结果：{poseExeResult}，耗时：{sw.ElapsedMilliseconds}ms");
                //var structureeResult = viewFace.FaceQuality(bitmap, info, markPoints, QualityType.Structure);
                //logger.Info($"第{i + 1}次{QualityType.Structure}评估，结果：{structureeResult}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        /// <summary>
        /// 活体检测测试
        /// </summary>
        private static void AntiSpoofingTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            var infos = viewFace.FaceDetector(bitmap);
            var info = infos.First();
            var markPoints = GetFaceMarkPoint(viewFace, bitmap);

            Worker((sw, i) =>
            {
                var result = viewFace.AntiSpoofing(bitmap, info, markPoints);
                logger.Info($"第{i + 1}次检测，结果：{result}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        /// <summary>
        /// 人脸追踪测试
        /// </summary>
        private static void FaceTrackTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            Worker((sw, i) =>
            {
                var result = viewFace.FaceTrack(bitmap).ToList();
                logger.Info($"第{i + 1}次检测，结果：{result.Count}，{result[0].ToString()}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        /// <summary>
        /// 人脸特征值测试
        /// </summary>
        private static void ExtractTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            Worker((sw, i) =>
            {
                var result = viewFace.Extract(bitmap, GetFaceMarkPoint(viewFace, bitmap)).ToList();
                logger.Info($"第{i + 1}次{nameof(ViewFace.Extract)}检测，结果：{result.Count()}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        /// <summary>
        /// 年龄预测
        /// </summary>
        private static void FaceAgePredictorTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            Worker((sw, i) =>
            {
                var result = viewFace.FaceAgePredictor(bitmap, GetFaceMarkPoint(viewFace, bitmap));
                logger.Info($"第{i + 1}次{nameof(ViewFace.FaceAgePredictor)}检测，结果：{result}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        /// <summary>
        /// 性别预测
        /// </summary>
        private static void FaceGenderPredictorTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            Worker((sw, i) =>
            {
                var result = viewFace.FaceGenderPredictor(bitmap, GetFaceMarkPoint(viewFace, bitmap));
                logger.Info($"第{i + 1}次{nameof(ViewFace.FaceGenderPredictor)}检测，结果：{result}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        /// <summary>
        /// 眼睛状态检测
        /// </summary>
        private static void FaceEyeStateDetectorTest()
        {
            using SKBitmap bitmap = SKBitmap.Decode(imagePath);
            ViewFace viewFace = new ViewFace();
            Worker((sw, i) =>
            {
                var result = viewFace.FaceEyeStateDetector(bitmap, GetFaceMarkPoint(viewFace, bitmap));
                logger.Info($"第{i + 1}次{nameof(ViewFace.FaceEyeStateDetector)}检测，结果：{result.ToString()}，耗时：{sw.ElapsedMilliseconds}ms");
            });
        }

        #region Helpers

        private static IEnumerable<FaceMarkPoint> GetFaceMarkPoint(ViewFace viewFace, SKBitmap bitmap)
        {
            var infos = viewFace.FaceDetector(bitmap);
            var info = infos.First();
            return viewFace.FaceMark(bitmap, info).ToList();
        }

        private static IEnumerable<float> GetExtract(ViewFace viewFace, SKBitmap bitmap)
        {
            return viewFace.Extract(bitmap, GetFaceMarkPoint(viewFace, bitmap));
        }

        private static void Worker(Action<Stopwatch, int> action)
        {
            Stopwatch sw = new Stopwatch();
            sw.Start();
            int i = 0;
            while (true)
            {
                sw.Restart();
                action(sw, i);
                sw.Stop();
                i++;
            }
        }

        #endregion
    }
}