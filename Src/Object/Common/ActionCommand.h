#pragma once
#include <concepts>
#include <functional>
#include <memory>
#include <utility>

/// @brief 行動コマンドの基底(Commandパターンの Command)
class IActionCommand
{
public:

	virtual ~IActionCommand(void) = default;

	/// @brief 行動の実行(行動有効タイミングで1度だけ呼ばれる)
	virtual void Execute(void) = 0;
};

/// @brief std::function を包む汎用コマンド
class FunctionActionCommand final : public IActionCommand
{
public:

	explicit FunctionActionCommand(std::function<void(void)> _func)
		: func_(std::move(_func))
	{
	}

	void Execute(void) override
	{
		if (func_) { func_(); }
	}

private:

	std::function<void(void)> func_;
};

/// @brief 呼び出し可能オブジェクト(ラムダ等)からコマンドを生成する (C++20 concepts)
template<std::invocable Func>
std::unique_ptr<IActionCommand> MakeActionCommand(Func&& _func)
{
	return std::make_unique<FunctionActionCommand>(std::function<void(void)>(std::forward<Func>(_func)));
}
