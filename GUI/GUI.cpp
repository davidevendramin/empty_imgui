#include "GUI.h"
#include "Menu.h"
#include "Controller.h"
#include "Widget.h"
#include "ExampleWidget.h"

GUI::GUI()
{
	this->controller = new Controller();
	this->menu = new Menu(this->controller, this);
}

GUI::~GUI()
{
	delete this->menu;
	delete this->controller;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
	this->cleanupDeviceD3D();
	::DestroyWindow(this->hwnd);

	::UnregisterClass(this->wc.lpszClassName, this->wc.hInstance);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void GUI::init()
{

	HICON hIcon = NULL;
	wchar_t Title[128];
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ImGuiIO* io;


	std::srand((unsigned int)time(NULL));

	ImGui_ImplWin32_EnableDpiAwareness();
	this->wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Empty_ImGUI", NULL };
	::RegisterClassEx(&this->wc);

#ifdef NDEBUG
	wsprintf(Title, L"Empty_GUI");
#else
	wsprintf(Title, L"Empty_GUI-Debug");
#endif

	this->hwnd = ::CreateWindowW(this->wc.lpszClassName, Title, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE, 100, 100, 1280, 800, NULL, NULL, this->wc.hInstance, NULL);

	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));


	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	RegisterDeviceNotification(
		this->hwnd,                // events recipient
		&NotificationFilter,        // type of device
		DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
	);

	if (!this->createDeviceD3D(hwnd))
	{
		this->cleanupDeviceD3D();
		::UnregisterClass(this->wc.lpszClassName, this->wc.hInstance);
		return;
	}

	::ShowWindow(this->hwnd, SW_MAXIMIZE);
	::UpdateWindow(this->hwnd);

	// Initialization code
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	   // Enable Multi-Viewport / Platform Windows
	io->ConfigWindowsMoveFromTitleBarOnly = true;
	io->IniFilename = nullptr;

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(this->hwnd);
	ImGui_ImplDX11_Init(this->m_pd3dDevice, this->m_pd3dDeviceContext);

	ImGui_ImplDX11_InvalidateDeviceObjects();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 5.0f;       // Arrotondamento finestre
	style.ChildRounding = 5.0f;        // Arrotondamento child windows
	style.FrameRounding = 5.0f;        // Arrotondamento bottoni, checkbox, etc.
	style.PopupRounding = 3.0f;        // Arrotondamento popup
	style.ScrollbarRounding = 12.0f;    // Arrotondamento scrollbar
	style.GrabRounding = 2.0f;         // Arrotondamento grab (per slider, etc.)
	style.TabRounding = 5.0f;          // Arrotondamento tabs
	style.DockingSeparatorSize = 0.0f; // Spessore separatore dock

}

void GUI::run()
{
	/*
		This is the main loop of the application. It handles the window messages and updates the GUI.
	*/

	// use two variables to control the rendering loop. stop brings to the eventual popup of choosing the csv name
	// while realStop actually stops the program ending the loop
	MSG msg;
	// this is the color of the background, when no widgets are placed in the main dock
	ImVec4 bgColor = ImVec4(0.3f, 0.3f, 0.3f, 1.00f);
	const float bg_color_with_alpha[4] = { bgColor.x * bgColor.w, bgColor.y * bgColor.w, bgColor.z * bgColor.w, bgColor.w };

	// initialize the Graphical Environment, creating the window and the D3D device
	this->init();

	long long startTime;
	while (!stop)
	{
		startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT) {
				stop = true;
			}
		}
		if (stop)
		{
			break;
		}

		// handle window being minimized or screen locked
		if (this->m_SwapChainOccluded && this->m_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		this->m_SwapChainOccluded = false;

		// handle window resize (we don't resize directly in the WM_SIZE handler)
		if (this->m_ResizeWidth != 0 && this->m_ResizeHeight != 0)
		{
			this->cleanupRenderTarget();
			this->m_pSwapChain->ResizeBuffers(0, this->m_ResizeWidth, this->m_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			this->m_ResizeWidth = this->m_ResizeHeight = 0;
			this->createRenderTarget();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		this->renderDockspace();
		this->renderItems();

		ImGui::Render();
		m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
		m_pd3dDeviceContext->ClearRenderTargetView(m_mainRenderTargetView, bg_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		HRESULT hr = this->m_pSwapChain->Present(1, 0);   // with vsync
		//HRESULT hr = this->m_pSwapChain->Present(0, 0); // without vsync
		this->m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

	}
}

void GUI::renderDockspace() {
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	float MENU_BAR_HEIGHT = ImGui::GetFrameHeight();
	float reserved_top = MENU_BAR_HEIGHT;

	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + reserved_top));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - reserved_top));

	ImGuiWindowFlags host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("DockSpaceHostWindow", nullptr, host_window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_AutoHideTabBar;

	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

	ImGui::End();
}

void GUI::initializeShortcuts() {
	// add here your custom shortcut with a thunk that calls the function you want to execute
	// example: this->shortcuts[HK_CTRL_X] = [this]() { this->addWidget(new WidgetLineGraph())};
	this->shortcuts[HK_CTRL_E] = [this]() { this->addWidget(new ExampleWidget(this->controller, this)); };
}

void GUI::manageShortcuts() {
	// for each shortcut, call the function associated with it if it is pressed
	for (auto& shortcut : this->shortcuts) {
		if (ImGui::Shortcut(shortcut.first, 0, ImGuiInputFlags_RouteAlways)) {
			shortcut.second();
		}
	}
}

void GUI::renderItems() {
	// general stuff to do once for each main loop iteration
	this->removeClosedWidgets();
	this->manageShortcuts();

	// render the main view of the application
	this->renderMenu();
	this->renderWidgets();
}

void GUI::renderMenu() {
	// render menu bar
	this->menu->render();
}

void GUI::renderWidgets() {
	// render widgets
	for (auto& widget : this->widgets) {
		widget->render();
	}
}

void GUI::removeClosedWidgets() {
	// remove closed widgets from the vector
	auto removeIt = std::remove_if(this->widgets.begin(), this->widgets.end(),
		[&](const auto& wm)
		{
			if (!wm->getIsOpen()) {
				delete wm;
				return true;
			}
			else {
				return false;
			}
		});

	this->widgets.erase(removeIt, this->widgets.end());
}

void GUI::addWidget(Widget* widget) {
	this->widgets.push_back(widget);
	this->widgetIndex++;
}

int GUI::getWidgetIndex() {
	return this->widgetIndex;
}

bool GUI::createDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    createRenderTarget();
    return true;
}

void GUI::cleanupDeviceD3D()
{
    cleanupRenderTarget();
    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = nullptr; }
    if (m_pd3dDeviceContext) { m_pd3dDeviceContext->Release(); m_pd3dDeviceContext = nullptr; }
    if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = nullptr; }
}

void GUI::createRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
    pBackBuffer->Release();
}

void GUI::cleanupRenderTarget()
{
    if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = nullptr; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	GUI* gui = nullptr;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	gui = reinterpret_cast<GUI*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (gui == nullptr)
	{
		return ::DefWindowProcW(hWnd, msg, wParam, lParam);
	}

    switch (msg)
    {
    case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			gui->setResizeWidth((UINT)LOWORD(lParam));
			gui->setResizeHeight((UINT)HIWORD(lParam));
		}
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}