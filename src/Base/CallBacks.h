#pragma once


namespace cncpp
{
	class BaseTask
	{
		// run in back thread
        virtual void run() = 0;

		// run in main  thread
		virtual void done() = 0;
	};



}