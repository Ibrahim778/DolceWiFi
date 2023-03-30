#include <kernel.h>
#include <paf.h>
#include <quickmenureborn/qm_reborn.h>
#include <registrymgr.h>
#include <power.h>
#include <appmgr.h>
#include <vshbridge.h>

extern "C" {
    unsigned int sceWlanGetConfiguration();
    int sceWlanSetConfiguration(bool);
    int sceAppMgrGetStatusByName(const char *, SceAppMgrAppStatus *);
    int sceAppMgrQuitApp(SceUID);
}

using namespace paf;

#define PREFIX "DolceWiFi_"

widgetData *iconPlaneData = SCE_NULL, *checkBox = SCE_NULL;
bool textSet = SCE_FALSE;


void CheckBoxRecall(const char *refID)
{
    int val = 0;

    sceRegMgrGetKeyInt("/CONFIG/NET", "wifi_flag", &val);
    ((ui::CheckBox *)checkBox->widget)->SetChecked(0, val, 0);
}

bool GetCondition()
{
    unsigned int ret = sceWlanGetConfiguration();
    return (ret & 9) != 0;
}

void OnCheckBoxPressed(const char *refID, SceInt32 hash, SceInt32 eventID, void *pUserDat)
{
    unsigned int wlanConfig = sceWlanGetConfiguration();
    SceBool wlanThing = GetCondition();

    SceAppMgrAppStatus status;
    
    if((wlanConfig & 0x200) == 0 && sceWlanSetConfiguration(!wlanThing) == 0)
    {
        if(sceAppMgrGetStatusByName("NPXS10015", &status) == 0)
            sceAppMgrQuitApp(status.appId);
        sceRegMgrSetKeyInt("/CONFIG/NET", "wifi_flag", !wlanThing ? 1 : 0);
        QuickMenuRebornSetWidgetColor(PREFIX "icon_plane", 1,1,1, !wlanThing ? 1 : 0.4f);
    }
}

void OnTextLoaded(const char *refID)
{
    if(textSet)
        return;

    rco::Element e;
    e.hash = 0x8f0ca453; // msg_wifi
    
    paf::string str;
    common::Utf16ToUtf8(Plugin::Find("impose_plugin")->GetWString(&e), &str);
    
    QuickMenuRebornSetWidgetLabel(refID, str.c_str());

    textSet = SCE_TRUE;
}

void OnIconPlaneLoaded(const char *refID)
{
    thread::s_mainThreadMutex.Lock();
    Plugin *impose_plugin = Plugin::Find("impose_plugin");

    graph::Surface *wifiTex = SCE_NULL;
    rco::Element e;

    e.hash = 0x5dd183bf; // WiFi Icon
    Plugin::GetTexture(&wifiTex, impose_plugin, &e);

    if(wifiTex)
        ((ui::Widget *)iconPlaneData->widget)->SetSurface(&wifiTex, 0);

    thread::s_mainThreadMutex.Unlock();
}

extern "C" {
    SceInt32 module_start(SceSize args, ScePVoid argp)
    {
#ifndef _DEBUG
        if(vshSblAimgrIsVITA())
            return SCE_KERNEL_START_NO_RESIDENT;
#endif

        QuickMenuRebornSeparator(PREFIX "separator", SCE_SEPARATOR_HEIGHT);
        
        QuickMenuRebornRegisterWidget(PREFIX "root_plane", SCE_NULL, plane);       
        QuickMenuRebornSetWidgetSize(PREFIX "root_plane", SCE_PLANE_WIDTH, 65, 0, 0);
        QuickMenuRebornSetWidgetColor(PREFIX "root_plane", 1,1,1,0); // transparent

        iconPlaneData = QuickMenuRebornRegisterWidget(PREFIX "icon_plane", PREFIX "root_plane", plane);
        QuickMenuRebornSetWidgetSize(PREFIX "icon_plane", 65, 65, 0, 0);
        QuickMenuRebornSetWidgetColor(PREFIX "icon_plane", 1,1,1,1);
        QuickMenuRebornSetWidgetPosition(PREFIX "icon_plane", -338.5, 0, 0, 0);

        QuickMenuRebornAssignOnLoadHandler(OnIconPlaneLoaded, PREFIX "icon_plane");

        QuickMenuRebornRegisterWidget(PREFIX "text", PREFIX "root_plane", text);
        QuickMenuRebornSetWidgetSize(PREFIX "text", 200, 65, 0, 0);
        QuickMenuRebornSetWidgetColor(PREFIX "text", 1,1,1,1);
        QuickMenuRebornSetWidgetPosition(PREFIX "text", -270, 0, 0, 0);
        QuickMenuRebornSetWidgetLabel(PREFIX "text", "Fallback (WiFi)");

        QuickMenuRebornAssignOnLoadHandler(OnTextLoaded, PREFIX "text");

        checkBox = QuickMenuRebornRegisterWidget(PREFIX "check_box", PREFIX "root_plane", check_box);
        QuickMenuRebornSetWidgetSize(PREFIX "check_box", 46, 46, 0, 0);
        QuickMenuRebornSetWidgetColor(PREFIX "check_box", 1,1,1,1);
        QuickMenuRebornSetWidgetPosition(PREFIX "check_box", 350.5, 0, 0, 0);
        QuickMenuRebornSetCheckBoxState(PREFIX "check_box", 1);

        QuickMenuRebornAssignRecallHandler(CheckBoxRecall, PREFIX "check_box");
        QuickMenuRebornRegisterEventHanlder(PREFIX "check_box", QMR_BUTTON_RELEASE_ID, OnCheckBoxPressed, SCE_NULL);
    
        return SCE_KERNEL_START_SUCCESS;
    }

    SceInt32 module_stop(SceSize args, ScePVoid argp)
    {
        QuickMenuRebornRemoveSeparator(PREFIX "separator");
        QuickMenuRebornUnregisterWidget(PREFIX "text");
        QuickMenuRebornUnregisterWidget(PREFIX "icon_plane");
        QuickMenuRebornUnregisterWidget(PREFIX "root_plane");
        return SCE_KERNEL_STOP_SUCCESS;
    }
};