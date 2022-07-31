﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ViewFaceCore.Model;
using ViewFaceCore.Native;

namespace ViewFaceCore.Core.Predictor
{
    /// <summary>
    /// 性别预测。
    /// 需要模型 <a href="https://www.nuget.org/packages/ViewFaceCore.model.gender_predictor">gender_predictor.csta</a>
    /// </summary>
    public sealed class GenderPredictor : BaseViewFace, IPredictor
    {
        private readonly IntPtr _handle = IntPtr.Zero;
        private readonly static object _locker = new object();

        public GenderPredictor()
        {
            _handle = ViewFaceNative.GetGenderPredictorHandler();
            if (_handle == IntPtr.Zero)
            {
                throw new Exception("Get gender predictor handler failed.");
            }
        }

        /// <summary>
        /// 性别预测。
        /// <para>
        /// 需要模型 <a href="https://www.nuget.org/packages/ViewFaceCore.model.gender_predictor">gender_predictor.csta</a>
        /// </para>
        /// </summary>
        /// <param name="image">人脸图像信息</param>
        /// <param name="points">关键点坐标<para>通过 <see cref="FaceMark(FaceImage, FaceInfo)"/> 获取</para></param>
        /// <returns>性别。<see cref="Gender.Unknown"/> 代表识别失败</returns>
        public int PredictGender(FaceImage image, FaceMarkPoint[] points)
        {
            lock (_locker)
            {
                return ViewFaceNative.PredictGender(_handle, ref image, points);
            }
        }

        public void Dispose()
        {
            lock (_locker)
            {
                ViewFaceNative.DisposeGenderPredictor(_handle);
            }
        }
    }
}
