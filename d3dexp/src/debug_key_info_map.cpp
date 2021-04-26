#include "debug_key_info_map.h"

#include <sstream>
#include <iomanip>

#define REGISTER_KEY_INFO(key_code, context) {key_t::##key_code, {#key_code, context}}

namespace d3dexp::chili
{

	debug_key_info_map::debug_key_info_map() noexcept :
		m_map({
			REGISTER_KEY_INFO(undefined,		"[???]"),
			REGISTER_KEY_INFO(error,			"[ERR]"),
			REGISTER_KEY_INFO(lmb,				"[LMB]"),
			REGISTER_KEY_INFO(rmb,				"[RMB]"),
			REGISTER_KEY_INFO(mmb,				"[MMB]"),
			REGISTER_KEY_INFO(x1mb,				"[X1M]"),
			REGISTER_KEY_INFO(x2mb,				"[X2M]"),
			REGISTER_KEY_INFO(backspace,		"[<--]"),
			REGISTER_KEY_INFO(delete_,			"[DEL]"),
			REGISTER_KEY_INFO(enter,			"[<-']"),
			REGISTER_KEY_INFO(insert,			"[INS]"),
			REGISTER_KEY_INFO(home,				"[HOM]"),
			REGISTER_KEY_INFO(end,				"[END]"),
			REGISTER_KEY_INFO(pgup,				"[PGU]"),
			REGISTER_KEY_INFO(pgdown,			"[PGD]"),
			REGISTER_KEY_INFO(lshift,			"[SHF]"),
			REGISTER_KEY_INFO(rshift,			"[SHF]"),
			REGISTER_KEY_INFO(lctrl,			"[CTL]"),
			REGISTER_KEY_INFO(rctrl,			"[CTL]"),
			REGISTER_KEY_INFO(lalt,				"[ALT]"),
			REGISTER_KEY_INFO(ralt,				"[ALT]"),
			REGISTER_KEY_INFO(lsys,				"[SYS]"),
			REGISTER_KEY_INFO(rsys,				"[SYS]"),
			REGISTER_KEY_INFO(tab,				"[-->]"),
			REGISTER_KEY_INFO(escape,			"[ESC]"),
			REGISTER_KEY_INFO(left,				"[<<<]"),
			REGISTER_KEY_INFO(right,			"[>>>]"),
			REGISTER_KEY_INFO(up,			   "[/|\\]"),
			REGISTER_KEY_INFO(down,			   "[\\|/]"),
			REGISTER_KEY_INFO(spacebar,			"[___]"),
			REGISTER_KEY_INFO(_0,				"['0']"),
			REGISTER_KEY_INFO(_1,				"['1']"),
			REGISTER_KEY_INFO(_2,				"['2']"),
			REGISTER_KEY_INFO(_3,				"['3']"),
			REGISTER_KEY_INFO(_4,				"['4']"),
			REGISTER_KEY_INFO(_5,				"['5']"),
			REGISTER_KEY_INFO(_6,				"['6']"),
			REGISTER_KEY_INFO(_7,				"['7']"),
			REGISTER_KEY_INFO(_8,				"['8']"),
			REGISTER_KEY_INFO(_9,				"['9']"),
			REGISTER_KEY_INFO(num_0,			"['0']"),
			REGISTER_KEY_INFO(num_1,			"['1']"),
			REGISTER_KEY_INFO(num_2,			"['2']"),
			REGISTER_KEY_INFO(num_3,			"['3']"),
			REGISTER_KEY_INFO(num_4,			"['4']"),
			REGISTER_KEY_INFO(num_5,			"['5']"),
			REGISTER_KEY_INFO(num_6,			"['6']"),
			REGISTER_KEY_INFO(num_7,			"['7']"),
			REGISTER_KEY_INFO(num_8,			"['8']"),
			REGISTER_KEY_INFO(num_9,			"['9']"),
			REGISTER_KEY_INFO(num_plus,			"['+']"),
			REGISTER_KEY_INFO(num_minus,		"['-']"),
			REGISTER_KEY_INFO(num_mul,			"['*']"),
			REGISTER_KEY_INFO(num_div,			"['/']"),
			REGISTER_KEY_INFO(num_period,		"['.']"),
			REGISTER_KEY_INFO(num_enter,		"[<-']"),
			REGISTER_KEY_INFO(f1,				"[F01]"),
			REGISTER_KEY_INFO(f2,				"[F02]"),
			REGISTER_KEY_INFO(f3,				"[F03]"),
			REGISTER_KEY_INFO(f4,				"[F04]"),
			REGISTER_KEY_INFO(f5,				"[F05]"),
			REGISTER_KEY_INFO(f6,				"[F06]"),
			REGISTER_KEY_INFO(f7,				"[F07]"),
			REGISTER_KEY_INFO(f8,				"[F08]"),
			REGISTER_KEY_INFO(f9,				"[F09]"),
			REGISTER_KEY_INFO(f10,				"[F10]"),
			REGISTER_KEY_INFO(f11,				"[F11]"),
			REGISTER_KEY_INFO(f12,				"[F12]"),
			REGISTER_KEY_INFO(f13,				"[F13]"),
			REGISTER_KEY_INFO(f14,				"[F14]"),
			REGISTER_KEY_INFO(f15,				"[F15]"),
			REGISTER_KEY_INFO(f16,				"[F16]"),
			REGISTER_KEY_INFO(f17,				"[F17]"),
			REGISTER_KEY_INFO(f18,				"[F18]"),
			REGISTER_KEY_INFO(f19,				"[F19]"),
			REGISTER_KEY_INFO(f20,				"[F20]"),
			REGISTER_KEY_INFO(f21,				"[F21]"),
			REGISTER_KEY_INFO(f22,				"[F22]"),
			REGISTER_KEY_INFO(f23,				"[F23]"),
			REGISTER_KEY_INFO(f24,				"[F24]"),
			REGISTER_KEY_INFO(tilde,			"['~']"),
			REGISTER_KEY_INFO(a,				"['a']"),
			REGISTER_KEY_INFO(a,				"['a']"),
			REGISTER_KEY_INFO(b,				"['b']"),
			REGISTER_KEY_INFO(c,				"['c']"),
			REGISTER_KEY_INFO(d,				"['d']"),
			REGISTER_KEY_INFO(e,				"['e']"),
			REGISTER_KEY_INFO(f,				"['f']"),
			REGISTER_KEY_INFO(g,				"['g']"),
			REGISTER_KEY_INFO(h,				"['h']"),
			REGISTER_KEY_INFO(i,				"['i']"),
			REGISTER_KEY_INFO(j,				"['j']"),
			REGISTER_KEY_INFO(k,				"['k']"),
			REGISTER_KEY_INFO(l,				"['l']"),
			REGISTER_KEY_INFO(m,				"['m']"),
			REGISTER_KEY_INFO(n,				"['n']"),
			REGISTER_KEY_INFO(o,				"['o']"),
			REGISTER_KEY_INFO(p,				"['p']"),
			REGISTER_KEY_INFO(q,				"['q']"),
			REGISTER_KEY_INFO(r,				"['r']"),
			REGISTER_KEY_INFO(s,				"['s']"),
			REGISTER_KEY_INFO(t,				"['t']"),
			REGISTER_KEY_INFO(u,				"['u']"),
			REGISTER_KEY_INFO(v,				"['v']"),
			REGISTER_KEY_INFO(w,				"['w']"),
			REGISTER_KEY_INFO(x,				"['x']"),
			REGISTER_KEY_INFO(y,				"['y']"),
			REGISTER_KEY_INFO(z,				"['z']"),
			REGISTER_KEY_INFO(plus,				"['+']"),
			REGISTER_KEY_INFO(minus,			"['-']"),
			REGISTER_KEY_INFO(period,			"['.']"),
			REGISTER_KEY_INFO(comma,			"[',']"),
			REGISTER_KEY_INFO(semicolon,		"[';']"),
			REGISTER_KEY_INFO(slash,			"['/']"),
			REGISTER_KEY_INFO(backslash,		"['\\']"),
			REGISTER_KEY_INFO(quote,			"['\"']"),
			REGISTER_KEY_INFO(lbracket,			"['[']"),
			REGISTER_KEY_INFO(rbracket,			"[']']"),
			REGISTER_KEY_INFO(misc_1,			"[???]"),
			REGISTER_KEY_INFO(misc_2,			"[???]"),
			REGISTER_KEY_INFO(break_,			"[BRK]"),
			REGISTER_KEY_INFO(clear,			"[CLR]"),
			REGISTER_KEY_INFO(pause,			"[PAU]"),
			REGISTER_KEY_INFO(capslock,			"[CLK]"),
			REGISTER_KEY_INFO(printscreen,		"[PSC]"),
			REGISTER_KEY_INFO(numlock,			"[NLK]"),
			REGISTER_KEY_INFO(scrolllock,		"[SLK]"),
			REGISTER_KEY_INFO(select,			"[SEL]"),
			REGISTER_KEY_INFO(print,			"[PRN]"),
			REGISTER_KEY_INFO(execute,			"[EXE]"),
			REGISTER_KEY_INFO(help,				"[HLP]"),
			REGISTER_KEY_INFO(apps,				"[APP]"),
			REGISTER_KEY_INFO(sleep,			"[SLP]"),
			REGISTER_KEY_INFO(numsep,			"[SEP]"),
			{ static_cast<key_t>(0xff), {"", "[   ]"}}})
	{}

