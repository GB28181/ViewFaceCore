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

#include <iostream>
#include <string>

using namespace std;
using namespace seeta;

typedef void(STDCALL* LogCallBack)(const char* logText);
LogCallBack logger = nullptr; // ��־�ص�����

string modelPath = "./models/"; // ģ������·��

// ��ӡ��־
void WriteLog(string str) {
	if (logger == nullptr) { cout << str << endl; }
	else { logger(str.c_str()); }
}
// ��ӡָ������������һ����Ϣ
void WriteMessage(string fanctionName, string message) { WriteLog(fanctionName + "\t Message:" + message); }

// ע����־�ص�����
View_Api void SetLogFunction(LogCallBack writeLog)
{
	logger = writeLog;
}
// ��������ģ��Ŀ¼
View_Api void SetModelPath(const char* path)
{
	modelPath = path;
	WriteMessage("SetModelPath", "Model.Path:" + modelPath);
}

// ��ȡ����ģ��Ŀ¼
View_Api void GetModelPath(char** path)
{
	strcpy(*path, modelPath.c_str());
}

// �ͷ��� malloc ������ڴ�
View_Api void Free(void* address) {
	free(address);
}

// ��ȡ��������
View_Api SeetaFaceInfo* Detector(SeetaImageData& img, int* size, double faceSize = 20, double threshold = 0.9, double maxWidth = 2000, double maxHeight = 2000, int type = 0)
{
	auto faceDetector = new FaceDetector(ModelSetting(modelPath + (type == 0 ? "face_detector.csta" : "mask_detector.csta")));

	faceDetector->set(FaceDetector::Property::PROPERTY_MIN_FACE_SIZE, faceSize);
	faceDetector->set(FaceDetector::Property::PROPERTY_THRESHOLD, threshold);
	faceDetector->set(FaceDetector::Property::PROPERTY_MAX_IMAGE_WIDTH, maxWidth);
	faceDetector->set(FaceDetector::Property::PROPERTY_MAX_IMAGE_HEIGHT, maxHeight);

	auto infos = faceDetector->detect(img);
	delete faceDetector;
	*size = infos.size;
	return infos.data;
}

// �����ؼ�����
View_Api SeetaPointF* FaceMark(SeetaImageData& img, SeetaRect faceRect, int* size, int type = 0)
{
	string modelName = "face_landmarker_pts68.csta";
	if (type == 1) { modelName = "face_landmarker_mask_pts5.csta"; }
	if (type == 2) { modelName = "face_landmarker_pts5.csta"; }
	auto faceLandmarker = new FaceLandmarker(ModelSetting(modelPath + modelName, SEETA_DEVICE_AUTO));

	*size = faceLandmarker->number();
	std::vector<SeetaPointF> _points = faceLandmarker->mark(img, faceRect);
	delete faceLandmarker;

	*size = _points.size();
	if (!_points.empty()) {
		SeetaPointF* points = (SeetaPointF*)malloc((*size) * sizeof(SeetaPointF));
		SeetaPointF* start = points;
		for (auto iter = _points.begin(); iter != _points.end(); iter++)
		{
			*start = *iter;
			start++;
		}
		return points;
	}
	return 0;
}

// ��ȡ��������ֵ
View_Api float* Extract(SeetaImageData& img, SeetaPointF* points, int* size, int type = 0)
{
	string modelName = "face_recognizer.csta";
	if (type == 1) { modelName = "face_recognizer_mask.csta"; }
	if (type == 2) { modelName = "face_recognizer_light.csta"; }
	auto faceRecognizer = new FaceRecognizer(ModelSetting(modelPath + modelName, SEETA_DEVICE_CPU, 0));

	*size = faceRecognizer->GetExtractFeatureSize();
	std::shared_ptr<float> _features(new float[faceRecognizer->GetExtractFeatureSize()], std::default_delete<float[]>());
	faceRecognizer->Extract(img, points, _features.get());
	delete faceRecognizer;

	float* features = (float*)malloc(*size * sizeof(float));
	for (int i = 0; i < *size; i++)
	{
		*features = _features.get()[i];
		features++;
	}
	return features;
}
// ��������ֵ���ƶȼ���
View_Api float CalculateSimilarity(float* leftFeatures, float* rightFeatures, int type = 0)
{
	string modelName = "face_recognizer.csta";
	if (type == 1) { modelName = "face_recognizer_mask.csta"; }
	if (type == 2) { modelName = "face_recognizer_light.csta"; }
	auto faceRecognizer = new FaceRecognizer(ModelSetting(modelPath + modelName, SEETA_DEVICE_CPU, 0));

	float similarity = faceRecognizer->CalculateSimilarity(leftFeatures, rightFeatures);
	delete faceRecognizer;
	return similarity;
}

