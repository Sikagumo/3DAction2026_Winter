#pragma once
#include <string>
#include "../Common/Vector2.h"
#include <DxLib.h>

class UtilityCommon
{
public:

	/// @brief 文字列→int型変換																							
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	/// @param minNum 最小値
	static void ChangeString(const std::string& _text, int& _target, int minNum);

	/// @brief 文字列→int型変換
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	static void ChangeString(const std::string& _text, int& _target);

	/// @brief 文字列→float型変換
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	/// @param minNum 最小値
	static void ChangeString(const std::string& _text, float& _target, float minNum);

	/// @brief 文字列→float型変換
	/// @param _text 対象のテキスト
	/// @param _target 数値割り当て先
	static void ChangeString(const std::string& _text, float& _target);


	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _vec 対象
	/// @param maxVec 最大値
	/// @param minVec 最小値
	static bool WrapValue(VECTOR& _vec, const VECTOR& maxVec, const VECTOR& minVec = {0.0f, 0.0f, 0.0f});

	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _vec 対象
	/// @param maxVec 最大値
	/// @param minVec 最小値
	/// @return 
	static bool WrapValue(Vector2& _vec, const Vector2& maxVec, const Vector2& minVec = {0, 0});

	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _num 対象
	/// @param maxNum 最大値
	/// @param minNum 最小値
	/// @return 
	static bool WrapValue(int& _num, int maxNum, int minNum = 0);

	/// @brief 対象が最大値/最小値を超えたときに値を変える
	/// @param _num 対象
	/// @param maxNum 最大値
	/// @param minNum 最小値
	static bool WrapValue(float& _num, float maxNum, float minNum = 0.0f);


	/// @brief 線形補間
	/// @param curNum 現在値
	/// @param maxNum 最大値
	/// @param _delta 累乗の値
	/// @return 補間後の値
	static float EaseLine(float curNum, float maxNum, float powNum);

	/// @brief QuadInOut補間値
	/// @param term 補間地点(0.0～1.0)
	static float EaseInOutQuart(float term);
};