	[[nodiscard]] std::string debug_key_info_map::operator() (char const * context, key_t key) const noexcept
	{
		constexpr int first_column_width = 15;
		const auto iter = m_map.find(key);

		std::ostringstream oss;
		if (iter != m_map.end())
		{
			oss << std::left << std::setw(first_column_width) << context << iter->second.second << ' ' << iter->second.first << '\n';
		}
		else
		{
			std::ostringstream padss;
			padss << "Unknown key code: 0x" << std::hex << static_cast<std::uint32_t>(key) << '\n';
			oss << std::left << std::setw(first_column_width) << padss.str() << std::right;
		}

		return oss.str();
	}

	[[nodiscard]] std::string debug_key_info_map::operator() (char const* context, key_t key, std::int16_t x_pos, std::int16_t y_pos) const noexcept
	{
		constexpr int first_column_width = 15;
		constexpr int second_column_width = 15;
		const auto iter = m_map.find(key);

		std::ostringstream oss;
		if (iter != m_map.end())
		{
			oss << std::left << std::setw(first_column_width) << context << iter->second.second << ' ' << std::left << std::setw(second_column_width) << iter->second.first;
			oss << std::left << "pos = (" << std::right << std::setw(5) << x_pos << ", " << std::right << std::setw(5) << y_pos << ")\n";
		}
		else
		{
			std::ostringstream padss;
			padss << "Unknown key code: 0x" << std::hex << static_cast<std::uint32_t>(key) << '\n';
			oss << std::left << std::setw(first_column_width) << padss.str() << std::right;
		}

		return oss.str();
	}

}