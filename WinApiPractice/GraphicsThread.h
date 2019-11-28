#pragma once
#include "Windows.h"
#include <thread>
#include "libpic.h"
#include <condition_variable>

class GameSession;

class GraphicsThread
{
public:
	explicit GraphicsThread(HWND& window, const GameSession* game);

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
	void Redraw();

	~GraphicsThread();

private:
	struct Context
	{
		const GameSession* game;
		HWND window;
		Image crossImage;
		Image circleImage;
	};

	std::thread workerThread;
	Context context;
	bool isSuspended = false;
	std::condition_variable cv;
	std::mutex m;
	bool needRedraw = true;

	void Run(Context& context);
};
