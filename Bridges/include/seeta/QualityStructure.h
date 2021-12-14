//
// Created by kier on 19-7-24.
//

#ifndef SEETA_QULITY_EVALUATOR_QULITY_STRUCTURE_H
#define SEETA_QULITY_EVALUATOR_QULITY_STRUCTURE_H

#include "Struct.h"

namespace seeta {
	namespace v3 {
		/// <summary>
		/// ���������ȼ�
		/// </summary>
		enum QualityLevel {
			/// <summary>
			/// ������
			/// </summary>
			LOW = 0,
			/// <summary>
			/// ����һ��
			/// </summary>
			MEDIUM = 1,
			/// <summary>
			/// ������
			/// </summary>
			HIGH = 2,
		};

		/// <summary>
		/// �����������
		/// </summary>
		class QualityResult {
		public:
			using self = QualityResult;
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <returns></returns>
			QualityResult() = default;
			/// <summary>
			/// 
			/// </summary>
			/// <param name="level"></param>
			/// <param name="score"></param>
			/// <returns></returns>
			QualityResult(QualityLevel level, float score = 0) : level(level), score(score) {}

			/// <summary>
			/// ���������ȼ�
			/// </summary>
			QualityLevel level = LOW;
			/// <summary>
			/// ������������
			/// <para>Խ��Խ�ã�û�з�Χ����</para>
			/// </summary>
			float score = 0;
		};

		struct QualityResultEx {
			int attr;
			QualityLevel level;   ///< quality level
			float score;          ///< greater means better, no range limit
		};

		struct QualityResultExArray {
			int size;
			QualityResultEx* data;
		};

		class QualityRule {
		public:
			using self = QualityRule;

			virtual ~QualityRule() = default;

			/**
			 * ��ʼ����
			 * @param image original image
			 * @param face face location
			 * @param points landmark on face
			 * @param N how many landmark on face given, normally 5
			 * @return Quality result
			 */
			virtual QualityResult check(
				const SeetaImageData& image,
				const SeetaRect& face,
				const SeetaPointF* points,
				int32_t N) = 0;
		};
	}
	using namespace v3;
}

#endif //SEETA_QULITY_EVALUATOR_QULITY_STRUCTURE_H
