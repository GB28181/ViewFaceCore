using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;
using ViewFaceCore;
using ViewFaceCore.Extension;

namespace ViewFaceCoreTest
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
            using Bitmap bitmap = (Bitmap)Image.FromFile(@"images\Jay_3.jpg");
            var buffer = bitmap.To24BGRByteArray(out int width, out int height, out int channels);
            using var image = new ViewFaceCore.Model.FaceImage(width, height, channels, buffer);
            ViewFace viewFace = new ViewFace();

            for (int i = 0; i < 100; i++)
            {
                var infos = viewFace.FaceDetector(image);
                System.Console.WriteLine(string.Join(System.Environment.NewLine, infos.Select(x => $"{i}. {x.Score} - {{{x.Location.X},{x.Location.Y}}} - {{{x.Location.Width},{x.Location.Height}}}")));
            }
        }
    }


    internal static class BitmapExtension
    {
        /// <summary>
        /// <see cref="Bitmap"/> תΪ 3*8bit BGR <see cref="byte"/> ���顣
        /// </summary>
        /// <param name="bitmap">��ת��ͼ��</param>
        /// <param name="width">ͼ����</param>
        /// <param name="height">ͼ��߶�</param>
        /// <param name="channels">ͼ��ͨ��</param>
        /// <returns>ͼ��� BGR <see cref="byte"/> ����</returns>
        public static byte[] To24BGRByteArray(this Bitmap bitmap, out int width, out int height, out int channels)
        {
            width = bitmap.Width;
            height = bitmap.Height;
            channels = 3;
            BitmapData bitmapData = bitmap.LockBits(new Rectangle(0, 0, bitmap.Width, bitmap.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppRgb);
            try
            {
                int num = bitmap.Height * bitmapData.Stride;
                byte[] array = new byte[num];
                Marshal.Copy(bitmapData.Scan0, array, 0, num);
                byte[] bgra = new byte[array.Length / 4 * channels];
                // brga
                int j = 0;
                for (int i = 0; i < array.Length; i++)
                {
                    if ((i + 1) % 4 == 0) continue;
                    bgra[j] = array[i];
                    j++;
                }
                return bgra;
            }
            finally
            {
                bitmap.UnlockBits(bitmapData);
            }
        }
    }
}