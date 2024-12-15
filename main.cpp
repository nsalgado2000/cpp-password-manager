#include <windows.h>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Definições de IDs para controles
#define ID_ADD_BUTTON 1
#define ID_REMOVE_BUTTON 2
#define ID_SHOW_BUTTON 3
#define ID_SERVICE_EDIT 4
#define ID_PASSWORD_EDIT 5
#define ID_LISTBOX 6

// Cores modernas
#define COLOR_BACKGROUND RGB(24, 24, 27)
#define COLOR_SURFACE RGB(39, 39, 42)
#define COLOR_PRIMARY RGB(161, 161, 170)
#define COLOR_TEXT RGB(244, 244, 245)
#define COLOR_TEXT_SECONDARY RGB(161, 161, 170)
#define COLOR_DANGER RGB(239, 68, 68)
#define COLOR_INPUT_BG RGB(63, 63, 70)
#define COLOR_BUTTON_HOVER RGB(190, 190, 196)

class ModernButton {
public:
    static WNDPROC originalProc;
    static LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
        static bool isHovered = false;

        switch (msg) {
            case WM_MOUSEMOVE: {
                if (!isHovered) {
                    isHovered = true;
                    InvalidateRect(hwnd, NULL, TRUE);
                    
                    TRACKMOUSEEVENT tme;
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = hwnd;
                    TrackMouseEvent(&tme);
                }
                break;
            }

            case WM_MOUSELEAVE: {
                isHovered = false;
                InvalidateRect(hwnd, NULL, TRUE);
                break;
            }

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                
                RECT rect;
                GetClientRect(hwnd, &rect);
                
                // Cor de fundo do botão
                COLORREF bgColor = isHovered ? COLOR_BUTTON_HOVER : COLOR_PRIMARY;
                
                // Cria um caminho arredondado
                HRGN region = CreateRoundRectRgn(0, 0, rect.right, rect.bottom, 10, 10);
                SelectClipRgn(hdc, region);
                
                // Preenche o fundo
                HBRUSH hBrush = CreateSolidBrush(bgColor);
                FillRect(hdc, &rect, hBrush);
                DeleteObject(hBrush);

                // Configura o texto
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, COLOR_BACKGROUND);
                
                // Usa uma fonte moderna
                HFONT hFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
                
                SelectObject(hdc, hFont);
                
                // Obtém e desenha o texto
                char text[256];
                GetWindowText(hwnd, text, 256);
                DrawText(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                DeleteObject(hFont);
                DeleteObject(region);
                EndPaint(hwnd, &ps);
                return 0;
            }
        }
        return CallWindowProc(originalProc, hwnd, msg, wp, lp);
    }
};

WNDPROC ModernButton::originalProc = nullptr;

class PasswordManager {
private:
    std::map<std::string, std::string> passwords;
    HWND hwnd;
    HWND serviceEdit;
    HWND passwordEdit;
    HWND listBox;
    HWND serviceLabel;
    HWND passwordLabel;
    HWND addButton;
    HWND removeButton;
    HWND showButton;
    HFONT hFont;
    HBRUSH hBackgroundBrush;
    
    void SavePasswords() {
        std::ofstream file("passwords.txt");
        for (const auto& pair : passwords) {
            file << pair.first << ":" << pair.second << std::endl;
        }
    }

    void LoadPasswords() {
        std::ifstream file("passwords.txt");
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string service = line.substr(0, pos);
                std::string password = line.substr(pos + 1);
                passwords[service] = password;
            }
        }
    }

    void UpdateListBox() {
        SendMessage(listBox, LB_RESETCONTENT, 0, 0);
        for (const auto& pair : passwords) {
            SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)pair.first.c_str());
        }
    }

