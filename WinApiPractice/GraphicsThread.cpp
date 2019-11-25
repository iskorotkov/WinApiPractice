﻿#include "GraphicsThread.h"
#include <iostream>
#include "Windows.h"
#include <processthreadsapi.h>
#include "GameSession.h"
#include "GridPainter.h"
#include "preferences.h"
#include "LibraryHandle.h"

GraphicsThread::GraphicsThread(HWND& window, const GameSession* game)
{
	context.window = window;
	context.game = game;

	// TODO: Extract methods. Use AssetManager class for image loading.
	try
	{
		const LibraryHandle lib(L"libpic.dll");
		const auto loadPicFunc = reinterpret_cast<decltype(LoadPicW)*>(lib.GetMethod("LoadPicW"));
		const auto isValidFunc = reinterpret_cast<decltype(IsValid)*>(lib.GetMethod("IsValid"));

		const auto loadPicWrapper = [loadPicFunc, isValidFunc](Image& img, const wchar_t* name)
		{
			img = loadPicFunc(name);
			if (!isValidFunc(img))
			{
				throw std::exception("Unable to load a picture.");
			}
		};

		const auto prefs = game->GetPreferences();
		loadPicWrapper(context.crossImage, prefs->IconFile);
		loadPicWrapper(context.circleImage, prefs->CursorFile);
	}
	catch (std::exception& e)
	{
		printf_s("An error happened when tried to use DLL. Error info: %s", e.what());
		throw;
	}
}

void GraphicsThread::Launch()
{
	workerThread = std::thread(&GraphicsThread::Run, std::ref(context));
}

void GraphicsThread::Suspend()
{
	isSuspended = true;
	SuspendThread(workerThread.native_handle());
}

void GraphicsThread::Resume()
{
	isSuspended = false;
	ResumeThread(workerThread.native_handle());
}

void GraphicsThread::Stop()
{
	TerminateThread(workerThread.native_handle(), 0);
	workerThread.join();
}

void GraphicsThread::SetPriority(int priority)
{
	const int values[] = { -15, -2, -1, 0, 1, 2, 15 };
	priority = values[priority];
	std::cout << "Priority has been set: " << priority;
	SetThreadPriority(workerThread.native_handle(), priority);
}

void GraphicsThread::ToggleSuspended()
{
	if (isSuspended)
	{
		Resume();
	}
	else
	{
		Suspend();
	}
}

GraphicsThread::~GraphicsThread()
{
	Stop();
}

void GraphicsThread::Run(Context& context)
{
	using namespace std::chrono_literals;
	const auto deltaSeconds = 1s / 60;
	try
	{
		const auto preferences = context.game->GetPreferences();
		const auto state = context.game->GetGameState();
		const GridPainter painter(context.window, preferences->GridSize);
		for (;;)
		{
			painter.DrawGrid(preferences->GridColor);

			painter.DrawImageWhere(1, state, context.crossImage);
			painter.DrawImageWhere(2, state, context.circleImage);
			std::this_thread::sleep_for(deltaSeconds);
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception thrown in graphics thread: " << e.what() << std::endl;
	}
}
