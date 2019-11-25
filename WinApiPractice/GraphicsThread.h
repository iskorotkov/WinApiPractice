#pragma once
#include "Windows.h"
#include "GridPainter.h"
#include "preferences.h"
#include <thread>

class GraphicsThread
{
public:
	explicit GraphicsThread(HWND& window, const Preferences& prefs);

	GraphicsThread(const GraphicsThread& other) = delete;
	GraphicsThread(GraphicsThread&& other) noexcept = delete;
	GraphicsThread& operator=(const GraphicsThread& other) = delete;
	GraphicsThread& operator=(GraphicsThread&& other) noexcept = delete;

	void Launch();
	void Suspend();
	void Resume();
	void Stop();
	void SetPriority(int priority);
	void ToggleSuspended();

	~GraphicsThread();

private:
	struct Context
	{
		explicit Context(int size);

		int size;
	};

	GridPainter painter;
	std::thread workerThread;
	Context context;
	bool isSuspended = false;

	static void Run(Context& context);
};