public:
    PasswordManager(HWND parentHwnd) : hwnd(parentHwnd) {
        LoadPasswords();
        hBackgroundBrush = CreateSolidBrush(COLOR_BACKGROUND);
        
        // Criar fonte moderna
        hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
    }

    ~PasswordManager() {
        DeleteObject(hFont);
        DeleteObject(hBackgroundBrush);
    }

    void CreateControls() {
        // Inicializa os controles comuns do Windows
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_STANDARD_CLASSES;
        InitCommonControlsEx(&icex);

        // Create service input with modern style
        serviceLabel = CreateWindow("STATIC", "Service:", 
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 20, 70, 20, hwnd, NULL, NULL, NULL);
        SendMessage(serviceLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
        
        serviceEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
            20, 45, 460, 35, hwnd, (HMENU)ID_SERVICE_EDIT, NULL, NULL);
        SendMessage(serviceEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Create password input with modern style
        passwordLabel = CreateWindow("STATIC", "Password:",
            WS_VISIBLE | WS_CHILD | SS_LEFT,
            20, 85, 70, 20, hwnd, NULL, NULL, NULL);
        SendMessage(passwordLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
        
        passwordEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_VISIBLE | WS_CHILD | ES_PASSWORD | ES_AUTOHSCROLL,
            20, 110, 460, 35, hwnd, (HMENU)ID_PASSWORD_EDIT, NULL, NULL);
        SendMessage(passwordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Create modern buttons
        addButton = CreateWindow("BUTTON", "ADD PASSWORD",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 160, 140, 40, hwnd, (HMENU)ID_ADD_BUTTON, NULL, NULL);
        SendMessage(addButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        removeButton = CreateWindow("BUTTON", "REMOVE",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            180, 160, 140, 40, hwnd, (HMENU)ID_REMOVE_BUTTON, NULL, NULL);
        SendMessage(removeButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        showButton = CreateWindow("BUTTON", "SHOW PASSWORD",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            340, 160, 140, 40, hwnd, (HMENU)ID_SHOW_BUTTON, NULL, NULL);
        SendMessage(showButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Create modern listbox
        listBox = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "",
            WS_VISIBLE | WS_CHILD | LBS_NOTIFY | WS_VSCROLL | LBS_HASSTRINGS,
            20, 215, 460, 325, hwnd, (HMENU)ID_LISTBOX, NULL, NULL);
        SendMessage(listBox, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Subclassing dos botões para visual moderno
        ModernButton::originalProc = (WNDPROC)SetWindowLongPtr(addButton, 
            GWLP_WNDPROC, (LONG_PTR)ModernButton::ButtonProc);
        SetWindowLongPtr(removeButton, GWLP_WNDPROC, (LONG_PTR)ModernButton::ButtonProc);
        SetWindowLongPtr(showButton, GWLP_WNDPROC, (LONG_PTR)ModernButton::ButtonProc);

        UpdateListBox();
    }

    void ResizeControls(int width, int height) {
        // Mantém margens consistentes
        const int MARGIN = 20;
        const int CONTROL_WIDTH = width - (MARGIN * 2);
        
        // Ajusta campos de texto
        SetWindowPos(serviceEdit, NULL, 
            MARGIN, 45, CONTROL_WIDTH, 30, SWP_NOZORDER);
        SetWindowPos(passwordEdit, NULL, 
            MARGIN, 110, CONTROL_WIDTH, 30, SWP_NOZORDER);

        // Ajusta botões
        int buttonWidth = (CONTROL_WIDTH - 40) / 3;
        SetWindowPos(addButton, NULL, 
            MARGIN, 160, buttonWidth, 35, SWP_NOZORDER);
        SetWindowPos(removeButton, NULL, 
            MARGIN + buttonWidth + 20, 160, buttonWidth, 35, SWP_NOZORDER);
        SetWindowPos(showButton, NULL, 
            MARGIN + (buttonWidth + 20) * 2, 160, buttonWidth, 35, SWP_NOZORDER);

        // Ajusta listbox
        SetWindowPos(listBox, NULL, 
            MARGIN, 215, CONTROL_WIDTH, height - 245, SWP_NOZORDER);

        // Força redesenho
        InvalidateRect(hwnd, NULL, TRUE);
    }

    void AddPassword() {
        char service[256];
        char password[256];
        
        GetWindowText(serviceEdit, service, 256);
        GetWindowText(passwordEdit, password, 256);

        if (strlen(service) > 0 && strlen(password) > 0) {
            passwords[service] = password;
            SavePasswords();
            UpdateListBox();
            
            SetWindowText(serviceEdit, "");
            SetWindowText(passwordEdit, "");
            
            MessageBox(hwnd, "Password added successfully!", "Success", MB_OK);
        } else {
            MessageBox(hwnd, "Please enter both service and password!", "Error", MB_OK);
        }
    }

    void RemovePassword() {
        int idx = SendMessage(listBox, LB_GETCURSEL, 0, 0);
        if (idx != LB_ERR) {
            char service[256];
            SendMessage(listBox, LB_GETTEXT, idx, (LPARAM)service);
            
            passwords.erase(service);
            SavePasswords();
            UpdateListBox();
            
            MessageBox(hwnd, "Password removed successfully!", "Success", MB_OK);
        } else {
            MessageBox(hwnd, "Please select a service to remove!", "Error", MB_OK);
        }
    }

    void ShowPassword() {
        int idx = SendMessage(listBox, LB_GETCURSEL, 0, 0);
        if (idx != LB_ERR) {
            char service[256];
            SendMessage(listBox, LB_GETTEXT, idx, (LPARAM)service);
            
            std::string message = "Password for " + std::string(service) + ":\n" + passwords[service];
            MessageBox(hwnd, message.c_str(), "Password", MB_OK);
        } else {
            MessageBox(hwnd, "Please select a service!", "Error", MB_OK);
        }
    }
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PasswordManager* manager = (PasswordManager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (uMsg) {
        case WM_GETMINMAXINFO: {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = 500;
            lpMMI->ptMinTrackSize.y = 600;
            return 0;
        }

        case WM_CTLCOLORBTN:
            return (LRESULT)GetStockObject(WHITE_BRUSH);

        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, COLOR_TEXT);
            SetBkColor(hdcStatic, COLOR_BACKGROUND);
            return (LRESULT)CreateSolidBrush(COLOR_BACKGROUND);
        }

        case WM_CTLCOLOREDIT: {
            HDC hdcEdit = (HDC)wParam;
            SetTextColor(hdcEdit, COLOR_TEXT);
            SetBkColor(hdcEdit, COLOR_INPUT_BG);
            return (LRESULT)CreateSolidBrush(COLOR_INPUT_BG);
        }

        case WM_CTLCOLORLISTBOX: {
            HDC hdcList = (HDC)wParam;
            SetTextColor(hdcList, COLOR_TEXT);
            SetBkColor(hdcList, COLOR_SURFACE);
            return (LRESULT)CreateSolidBrush(COLOR_SURFACE);
        }

        case WM_SIZE:
            if (manager && wParam != SIZE_MINIMIZED) {
                RECT rect;
                GetClientRect(hwnd, &rect);
                manager->ResizeControls(rect.right - rect.left, rect.bottom - rect.top);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_ADD_BUTTON:
                    if (manager) manager->AddPassword();
                    break;
                case ID_REMOVE_BUTTON:
                    if (manager) manager->RemovePassword();
                    break;
                case ID_SHOW_BUTTON:
                    if (manager) manager->ShowPassword();
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, CreateSolidBrush(COLOR_BACKGROUND));
            return TRUE;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "Modern Password Manager";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        CLASS_NAME,
        "Password Manager",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        500, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) return 0;

    PasswordManager* manager = new PasswordManager(hwnd);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)manager);
    manager->CreateControls();

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete manager;
    return 0;
}