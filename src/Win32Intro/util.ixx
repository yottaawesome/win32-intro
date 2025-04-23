export module util;
import std;

export namespace Util
{
	template<typename T>
	concept ValidCharType = std::same_as<T, char> or std::same_as<T, wchar_t>;

	template<size_t N, ValidCharType TChar>
	struct FixedString
	{
		TChar Buffer[N];

		constexpr FixedString(const TChar(&args)[N]) noexcept
		{
			std::copy_n(args, N, Buffer);
		}

		constexpr auto View() const noexcept
		{
			return std::basic_string_view<TChar, std::char_traits<TChar>>(Buffer);
		}

		constexpr auto String() const noexcept
		{
			return std::basic_string<TChar, std::char_traits<TChar>>(Buffer);
		}

		constexpr auto Data() const noexcept
		{
			return Buffer;
		}
	};
	template<size_t N>
	FixedString(char buffer[N]) -> FixedString<N, char>;
	template<size_t N>
	FixedString(wchar_t buffer[N]) -> FixedString<N, wchar_t>;
}
