// MFEVRPlayback.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MFEVRPlayback.h"

#include "player.h"

#define MAX_LOADSTRING 100

//BOOL        g_bRepaintClient = TRUE;            // Repaint the application client area?
CPlayer* g_pPlayer = NULL;                  // Global player object. 

// Note: After WM_CREATE is processed, g_pPlayer remains valid until the
// window is destroyed.

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void                NotifyError(HWND hwnd, const WCHAR* sErrorMessage, HRESULT hr);

// Message handlers
void                OnFileOpen(HWND hwnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MFEVRPLAYBACK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MFEVRPLAYBACK));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MFEVRPLAYBACK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MFEVRPLAYBACK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // Initialize the player object.
        HRESULT hr = CPlayer::CreateInstance(hWnd, hWnd, &g_pPlayer);
        if (SUCCEEDED(hr))
        {
            return 0;   // Success.
        }
        else
        {
            NotifyError(NULL, L"Could not initialize the player object.", hr);
            return -1;  // Destroy the window
        }
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case ID_FILE_OPENFILE:
                OnFileOpen(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
        if ((g_pPlayer->GetState() == Started) || (g_pPlayer->GetState() == Paused))
            g_pPlayer->ResizeVideo(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_ERASEBKGND:
        // Suppress window erasing, to reduce flickering while the video is playing.
        return 1;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CHAR:
        switch (wParam)
        {
            // Space key toggles between running and paused
        case VK_SPACE:
            if (g_pPlayer->GetState() == Started)
            {
                g_pPlayer->Pause();
            }
            else if (g_pPlayer->GetState() == Paused)
            {
                g_pPlayer->Play();
            }
            break;
        }
        break;
    case WM_APP_PLAYER_EVENT:
        {
            HRESULT hr = g_pPlayer->HandleEvent(wParam);
            if (FAILED(hr))
            {
                NotifyError(hWnd, L"An error occurred.", hr);
            }
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//  Open an audio/video file.
void OnFileOpen(HWND hwnd)
{
    IFileOpenDialog* pFileOpen = NULL;
    IShellItem* pItem = NULL;
    PWSTR pszFilePath = NULL;

    // Create the FileOpenDialog object.
    HRESULT hr = CoCreateInstance(__uuidof(FileOpenDialog), NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
    if (FAILED(hr))
    {
        goto done;
    }

    // Show the Open dialog box.
    hr = pFileOpen->Show(NULL);
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
    {
        // The user canceled the dialog. Do not treat as an error.
        hr = S_OK;
        goto done;
    }
    else if (FAILED(hr))
    {
        goto done;
    }

    // Get the file name from the dialog box.
    hr = pFileOpen->GetResult(&pItem);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr))
    {
        goto done;
    }

    //RECT winRect;
    //GetWindowRect(hwnd, &winRect);
    //SetWindowPos(hwnd, NULL, 0, 0, 1600, 1200, 0);
// Display the file name to the user.
    hr = g_pPlayer->OpenURL(pszFilePath);
    //SetWindowPos(hwnd, NULL, winRect.left, winRect.top, winRect.right, winRect.bottom, 0);

done:
    if (FAILED(hr))
    {
        NotifyError(hwnd, L"Could not open the file.", hr);
    }
    CoTaskMemFree(pszFilePath);
    SafeRelease(&pItem);
    SafeRelease(&pFileOpen);
}

//  Show a message box with an error message.
void NotifyError(HWND hwnd, PCWSTR pszErrorMessage, HRESULT hrErr)
{
    const size_t MESSAGE_LEN = 512;
    WCHAR message[MESSAGE_LEN];

    if (SUCCEEDED(StringCchPrintf(message, MESSAGE_LEN, L"%s (HRESULT = 0x%X)",
        pszErrorMessage, hrErr)))
    {
        MessageBox(hwnd, message, NULL, MB_OK | MB_ICONERROR);
    }
}