/***************************************************************************************************************/
// ��������
FaceAntiSpoofing* v_faceAntiSpoofing = nullptr;
// ������ - ��֡
View_Api int V_AntiSpoofing(unsigned char* imgData, SeetaImageData& img, SeetaRect faceRect, SeetaPointF* points, bool global)
{
	img.data = imgData;
	if (v_faceAntiSpoofing == nullptr) {
		ModelSetting setting;
		setting.set_id(0);
		setting.set_device(SEETA_DEVICE_CPU);
		string modelName = "fas_first.csta";
		setting.append(modelPath + modelName);
		if (global) { // ����ȫ�ּ������
			modelName = "fas_second.csta";
			setting.append(modelPath + modelName);
		}
		v_faceAntiSpoofing = new FaceAntiSpoofing(setting);
	}

	return v_faceAntiSpoofing->Predict(img, faceRect, points);
}
// ������ - ��Ƶ
View_Api int V_AntiSpoofingVideo(unsigned char* imgData, SeetaImageData& img, SeetaRect faceRect, SeetaPointF* points, bool global)
{
	img.data = imgData;
	if (v_faceAntiSpoofing == nullptr) {
		ModelSetting setting;
		setting.set_id(0);
		setting.set_device(SEETA_DEVICE_CPU);
		string modelName = "fas_first.csta";
		setting.append(modelPath + modelName);
		if (global) { // ����ȫ�ּ������
			modelName = "fas_second.csta";
			setting.append(modelPath + modelName);
		}
		v_faceAntiSpoofing = new FaceAntiSpoofing(setting);
	}

	return v_faceAntiSpoofing->PredictVideo(img, faceRect, points);
}


// ����������
FaceTracker* v_faceTracker = nullptr;
static SeetaTrackingFaceInfoArray trackingInfos;

// ��ȡ���ٵ���������
View_Api int V_FaceTrackSize(unsigned char* imgData, SeetaImageData& img,
	bool stable = false, int interval = 10, double faceSize = 20, double threshold = 0.9, int type = 0)
{
	img.data = imgData;
	if (v_faceTracker == nullptr) {
		ModelSetting setting;
		setting.set_device(SEETA_DEVICE_CPU);
		string modelName = "face_detector.csta";
		if (type == 1) { modelName = "mask_detector.csta"; }
		setting.append(modelPath + modelName);
		v_faceTracker = new FaceTracker(setting, img.width, img.height);
	}

	v_faceTracker->SetVideoStable(stable);
	v_faceTracker->SetMinFaceSize(faceSize);
	v_faceTracker->SetThreshold(threshold);
	v_faceTracker->SetInterval(interval);

	auto faceInfos = v_faceTracker->Track(img);

	trackingInfos = faceInfos;

	return faceInfos.size;
}

// ����������Ϣ
View_Api bool V_FaceTrack(float* score, int* PID, int* x, int* y, int* width, int* height)
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

	trackingInfos.data = nullptr;
	trackingInfos.size = 0;
	return true;
}


