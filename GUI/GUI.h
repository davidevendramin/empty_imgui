#pragma once
#include "pch.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Widget;
class Controller;
class Menu;

typedef enum Shortcut {
	HK_CTRL_X = ImGuiMod_Ctrl | ImGuiKey_X,
	HK_CTRL_B = ImGuiMod_Ctrl | ImGuiKey_B,
	HK_SHIFT_B = ImGuiMod_Shift | ImGuiKey_B,
	HK_CTRL_W = ImGuiMod_Ctrl | ImGuiKey_W,
	HK_CTRL_C = ImGuiMod_Ctrl | ImGuiKey_C,
	HK_CTRL_V = ImGuiMod_Ctrl | ImGuiKey_V,
	HK_CTRL_D = ImGuiMod_Ctrl | ImGuiKey_D,
	HK_CTRL_T = ImGuiMod_Ctrl | ImGuiKey_T,
	HK_CTRL_F = ImGuiMod_Ctrl | ImGuiKey_F,
	HK_CTRL_L = ImGuiMod_Ctrl | ImGuiKey_L,
	HK_CTRL_A = ImGuiMod_Ctrl | ImGuiKey_A,
	HK_ALT_B = ImGuiMod_Alt | ImGuiKey_B,
	HK_ALT_F = ImGuiMod_Alt | ImGuiKey_F,
	HK_ALT_S = ImGuiMod_Alt | ImGuiKey_S,
	HK_CTRL_E = ImGuiMod_Ctrl | ImGuiKey_E,
	HK_CTRL_SHIFT_S = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S,
	HK_CTRL_ALT_SHIFT_A = ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiMod_Shift | ImGuiKey_A,
	HK_CTRL_S = ImGuiMod_Ctrl | ImGuiKey_S,
	HK_CTRL_O = ImGuiMod_Ctrl | ImGuiKey_O,
	HK_CTRL_Q = ImGuiMod_Ctrl | ImGuiKey_Q,
	HK_CTRL_SHIFT_P = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_P,
	HK_CTRL_SHIFT_B = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_B,
	HK_CTRL_ALT_SHIFT_P = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiMod_Alt | ImGuiKey_P,
} Shortcut_t;

class GUI
{
private:
	HINSTANCE hInstance;
	WNDCLASSEX wc;
	WNDCLASSEX wcSplash;
	HWND hwnd;
	HWND hwndSplash;
	ID3D11RenderTargetView* m_mainRenderTargetView;
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pd3dDeviceContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	bool m_SwapChainOccluded = false;
	bool m_mainWindowHasBeenShown = false;
	UINT m_ResizeWidth = 0;
	UINT m_ResizeHeight = 0;

	bool stop = false;
	std::unordered_map<Shortcut_t, std::function<void()>> shortcuts;
	uint64_t widgetIndex;
	std::vector<Widget*> widgets;
	Controller* controller;
	Menu* menu;

	bool createDeviceD3D(HWND hWnd);
	void cleanupDeviceD3D();
	void createRenderTarget();
	void cleanupRenderTarget();
	void renderDockspace();
	void renderItems();
	void renderMenu();
	void renderWidgets();
	void removeClosedWidgets();
	void manageShortcuts();
	void initializeShortcuts();
	void init();
public:
	GUI();
	~GUI();
	void run();

	void addWidget(Widget* widget);
	int getWidgetIndex();
	
	void setResizeWidth(UINT width) {
		m_ResizeWidth = width;
	}
	void setResizeHeight(UINT height) {
		m_ResizeHeight = height;
	}
};