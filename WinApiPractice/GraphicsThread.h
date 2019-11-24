#pragma once
#include "Windows.h"
#include "GridPainter.h"
#include "preferences.h"
#include <thread>
#include <atomic>

class GraphicsThread
{
public:
	explicit GraphicsThread(HWND& window, const Preferences& prefs);

	GraphicsThread(const GraphicsThread& other) = delete;
	GraphicsThread(GraphicsThread&& other) noexcept = delete;
	GraphicsThread& operator=(const GraphicsThread& other) = delete;
	GraphicsThread& operator=(GraphicsThread&& other) noexcept = delete;

	void Launch();
	void Pause();
	void Resume();
	void Stop();
	void SetThreadPriority(int priority);

	~GraphicsThread();

private:
	struct Context
	{
		explicit Context(int size);

		int size;

		std::atomic<bool> paused{};
		std::atomic<bool> stopped{};
	};

	GridPainter painter;
	std::thread workerThread;
	Context context;

	static void Run(Context& context);
};
