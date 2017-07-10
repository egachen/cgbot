#include "log.h"

namespace CgBot
{
	Log logger;

	Log &Log::operator <<(Log::ostream_manipulator fn)
	{
		// Pass manipulator into the stream
		ss << fn;

		if (fn == &std::endl<char, std::char_traits<char>> || fn == &std::ends<char, std::char_traits<char>>)
		{
			this->flush();
		}
		return *this;
	};

}