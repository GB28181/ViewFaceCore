﻿#if NET45_OR_GREATER || NETCOREAPP || NETSTANDARD

using System;
using System.Threading.Tasks;
using ViewFaceCore.Core.Interface;
using ViewFaceCore.Exceptions;
using ViewFaceCore.Model;

namespace ViewFaceCore
{
    /// <summary>
    /// 异步扩展，对于 CPU 绑定的操作直接使用 <see cref="Task.Run(Action)"/> 进行包装。
    /// <para>参考: <a href="https://docs.microsoft.com/zh-cn/dotnet/standard/async-in-depth#deeper-dive-into-task-and-taskt-for-a-cpu-bound-operation">深入了解绑定 CPU 的操作的任务和 Task&lt;T&gt;</a></para>
    /// </summary>
    public static class ViewFaceAsyncExtensions
    {
        /// <summary>
        /// 识别 <paramref name="image"/> 中的人脸，并返回人脸的信息。
        /// <para><see cref="BaseViewFace.FaceDetector(FaceImage)"/> 的异步版本。</para>
        /// <para>
        /// 可以通过 <see cref="BaseViewFace.DetectorConfig"/> 属性对人脸检测器进行配置，以应对不同场景的图片。
        /// </para>
        /// <para>
        /// 当 <c><see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Normal"/> <see langword="||"/> <see cref="FaceType.Light"/></c> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_detector">ViewFaceCore.model.face_detector</a><br/>
        /// 当 <c><see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Mask"/></c> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.mask_detector">ViewFaceCore.model.mask_detector</a><br/>
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <returns>人脸信息集合。若 <see cref="Array.Length"/> == 0 ，代表未检测到人脸信息。如果图片中确实有人脸，可以修改 <see cref="BaseViewFace.DetectorConfig"/> 重新检测。</returns>
        public static async Task<FaceInfo[]> FaceDetectorAsync<T>(this BaseViewFace face, T image) where T : class
            => await Task.Run(() => face.FaceDetector(image));

        /// <summary>
        /// 识别 <paramref name="image"/> 中指定的人脸信息 <paramref name="info"/> 的关键点坐标。
        /// <para><see cref="BaseViewFace.FaceMark(FaceImage, FaceInfo)"/> 的异步版本。</para>
        /// <para>
        /// 当 <see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Normal"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_landmarker_pts68">ViewFaceCore.model.face_landmarker_pts68</a><br/>
        /// 当 <see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Mask"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_landmarker_mask_pts5">ViewFaceCore.model.face_landmarker_mask_pts5</a><br/>
        /// 当 <see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Light"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_landmarker_pts5">ViewFaceCore.model.face_landmarker_pts5</a><br/>
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="info">指定的人脸信息</param>
        /// <exception cref="MarkException"/>
        /// <returns>若失败，则返回结果 Length == 0</returns>
        public static async Task<FaceMarkPoint[]> FaceMarkAsync<T>(this BaseViewFace face, T image, FaceInfo info) where T : class
            => await Task.Run(() => face.FaceMark(image, info));

        /// <summary>
        /// 提取人脸特征值。
        /// <para><see cref="BaseViewFace.Extract(FaceImage, FaceMarkPoint[])"/> 的异步版本。</para>
        /// <para>
        /// 当 <see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Normal"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_recognizer">ViewFaceCore.model.face_recognizer</a><br/>
        /// 当 <see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Mask"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_recognizer_mask">ViewFaceCore.model.face_recognizer_mask</a><br/>
        /// 当 <see cref="FaceType"/> <see langword="="/> <see cref="FaceType.Light"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.face_recognizer_light">ViewFaceCore.model.face_recognizer_light</a><br/>
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="points"></param>
        /// <returns></returns>
        public static async Task<float[]> ExtractAsync<T>(this BaseViewFace face, T image, FaceMarkPoint[] points) where T : class
            => await Task.Run(() => face.Extract(image, points));

        /// <summary>
        /// 活体检测器。
        /// <para><see cref="BaseViewFace.AntiSpoofing(FaceImage, FaceInfo, FaceMarkPoint[], bool)"/> 的异步版本。</para>
        /// <para>
        /// 单帧图片，由 <paramref name="global"/> 指定是否启用全局检测能力 <br />
        /// 需通过 <see cref="BaseViewFace.FaceDetector(FaceImage)"/> 获取 <paramref name="info"/> 参数<br/>
        /// 通过 <see cref="BaseViewFace.FaceMark(FaceImage, FaceInfo)"/> 获取与 <paramref name="info"/> 参数对应的 <paramref name="points"/>
        /// </para>
        /// <para>
        /// 当 <paramref name="global"/> <see langword="= false"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.fas_first">ViewFaceCore.model.fas_first</a><br/>
        /// 当 <paramref name="global"/> <see langword="= true"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.fas_second">ViewFaceCore.model.fas_second</a>
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="info"></param>
        /// <param name="points"></param>
        /// <param name="global"></param>
        /// <returns></returns>
        public static async Task<AntiSpoofingStatus> AntiSpoofingAsync<T>(this BaseViewFace face, T image, FaceInfo info, FaceMarkPoint[] points, bool global = false) where T : class
            => await Task.Run(() => face.AntiSpoofing(image, info, points, global));

