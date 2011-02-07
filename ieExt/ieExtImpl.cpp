// ieExtImpl.cpp : Implementation of CieExtImpl

#include "stdafx.h"
#include "ieExtImpl.h"


// CieExtImpl
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::StatusTextChangeStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BSTR}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::TitleChangeStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BSTR}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::PropertyChangeStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BSTR}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::DownloadBeginStruct = {CC_STDCALL, VT_EMPTY, 0, {NULL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::DownloadCompleteStruct = {CC_STDCALL, VT_EMPTY, 0, {NULL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnQuitStruct = {CC_STDCALL, VT_EMPTY, 0, {NULL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::NewWindow2Struct = {CC_STDCALL, VT_EMPTY, 2, {VT_BYREF|VT_BOOL,VT_BYREF|VT_DISPATCH}}; 
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::CommandStateChangeStruct = {CC_STDCALL, VT_EMPTY, 2, {VT_I4,VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::BeforeNavigate2Struct = {CC_STDCALL, VT_EMPTY, 7, {VT_DISPATCH,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::ProgressChangeStruct = {CC_STDCALL, VT_EMPTY, 2, {VT_I4,VT_I4}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::NavigateComplete2Struct = {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH, VT_BYREF|VT_VARIANT}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::DocumentComplete2Struct = {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH, VT_BYREF|VT_VARIANT}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnVisibleStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnToolBarStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnMenuBarStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnStatusBarStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnFullScreenStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::OnTheaterModeStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::SetSecureLockIconStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_I4}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::NavigateErrorStruct = {CC_STDCALL, VT_EMPTY, 5, {VT_BYREF|VT_BOOL,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_VARIANT,VT_BYREF|VT_DISPATCH}};
__declspec(selectany) ATL::_ATL_FUNC_INFO CWebBrowser2EventsBase::PrivacyImpactedStateChangeStruct = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};