#include "GraphicsThread.h"
#include <iostream>

GraphicsThread::GraphicsThread(HWND& window, const Preferences& prefs) :
	painter(window, prefs.GridSize),
	context(prefs.GridSize)
{
}

void GraphicsThread::Launch()
{
	workerThread = std::thread(&GraphicsThread::Run, std::ref(context));
}

void GraphicsThread::Pause()
{
	context.paused.store(true, std::memory_order_relaxed);
}

void GraphicsThread::Resume()
{
	context.paused.store(false, std::memory_order_relaxed);
}

void GraphicsThread::Stop()
{
	context.stopped.store(true, std::memory_order_relaxed);
	if (workerThread.joinable())
	{
		workerThread.join();
	}
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

	for (;;)
	{
		std::cout << "window redraw\n";
		std::this_thread::sleep_for(deltaSeconds);
	}
}