        /// <summary>
        /// 活体检测器。
        /// <para><see cref="BaseViewFace.AntiSpoofingVideo(FaceImage, FaceInfo, FaceMarkPoint[], bool)"/> 的异步版本。</para>
        /// <para>
        /// 视频帧图片，由 <paramref name="global"/> 指定是否启用全局检测能力 <br />
        /// 需通过 <see cref="BaseViewFace.FaceDetector(FaceImage)"/> 获取 <paramref name="info"/> 参数<br/>
        /// 通过 <see cref="BaseViewFace.FaceMark(FaceImage, FaceInfo)"/> 获取与 <paramref name="info"/> 参数对应的 <paramref name="points"/>
        /// </para>
        /// <para>
        /// 当 <paramref name="global"/> <see langword="= false"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.fas_first">ViewFaceCore.model.fas_first</a><br/>
        /// 当 <paramref name="global"/> <see langword="= true"/> 时， 需要模型：<a href="https://www.nuget.org/packages/ViewFaceCore.model.fas_second">ViewFaceCore.model.fas_second</a>
        /// </para>
        /// <para>如果返回结果为 <see cref="AntiSpoofingStatus.Detecting"/>，则说明需要继续调用此方法，传入更多的图片</para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="info"></param>
        /// <param name="points"></param>
        /// <param name="global">是否启用全局检测能力</param>
        /// <returns></returns>
        public static async Task<AntiSpoofingStatus> AntiSpoofingVideoAsync<T>(this BaseViewFace face, T image, FaceInfo info, FaceMarkPoint[] points, bool global) where T : class
            => await Task.Run(() => face.AntiSpoofingVideo(image, info, points, global));

        /// <summary>
        /// 人脸质量评估
        /// <para><see cref="BaseViewFace.FaceQuality(FaceImage, FaceInfo, FaceMarkPoint[], QualityType)"/> 的异步版本。</para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="info"></param>
        /// <param name="points"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        public static async Task<QualityResult> FaceQualityAsync<T>(this BaseViewFace face, T image, FaceInfo info, FaceMarkPoint[] points, QualityType type) where T : class
            => await Task.Run(() => face.FaceQuality(image, info, points, type));

        /// <summary>
        /// 年龄预测。
        /// <para>
        /// <see cref="BaseViewFace.FaceAgePredictor(FaceImage, FaceMarkPoint[])"/> 的异步版本。<br />
        /// 需要模型 <a href="https://www.nuget.org/packages/ViewFaceCore.model.age_predictor">ViewFaceCore.model.age_predictor</a> 
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <returns></returns>
        public static async Task<int> FaceAgePredictorAsync<T>(this BaseViewFace face, T image, FaceMarkPoint[] points) where T : class
            => await Task.Run(() => face.FaceAgePredictor(image, points));

        /// <summary>
        /// 性别预测。
        /// <para>
        /// <see cref="BaseViewFace.FaceGenderPredictor(FaceImage, FaceMarkPoint[])"/> 的异步版本。<br />
        /// 需要模型 <a href="https://www.nuget.org/packages/ViewFaceCore.model.gender_predictor">ViewFaceCore.model.gender_predictor</a> 
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <returns></returns>
        public static async Task<Gender> FaceGenderPredictorAsync<T>(this BaseViewFace face, T image, FaceMarkPoint[] points) where T : class
            => await Task.Run(() => face.FaceGenderPredictor(image, points));

        /// <summary>
        /// 眼睛状态检测。
        /// <para>
        /// <see cref="BaseViewFace.FaceEyeStateDetector(FaceImage, FaceMarkPoint[])"/> 的异步版本。<br />
        /// 眼睛的左右是相对图片内容而言的左右 <br />
        /// 需要模型 <a href="https://www.nuget.org/packages/ViewFaceCore.model.eye_state">ViewFaceCore.model.eye_state</a> 
        /// </para>
        /// </summary>
        /// <param name="face"></param>
        /// <param name="image">人脸图像信息</param>
        /// <param name="points">人脸关键点 数组</param>
        /// <returns></returns>
        public static async Task<EyeStateResult> FaceEyeStateDetectorAsync<T>(this BaseViewFace face, T image, FaceMarkPoint[] points) where T : class
            => await Task.Run(() => face.FaceEyeStateDetector(image, points));
    }
}

#endif