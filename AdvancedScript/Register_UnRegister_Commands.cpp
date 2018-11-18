#include "Register_UnRegister_Commands.h"
#include "MainForm.h"
#include "HelperFunctions.h"

bool LogOff_ = false;

void RegisterCommands(PLUG_INITSTRUCT* initStruct)
{
	_plugin_logprintf("[AdvancedScript] pluginHandle: %d\n", pluginHandle);


	if (!_plugin_registercommand(pluginHandle, "AdvancedScript", cbMainForm, false))
		_plugin_logputs("[AdvancedScript] error registering the \AdvancedScript\ command!");

	if (!_plugin_registercommand(pluginHandle, "LogAtBP", cbLogAtBP, false))
		_plugin_logputs("[AdvancedScript] error registering the \AdvancedScript\ command!");

}


////////////////////////////////////////////////////////////////////////////////////////////////////
static void ShowDialog_IATFixer()
{
	AdvancedScript::MainForm IATFixer;
	IATFixer.ShowDialog();
}

static bool cbMainForm(int argc, char* argv[])
{
	_plugin_logputs("[MainForm] Loading Form!");
	///////////////////////////////////////////////////////////////////////////////////////
	// this will keep form activate and x64dbg unable to do it's work by excuted commands 
	//ScriptS::IATFixer IATFixer;
	//IATFixer.ShowDialog();
	///////////////////////////////////////////////////////////////////////////////////////

	// we used this (New Thread) to Create our Form in new Thread so we able to comunicated with x64dbg
	// and be able to send command let go to address or do some  other commands 
	System::Threading::Thread^ thread_ = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(&ShowDialog_IATFixer));
	thread_->Start();

	return true;
}

static bool cbLogAtBP(int argc, char* argv[]) {
	if (argc != 2) { _plugin_logprintf("worng arguments"); return false; }
	String^ temp = charPTR2String(argv[0]);
	temp = temp->Substring(temp->IndexOf(" ") + 1, temp->Length - (temp->IndexOf(" ") + 1));
	LogOff_ = Str2bool(temp);
	if (LogOff_) {
		GuiDisableLog(); 
	}else{
		GuiEnableLog();
	}


}