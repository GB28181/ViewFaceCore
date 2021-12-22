#include "environment.h"


#ifndef STDCALL
#define STDCALL 
#endif // !STDCALL

#ifndef View_Api
#define View_Api 
#endif // !View_Api


#include "seeta/FaceDetector.h"
#include "seeta/FaceLandmarker.h"
#include "seeta/FaceRecognizer.h"
#include "seeta/FaceAntiSpoofing.h"
#include "seeta/FaceTracker.h"

#include "seeta/QualityOfBrightness.h"
#include "seeta/QualityOfClarity.h"
#include "seeta/QualityOfIntegrity.h"
#include "seeta/QualityOfPose.h"
#include "seeta/QualityOfPoseEx.h"
#include "seeta/QualityOfResolution.h"

#include "seetaEx/QualityOfClarityEx.h"
#include "seetaEx/QualityOfNoMask.h"

#include "seeta/AgePredictor.h"
#include "seeta/GenderPredictor.h"
#include "seeta/EyeStateDetector.h"

#include <time.h>
#include <iostream>

using namespace std;

string modelPath = "./models/"; // ģ������·��
LogCallBack logger = NULL; // ��־�ص�����

/***************************************************************************************************************/
// ��ӡ��־
void WriteLog(string str) {
	if (logger == NULL) { cout << str << endl; }
	else { logger(str.c_str()); }
}
// ��ӡָ������������һ����Ϣ
void WriteMessage(string fanctionName, string message) { WriteLog(fanctionName + "\t Message:" + message); }
// ��ӡָ������������ģ������
void WriteModelName(string fanctionName, string modelName) { WriteLog(fanctionName + "\t Model.Name:" + modelName); }
// ��ӡָ����������������ʱ��
void WriteRunTime(string fanctionName, clock_t start) { WriteLog(fanctionName + "\t Run.Time:" + to_string(clock() - start) + " ms"); }
// ��ӡָ�����������Ĵ�����Ϣ
void WriteError(string fanctionName, const std::exception& e) { WriteLog(fanctionName + "\t Error:" + e.what()); }

/***************************************************************************************************************/

// ע����־�ص�����
/// <summary>
/// ע����־�ص�����
/// </summary>
/// <param name="writeLog">�ص�����</param>
View_Api void V_SetLogFunction(LogCallBack writeLog)
{
	logger = writeLog;
	WriteMessage("SetLogFunction", "Successed.");
}
// ��������ģ��Ŀ¼
/// <summary>
/// ��������ģ��Ŀ¼
/// </summary>
/// <param name="path">����ģ��Ŀ¼</param>
View_Api void V_SetModelPath(const char* path)
{
	modelPath = path;
	WriteMessage("SetModelPath", "Model.Path:" + modelPath);
}
// ��ȡ����ģ��Ŀ¼
/// <summary>
/// ��ȡ����ģ��Ŀ¼
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
View_Api bool V_GetModelPath(char** path)
{
	try
	{
		strcpy(*path, modelPath.c_str());
		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("GetModelPath", e);
		return false;
	}
}

