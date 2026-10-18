#include "UtilityCommon.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>
#include <math.h>
#include "./UtilityMath.h"


void UtilityCommon::ChangeString(const std::string& _text, int& target, int minNum)
{
    /* 文字列→int */

    std::string txt = "";

    // マイナス
    const char& minus = *"-";

    // 文字が空白のとき、最小値にする
    if (_text == "" || _text == " ")
    {
        target = minNum;
        return;
    }

    for (char t : _text)
    {
        // 数字・マイナスではない文字列の時、終了
        if (!std::isdigit(static_cast<unsigned char>(t)) &&
            t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }
    
    
    if (!txt.empty())
    {    
        // 文字列→int変換
        int num = stoi(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < minNum) ? minNum : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！int→文字列変換の値に誤りがあります。！！\n");
        assert(false);
    }
}

void UtilityCommon::ChangeString(const std::string& text, int& target)
{
    /* 文字列→int */

    std::string txt = "";

    // マイナス
    const char& minus = *"-";


    // 文字が空白のとき、0にする
    if (text == "" || text == " ")
    {
        target = 0;
        return;
    }

    for (char t : text)
    {
        // 数字・マイナスではない文字列のとき、終了
        if (!isdigit(static_cast<unsigned char>(t)) &&
            t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }
    

    if (!txt.empty())
    {
        // 文字列→int変換
        int num = stoi(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < 0) ? 0 : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！int→文字列変換の値に誤りがあります。！！\n");
        assert(false);
    }
}

void UtilityCommon::ChangeString(const std::string& text, float& target, float minNum)
{
    /* 文字列→float */

    std::string txt = "";

    // 小数点
    const char& dot = *".";

    // マイナス
    const char& minus = *"-";


    // 文字が空白のとき、最小値の文字列にする
    if (text == "" || text == " ")
    {
        target = minNum;
        return;
    }

    for (auto t : text)
    {
        // 数字・小数点・マイナスではない文字列のとき、終了
        if (!isdigit(static_cast<unsigned char>(t)) &&
            t != dot && t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }

    if (!txt.empty())
    {
        // 文字列→float変換
        float num = stof(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < minNum) ? minNum : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！float→文字列変換の値に誤りがあります。！！\n");
        assert(false);

    }
}

void UtilityCommon::ChangeString(const std::string& text, float& target)
{
    /* 文字列→float */

    /* 文字列→float */
    std::string txt = "";

    // 小数点
    const char& dot = *".";

    // マイナス
    const char& minus = *"-";


    // 文字が空白のとき、0にする
    if (text == "" || text == " ")
    {
        txt = std::to_string(0.0f);
        return;
    }


    for (auto t : text)
    {
        // 数字・小数点・マイナスではない文字列のとき、終了
        if (!isdigit(static_cast<unsigned char>(t)) &&
            t != dot && t != minus)
        {
            break;
        }

        // 文字に現在の単語を追加 (文章の末尾の余分な文字対策)
        txt += t;
    }

    if (!txt.empty())
    {
        // 文字列→float変換
        float num = stof(txt);

        // 最小値より小さいとき最小値にする
        num = ((num < 0.0f) ? 0.0f : num);

        // 数値に反映
        target = num;
    }
    else
    {
        // エラー
        OutputDebugString("\n！！float→文字列変換の値に誤りがあります。！！\n");
        assert(false);
    }
}


bool UtilityCommon::WrapValue(VECTOR& _vec, const VECTOR& maxVec, const VECTOR& minVec)
{
    // 最小値を超えている場合は最大値に、最大値を超えている場合は最小値で返す
    bool isWrap = (WrapValue(_vec.x, maxVec.x, minVec.x) ||
                   WrapValue(_vec.y, maxVec.y, minVec.y) ||
                   WrapValue(_vec.z, maxVec.z, minVec.z));
    return isWrap;
}
bool UtilityCommon::WrapValue(Vector2& _vec, const Vector2& maxVec, const Vector2& minVec)
{
    bool isWrap = (WrapValue(_vec.x, maxVec.x, minVec.x) ||
                   WrapValue(_vec.y, maxVec.y, minVec.y));
    return isWrap;
}
bool UtilityCommon::WrapValue(int& _num, int maxNum, int minNum)
{
    bool isWrap = false;

    // 最小値を超えている場合は最大値に、最大値を超えている場合は最小値で返す
    if (_num < minNum)
    {
        isWrap = true;
        _num = maxNum;
    }
    else if (_num > maxNum)
    {
        isWrap = true;
        _num = minNum;
    }

    return isWrap;
}
bool UtilityCommon::WrapValue(float& _num, float maxNum, float minNum)
{
    bool isWrap = false;

    // 最小値を超えている場合は最大値に、最大値を超えている場合は最小値で返す
    if (_num < minNum)
    {
        isWrap = true;
        _num = maxNum;
    }
    else if (_num > maxNum)
    {
        isWrap = true;
        _num = minNum;
    }

    return isWrap;
}

float UtilityCommon::EaseLine(float curNum, float maxNum, float powNum)
{
    /* 線形補間処理 */
    float num = (curNum / maxNum);

    // 補正値を最大に補正
    if (num > 1.0f) { num = 1.0f; }

    return (1.0f - powf((1.0f - num), powNum));
}

float UtilityCommon::EaseInOutQuart(float term)
{
    /* EaseOutQuart補間：1 - (1 - term)^4 */
    term = std::clamp(term, 0.0f, 1.0f);
    const float unit = (1.0f - term);

    return 1.0f - unit * unit * unit * unit;
}