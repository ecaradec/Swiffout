// ChSurfMatch.cpp : Defines the entry point for the DLL application.
//
// in-process-plugins
// process-per-tab
// process-per-site
// single-process
#include "stdafx.h"
#include "npapi.h"
#include "npfunctions.h"
#include "npruntime.h"
#include "nptypes.h"
#include <stdlib.h>
#include <malloc.h>
#include <atlstr.h>
#include <shellapi.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif


CString GetModuleDirectory() {
    HMODULE hmod=GetModuleHandle("npprotocol.dll");
    char filename[MAX_PATH+1];
    GetModuleFileNameA(hmod, filename, sizeof(filename));
    
    char *lastSlash=strrchr(filename, '\\');
    if(!lastSlash)
        return "";

    *lastSlash=0;
    return filename;
}

NPNetscapeFuncs g_npnFuncs;
NPP             g_instance;

NPError NPP_Initialize()
{
    return NPERR_NO_ERROR;
}
void NPP_Shutdown()
{
    return;
}
NPError NPP_New(NPMIMEType pluginType, NPP instance,
                          uint16_t mode, int16_t argc, char* argn[],
                          char* argv[], NPSavedData* saved)
{
    g_instance=instance;
    return NPERR_NO_ERROR;
}
NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
    return NPERR_NO_ERROR;
}
NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
    return NPERR_NO_ERROR;
}
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    return NPERR_NO_ERROR;
}
NPError NPP_DestroyStream(NPP instance, NPStream* stream,
                                    NPReason reason)
{
    return NPERR_NO_ERROR;
}
int32_t NPP_WriteReady(NPP instance, NPStream* stream)
{
    return 0;
}
int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    return 0;
}
void NPP_StreamAsFile(NPP instance, NPStream* stream,
                                   const char* fname)
{
    return;
}
void NPP_Print(NPP instance, NPPrint* platformPrint)
{
    return;
}
int16_t NPP_HandleEvent(NPP instance, void* event)
{
    return NPERR_NO_ERROR;
}
void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    return;
}

// NPObject functions
NPObject *NPAllocate(NPP npp, NPClass *aClass) {
    NPObject *p=(NPObject*)malloc(sizeof(NPObject));
    p->_class=aClass;
    p->referenceCount=0;
    return p;
}
void NPDeallocate(NPObject *npobj) {
    free(npobj);
}
void NPInvalidate(NPObject *npobj) {
}
bool NPHasMethod(NPObject *npobj, NPIdentifier name) {    
    char *utf8name=g_npnFuncs.utf8fromidentifier(name);  
    if(strcmp("playSwf", utf8name)==0)
        return true;
    return false;
}
bool NPInvoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {
    char *utf8name=g_npnFuncs.utf8fromidentifier(name);
    if(strcmp("playSwf", utf8name)==0 && argCount==2 && args[0].type==NPVariantType_String, args[1].type==NPVariantType_String) {

        CStringA swf(args[0].value.stringValue.utf8characters);
        CStringA flashVars(args[1].value.stringValue.utf8characters);
        
        CStringA cmdLine;
        cmdLine.Format("swiffout:swiffout_href=%s,swiffout_width=320,swiffout_height=200,swiffout_flashvars=%s", swf, flashVars);
        ShellExecute(0, 0, GetModuleDirectory()+"\\swiffoutrunner.exe", cmdLine, 0, SW_SHOWNORMAL);
        return true;
    }
    return false;
}
bool NPInvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) {
    return false;
}
bool NPHasProperty(NPObject *npobj, NPIdentifier name) {
    return false;
}
bool NPGetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result) {
    return false;
}
bool NPSetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value) {
    return false;
}
bool NPRemoveProperty(NPObject *npobj, NPIdentifier name) {
    return false;
}
bool NPEnumeration(NPObject *npobj, NPIdentifier **value, uint32_t *count) {
    return false;
}
bool NPConstruct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) {
    return false;
}

NPClass clss={
    NP_CLASS_STRUCT_VERSION,
    NPAllocate,
    NPDeallocate,
    NPInvalidate,
    NPHasMethod,
    NPInvoke,
    NPInvokeDefault,
    NPHasProperty,
    NPGetProperty,
    NPSetProperty,
    NPRemoveProperty,
    NPEnumeration,
    NPConstruct
};

NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    if(variable==NPPVpluginScriptableNPObject) {
        NPObject *p=g_npnFuncs.createobject(g_instance, &clss);
        *(NPObject**)value = p;
    }
    return NPERR_NO_ERROR;
}
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    //memcpy(&NPNPluginFunc, pFuncs, sizeof(pFuncs->size));
    pFuncs->newp          = NPP_New;
    pFuncs->destroy       = NPP_Destroy;
    pFuncs->setwindow     = NPP_SetWindow;
    pFuncs->newstream     = NPP_NewStream;
    pFuncs->destroystream = NPP_DestroyStream;
    pFuncs->asfile        = NPP_StreamAsFile;
    pFuncs->writeready    = NPP_WriteReady;
    pFuncs->write         = NPP_Write;
    pFuncs->print         = NPP_Print;
    pFuncs->event         = NPP_HandleEvent;
    pFuncs->urlnotify     = NPP_URLNotify;
    pFuncs->getvalue      = NPP_GetValue;
    pFuncs->setvalue      = NPP_SetValue;
    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs* pFuncs)
{
    /*HKEY hkey;
    RegOpenKey(HKEY_CURRENT_USER, "Software\\Classes\\swiffout", &hkey);
    RegSetValueEx(hkey, "url protocol", 0, REG_SZ, (BYTE*)"", 0);
    RegCloseKey(hkey);

    RegSetValue(HKEY_CURRENT_USER, "Software\\Classes\\swiffout\\shell\\open\\command", REG_SZ, GetModuleDirectory()+"swiffoutrunner.exe", -1);*/

    if(pFuncs == NULL)
        return NPERR_INVALID_FUNCTABLE_ERROR;

    if(HIBYTE(pFuncs->version) > NP_VERSION_MAJOR)
        return NPERR_INCOMPATIBLE_VERSION_ERROR;

    memcpy(&g_npnFuncs, pFuncs, pFuncs->size);    

    return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
    return NPERR_NO_ERROR;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