// ����������
QualityOfBrightness* V_Quality_Brightness = nullptr;
// ��������
View_Api bool V_QualityOfBrightness(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float v0 = 70, float v1 = 100, float v2 = 210, float v3 = 230)
{
	img.data = imgData;
	if (V_Quality_Brightness == nullptr)
	{
		V_Quality_Brightness = new QualityOfBrightness(v0, v1, v2, v3);
	}
	auto result = V_Quality_Brightness->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// ������������
QualityOfClarity* V_Quality_Clarity = nullptr;
// ����������
View_Api bool V_QualityOfClarity(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float low = 0.1f, float high = 0.2f)
{
	img.data = imgData;
	if (V_Quality_Clarity == nullptr)
	{
		V_Quality_Clarity = new QualityOfClarity(low, high);
	}
	auto result = V_Quality_Clarity->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// ������������
QualityOfIntegrity* V_Quality_Integrity = nullptr;
// ����������
View_Api bool V_QualityOfIntegrity(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float low = 10, float high = 1.5f)
{
	img.data = imgData;
	if (V_Quality_Integrity == nullptr)
	{
		WriteMessage("QualityOfIntegrity", "low:" + to_string(low) + " - high:" + to_string(high));
		V_Quality_Integrity = new QualityOfIntegrity(low, high);
	}
	auto result = V_Quality_Integrity->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// ��̬������
QualityOfPose* V_Quality_Pose = nullptr;
// ��̬����
View_Api bool V_QualityOfPose(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score)
{
	img.data = imgData;
	if (V_Quality_Pose == nullptr)
	{
		V_Quality_Pose = new QualityOfPose();
	}
	auto result = V_Quality_Pose->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// ��̬ (���)������
QualityOfPoseEx* V_Quality_PoseEx = nullptr;
// ��̬ (���)����
View_Api bool V_QualityOfPoseEx(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float yawLow = 25, float yawHigh = 10, float pitchLow = 20, float pitchHigh = 10, float rollLow = 33.33f, float rollHigh = 16.67f)
{
	img.data = imgData;
	if (V_Quality_PoseEx == nullptr)
	{
		ModelSetting setting;
		string modelName = "pose_estimation.csta";
		setting.append(modelPath + modelName);
		V_Quality_PoseEx = new QualityOfPoseEx(setting);
	}

	V_Quality_PoseEx->set(QualityOfPoseEx::YAW_LOW_THRESHOLD, yawLow);
	V_Quality_PoseEx->set(QualityOfPoseEx::YAW_HIGH_THRESHOLD, yawHigh);
	V_Quality_PoseEx->set(QualityOfPoseEx::PITCH_LOW_THRESHOLD, pitchLow);
	V_Quality_PoseEx->set(QualityOfPoseEx::PITCH_HIGH_THRESHOLD, pitchHigh);
	V_Quality_PoseEx->set(QualityOfPoseEx::ROLL_LOW_THRESHOLD, rollLow);
	V_Quality_PoseEx->set(QualityOfPoseEx::ROLL_HIGH_THRESHOLD, rollHigh);

	auto result = V_Quality_PoseEx->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// �ֱ���������
QualityOfResolution* V_Quality_Resolution = nullptr;
// �ֱ�������
View_Api bool V_QualityOfResolution(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float low = 80, float high = 120)
{
	img.data = imgData;
	if (V_Quality_Resolution == nullptr)
	{
		V_Quality_Resolution = new QualityOfResolution(low, high);
	}
	auto result = V_Quality_Resolution->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// ������ (���)������
QualityOfClarityEx* V_Quality_ClarityEx = nullptr;
// ������ (���)����
View_Api bool V_QualityOfClarityEx(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score,
	float blur_thresh = 0.8f)
{
	img.data = imgData;
	if (V_Quality_ClarityEx == nullptr)
	{
		V_Quality_ClarityEx = new QualityOfClarityEx(blur_thresh, modelPath);
	}
	auto result = V_Quality_ClarityEx->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

// �ڵ�������
QualityOfNoMask* V_Quality_NoMask = nullptr;
// �ڵ�����
View_Api bool V_QualityOfNoMask(unsigned char* imgData, SeetaImageData& img,
	SeetaRect faceRect, SeetaPointF* points, int pointsLength,
	int* level, float* score)
{
	img.data = imgData;
	if (V_Quality_NoMask == nullptr)
	{
		V_Quality_NoMask = new QualityOfNoMask(modelPath);
	}
	auto result = V_Quality_NoMask->check(img, faceRect, points, pointsLength);

	*level = result.level;
	*score = result.score;

	return true;
}

/******��������***********************************************************************************************/
// ����Ԥ����
AgePredictor* V_Age_Predictor = nullptr;
// ����Ԥ��
View_Api int V_AgePredictor(unsigned char* imgData, SeetaImageData& img,
	SeetaPointF* points, int pointsLength)
{
	img.data = imgData;
	if (V_Age_Predictor == nullptr) {
		ModelSetting setting;
		setting.set_device(SEETA_DEVICE_CPU);
		string modelName = "age_predictor.csta";
		setting.append(modelPath + modelName);
		V_Age_Predictor = new AgePredictor(setting);
	}
	int age = 0;
	auto result = V_Age_Predictor->PredictAgeWithCrop(img, points, age);
	if (result)
		return age;
	else return -1;
}

// ����Ԥ����
GenderPredictor* V_Gender_Predictor = nullptr;
// ����Ԥ��
View_Api int V_GenderPredictor(unsigned char* imgData, SeetaImageData& img,
	SeetaPointF* points, int pointsLength)
{
	img.data = imgData;
	if (V_Gender_Predictor == nullptr) {
		ModelSetting setting;
		setting.set_device(SEETA_DEVICE_CPU);
		string modelName = "gender_predictor.csta";
		setting.append(modelPath + modelName);
		V_Gender_Predictor = new GenderPredictor(setting);
	}

	GenderPredictor::GENDER gender = GenderPredictor::GENDER::MALE;
	auto result = V_Gender_Predictor->PredictGenderWithCrop(img, points, gender);
	if (result)
		return gender;
	else return -1;
}

// ����Ԥ����
EyeStateDetector* V_EyeState_Detector = nullptr;
// ����Ԥ��
View_Api bool V_EyeStateDetector(unsigned char* imgData, SeetaImageData& img, SeetaPointF* points, int pointsLength,
	EyeStateDetector::EYE_STATE& left_eye, EyeStateDetector::EYE_STATE& right_eye)
{
	img.data = imgData;
	if (V_EyeState_Detector == nullptr) {
		ModelSetting setting;
		setting.set_device(SEETA_DEVICE_CPU);
		string modelName = "eye_state.csta";
		setting.append(modelPath + modelName);
		V_EyeState_Detector = new EyeStateDetector(setting);
	}

	V_EyeState_Detector->Detect(img, points, left_eye, right_eye);
	return true;
}


// �ͷ���Դ
View_Api void V_Dispose()
{
	//if (faceDetector != nullptr) { delete faceDetector; faceDetector = nullptr; }
	//if (v_faceLandmarker != nullptr) { delete v_faceLandmarker; v_faceLandmarker = nullptr; }
	//if (v_faceRecognizer != nullptr) { delete v_faceRecognizer; v_faceRecognizer = nullptr; }
	if (v_faceAntiSpoofing != nullptr) { delete v_faceAntiSpoofing; v_faceAntiSpoofing = nullptr; }
	if (v_faceTracker != nullptr) { delete v_faceTracker; v_faceTracker = nullptr; }

	if (V_Quality_Brightness != nullptr) { delete V_Quality_Brightness; V_Quality_Brightness = nullptr; }
	if (V_Quality_Clarity != nullptr) { delete V_Quality_Clarity; V_Quality_Clarity = nullptr; }
	if (V_Quality_Integrity != nullptr) { delete V_Quality_Integrity; V_Quality_Integrity = nullptr; }
	if (V_Quality_Pose != nullptr) { delete V_Quality_Pose; V_Quality_Pose = nullptr; }
	if (V_Quality_PoseEx != nullptr) { delete V_Quality_PoseEx; V_Quality_PoseEx = nullptr; }
	if (V_Quality_Resolution != nullptr) { delete V_Quality_Resolution; V_Quality_Resolution = nullptr; }
	if (V_Quality_ClarityEx != nullptr) { delete V_Quality_ClarityEx; V_Quality_ClarityEx = nullptr; }
	if (V_Quality_NoMask != nullptr) { delete V_Quality_NoMask; V_Quality_NoMask = nullptr; }

	if (V_Age_Predictor != nullptr) { delete V_Age_Predictor; V_Age_Predictor = nullptr; }
	if (V_Gender_Predictor != nullptr) { delete V_Gender_Predictor; V_Gender_Predictor = nullptr; }
	if (V_EyeState_Detector != nullptr) { delete V_EyeState_Detector; V_EyeState_Detector = nullptr; }
}