#pragma once

#include <iostream>
#include <fstream>
#include <BWAPI.h>

namespace CgBot 
{
	class Log
	{
	public:
		Log(){
			logFile_.open("mylog.txt");
		};

		typedef std::ostream& (*ostream_manipulator)(std::ostream&);
		template < class T >
		inline Log &operator <<(const T &in)
		{
			// Pass whatever into the stream
			ss << in;
			return *this;
		};
		/// @overload
		Log &operator <<(ostream_manipulator fn);
		void Log::flush()
		{
			if (!BWAPI::Broodwar->self()) return;
			if (ss.str().empty()) return;

			BWAPI::Broodwar->printf("%s", ss.str().c_str());
			logFile_ << ss.str().c_str();
			logFile_.flush();
			ss.str("");
		};


		~Log(){
			logFile_.close();
		}

	private:
		std::ostringstream ss;
		std::ofstream logFile_;
	};

	extern Log logger;
}