/***************************************************************************************************************/
// ���������
seeta::FaceDetector* v_faceDetector = NULL;
// ���������
static SeetaFaceInfoArray detectorInfos;
// ��ȡ��������
/// <summary>
/// ��ȡ��������
/// </summary>
/// <param name="imgData">ͼ�� BGR ����</param>
/// <param name="width">ͼ�� ���</param>
/// <param name="height">ͼ�� �߶�</param>
/// <param name="channels">ͼ�� ͨ����</param>
/// <param name="faceSize">��С�����ߴ�</param>
/// <param name="threshold">�������Ŷ�</param>
/// <param name="maxWidth">�ɼ��������</param>
/// <param name="maxHeight">�ɼ������߶�</param>
/// <param name="type">ģ�����͡�0��face_detector��1��mask_detector��2��face_detector��</param>
/// <returns></returns>
View_Api int V_DetectorSize(unsigned char* imgData, SeetaImageData& img, double faceSize = 20, double threshold = 0.9, double maxWidth = 2000, double maxHeight = 2000, int type = 0)
{
	try {
		clock_t start = clock();

		img.data = imgData;
		if (v_faceDetector == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_detector.csta";
			if (type == 1) { modelName = "mask_detector.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("DetectorSize", modelName);
			v_faceDetector = new seeta::FaceDetector(setting);
		}

		v_faceDetector->set(seeta::FaceDetector::Property::PROPERTY_MIN_FACE_SIZE, faceSize);
		v_faceDetector->set(seeta::FaceDetector::Property::PROPERTY_THRESHOLD, threshold);
		v_faceDetector->set(seeta::FaceDetector::Property::PROPERTY_MAX_IMAGE_WIDTH, maxWidth);
		v_faceDetector->set(seeta::FaceDetector::Property::PROPERTY_MAX_IMAGE_HEIGHT, maxHeight);

		auto infos = v_faceDetector->detect(img);
		detectorInfos = infos;

		WriteRunTime("Detector", start); // �˷����Ѿ�����������ȫ���̣��ʼ�ʱ����ʾΪ ����ʶ�𷽷�
		return infos.size;
	}
	catch (const std::exception& e)
	{
		WriteError("DetectorSize", e);
		return -1;
	}
}
// ���������
/// <summary>
/// ���������
/// </summary>
/// <param name="score">�������Ŷȷ��� ����</param>
/// <param name="x">����λ�� x ����</param>
/// <param name="y">����λ�� y ����</param>
/// <param name="width">������С width ����</param>
/// <param name="height">������С height ����</param>
/// <returns></returns>
View_Api bool V_Detector(float* score, int* x, int* y, int* width, int* height)
{
	try
	{
		//clock_t start = clock();

		for (int i = 0; i < detectorInfos.size; i++, detectorInfos.data++)
		{
			*score = detectorInfos.data->score;
			*x = detectorInfos.data->pos.x;
			*y = detectorInfos.data->pos.y;
			*width = detectorInfos.data->pos.width;
			*height = detectorInfos.data->pos.height;
			score++, x++, y++, width++, height++;
		}
		detectorInfos.data = NULL;
		detectorInfos.size = NULL;

		//WriteRunTime(__FUNCDNAME__, start); // �˷���ֻ�ǽ� ������������� ��ȡ�������ݸ�ֵ���ݣ�������ʱ���ʲ���ʾ�˷����ĵ���ʱ��
		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("Detector", e);
		return false;
	}
}

/***************************************************************************************************************/
// �����ؼ�����
seeta::FaceLandmarker* v_faceLandmarker = NULL;
// �����ؼ�������
/// <summary>
/// �����ؼ�������
/// </summary>
/// <param name="type">ģ�����͡�0��face_landmarker_pts68��1��face_landmarker_mask_pts5��2��face_landmarker_pts5��</param>
/// <returns></returns>
View_Api int V_FaceMarkSize(int type = 0)
{
	try
	{
		clock_t start = clock();

		if (v_faceLandmarker == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_landmarker_pts68.csta";
			if (type == 1) { modelName = "face_landmarker_mask_pts5.csta"; }
			if (type == 2) { modelName = "face_landmarker_pts5.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("FaceMarkSize", modelName);
			v_faceLandmarker = new seeta::FaceLandmarker(setting);
		}
		int size = v_faceLandmarker->number();

		WriteRunTime("FaceMarkSize", start);
		return size;
	}
	catch (const std::exception& e)
	{
		WriteError("FaceMarkSize", e);
		return -1;
	}
}
// ��ȡ�����ؼ���
/// <summary>
/// ��ȡ�����ؼ���
/// </summary>
/// <param name="imgData">ͼ�� BGR ����</param>
/// <param name="width">ͼ�� ���</param>
/// <param name="height">ͼ�� �߶�</param>
/// <param name="channels">ͼ�� ͨ����</param>
/// <param name="x">����λ�� X</param>
/// <param name="y">����λ�� Y</param>
/// <param name="fWidth">������С width</param>
/// <param name="fHeight">������С height</param>
/// <param name="pointX">�洢�ؼ��� x ����� ����</param>
/// <param name="pointY">�洢�ؼ��� y ����� ����</param>
/// <param name="type">ģ�����͡�0��face_landmarker_pts68��1��face_landmarker_mask_pts5��2��face_landmarker_pts5��</param>
/// <returns></returns>
View_Api bool V_FaceMark(unsigned char* imgData, SeetaImageData& img, SeetaRect faceRect, double* pointX, double* pointY, int type = 0)
{
	try
	{
		clock_t start = clock();

		img.data = imgData;
		if (v_faceLandmarker == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_landmarker_pts68.csta";
			if (type == 1) { modelName = "face_landmarker_mask_pts5.csta"; }
			if (type == 2) { modelName = "face_landmarker_pts5.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("FaceMark", modelName);
			v_faceLandmarker = new seeta::FaceLandmarker(setting);
		}
		std::vector<SeetaPointF> _points = v_faceLandmarker->mark(img, faceRect);

		if (!_points.empty()) {
			for (auto iter = _points.begin(); iter != _points.end(); iter++)
			{
				*pointX = (*iter).x;
				*pointY = (*iter).y;
				pointX++;
				pointY++;
			}

			WriteRunTime("FaceMark", start);
			return true;
		}
		else { return false; }
	}
	catch (const std::exception& e)
	{
		WriteError("FaceMark", e);
		return false;
	}
}

/***************************************************************************************************************/
// ��������ֵ��
seeta::FaceRecognizer* v_faceRecognizer = NULL;
// ��ȡ��������ֵ����
/// <summary>
/// ��ȡ��������ֵ����
/// </summary>
/// <param name="type">ģ�����͡�0��face_recognizer��1��face_recognizer_mask��2��face_recognizer_light��</param>
/// <returns></returns>
View_Api int V_ExtractSize(int type = 0)
{
	try
	{
		clock_t start = clock();

		if (v_faceRecognizer == NULL) {
			seeta::ModelSetting setting;
			setting.set_id(0);
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_recognizer.csta";
			if (type == 1) { modelName = "face_recognizer_mask.csta"; }
			if (type == 2) { modelName = "face_recognizer_light.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("ExtractSize", modelName);
			v_faceRecognizer = new seeta::FaceRecognizer(setting);
		}
		int length = v_faceRecognizer->GetExtractFeatureSize();

		WriteRunTime("ExtractSize", start);
		return length;
	}
	catch (const std::exception& e)
	{
		WriteError("ExtractSize", e);
		return -1;
	}
}
// ��ȡ��������ֵ
/// <summary>
/// ��ȡ��������ֵ
/// </summary>
/// <param name="imgData">ͼ�� BGR ����</param>
/// <param name="width">ͼ�� ���</param>
/// <param name="height">ͼ�� �߶�</param>
/// <param name="channels">ͼ�� ͨ����</param>
/// <param name="points">�����ؼ��� ����</param>
/// <param name="features">��������ֵ ����</param>
/// <param name="type">ģ�����͡�0��face_recognizer��1��face_recognizer_mask��2��face_recognizer_light��</param>
/// <returns></returns>
View_Api bool V_Extract(unsigned char* imgData, SeetaImageData& img, SeetaPointF* points, float* features, int type = 0)
{
	try
	{
		clock_t start = clock();

		img.data = imgData;
		if (v_faceRecognizer == NULL) {
			seeta::ModelSetting setting;
			setting.set_id(0);
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_recognizer.csta";
			if (type == 1) { modelName = "face_recognizer_mask.csta"; }
			if (type == 2) { modelName = "face_recognizer_light.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("Extract", modelName);
			v_faceRecognizer = new seeta::FaceRecognizer(setting);
		}
		int length = v_faceRecognizer->GetExtractFeatureSize();
		std::shared_ptr<float> _features(new float[v_faceRecognizer->GetExtractFeatureSize()], std::default_delete<float[]>());
		v_faceRecognizer->Extract(img, points, _features.get());

		for (int i = 0; i < length; i++)
		{
			*features = _features.get()[i];
			features++;
		}

		WriteRunTime("Extract", start);
		return true;

	}
	catch (const std::exception& e)
	{
		WriteError("Extract", e);
		return false;
	}
}
// ��������ֵ���ƶȼ���
View_Api float V_CalculateSimilarity(float* leftFeatures, float* rightFeatures, int type = 0)
{
	try
	{
		clock_t start = clock();

		if (v_faceRecognizer == NULL) {
			seeta::ModelSetting setting;
			setting.set_id(0);
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_recognizer.csta";
			if (type == 1) { modelName = "face_recognizer_mask.csta"; }
			if (type == 2) { modelName = "face_recognizer_light.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("CalculateSimilarity", modelName);
			v_faceRecognizer = new seeta::FaceRecognizer(setting);
		}

		auto similarity = v_faceRecognizer->CalculateSimilarity(leftFeatures, rightFeatures);
		WriteMessage("CalculateSimilarity", "Similarity = " + to_string(similarity));
		WriteRunTime("CalculateSimilarity", start);
		return similarity;
	}
	catch (const std::exception& e)
	{
		WriteError("CalculateSimilarity", e);
		return -1;
	}
}

/***************************************************************************************************************/
// ��������
seeta::FaceAntiSpoofing* v_faceAntiSpoofing = NULL;
// ������ - ��֡
/// <summary>
/// ������ - ��֡
/// </summary>
/// <param name="imgData">ͼ�� BGR ����</param>
/// <param name="width">ͼ�� ���</param>
/// <param name="height">ͼ�� �߶�</param>
/// <param name="channels">ͼ�� ͨ����</param>
/// <param name="x">�������� x</param>
/// <param name="y">�������� y</param>
/// <param name="fWidth">������С width</param>
/// <param name="fHeight">������С height</param>
/// <param name="points">�����ؼ��� ����</param>
/// <param name="global">�Ƿ�����ȫ�ּ������</param>
/// <returns></returns>
View_Api int V_AntiSpoofing(unsigned char* imgData, SeetaImageData& img, SeetaRect faceRect, SeetaPointF* points, bool global)
{
	try
	{
		clock_t start = clock();

		img.data = imgData;
		if (v_faceAntiSpoofing == NULL) {
			seeta::ModelSetting setting;
			setting.set_id(0);
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "fas_first.csta";
			setting.append(modelPath + modelName);
			if (global) { // ����ȫ�ּ������
				modelName = "fas_second.csta";
				setting.append(modelPath + modelName);
				WriteModelName("AntiSpoofing", modelName);
			}
			WriteModelName("AntiSpoofing", modelName);
			v_faceAntiSpoofing = new seeta::FaceAntiSpoofing(setting);
		}

		auto status = v_faceAntiSpoofing->Predict(img, faceRect, points);

		WriteRunTime("AntiSpoofing", start);
		return status;
	}
	catch (const std::exception& e)
	{
		WriteError("AntiSpoofing", e);
		return -1;
	}
}
// ������ - ��Ƶ
/// <summary>
/// ������ - ��Ƶ
/// </summary>
/// <param name="imgData">ͼ�� BGR ����</param>
/// <param name="width">ͼ�� ���</param>
/// <param name="height">ͼ�� �߶�</param>
/// <param name="channels">ͼ�� ͨ����</param>
/// <param name="x">�������� x</param>
/// <param name="y">�������� y</param>
/// <param name="fWidth">������С width</param>
/// <param name="fHeight">������С height</param>
/// <param name="points">�����ؼ��� ����</param>
/// <param name="global">�Ƿ�����ȫ�ּ������</param>
/// <returns></returns>
View_Api int V_AntiSpoofingVideo(unsigned char* imgData, SeetaImageData& img, SeetaRect faceRect, SeetaPointF* points, bool global)
{
	try
	{
		clock_t start = clock();

		img.data = imgData;
		if (v_faceAntiSpoofing == NULL) {
			seeta::ModelSetting setting;
			setting.set_id(0);
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "fas_first.csta";
			setting.append(modelPath + modelName);
			if (global) { // ����ȫ�ּ������
				modelName = "fas_second.csta";
				setting.append(modelPath + modelName);
				WriteModelName("AntiSpoofingVideo", modelName);
			}
			WriteModelName("AntiSpoofingVideo", modelName);
			v_faceAntiSpoofing = new seeta::FaceAntiSpoofing(setting);
		}

		auto status = v_faceAntiSpoofing->PredictVideo(img, faceRect, points);

		WriteRunTime("AntiSpoofingVideo", start);
		return status;
	}
	catch (const std::exception& e)
	{
		WriteError("AntiSpoofingVideo", e);
		return -1;
	}
}

/***************************************************************************************************************/
// ����������
seeta::FaceTracker* v_faceTracker = NULL;
static SeetaTrackingFaceInfoArray trackingInfos;
/// <summary>
/// ��ȡ���ٵ���������
/// </summary>
/// <param name="imgData">ͼ�� BGR ����</param>
/// <param name="width">ͼ�� ���</param>
/// <param name="height">ͼ�� �߶�</param>
/// <param name="channels">ͼ�� ͨ����</param>
/// <param name="videoWidth">��Ƶ���</param>
/// <param name="videoHeight">��Ƶ�߶�</param>
/// <param name="type">ģ�����͡�0��face_detector��1��mask_detector��2��mask_detector��</param>
/// <returns></returns>
View_Api int V_FaceTrackSize(unsigned char* imgData, SeetaImageData& img,
	bool stable = false, int interval = 10, double faceSize = 20, double threshold = 0.9, int type = 0)
{
	try
	{
		clock_t start = clock();

		img.data = imgData;
		if (v_faceTracker == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "face_detector.csta";
			if (type == 1) { modelName = "mask_detector.csta"; }
			setting.append(modelPath + modelName);
			WriteModelName("FaceTrackSize", modelName);
			v_faceTracker = new seeta::FaceTracker(setting, img.width, img.height);
		}

		v_faceTracker->SetVideoStable(stable);
		v_faceTracker->SetMinFaceSize(faceSize);
		v_faceTracker->SetThreshold(threshold);
		v_faceTracker->SetInterval(interval);

		auto faceInfos = v_faceTracker->Track(img);

		trackingInfos = faceInfos;

		WriteRunTime("FaceTrack", start);
		return faceInfos.size;

	}
	catch (const std::exception& e)
	{
		WriteError("FaceTrackSize", e);
		return -1;
	}
}
/// <summary>
/// ����������Ϣ
/// </summary>
/// <param name="score">�������Ŷȷ��� ����</param>
/// <param name="PID">������ʶID ����</param>
/// <param name="x">����λ�� x ����</param>
/// <param name="y">����λ�� y ����</param>
/// <param name="width">������С width ����</param>
/// <param name="height">������С height ����</param>
/// <returns></returns>
View_Api bool V_FaceTrack(float* score, int* PID, int* x, int* y, int* width, int* height)
{
	try
	{
		for (int i = 0; i < trackingInfos.size; i++, trackingInfos.data++)
		{
			*score = trackingInfos.data->score;
			*PID = trackingInfos.data->PID;
			*x = trackingInfos.data->pos.x;
			*y = trackingInfos.data->pos.y;
			*width = trackingInfos.data->pos.width;
			*height = trackingInfos.data->pos.height;
			score++; PID++; x++; y++; width++; height++;
		}

		trackingInfos.data = NULL;
		trackingInfos.size = NULL;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("FaceTrack", e);
		return false;
	}
}

/***************************************************************************************************************/
// ����������
seeta::QualityOfBrightness* V_Quality_Brightness = NULL;
// ��������
View_Api bool V_QualityOfBrightness(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float v0 = 70, float v1 = 100, float v2 = 210, float v3 = 230)
{
	try
	{
		img.data = imgData;
		if (V_Quality_Brightness == NULL)
		{
			V_Quality_Brightness = new seeta::QualityOfBrightness(v0, v1, v2, v3);
		}
		auto result = V_Quality_Brightness->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfBrightness", e);
		return false;
	}
}

// ������������
seeta::QualityOfClarity* V_Quality_Clarity = NULL;
// ����������
View_Api bool V_QualityOfClarity(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float low = 0.1f, float high = 0.2f)
{
	try
	{
		img.data = imgData;
		if (V_Quality_Clarity == NULL)
		{
			V_Quality_Clarity = new seeta::QualityOfClarity(low, high);
		}
		auto result = V_Quality_Clarity->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfClarity", e);
		return false;
	}
}

// ������������
seeta::QualityOfIntegrity* V_Quality_Integrity = NULL;
// ����������
View_Api bool V_QualityOfIntegrity(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float low = 10, float high = 1.5f)
{
	try
	{
		img.data = imgData;
		if (V_Quality_Integrity == NULL)
		{
			WriteMessage("QualityOfIntegrity", "low:" + to_string(low) + " - high:" + to_string(high));
			V_Quality_Integrity = new seeta::QualityOfIntegrity(low, high);
		}
		auto result = V_Quality_Integrity->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfIntegrity", e);
		return false;
	}
}

// ��̬������
seeta::QualityOfPose* V_Quality_Pose = NULL;
// ��̬����
View_Api bool V_QualityOfPose(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score)
{
	try
	{
		img.data = imgData;
		if (V_Quality_Pose == NULL)
		{
			V_Quality_Pose = new seeta::QualityOfPose();
		}
		auto result = V_Quality_Pose->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfPose", e);
		return false;
	}
}

// ��̬ (���)������
seeta::QualityOfPoseEx* V_Quality_PoseEx = NULL;
// ��̬ (���)����
View_Api bool V_QualityOfPoseEx(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float yawLow = 25, float yawHigh = 10, float pitchLow = 20, float pitchHigh = 10, float rollLow = 33.33f, float rollHigh = 16.67f)
{
	try
	{
		img.data = imgData;
		if (V_Quality_PoseEx == NULL)
		{
			seeta::ModelSetting setting;
			string modelName = "pose_estimation.csta";
			setting.append(modelPath + modelName);
			WriteModelName("QualityOfPoseEx", modelName);
			V_Quality_PoseEx = new seeta::QualityOfPoseEx(setting);
		}

		V_Quality_PoseEx->set(seeta::QualityOfPoseEx::YAW_LOW_THRESHOLD, yawLow);
		V_Quality_PoseEx->set(seeta::QualityOfPoseEx::YAW_HIGH_THRESHOLD, yawHigh);
		V_Quality_PoseEx->set(seeta::QualityOfPoseEx::PITCH_LOW_THRESHOLD, pitchLow);
		V_Quality_PoseEx->set(seeta::QualityOfPoseEx::PITCH_HIGH_THRESHOLD, pitchHigh);
		V_Quality_PoseEx->set(seeta::QualityOfPoseEx::ROLL_LOW_THRESHOLD, rollLow);
		V_Quality_PoseEx->set(seeta::QualityOfPoseEx::ROLL_HIGH_THRESHOLD, rollHigh);

		auto result = V_Quality_PoseEx->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfPoseEx", e);
		return false;
	}
}

// �ֱ���������
seeta::QualityOfResolution* V_Quality_Resolution = NULL;
// �ֱ�������
View_Api bool V_QualityOfResolution(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float low = 80, float high = 120)
{
	try
	{
		img.data = imgData;
		if (V_Quality_Resolution == NULL)
		{
			V_Quality_Resolution = new seeta::QualityOfResolution(low, high);
		}
		auto result = V_Quality_Resolution->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfResolution", e);
		return false;
	}
}

// ������ (���)������
seeta::QualityOfClarityEx* V_Quality_ClarityEx = NULL;
// ������ (���)����
View_Api bool V_QualityOfClarityEx(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float blur_thresh = 0.8f)
{
	try
	{
		img.data = imgData;
		if (V_Quality_ClarityEx == NULL)
		{
			V_Quality_ClarityEx = new seeta::QualityOfClarityEx(blur_thresh, modelPath);
		}
		auto result = V_Quality_ClarityEx->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfClarity", e);
		return false;
	}
}

// �ڵ�������
seeta::QualityOfNoMask* V_Quality_NoMask = NULL;
// �ڵ�����
View_Api bool V_QualityOfNoMask(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score)
{
	try
	{
		img.data = imgData;
		if (V_Quality_NoMask == NULL)
		{
			V_Quality_NoMask = new seeta::QualityOfNoMask(modelPath);
		}
		auto result = V_Quality_NoMask->check(img, faceRect, points, pointsLength);

		*level = result.level;
		*score = result.score;

		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("QualityOfNoMask", e);
		return false;
	}
}

/******��������***********************************************************************************************/
// ����Ԥ����
seeta::AgePredictor* V_Age_Predictor = NULL;
// ����Ԥ��
View_Api int V_AgePredictor(unsigned char* imgData, SeetaImageData& img,
	SeetaPointF* points, int pointsLength)
{
	try
	{
		img.data = imgData;
		if (V_Age_Predictor == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "age_predictor.csta";
			setting.append(modelPath + modelName);
			WriteModelName("AgePredictor", modelName);
			V_Age_Predictor = new seeta::AgePredictor(setting);
		}
		int age = 0;
		auto result = V_Age_Predictor->PredictAgeWithCrop(img, points, age);
		if (result)
			return age;
		else return -1;
	}
	catch (const std::exception& e)
	{
		WriteError("AgePredictor", e);
		return -1;
	}
}

// ����Ԥ����
seeta::GenderPredictor* V_Gender_Predictor = NULL;
// ����Ԥ��
View_Api int V_GenderPredictor(unsigned char* imgData, SeetaImageData& img,
	SeetaPointF* points, int pointsLength)
{
	try
	{
		img.data = imgData;
		if (V_Gender_Predictor == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "gender_predictor.csta";
			setting.append(modelPath + modelName);
			WriteModelName("GenderPredictor", modelName);
			V_Gender_Predictor = new seeta::GenderPredictor(setting);
		}
		seeta::GenderPredictor::GENDER gender = (seeta::GenderPredictor::GENDER)0;
		auto result = V_Gender_Predictor->PredictGenderWithCrop(img, points, gender);
		if (result)
			return gender;
		else return -1;
	}
	catch (const std::exception& e)
	{
		WriteError("GenderPredictor", e);
		return -1;
	}
}

// ����Ԥ����
seeta::EyeStateDetector* V_EyeState_Detector = NULL;
// ����Ԥ��
View_Api bool V_EyeStateDetector(unsigned char* imgData, SeetaImageData& img, SeetaPointF* points, int pointsLength,
	seeta::EyeStateDetector::EYE_STATE& left_eye, seeta::EyeStateDetector::EYE_STATE& right_eye)
{
	try
	{
		img.data = imgData;
		if (V_EyeState_Detector == NULL) {
			seeta::ModelSetting setting;
			setting.set_device(SEETA_DEVICE_CPU);
			string modelName = "eye_state.csta";
			setting.append(modelPath + modelName);
			WriteModelName("EyeStateDetector", modelName);
			V_EyeState_Detector = new seeta::EyeStateDetector(setting);
		}

		V_EyeState_Detector->Detect(img, points, left_eye, right_eye);
		return true;
	}
	catch (const std::exception& e)
	{
		WriteError("EyeStateDetector", e);
		return false;
	}
}
/***************************************************************************************************************/

// �ͷ���Դ
View_Api void V_Dispose()
{
	if (v_faceDetector != NULL) delete v_faceDetector;
	if (v_faceLandmarker != NULL) delete v_faceLandmarker;
	if (v_faceRecognizer != NULL) delete v_faceRecognizer;
	if (v_faceAntiSpoofing != NULL) delete v_faceAntiSpoofing;
	if (v_faceTracker != NULL) delete v_faceTracker;

	if (V_Quality_Brightness != NULL) delete V_Quality_Brightness;
	if (V_Quality_Clarity != NULL) delete V_Quality_Clarity;
	if (V_Quality_Integrity != NULL) delete V_Quality_Integrity;
	if (V_Quality_Pose != NULL) delete V_Quality_Pose;
	if (V_Quality_PoseEx != NULL) delete V_Quality_PoseEx;
	if (V_Quality_Resolution != NULL) delete V_Quality_Resolution;
	if (V_Quality_ClarityEx != NULL) delete V_Quality_ClarityEx;
	if (V_Quality_NoMask != NULL) delete V_Quality_NoMask;

	if (V_Age_Predictor != NULL) delete V_Age_Predictor;
	if (V_Gender_Predictor != NULL) delete V_Gender_Predictor;
	if (V_EyeState_Detector != NULL) delete V_EyeState_Detector;
}