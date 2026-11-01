#pragma once
#include <vector>
#include <string>

/// @brief メモリ上のバイナリを範囲チェックしながら読む
class BinaryReader
{
public:

	BinaryReader(void) = default;
	explicit BinaryReader(std::vector<char>&& data)
		: data_(std::move(data)) {}

	template<typename t_Param>
	bool Read(t_Param& out)
	{
		static_assert(std::is_trivially_copyable_v<t_Param>);
		if (pos_ + sizeof(t_Param) > data_.size()) { return false; }
		std::memcpy(&out, data_.data() + pos_, sizeof(t_Param));
		pos_ += sizeof(t_Param);
		return true;
	}

	/// @brief「長さ(uint16_t) + 本体」
	/// @param out 
	/// @return 
	bool ReadString(std::string& out)
	{
		uint16_t size = 0;
		if (!Read(size) || pos_ + size > data_.size()) { return false; }
		out.assign(data_.data() + pos_, size);
		pos_ += size;
		return true;
	}

private:

	std::vector<char> data_;
	size_t pos_ = 0;
};
