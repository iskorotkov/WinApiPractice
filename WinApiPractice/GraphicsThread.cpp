#include "GraphicsThread.h"
#include <iostream>
#include "Windows.h"
#include <processthreadsapi.h>

GraphicsThread::GraphicsThread(HWND& window, const Preferences& prefs) :
	painter(window, prefs.GridSize),
	context(prefs.GridSize)
{
}

void GraphicsThread::Launch()
{
	workerThread = std::thread(&GraphicsThread::Run, std::ref(context));
}

void GraphicsThread::Suspend()
{
	SuspendThread(workerThread.native_handle());
}

void GraphicsThread::Resume()
{
	ResumeThread(workerThread.native_handle());
}

void GraphicsThread::Stop()
{
	TerminateThread(workerThread.native_handle(), 0);
	workerThread.join();
}

void GraphicsThread::SetPriority(const int priority)
{
	SetThreadPriority(workerThread.native_handle(), priority);
}

GraphicsThread::~GraphicsThread()
{
	Stop();
	
}

GraphicsThread::Context::Context(const int size) : size(size)
{
}

void GraphicsThread::Run(Context& context)
{
	using namespace std::chrono_literals;

	const auto deltaSeconds = 1s / 60;

	try
	{
		for (;;)
		{
			std::cout << "window redraw\n";
			std::this_thread::sleep_for(deltaSeconds);
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception thrown in graphics thread: " << e.what() << std::endl;
	}
}
